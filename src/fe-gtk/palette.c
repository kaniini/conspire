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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <gtk/gtk.h>

#include "fe-gtk.h"
#include "palette.h"

#include "../common/xchat.h"
#include "../common/util.h"
#include "../common/cfgfiles.h"

#include "../libcontrast/contrast.h"

/* defaults: will probably be overridden from gtk theme. */
GdkColor colors[] = {
	/* colors for xtext */
	{0, 0xFFFF, 0xFFFF, 0xFFFF}, /* 00 white */
	{0, 0x0000, 0x0000, 0x0000}, /* 01 black */
	{0, 0x0000, 0x0000, 0xBBBB}, /* 02 blue */
	{0, 0x0000, 0xBBBB, 0x0000}, /* 03 green */
	{0, 0xFFFF, 0x5555, 0x5555}, /* 04 bright red */
	{0, 0xBBBB, 0x0000, 0x0000}, /* 05 red */
	{0, 0xBBBB, 0x0000, 0xBBBB}, /* 06 magenta */
	{0, 0x8080, 0x5555, 0x0000}, /* 07 brown */
	{0, 0xFFFF, 0xFFFF, 0x5555}, /* 08 bright yellow */
	{0, 0x5555, 0xFFFF, 0x5555}, /* 09 bright green */
	{0, 0x0000, 0xBBBB, 0xBBBB}, /* 10 cyan */
	{0, 0x5555, 0xFFFF, 0xFFFF}, /* 11 bright cyan */
	{0, 0x5555, 0x5555, 0xFFFF}, /* 12 bright blue */
	{0, 0xb0b0, 0x3737, 0xb0b0}, /* 13 bright magenta */
	{0, 0x4c4c, 0x4c4c, 0x4c4c}, /* 14 grey */
	{0, 0x9595, 0x9595, 0x9595}, /* 15 light grey */

	{0, 0xcccc, 0xcccc, 0xcccc}, /* 16 white */
	{0, 0x0000, 0x0000, 0x0000}, /* 17 black */
	{0, 0x35c2, 0x35c2, 0xb332}, /* 18 blue */
	{0, 0x2a3d, 0x8ccc, 0x2a3d}, /* 19 green */
	{0, 0xc3c3, 0x3b3b, 0x3b3b}, /* 20 red */
	{0, 0xc7c7, 0x3232, 0x3232}, /* 21 light red */
	{0, 0x8000, 0x2666, 0x7fff}, /* 22 purple */
	{0, 0x6666, 0x3636, 0x1f1f}, /* 23 orange */
	{0, 0xd999, 0xa6d3, 0x4147}, /* 24 yellow */
	{0, 0x3d70, 0xcccc, 0x3d70}, /* 25 green */
	{0, 0x199a, 0x5555, 0x5555}, /* 26 aqua */
	{0, 0x2eef, 0x8ccc, 0x74df}, /* 27 light aqua */
	{0, 0x451e, 0x451e, 0xe666}, /* 28 blue */
	{0, 0xb0b0, 0x3737, 0xb0b0}, /* 29 light purple */
	{0, 0x4c4c, 0x4c4c, 0x4c4c}, /* 30 grey */
	{0, 0x9595, 0x9595, 0x9595}, /* 31 light grey */

	{0, 0xffff, 0xffff, 0xffff}, /* 32 marktext Fore (white) */
	{0, 0x3535, 0x6e6e, 0xc1c1}, /* 33 marktext Back (blue) */
	{0, 0xBBBB, 0xBBBB, 0xBBBB}, /* 34 foreground (black) */
	{0, 0x0000, 0x0000, 0x0000}, /* 35 background (white) */
	{0, 0xcccc, 0x1010, 0x1010}, /* 36 marker line (red) */

	/* colors for GUI */
	{0, 0x9999, 0x0000, 0x0000}, /* 37 tab New Data (dark red) */
	{0, 0x0000, 0x0000, 0xffff}, /* 38 tab Nick Mentioned (blue) */
	{0, 0xffff, 0x0000, 0x0000}, /* 39 tab New Message (red) */
	{0, 0x9595, 0x9595, 0x9595}, /* 40 away user (grey) */
};
#define MAX_COL 40

void
palette_alloc (GtkWidget * widget)
{
	int i;
	static int done_alloc = FALSE;
	GdkColormap *cmap;

	if (!done_alloc)		  /* don't do it again */
	{
		done_alloc = TRUE;
		cmap = gtk_widget_get_colormap (widget);
		for (i = MAX_COL; i >= 0; i--)
			gdk_colormap_alloc_color (cmap, &colors[i], FALSE, TRUE);
	}
}

static void
optimize_palette(GdkColor background)
{
	colors[ 0] = colors[16] = contrast_render_foreground_color(background, CONTRAST_COLOR_WHITE);
	colors[ 1] = colors[17] = contrast_render_foreground_color(background, CONTRAST_COLOR_BLACK);
	colors[ 2] = colors[18] = contrast_render_foreground_color(background, CONTRAST_COLOR_BLUE);
	colors[ 3] = colors[19] = contrast_render_foreground_color(background, CONTRAST_COLOR_GREEN);
	colors[ 4] = colors[20] = contrast_render_foreground_color(background, CONTRAST_COLOR_BROWN);
	colors[ 5] = colors[21] = contrast_render_foreground_color(background, CONTRAST_COLOR_RED);
	colors[ 6] = colors[22] = contrast_render_foreground_color(background, CONTRAST_COLOR_PURPLE);
	colors[ 7] = colors[23] = contrast_render_foreground_color(background, CONTRAST_COLOR_ORANGE);
	colors[ 8] = colors[24] = contrast_render_foreground_color(background, CONTRAST_COLOR_YELLOW);
	colors[ 9] = colors[25] = contrast_render_foreground_color(background, CONTRAST_COLOR_LIGHT_GREEN);
	colors[10] = colors[26] = contrast_render_foreground_color(background, CONTRAST_COLOR_AQUA);
	colors[11] = colors[27] = contrast_render_foreground_color(background, CONTRAST_COLOR_LIGHT_BLUE);
	colors[12] = colors[28] = contrast_render_foreground_color(background, CONTRAST_COLOR_BLUE);
	colors[13] = colors[29] = contrast_render_foreground_color(background, CONTRAST_COLOR_MAGENTA);
	colors[14] = colors[30] = contrast_render_foreground_color(background, CONTRAST_COLOR_GREY);
	colors[15] = colors[31] = contrast_render_foreground_color(background, CONTRAST_COLOR_LIGHT_GREY);
}

static void
extract_theme_colors(void)
{
	GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_ensure_style(w);

	colors[32] = w->style->text[GTK_STATE_SELECTED];
	colors[33] = w->style->base[GTK_STATE_SELECTED];
	colors[34] = w->style->text[GTK_STATE_NORMAL];
	colors[35] = w->style->base[GTK_STATE_NORMAL];
	colors[36] = w->style->text[GTK_STATE_INSENSITIVE];

	gtk_widget_destroy(w);

	/* optimize mIRC colours for background */
	optimize_palette(colors[33]);
}

/* maps XChat 2.0.x colors to current */
static const int remap[] =
{
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	33,	/* 16:marktextback */
	32,	/* 17:marktextfore */
	34,	/* 18: fg */
	35,	/* 19: bg */
	37,	/* 20: newdata */
	38,	/* 21: blue */
	39,	/* 22: newmsg */
	40		/* 23: away */
};

void
palette_load (void)
{
	int i, j, l, fh, res;
	char prefname[256];
	struct stat st;
	char *cfg;
	int red, green, blue;
	int upgrade = FALSE;

#if 1
	extract_theme_colors();
#else
	fh = xchat_open_file ("colors.conf", O_RDONLY, 0, 0);
	if (fh == -1)
	{
		fh = xchat_open_file ("palette.conf", O_RDONLY, 0, 0);
		upgrade = TRUE;
	}

	if (fh != -1)
	{
		fstat (fh, &st);
		cfg = malloc (st.st_size + 1);
		if (cfg)
		{
			cfg[0] = '\0';
			l = read (fh, cfg, st.st_size);
			if (l >= 0)
				cfg[l] = '\0';

			if (!upgrade)
			{
				/* mIRC colors 0-31 are here */
				for (i = 0; i < 32; i++)
				{
					snprintf (prefname, sizeof prefname, "color_%d", i);
					cfg_get_color (cfg, prefname, &red, &green, &blue);
					colors[i].red = red;
					colors[i].green = green;
					colors[i].blue = blue;
				}

				/* our special colors are mapped at 256+ */
				for (i = 256, j = 32; j < MAX_COL+1; i++, j++)
				{
					snprintf (prefname, sizeof prefname, "color_%d", i);
					cfg_get_color (cfg, prefname, &red, &green, &blue);
					colors[j].red = red;
					colors[j].green = green;
					colors[j].blue = blue;
				}

			} else
			{
				/* loading 2.0.x palette.conf */
				for (i = 0; i < MAX_COL+1; i++)
				{
					snprintf (prefname, sizeof prefname, "color_%d_red", i);
					red = cfg_get_int (cfg, prefname);

					snprintf (prefname, sizeof prefname, "color_%d_grn", i);
					green = cfg_get_int (cfg, prefname);

					snprintf (prefname, sizeof prefname, "color_%d_blu", i);
					blue = cfg_get_int_with_result (cfg, prefname, &res);

					if (res)
					{
						colors[remap[i]].red = red;
						colors[remap[i]].green = green;
						colors[remap[i]].blue = blue;
					}
				}

				/* copy 0-15 to 16-31 */
				for (i = 0; i < 16; i++)
				{
					colors[i+16].red = colors[i].red;
					colors[i+16].green = colors[i].green;
					colors[i+16].blue = colors[i].blue;
				}
			}
			free (cfg);
		}
		close (fh);
	}
#endif
}

void
palette_save (void)
{
	int i, j, fh;
	char prefname[256];

	fh = xchat_open_file ("colors.conf", O_TRUNC | O_WRONLY | O_CREAT, 0600, XOF_DOMODE);
	if (fh != -1)
	{
		/* mIRC colors 0-31 are here */
		for (i = 0; i < 32; i++)
		{
			snprintf (prefname, sizeof prefname, "color_%d", i);
			cfg_put_color (fh, colors[i].red, colors[i].green, colors[i].blue, prefname);
		}

		/* our special colors are mapped at 256+ */
		for (i = 256, j = 32; j < MAX_COL+1; i++, j++)
		{
			snprintf (prefname, sizeof prefname, "color_%d", i);
			cfg_put_color (fh, colors[j].red, colors[j].green, colors[j].blue, prefname);
		}

		close (fh);
	}
}
