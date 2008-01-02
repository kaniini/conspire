/* X-Chat
 * Copyright (C) 1998 Peter Zelezny.
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

#define GTK_DISABLE_DEPRECATED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fe-gtk.h"
#include "../common/xchat.h"
#include "../common/fe.h"

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <gtk/gtkstock.h>

GdkPixbuf *pix_conspire;
GdkPixbuf *pix_book;

GdkPixbuf *pix_purple;
GdkPixbuf *pix_red;
GdkPixbuf *pix_op;
GdkPixbuf *pix_hop;
GdkPixbuf *pix_voice;

GdkPixbuf *pix_tray_blank;
GdkPixbuf *pix_tray_file;

GdkPixbuf *pix_channel;
GdkPixbuf *pix_dialog;
GdkPixbuf *pix_server;
GdkPixbuf *pix_util;


static GdkPixmap *
pixmap_load_from_file_real (char *file)
{
	GdkPixbuf *img;
	GdkPixmap *pixmap;

	img = gdk_pixbuf_new_from_file (file, 0);
	if (!img)
		return NULL;
	gdk_pixbuf_render_pixmap_and_mask (img, &pixmap, NULL, 128);
	gdk_pixbuf_unref (img);

	return pixmap;
}

GdkPixmap *
pixmap_load_from_file (char *filename)
{
	char buf[256];
	GdkPixmap *pix;

	if (!filename)
		return NULL;

	pix = pixmap_load_from_file_real (filename);
	if (pix == NULL)
	{
		strcpy (buf, "Cannot open:\n\n");
		strncpy (buf + 14, filename, sizeof (buf) - 14);
		buf[sizeof (buf) - 1] = 0;
		fe_message (buf, FE_MSG_ERROR);
	}

	return pix;
}

void
pixmaps_init (void)
{
	pix_book = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/book.png", NULL);
	pix_conspire = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/conspire.png", NULL);

	pix_hop = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/hop.png", NULL);
	pix_purple = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/purple.png", NULL);
	pix_red = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/red.png", NULL);
	pix_op = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/op.png", NULL);
	pix_voice = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/voice.png", NULL);

	pix_tray_blank = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/balloon.png", NULL);
	pix_tray_file = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/fileoffer.png", NULL);

	pix_channel = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/channel.png", NULL);
	pix_dialog = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/dialog.png", NULL);
	pix_server = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/server.png", NULL);
	pix_util = gdk_pixbuf_new_from_file (SHAREDIR "/conspire/pixmaps/util.png", NULL);
}
