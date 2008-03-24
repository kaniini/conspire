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

#include "signal_factory.h"

static mowgli_dictionary_t *signal_dict_ = NULL;
static Signal *current_sig_ = NULL;

static Signal *
signal_get(const gchar *signal, gboolean allocate)
{
	Signal *sig;

	if (signal_dict_ == NULL)
		signal_dict_ = mowgli_dictionary_create(g_ascii_strcasecmp);

	if ((sig = mowgli_dictionary_retrieve(signal_dict_, signal)) != NULL)
		return sig;

	if (allocate == FALSE)
		return NULL;

	sig = g_slice_new0(Signal);
	sig->name = signal;
	mowgli_dictionary_add(signal_dict_, signal, sig);

	return sig;
}

void
signal_attach(const gchar *signal, SignalHandler hdl)
{
	Signal *sig;

	sig = signal_get(signal, TRUE);

	g_assert(sig != NULL);

	sig->handlers = g_list_append(sig->handlers, hdl);
}

void
signal_attach_head(const gchar *signal, SignalHandler hdl)
{
	Signal *sig;

	sig = signal_get(signal, TRUE);

	g_assert(sig != NULL);

	sig->handlers = g_list_prepend(sig->handlers, hdl);
}

gint
signal_emit(const gchar *signal, int params, ...)
{
	gint i;
	Signal *sig;
	va_list va;
	GList *node;

	sig = signal_get(signal, FALSE);

	if (sig == NULL)
		return 0;

	sig->values = g_new0(gpointer, params);

	current_sig_ = sig;

	va_start(va, params);

	for (i = 0; i < params; i++)
		sig->values[i] = va_arg(va, gpointer);

	va_end(va);

	for (i = 0, node = sig->handlers; node != NULL && sig->stop == FALSE; node = node->next, i++)
	{
		SignalHandler hdl = (SignalHandler) node->data;
		hdl(sig->values);
	}

	sig->stop = FALSE;
	g_free(sig->values);
	current_sig_ = NULL;

	return i;
}

void
signal_continue(int params, ...)
{
	gint i;
	Signal *sig = current_sig_;
	va_list va;

	if (sig == NULL)
		return;

	g_free(sig->values);
	sig->values = g_new0(gpointer, params);

	va_start(va, params);

	for (i = 0; i < params; i++)
		sig->values[i] = va_arg(va, gpointer);

	va_end(va);
}

void
signal_stop(const gchar *signal)
{
	Signal *sig;

	sig = signal_get(signal, FALSE);

	if (sig == NULL)
		return;

	sig->stop++;
}

const gchar *
signal_get_current_name(void)
{
	Signal *sig = current_sig_;

	if (sig == NULL)
		return "<none>";

	return sig->name;
}

void
signal_disconnect(const gchar *signal, SignalHandler hdl)
{
	Signal *sig;

	sig = signal_get(signal, FALSE);
	if (sig == NULL)
		return;

	sig->handlers = g_list_remove(sig->handlers, hdl);

	if (sig->handlers == NULL)
	{
		mowgli_dictionary_delete(signal_dict_, signal);
		g_slice_free(Signal, sig);
	}
}
