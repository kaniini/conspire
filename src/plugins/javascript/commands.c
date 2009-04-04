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

static mowgli_dictionary_t *cjs_commands_;

/*
 * Proxy conversion for command handlers, adding commands and receiving
 * commands.
 */
static CommandResult
cjs_cmd_command_proxy_(session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
	mowgli_node_t *n;
	mowgli_list_t *commands = mowgli_dictionary_retrieve(cjs_commands_, word[1]);

	MOWGLI_LIST_FOREACH(n, commands->head) {
		jsval rval;
		int result;
		JSObject *argv, *argv_eol;
		jsval *functor_argv;
		void *frame_ptr;
		gint i;

		cjs_callback_t *cb = (cjs_callback_t *)n->data;

		if(!JS_EnterLocalRootScope(cb->context))
			return CMD_EXEC_FAIL;

		argv = JS_NewArrayObject(cb->context, 0, NULL);
		argv_eol = JS_NewArrayObject(cb->context, 0, NULL);

		for (i = 1; i < PDIWORDS && word[i] != NULL; i++) {
			jsval jsword = STRING_TO_JSVAL(JS_NewStringCopyZ(cb->context, word[i]));
			jsval jsword_eol = STRING_TO_JSVAL(JS_NewStringCopyZ(cb->context, word_eol[i]));

			JS_SetElement(cb->context, argv, i, &jsword);
			JS_SetElement(cb->context, argv_eol, i, &jsword_eol);
		}

		functor_argv = JS_PushArguments(cb->context, &frame_ptr, "oo", argv, argv_eol);
		JS_CallFunctionValue(cb->context, cb->functor, OBJECT_TO_JSVAL(cb->functor), 2, functor_argv, &rval);

		JS_LeaveLocalRootScope(cb->context);

		if(JSVAL_IS_INT(rval) && (result = JSVAL_TO_INT(rval)) != CMD_EXEC_OK)
			return result;
	}

	return CMD_EXEC_OK;
}

static JSBool
js_command_register_(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	const char *name, *desc;
	int args;
	JSObject *functor;
	mowgli_list_t *commands;

	if(!JS_ConvertArguments(cx, argc, argv, "ssio", &name, &desc, &args, &functor))
		return JS_FALSE;

	if((commands = mowgli_dictionary_retrieve(cjs_commands_, name)) == NULL) {
		commands = mowgli_list_create();

		mowgli_dictionary_add(cjs_commands_, name, commands);
		command_register(name, desc, args, cjs_cmd_command_proxy_);
	}

	mowgli_node_add(cjs_callback_create(cx, obj, functor), mowgli_node_create(), commands);

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

static JSBool
js_command_remove_handler_(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	const char *name;
	JSObject *functor;
	mowgli_list_t *commands;

	if(!JS_ConvertArguments(cx, argc, argv, "so", &name, &functor))
		return JS_FALSE;

	if((commands = mowgli_dictionary_retrieve(cjs_commands_, name)) != NULL) {
		mowgli_node_t *n, *tn;

		MOWGLI_LIST_FOREACH_SAFE(n, tn, commands->head) {
			cjs_callback_t *cb = (cjs_callback_t *)n->data;

			if(cb->context == cx && cb->functor == functor) {
				cjs_callback_free(cb);
				mowgli_node_delete(n, commands);
				mowgli_node_free(n);
			}
		}

		if(MOWGLI_LIST_LENGTH(commands) == 0) {
			command_remove_handler(name, cjs_cmd_command_proxy_);
			mowgli_dictionary_delete(cjs_commands_, name);
			mowgli_list_free(commands);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/* Plugin initialization/finalization routines. */
gboolean
cjs_commands_initialize(JSRuntime *rt)
{
	cjs_commands_ = mowgli_dictionary_create(g_ascii_strcasecmp);

	return TRUE;
}

static void
cjs_commands_destroy_cb_(mowgli_dictionary_elem_t *e, void *privdata)
{
	mowgli_node_t *n, *tn;
	mowgli_list_t *commands = (mowgli_list_t *)e->data;

	MOWGLI_LIST_FOREACH_SAFE(n, tn, commands->head) {
		cjs_callback_free((cjs_callback_t *)n->data);
		mowgli_node_delete(n, commands);
		mowgli_node_free(n);
	}

	command_remove_handler(e->key, cjs_cmd_command_proxy_);

	mowgli_list_free(commands);
}

gboolean
cjs_commands_finalize(JSRuntime *rt)
{
	mowgli_dictionary_destroy(cjs_commands_, cjs_commands_destroy_cb_, NULL);

	return TRUE;
}

/* Script initialization/finalization routines. */
static JSFunctionSpec js_command_functions_[] = {
	JS_FS("command_register",		js_command_register_,		4,	0,	0),
	JS_FS("command_remove_handler",	js_command_remove_handler_,	2,	0,	0),
	JS_FS_END,
};

gboolean
cjs_script_commands_initialize(JSContext *cx, JSObject *global)
{
	return JS_DefineFunctions(cx, global, js_command_functions_);
}

gboolean
cjs_script_commands_finalize(JSContext *cx)
{
	/* Remove any associated commands. */
	mowgli_list_t *commands;
	mowgli_dictionary_iteration_state_t state;

	MOWGLI_DICTIONARY_FOREACH(commands, &state, cjs_commands_) {
		mowgli_node_t *n, *tn;

		MOWGLI_LIST_FOREACH_SAFE(n, tn, commands->head) {
			cjs_callback_t *cb = (cjs_callback_t *)n->data;

			if(cb->context == cx) {
				cjs_callback_free(cb);
				mowgli_node_delete(n, commands);
				mowgli_node_free(n);
			}
		}

		if(MOWGLI_LIST_LENGTH(commands) == 0) {
			command_remove_handler(state.cur->key, cjs_cmd_command_proxy_);
			mowgli_dictionary_delete(cjs_commands_, state.cur->key);
			mowgli_list_free(commands);
		}
	}

	return TRUE;
}
