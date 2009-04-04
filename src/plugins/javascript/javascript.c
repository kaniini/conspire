/* javascript.c
 *
 * A javascript plugin for conspire.
 *
 * Copyright (C) 2008 William Pitcock <nenolod@dereferenced.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "javascript.h"

static JSRuntime *rt_;

struct cjs_script_
{
	JSScript *script;
	JSObject *script_object;
	JSContext *context;
	JSObject *global;
};
static mowgli_heap_t *cjs_script_heap_;
static mowgli_dictionary_t *cjs_scripts_;

static JSClass global_class = {
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

/* Error reporting. */
static void
js_report_error_(JSContext *cx, const char *message, JSErrorReport *report)
{
	PrintTextf(current_sess, "*\tJavaScript error: %s:%u:%s\n",
		report->filename ? report->filename : "<no filename>", (unsigned int) report->lineno,
		message);
}

/* Consider calling the garbage collector periodically. */
static JSBool
js_branch_callback_hook_(JSContext *cx, JSScript *script)
{
	static int counter = 0;

	if (++counter == 20) {
		JS_MaybeGC(cx);
		counter = 0;
	}

	return JS_TRUE;
}

/* Basic tools. */
static JSBool
js_print_(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	extern session *current_sess; /* XXX */
	const char *buf;

	if(!JS_ConvertArguments(cx, argc, argv, "s", &buf))
		return JS_FALSE;

	PrintText(current_sess, (char *)buf);

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

static JSBool
js_command_(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	extern session *current_sess; /* XXX */
	const char *buf;

	if(!JS_ConvertArguments(cx, argc, argv, "s", &buf))
		return JS_FALSE;

	handle_command(current_sess, (char *)buf, FALSE);

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/* Script loading and unloading. */
static struct cjs_script_ *
cjs_script_create_(JSScript *script, JSContext *context, JSObject *global)
{
	JSObject *script_object = JS_NewScriptObject(context, script);
	if(!script_object)
		return NULL;

	struct cjs_script_ *cjs_script = mowgli_heap_alloc(cjs_script_heap_);

	cjs_script->script = script;
	cjs_script->script_object = script_object;
	cjs_script->context = context;
	cjs_script->global = global;

	JS_AddNamedRoot(cjs_script->context, &cjs_script->script_object, "conspire script object");

	return cjs_script;
}

static void
cjs_script_free_(struct cjs_script_ *cjs_script)
{
	/* Clean up. */
	cjs_script_signals_finalize(cjs_script->context);
	cjs_script_commands_finalize(cjs_script->context);

	JS_RemoveRoot(cjs_script->context, &cjs_script->script_object);

	/* And destroy everything. */
	JS_DestroyContextMaybeGC(cjs_script->context);

	mowgli_heap_free(cjs_script_heap_, cjs_script);
}

static JSFunctionSpec js_global_functions_[] = {
	JS_FS("print",		js_print_,		1,	0,	0),
	JS_FS("command",	js_command_,	1,	0,	0),
	JS_FS_END
};

gboolean
cjs_script_load(session *sess, const gchar *filename)
{
	gchar *data;
	gsize len;
	GError *err = NULL;

	if (g_file_get_contents(filename, &data, &len, &err)) {
		JSContext *cx;
		JSObject *global;
		JSScript *script;

		jsval rval;

		/* Create a context. */
		cx = JS_NewContext(rt_, 8192);
		if(cx == NULL)
			return FALSE;
		JS_SetOptions(cx, JSOPTION_VAROBJFIX);
		JS_SetVersion(cx, JSVERSION_LATEST);
		JS_SetErrorReporter(cx, js_report_error_);
		JS_SetBranchCallback(cx, js_branch_callback_hook_);

		global = JS_NewObject(cx, &global_class, NULL, NULL);
		if(global == NULL)
			return FALSE;

		if(!JS_InitStandardClasses(cx, global))
			return FALSE;

		if(!cjs_session_initialize(cx, global))
			return FALSE;

		if(!JS_DefineFunctions(cx, global, js_global_functions_))
			return FALSE;

		if(!cjs_script_signals_initialize(cx, global))
			return FALSE;

		if(!cjs_script_commands_initialize(cx, global))
			return FALSE;

		/* Compile and execute the script. */
		script = JS_CompileScript(cx, global, data, len, filename, 1);
		g_free(data);

		if(script == NULL) {
			JS_DestroyContextMaybeGC(cx);

			return FALSE;
		}

		struct cjs_script_ *cjs_script = cjs_script_create_(script, cx, global);
		if(!cjs_script) {
			JS_DestroyScript(cx, script);
			JS_DestroyContextMaybeGC(cx);

			return FALSE;
		}

		if(!JS_ExecuteScript(cjs_script->context, cjs_script->global, cjs_script->script, &rval)) {
			cjs_script_free_(cjs_script);

			return FALSE;
		}

		/* Preliminary: Unload any loaded script with the same filename.
		 * XXX: Consider allowing multiple instances of a given script,
		 * provided we can make a way for users to configure the behavior
		 * or unload specific instances. --impl */
		cjs_script_unload(sess, filename);

		/* We're likely to call the GC here if we just unloaded a script. */
		JS_MaybeGC(cjs_script->context);

		/* Place the script into the list of loaded scripts. */
		mowgli_dictionary_add(cjs_scripts_, filename, cjs_script);

		return TRUE;
	} else {
		PrintTextf(sess, "*\tError: %s", err->message);
		return FALSE;
	}
}

gboolean
cjs_script_unload(session *sess, const gchar *filename)
{
	struct cjs_script_ *cjs_script;

	if((cjs_script = mowgli_dictionary_retrieve(cjs_scripts_, filename)) == NULL)
		return FALSE;

	mowgli_dictionary_delete(cjs_scripts_, filename);
	cjs_script_free_(cjs_script);

	return TRUE;
}

/* Plugin functions. */
CommandResult
cjs_cmd_scriptload(session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
	if (*word[2] == '\0') {
		PrintTextf(sess, "*\tUsage: SCRIPTLOAD <javascript-file>");
		return CMD_EXEC_FAIL;
	}

	if(!cjs_script_load(sess, word[2])) {
		PrintTextf(sess, "*\tError: Cannot load script");
		return CMD_EXEC_FAIL;
	}

	return CMD_EXEC_OK;
}

CommandResult
cjs_cmd_scriptunload(session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
	if (*word[2] == '\0') {
		PrintTextf(sess, "*\tUsage: SCRIPTUNLOAD <javascript-file>");
		return CMD_EXEC_FAIL;
	}

	if(mowgli_dictionary_find(cjs_scripts_, word[2]) == NULL) {
		PrintTextf(sess, "*\tError: No script loaded with filename '%s'", word[2]);
		return CMD_EXEC_FAIL;
	}

	if(!cjs_script_unload(sess, word[2])) {
		PrintTextf(sess, "*\tError: Cannot unload script");
		return CMD_EXEC_FAIL;
	}

	return CMD_EXEC_OK;
}

/* Script internal API. */
static mowgli_heap_t *cjs_callback_heap_ = NULL;

cjs_callback_t *cjs_callback_create(JSContext *cx, JSObject *global, JSObject *functor)
{
	cjs_callback_t *callback = mowgli_heap_alloc(cjs_callback_heap_);
	callback->context = cx;
	callback->global = global;
	callback->functor = functor;

	/* Maintain a reference to the functor. */
	JS_AddNamedRoot(callback->context, &callback->functor, "conspire callback");

	return callback;
}

void cjs_callback_free(cjs_callback_t *callback)
{
	/* Remove the root. */
	JS_RemoveRoot(callback->context, &callback->functor);

	mowgli_heap_free(cjs_callback_heap_, callback);
}

gboolean
initialize(Plugin *p)
{
	/* Initialize the runtime. */
	rt_ = JS_NewRuntime(8L * 1024L * 1024L);
	if(rt_ == NULL)
		return FALSE;

	/* Initialize callback allocation. */
	cjs_callback_heap_ = mowgli_heap_create(sizeof(cjs_callback_t), 128, BH_LAZY);

	/* Initialize the script data holders. */
	cjs_script_heap_ = mowgli_heap_create(sizeof(struct cjs_script_), 64, BH_LAZY);
	cjs_scripts_ = mowgli_dictionary_create(strcmp);

	/* Initialize components. */
	if(!cjs_signals_initialize(rt_))
		return FALSE;

	if(!cjs_commands_initialize(rt_))
		return FALSE;

	command_register("SCRIPTLOAD", "Load a JavaScript script", CMD_NO_FLAGS, cjs_cmd_scriptload);
	command_register("SCRIPTUNLOAD", "Unload a JavaScript script", CMD_NO_FLAGS, cjs_cmd_scriptunload);

	return TRUE;
}

static void
cjs_scripts_destroy_cb_(mowgli_dictionary_elem_t *e, void *privdata)
{
	cjs_script_free_((struct cjs_script_ *)e->data);
}

gboolean
finalize(Plugin *p)
{
	/* Remove all scripts. */
	mowgli_dictionary_destroy(cjs_scripts_, cjs_scripts_destroy_cb_, NULL);

	cjs_signals_finalize(rt_);
	cjs_commands_finalize(rt_);

	JS_DestroyRuntime(rt_);
	JS_ShutDown();

	command_remove_handler("SCRIPTLOAD", cjs_cmd_scriptload);
	command_remove_handler("SCRIPTUNLOAD", cjs_cmd_scriptunload);

	return TRUE;
}

PLUGIN_DECLARE("JavaScript Host", PACKAGE_VERSION,
	"Run JavaScript scripts inside your conspire instance.",
	"William Pitcock, Noah Fontes", initialize, finalize);
