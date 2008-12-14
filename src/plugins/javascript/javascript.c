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

#define XP_UNIX			/* XXX */
#include "conspire-config.h"
#include <mowgli.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <dlfcn.h>
#include <string.h>
#include <mozjs/jsapi.h>

#include "common/plugin.h"
#include "common/xchat.h"
#include "common/server.h"
#include "common/outbound.h"
#include "common/text.h"

mowgli_list_t context_stack = {};
mowgli_dictionary_t *command_stack = NULL;

JSRuntime *rt = NULL;
JSContext *cx = NULL;
JSObject  *global = NULL;

static JSClass global_class = {
	"global", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

void
js_report_error(JSContext *cx, const char *message, JSErrorReport *report)
{
    extern session *current_sess; /* XXX */
    PrintTextf(current_sess, "*\tJavaScript error: %s:%u:%s\n",
            report->filename ? report->filename : "<no filename>",
            (unsigned int) report->lineno,
            message);
}


/*
 * Consider calling the garbage collector periodically.
 */
JSBool
js_branch_callback_hook(JSContext *cx_, JSScript *script)
{
	static int counter = 0;

	if (++counter == 20) {
		JS_MaybeGC(cx_);
		counter = 0;
	}

	return JS_TRUE;
}

/*
 * Proxy conversion for command handlers, adding commands and receiving
 * commands.
 */
CommandResult
js_proxy_command_execute(session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
	jsval rval;
	JSObject *functor;
	JSObject *argv, *argv_eol;
	jsval *functor_argv;
	void *frame_ptr;
	gint i;

	functor = mowgli_dictionary_retrieve(command_stack, word[1]);

	argv = JS_NewArrayObject(cx, 0, NULL);
	argv_eol = JS_NewArrayObject(cx, 0, NULL);
	JS_AddRoot(cx, argv);
	JS_AddRoot(cx, argv_eol);

	for (i = 1; i < PDIWORDS && word[i] != NULL; i++) {
		JSString *jsword = JS_NewString(cx, word[i], strlen(word[i]));
		JSString *jsword_eol = JS_NewString(cx, word_eol[i], strlen(word_eol[i]));

		JS_SetElement(cx, argv, i, (jsval *) jsword);
		JS_SetElement(cx, argv_eol, i, (jsval *) jsword_eol);
	}

	functor_argv = JS_PushArguments(cx, &frame_ptr, "oo", argv, argv_eol);
	JS_CallFunctionValue(cx, global, (jsval) functor, 2, functor_argv, &rval);

	JS_RemoveRoot(cx, argv);
	JS_RemoveRoot(cx, argv_eol);

	if (JSVAL_IS_INT(rval))
		return JSVAL_TO_INT(rval);
	else
		return CMD_EXEC_OK;
}

JSBool
js_command_register(JSContext *cx_, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	const char *name, *desc;
	int args;
	JSObject *functor;

	if (!JS_ConvertArguments(cx_, argc, argv, "ssio", &name, &desc, &args, &functor))
		return JS_FALSE;

	mowgli_dictionary_add(command_stack, name, functor);
	command_register(name, desc, args, js_proxy_command_execute);

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool
js_command_remove_handler(JSContext *cx_, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	const char *name;
	JSObject *functor;

	if (!JS_ConvertArguments(cx_, argc, argv, "so", &name, &functor))
		return JS_FALSE;

	mowgli_dictionary_delete(command_stack, name);
	command_remove_handler(name, js_proxy_command_execute);

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/* signals */

/* basic tools */
JSBool
js_print(JSContext *cx_, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	extern session *current_sess; /* XXX */
	const char *buf;

	if (!JS_ConvertArguments(cx_, argc, argv, "s", &buf))
		return JS_FALSE;

	PrintText(current_sess, (char *) buf);

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool
js_command(JSContext *cx_, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	extern session *current_sess; /* XXX */
	const char *buf;

	if (!JS_ConvertArguments(cx_, argc, argv, "s", &buf))
		return JS_FALSE;

	handle_command(current_sess, (char *) buf, FALSE);

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

static JSFunctionSpec js_global_functions[] = {
	JS_FS("command_register",	js_command_register,		4,	0,	0),
	JS_FS("command_remove_handler",	js_command_remove_handler,	2,	0,	0),
	JS_FS("print",			js_print,			1,	0,	0),
	JS_FS("command",		js_command,			1,	0,	0),
	JS_FS_END
};

/* plugin functions */
CommandResult
js_loadscript(session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
	jsval rval;
	JSScript *script;
	gchar *data;
	gsize len;
	GError *err = NULL;

	if (!word[2]) {
		PrintTextf(sess, "*\tUsage: /loadscript file.js");
		return CMD_EXEC_FAIL;
	}

	if (g_file_get_contents(word[2], &data, &len, &err)) {
		script = JS_CompileScript(cx, global, data, len, word[1], 1);
		if (script != NULL) {
			JS_ExecuteScript(cx, global, script, &rval);
		}

		g_free(data);
	} else {
		PrintTextf(sess, "*\tError: %s", err->message);
		return CMD_EXEC_FAIL;
	}

	return CMD_EXEC_OK;
}

gboolean
init(Plugin *p)
{
	command_stack = mowgli_dictionary_create(g_ascii_strcasecmp);

	rt = JS_NewRuntime(8L * 1024L * 1024L);
	if (rt == NULL)
		return FALSE;

	/* Create a context. */
	cx = JS_NewContext(rt, 8192);
	if (cx == NULL)
		return FALSE;
	JS_SetOptions(cx, JSOPTION_VAROBJFIX);
	JS_SetVersion(cx, JSVERSION_LATEST);
	JS_SetErrorReporter(cx, js_report_error);
	JS_SetBranchCallback(cx, js_branch_callback_hook);

	global = JS_NewObject(cx, &global_class, NULL, NULL);
	if (global == NULL)
		return FALSE;

	if (!JS_InitStandardClasses(cx, global))
		return FALSE;

	if (!JS_DefineFunctions(cx, global, js_global_functions))
		return FALSE;

	command_register("LOADSCRIPT", "Load a javascript script", 0, js_loadscript);

	return TRUE;
}

gboolean
fini(Plugin *p)
{
	JS_DestroyContextMaybeGC(cx);
	JS_DestroyRuntime(rt);
	JS_ShutDown();

	command_remove_handler("LOADSCRIPT", js_loadscript);

	return TRUE;
}

PLUGIN_DECLARE("Javascript Host", PACKAGE_VERSION,
	"Run javascript scripts inside your conspire instance.",
	"William Pitcock", init, fini);
