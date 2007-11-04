/*
 * Conspire
 * Copyright (C) 2007 William Pitcock, Kiyoshi Aman
 *
 * xchatemu: Translate xchat API calls into DBus calls.
 *
 * Based on:
 *    example.c - program to demonstrate some D-BUS stuffs.
 *    Copyright (C) 2006 Claessens Xavier
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "../../config.h"
#include <dbus/dbus-glib.h>
#include <stdlib.h>
#include "marshallers.h"

#include <glib.h>

#define DBUS_SERVICE "org.atheme.conspire"
#define DBUS_REMOTE "/org/atheme/conspire"
#define DBUS_REMOTE_INTERFACE "org.atheme.conspire"

guint command_id;
guint server_id;

static void
write_error (char *message,
	     GError **error)
{
	if (error == NULL || *error == NULL) {
		return;
	}
	g_printerr ("%s: %s\n", message, (*error)->message);
	g_clear_error (error);
}

typedef enum {
	XCHAT_HOOK_SERVER,
	XCHAT_HOOK_PRINT,
	XCHAT_HOOK_COMMAND,
	XCHAT_HOOK_TIMER
} xchat_hook_type;

struct xchat_hook_cb {
	guint hook_id;
	xchat_hook_type hook_type;
	gpointer userdata;
	void (*callback)(char *word[], char *word_eol[], gpointer);
	void (*pcallback)(char *word[], gpointer);
};

/* peter zelezny sucks. */
char **
xchat_revectorize(char **pvector)
{
	int size;
	char **out;

	for (size = 1; pvector[size - 1] != NULL; size++);

	out = g_new0(char *, size + 2);

	out[0] = "";
	for (size = 1; pvector[size - 1] != NULL; size++)
		out[size] = pvector[size - 1];
	out[size] = "";

	return out;
}

GList *server_hooks = NULL;

static void
server_signal_cb (DBusGProxy *proxy,
		char *word[],
		char *word_eol[],
		guint hook_id,
		guint context_id,
		gpointer user_data)
{
	GList *iter;

	word = xchat_revectorize(word);
	word_eol = xchat_revectorize(word_eol);

	for (iter = server_hooks; iter != NULL; iter = iter->next)
	{
		struct xchat_hook_cb *hook = iter->data;

		if (hook_id == hook->hook_id)
			hook->callback(word, word_eol, hook->userdata);
	}

	g_free(word);
	g_free(word_eol);
}

GList *print_hooks = NULL;

static void
print_signal_cb (DBusGProxy *proxy,
		char *word[],
		guint hook_id,
		guint context_id,
		gpointer user_data)
{
	GList *iter;

	word = xchat_revectorize(word);

	for (iter = print_hooks; iter != NULL; iter = iter->next)
	{
		struct xchat_hook_cb *hook = iter->data;

		if (hook_id == hook->hook_id)
			hook->pcallback(word, hook->userdata);
	}

	g_free(word);
}

GList *command_hooks = NULL;

static void
command_signal_cb (DBusGProxy *proxy,
		 char *word[],
		 char *word_eol[],
		 guint hook_id,
		 guint context_id,
		 gpointer user_data)
{
	GList *iter;

	word = xchat_revectorize(word);
	word_eol = xchat_revectorize(word_eol);

	for (iter = command_hooks; iter != NULL; iter = iter->next)
	{
		struct xchat_hook_cb *hook = iter->data;

		if (hook_id == hook->hook_id)
			hook->callback(word, word_eol, hook->userdata);
	}

	g_free(word);
	g_free(word_eol);
}

static void
unload_signal_cb (void)
{
	g_print ("conspire-xchatwrap: Wrapped plugin has been unloaded.\n");
	exit (EXIT_SUCCESS);
}

DBusGProxy *remote_object;

void
xchatwrap_connect(void)
{
	DBusGConnection *connection;
	GError *error = NULL;

	connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);
	if (connection == NULL) {
		write_error ("Couldn't connect to session bus", &error);
		exit(EXIT_FAILURE);
	}
  
	remote_object = dbus_g_proxy_new_for_name (connection,
						   DBUS_SERVICE,
						   DBUS_REMOTE,
						   DBUS_REMOTE_INTERFACE);

	dbus_g_proxy_add_signal (remote_object, "CommandSignal",
				 G_TYPE_STRV,
				 G_TYPE_STRV,
				 G_TYPE_UINT,
				 G_TYPE_UINT,
				 G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (remote_object, "CommandSignal",
				     G_CALLBACK (command_signal_cb),
				     NULL, NULL);

	dbus_g_proxy_add_signal (remote_object, "ServerSignal",
				 G_TYPE_STRV,
				 G_TYPE_STRV,
				 G_TYPE_UINT,
				 G_TYPE_UINT,
				 G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (remote_object, "ServerSignal",
				     G_CALLBACK (server_signal_cb),
				     NULL, NULL);

	dbus_g_proxy_add_signal (remote_object, "PrintSignal",
				 G_TYPE_STRV,
				 G_TYPE_UINT,
				 G_TYPE_UINT,
				 G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (remote_object, "PrintSignal",
				     G_CALLBACK (print_signal_cb),
				     NULL, NULL);

	dbus_g_proxy_add_signal (remote_object, "UnloadSignal",
				 G_TYPE_INVALID);
	dbus_g_proxy_connect_signal (remote_object, "UnloadSignal",
				     G_CALLBACK (unload_signal_cb),
				     NULL, NULL);
}

gpointer
xchat_plugingui_add(gpointer unused,
	const gchar *filename,
	const gchar *name,
	const gchar *desc,
	const gchar *version,
	gchar *reserved)
{
	GError *error = NULL;
	gchar *path;

	if (!dbus_g_proxy_call (remote_object, "Connect",
				&error,
				G_TYPE_STRING, filename,
				G_TYPE_STRING, name,
				G_TYPE_STRING, desc,
				G_TYPE_STRING, version,
				G_TYPE_INVALID,
				G_TYPE_STRING, &path, G_TYPE_INVALID)) {
		write_error ("Failed to complete Connect", &error);
		exit(EXIT_FAILURE);
	}

	return remote_object;
}

/* FIXME: STUB */
void
xchat_plugingui_remove(gpointer ph,
	gpointer handle)
{
}

typedef struct xchat_hook_cb xchat_hook;

xchat_hook *
xchat_hook_command(gpointer ph, const char *name, int pri,
		void (*callback)(char *word[], char *word_eol[], void *user_data),
		const char *help_text,
		void *userdata)
{
	xchat_hook *hook = g_new0(xchat_hook, 1);
	GError *error = NULL;

	if (!*name) return NULL;

	if (!dbus_g_proxy_call (remote_object, "HookCommand",
				&error,
				G_TYPE_STRING, name,
				G_TYPE_INT, pri,
				G_TYPE_STRING, help_text,
				G_TYPE_INT, 1, G_TYPE_INVALID,
				G_TYPE_UINT, &hook->hook_id, G_TYPE_INVALID)) {
		write_error ("Failed to complete HookCommand", &error);
		exit(EXIT_FAILURE);
	}

	hook->callback = callback;
	hook->userdata = userdata;
	hook->hook_type = XCHAT_HOOK_COMMAND;

	command_hooks = g_list_append(command_hooks, hook);

	return hook;
}

xchat_hook *
xchat_hook_server(gpointer ph, const char *name, int pri,
		void (*callback)(char *word[], char *word_eol[], void *user_data),
		void *userdata)
{
	xchat_hook *hook = g_new0(xchat_hook, 1);
	GError *error = NULL;

	if (!*name) return NULL;

	if (!dbus_g_proxy_call (remote_object, "HookServer",
				&error,
				G_TYPE_STRING, name,
				G_TYPE_INT, pri,
				G_TYPE_INT, 1, G_TYPE_INVALID,
				G_TYPE_UINT, &hook->hook_id, G_TYPE_INVALID)) {
		write_error ("Failed to complete HookServer", &error);
		exit(EXIT_FAILURE);
	}

	hook->callback = callback;
	hook->userdata = userdata;
	hook->hook_type = XCHAT_HOOK_SERVER;

	server_hooks = g_list_append(server_hooks, hook);

	return hook;
}

xchat_hook *
xchat_hook_print(gpointer ph, const char *name, int pri,
		void (*callback)(char *word[], void *user_data),
		void *userdata)
{
	xchat_hook *hook = g_new0(xchat_hook, 1);
	GError *error = NULL;

	if (!*name) return NULL;

	if (!dbus_g_proxy_call (remote_object, "HookPrint",
				&error,
				G_TYPE_STRING, name,
				G_TYPE_INT, pri,
				G_TYPE_INT, 1, G_TYPE_INVALID,
				G_TYPE_UINT, &hook->hook_id, G_TYPE_INVALID)) {
		write_error ("Failed to complete HookPrint", &error);
		exit(EXIT_FAILURE);
	}

	hook->pcallback = callback;
	hook->userdata = userdata;
	hook->hook_type = XCHAT_HOOK_PRINT;

	print_hooks = g_list_append(print_hooks, hook);

	return hook;
}

xchat_hook *
xchat_hook_timer(gpointer ph,
	int timeout,
	int (*callback)(void *user_data),
	void *userdata)
{
	xchat_hook *hook = g_new0(xchat_hook, 1);

	hook->hook_id = g_timeout_add(timeout, callback, userdata);
	hook->hook_type = XCHAT_HOOK_TIMER;

	return hook;
}

void
xchat_unhook(gpointer ph, xchat_hook *hook)
{
	GError *error = NULL;

	if (hook->hook_type == XCHAT_HOOK_COMMAND)
		command_hooks = g_list_remove(command_hooks, hook);	
	else if (hook->hook_type == XCHAT_HOOK_SERVER)
		server_hooks = g_list_remove(server_hooks, hook);
	else if (hook->hook_type == XCHAT_HOOK_PRINT)
		print_hooks = g_list_remove(print_hooks, hook);
	else if (hook->hook_type == XCHAT_HOOK_TIMER)
	{
		g_source_remove(hook->hook_id);
		return;	
	}

	if (!dbus_g_proxy_call (remote_object, "Unhook",
				&error,
				G_TYPE_UINT, hook->hook_id,
				G_TYPE_INVALID)) {
		write_error ("Failed to complete Unhook", &error);
		exit(EXIT_FAILURE);
	}

	g_free(hook);
}

void
xchat_print(gpointer ph, const char *text)
{
	GError *error = NULL;

	dbus_g_proxy_call (remote_object, "Print",
				&error,
				G_TYPE_STRING, text, G_TYPE_INVALID);
}

void
xchat_printf(gpointer ph, const char *text, ...)
{
	va_list va;
	gchar *str;

	va_start(va, text);
	str = g_strdup_vprintf(text, va);
	va_end(va);

	xchat_print(ph, str);
	g_free(str);
}

void
xchat_command(gpointer ph, const char *text)
{
	GError *error = NULL;

	dbus_g_proxy_call (remote_object, "Command",
				&error,
				G_TYPE_STRING, text,
				G_TYPE_INVALID);
}

void
xchat_commandf(gpointer ph, const char *text, ...)
{
	va_list va;
	gchar *str;

	va_start(va, text);
	str = g_strdup_vprintf(text, va);
	va_end(va);

	xchat_command(ph, str);
	g_free(str);
}

void
xchat_free(gpointer ph, gpointer ptr)
{
	g_free(ptr);
}

typedef struct _xchat_context {
	guint context_id;
} xchat_context;

xchat_context *
xchat_get_context(gpointer ph)
{
	xchat_context *ctx = g_new0(xchat_context, 1);
	GError *error = NULL;

	if (!dbus_g_proxy_call (remote_object, "GetContext",
				&error,
				G_TYPE_INVALID,
				G_TYPE_UINT, &ctx->context_id, 
				G_TYPE_INVALID)) {
		write_error ("Failed to complete GetContext", &error);
		exit(EXIT_FAILURE);
	}

	return ctx;
}

int
xchat_set_context(gpointer ph, xchat_context *ctx)
{
	GError *error = NULL;

	dbus_g_proxy_call (remote_object, "SetContext",
				&error,
				G_TYPE_UINT, &ctx->context_id, 
				G_TYPE_INVALID, G_TYPE_INVALID);

	return ctx->context_id;
}

int
xchat_nickcmp(gpointer ph, gchar *s1, gchar *s2)
{
	gint ret;
	GError *error = NULL;

	dbus_g_proxy_call (remote_object, "Nickcmp",
				&error,
				G_TYPE_STRING, s1, 
				G_TYPE_STRING, s2, 
				G_TYPE_INVALID,
				G_TYPE_INT, &ret,
				G_TYPE_INVALID);

	return ret;
}

xchat_context *
xchat_find_context(gpointer ph,
	const char *servname,
	const char *channel)
{
	xchat_context *ctx = g_new0(xchat_context, 1);
	GError *error = NULL;

	if (!dbus_g_proxy_call (remote_object, "FindContext",
				&error,
				G_TYPE_STRING, servname,
				G_TYPE_STRING, channel,
				G_TYPE_INVALID,
				G_TYPE_UINT, &ctx->context_id, 
				G_TYPE_INVALID)) {
		write_error ("Failed to complete FindContext", &error);
		exit(EXIT_FAILURE);
	}

	return ctx;
}

const char *
xchat_get_info(gpointer *ph, const char *id)
{
	char *out;
	GError *error = NULL;

	if (!dbus_g_proxy_call (remote_object, "GetInfo",
				&error,
				G_TYPE_STRING, id,
				G_TYPE_INVALID,
				G_TYPE_STRING, &out, 
				G_TYPE_INVALID)) {
		write_error ("Failed to complete GetInfo", &error);
		exit(EXIT_FAILURE);
	}

	return out;
}

typedef struct {
	guint list_id;
} xchat_list;

xchat_list *
xchat_list_get(gpointer ph, const char *name)
{
	xchat_list *out = g_new0(xchat_list, 1);
	GError *error = NULL;

	if (!dbus_g_proxy_call (remote_object, "ListGet",
				&error,
				G_TYPE_STRING, name,
				G_TYPE_INVALID,
				G_TYPE_UINT, &out->list_id, 
				G_TYPE_INVALID)) {
		write_error ("Failed to complete ListGet", &error);
		exit(EXIT_FAILURE);
	}

	return out;	
}

void
xchat_list_free(gpointer ph, xchat_list *list)
{
	GError *error = NULL;

	if (!dbus_g_proxy_call (remote_object, "ListFree",
				&error,
				G_TYPE_UINT, list->list_id,
				G_TYPE_INVALID,
				G_TYPE_INVALID)) {
		write_error ("Failed to complete ListFree", &error);
		exit(EXIT_FAILURE);
	}

	g_free(list);
}

gboolean
xchat_list_next(gpointer ph, xchat_list *list)
{
	gboolean ret = FALSE;
	GError *error = NULL;

	if (!dbus_g_proxy_call (remote_object, "ListNext",
				&error,
				G_TYPE_UINT, list->list_id,
				G_TYPE_INVALID,
				G_TYPE_BOOLEAN, &ret,
				G_TYPE_INVALID)) {
		write_error ("Failed to complete ListNext", &error);
		exit(EXIT_FAILURE);
	}

	return ret;
}

const gchar *
xchat_list_str(gpointer ph, xchat_list *list)
{
	gchar *ret;
	GError *error = NULL;

	if (!dbus_g_proxy_call (remote_object, "ListStr",
				&error,
				G_TYPE_UINT, list->list_id,
				G_TYPE_INVALID,
				G_TYPE_STRING, &ret,
				G_TYPE_INVALID)) {
		write_error ("Failed to complete ListStr", &error);
		exit(EXIT_FAILURE);
	}

	return ret;
}

gint
xchat_list_int(gpointer ph, xchat_list *list)
{
	gint ret;
	GError *error = NULL;

	if (!dbus_g_proxy_call (remote_object, "ListInt",
				&error,
				G_TYPE_UINT, list->list_id,
				G_TYPE_INVALID,
				G_TYPE_INT, &ret,
				G_TYPE_INVALID)) {
		write_error ("Failed to complete ListInt", &error);
		exit(EXIT_FAILURE);
	}

	return ret;
}

void
xchatemu_init (void)
{
	g_type_init ();

	dbus_g_object_register_marshaller (
		g_cclosure_user_marshal_VOID__POINTER_POINTER_UINT_UINT,
		G_TYPE_NONE,
		G_TYPE_STRV, G_TYPE_STRV, G_TYPE_UINT, G_TYPE_UINT,
		G_TYPE_INVALID);

	dbus_g_object_register_marshaller (
		g_cclosure_user_marshal_VOID__POINTER_UINT_UINT,
		G_TYPE_NONE,
		G_TYPE_STRV, G_TYPE_UINT, G_TYPE_UINT,
		G_TYPE_INVALID);

	dbus_g_object_register_marshaller (
		g_cclosure_marshal_VOID__VOID,
		G_TYPE_NONE,
		G_TYPE_INVALID);

	xchatwrap_connect();

	g_print("XChat API emulation is very incomplete. Expect bugs.\n");
}
