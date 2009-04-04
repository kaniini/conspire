/* Conspire
 * Copyright (C) 2008 William Pitcock
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "javascript.h"

#include "common/signal_factory.h"

struct cjs_signal_handler_
{
	SignalHandler handler;
	mowgli_list_t *js_handlers;
};
static mowgli_heap_t *cjs_signal_handler_heap_ = NULL;
static mowgli_dictionary_t *cjs_signal_handlers_ = NULL;

static struct cjs_signal_handler_ *
cjs_signal_handler_create_(SignalHandler handler)
{
	struct cjs_signal_handler_ *h = mowgli_heap_alloc(cjs_signal_handler_heap_);
	h->handler = handler;
	h->js_handlers = mowgli_list_create();

	return h;
}

static void
cjs_signal_handler_destroy_(struct cjs_signal_handler_ *handler)
{
	mowgli_node_t *n, *tn;

	MOWGLI_LIST_FOREACH_SAFE(n, tn, handler->js_handlers->head) {
		cjs_callback_free((cjs_callback_t *)n->data);
		mowgli_node_delete(n, handler->js_handlers);
		mowgli_node_free(n);
	}

	mowgli_list_free(handler->js_handlers);
	mowgli_heap_free(cjs_signal_handler_heap_, handler);
}

static void
cjs_sig_server_connect_(gpointer *params)
{
	session *sess = params[0];
	gchar *host   = params[1];
	gchar *ip     = params[2];
	gchar *data   = params[3];

	struct cjs_signal_handler_ *handler = mowgli_dictionary_retrieve(cjs_signal_handlers_, "server connect");
	mowgli_node_t *n;

	MOWGLI_LIST_FOREACH(n, handler->js_handlers->head) {
		jsval rval;
		JSObject *js_session;
		JSString *js_host, *js_ip, *js_data;
		jsval functor_argv[4];

		cjs_callback_t *cb = (cjs_callback_t *)n->data;

		if(!JS_EnterLocalRootScope(cb->context))
			continue;

		/* XXX: Can we cache this? Consider attaching to "session create" and
		 * "session destroy" signals to register/unregister JS sessions. This will
		 * need to work around GC. Alternatively, I think we can implement
		 * .equals() to compare sess pointers and that should work for userland.
		 *   --impl */
		js_session = JS_NewObject(cb->context, &ConspireSession_class, NULL, NULL);
		JS_SetPrivate(cb->context, js_session, sess);

		js_host = JS_NewStringCopyZ(cb->context, host);
		js_ip = JS_NewStringCopyZ(cb->context, ip);
		js_data = JS_NewStringCopyZ(cb->context, data);

		functor_argv[0] = OBJECT_TO_JSVAL(js_session);
		functor_argv[1] = STRING_TO_JSVAL(js_host);
		functor_argv[2] = STRING_TO_JSVAL(js_ip);
		functor_argv[3] = STRING_TO_JSVAL(js_data);

		JS_CallFunctionValue(cb->context, cb->functor, OBJECT_TO_JSVAL(cb->functor), 4, functor_argv, &rval);

		JS_LeaveLocalRootScope(cb->context);
	}
}

static JSBool
js_signal_attach_(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	const char *name;
	JSObject *functor;
	struct cjs_signal_handler_ *handler;

	if(!JS_ConvertArguments(cx, argc, argv, "so", &name, &functor))
		return JS_FALSE;

	if((handler = mowgli_dictionary_retrieve(cjs_signal_handlers_, name)) == NULL) {
		JS_ReportError(cx, "No signal handler is available for signal '%s'", name);
		return JS_FALSE;
	}

	mowgli_node_add(cjs_callback_create(cx, obj, functor), mowgli_node_create(), handler->js_handlers);

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

static JSBool
js_signal_disconnect_(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	const char *name;
	JSObject *functor;
	struct cjs_signal_handler_ *handler;
	mowgli_node_t *n, *tn;

	if(!JS_ConvertArguments(cx, argc, argv, "so", &name, &functor))
		return JS_FALSE;

	if((handler = mowgli_dictionary_retrieve(cjs_signal_handlers_, name)) == NULL) {
		JS_ReportError(cx, "No signal handler is available for signal '%s'", name);
		return JS_FALSE;
	}

	MOWGLI_LIST_FOREACH_SAFE(n, tn, handler->js_handlers->head) {
		cjs_callback_t *cb = (cjs_callback_t *)n->data;

		if(cb->context == cx && cb->functor == functor) {
			cjs_callback_free((cjs_callback_t *)n->data);
			mowgli_node_delete(n, handler->js_handlers);
			mowgli_node_free(n);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/* Plugin initialization/finalization routines. */
static struct {
	const gchar *signal;
	SignalHandler handler;
} cjs_signal_handler_map_[] = {
	{ "server connect",	cjs_sig_server_connect_ },
	{ NULL, NULL },
};

gboolean
cjs_signals_initialize(JSRuntime *rt)
{
	int i;

	cjs_signal_handler_heap_ = mowgli_heap_create(sizeof(struct cjs_signal_handler_), 32, BH_NOW);
	cjs_signal_handlers_ = mowgli_dictionary_create(g_ascii_strcasecmp);

	for(i = 0; cjs_signal_handler_map_[i].signal != NULL; i++) {
		mowgli_dictionary_add(cjs_signal_handlers_,
			cjs_signal_handler_map_[i].signal,
			cjs_signal_handler_create_(cjs_signal_handler_map_[i].handler));
		signal_attach(cjs_signal_handler_map_[i].signal, cjs_signal_handler_map_[i].handler);
	}

	return TRUE;
}

static void
cjs_signal_handlers_destroy_cb_(mowgli_dictionary_elem_t *e, void *privdata)
{
	struct cjs_signal_handler_ *handler = (struct cjs_signal_handler_ *)e->data;

	signal_disconnect(e->key, handler->handler);
	cjs_signal_handler_destroy_(handler);
}

gboolean
cjs_signals_finalize(JSRuntime *rt)
{
	mowgli_dictionary_destroy(cjs_signal_handlers_, cjs_signal_handlers_destroy_cb_, NULL);

	return TRUE;
}

/* Script initialization/finalization routines. */
static JSFunctionSpec js_signal_functions_[] = {
	JS_FS("signal_attach",		js_signal_attach_,		2,	0,	0),
	JS_FS("signal_disconnect",	js_signal_disconnect_,	2,	0,	0),
	JS_FS_END,
};

gboolean
cjs_script_signals_initialize(JSContext *cx, JSObject *global)
{
	return JS_DefineFunctions(cx, global, js_signal_functions_);
}

gboolean
cjs_script_signals_finalize(JSContext *cx)
{
	/* Disconnect any associated signals. */
	struct cjs_signal_handler_ *handler;
	mowgli_dictionary_iteration_state_t state;

	MOWGLI_DICTIONARY_FOREACH(handler, &state, cjs_signal_handlers_) {
		mowgli_node_t *n, *tn;

		MOWGLI_LIST_FOREACH_SAFE(n, tn, handler->js_handlers->head) {
			cjs_callback_t *cb = (cjs_callback_t *)n->data;

			if(cb->context == cx) {
				cjs_callback_free(cb);
				mowgli_node_delete(n, handler->js_handlers);
				mowgli_node_free(n);
			}
		}
	}

	return TRUE;
}
