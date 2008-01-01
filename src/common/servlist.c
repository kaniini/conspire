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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "xchat.h"
#include <glib/ghash.h>

#include "cfgfiles.h"
#include "fe.h"
#include "server.h"
#include "text.h"
#include "util.h" /* token_foreach */
#include "xchatc.h"

#include "servlist.h"

GSList *network_list = 0;

void
servlist_connect (session *sess, ircnet *net, gboolean join)
{
	ircserver *ircserv;
	GSList *list;
	char *port;
	server *serv;

	if (!sess)
		sess = new_ircwindow (NULL, NULL, SESS_SERVER, TRUE);

	serv = sess->server;

	/* connect to the currently selected Server-row */
	list = g_slist_nth (net->servlist, net->selected);
	if (!list)
		list = net->servlist;
	ircserv = list->data;

	/* incase a protocol switch is added to the servlist gui */
	server_fill_her_up (sess->server);

	if (join)
	{
		sess->willjoinchannel[0] = 0;

		if (net->autojoin)
			g_strlcpy (sess->willjoinchannel, net->autojoin,
							 sizeof (sess->willjoinchannel));
	}

	serv->password[0] = 0;
	if (net->pass)
		g_strlcpy (serv->password, net->pass, sizeof (serv->password));

	if (net->sasl_user && net->sasl_pass)
	{
		serv->sasl_user = g_strdup(net->sasl_user);
		serv->sasl_pass = g_strdup(net->sasl_pass);
	}

	if (net->flags & FLAG_USE_GLOBAL)
	{
		strcpy(serv->nick, prefs.nick1);
	} else
	{
		if (net->nick)
			strcpy(serv->nick, net->nick);
	}

	serv->dont_use_proxy = (net->flags & FLAG_USE_PROXY) ? FALSE : TRUE;

#ifdef GNUTLS
	serv->use_ssl = (net->flags & FLAG_USE_SSL) ? TRUE : FALSE;
	serv->accept_invalid_cert =
		(net->flags & FLAG_ALLOW_INVALID) ? TRUE : FALSE;
#endif

	serv->network = net;

	port = strrchr (ircserv->hostname, '/');
	if (port)
	{
		*port = 0;

		/* support "+port" to indicate SSL (like mIRC does) */
		if (port[1] == '+')
		{
#ifdef GNUTLS
			serv->use_ssl = TRUE;
			serv->accept_invalid_cert =
				(net->flags & FLAG_ALLOW_INVALID) ? TRUE : FALSE;
#endif
			serv->connect (serv, ircserv->hostname, atoi (port + 2), FALSE);
		} else
			serv->connect (serv, ircserv->hostname, atoi (port + 1), FALSE);

		*port = '/';
	} else
		serv->connect (serv, ircserv->hostname, -1, FALSE);

	server_set_encoding (serv, net->encoding);
}

int
servlist_connect_by_netname (session *sess, char *network, gboolean join)
{
	ircnet *net;
	GSList *list = network_list;

	while (list)
	{
		net = list->data;

		if (strcasecmp (net->name, network) == 0)
		{
			servlist_connect (sess, net, join);
			return 1;
		}

		list = list->next;
	}

	return 0;
}

int
servlist_have_auto (void)
{
	GSList *list = network_list;
	ircnet *net;

	while (list)
	{
		net = list->data;

		if (net->flags & FLAG_AUTO_CONNECT)
			return 1;

		list = list->next;
	}

	return 0;
}

int
servlist_auto_connect (session *sess)
{
	GSList *list = network_list;
	ircnet *net;
	int ret = 0;

	while (list)
	{
		net = list->data;

		if (net->flags & FLAG_AUTO_CONNECT)
		{
			servlist_connect (sess, net, TRUE);
			ret = 1;
		}

		list = list->next;
	}

	return 0;
}

static gint
servlist_cycle_cb (server *serv)
{
	if (serv->network)
	{
		PrintTextf (serv->server_session,
			_("Cycling to next server in %s...\n"), ((ircnet *)serv->network)->name);
		servlist_connect (serv->server_session, serv->network, TRUE);
	}

	return 0;
}

int
servlist_cycle (server *serv)
{
	ircnet *net;
	int max, del;

	net = serv->network;
	if (net)
	{
		max = g_slist_length (net->servlist);
		if (max > 0)
		{
			/* try the next server, if that option is on */
			if (net->flags & FLAG_CYCLE)
			{
				net->selected++;
				if (net->selected >= max)
					net->selected = 0;
			}

			del = prefs.recon_delay * 1000;
			if (del < 1000)
				del = 500;				  /* so it doesn't block the gui */

			if (del)
				serv->recondelay_tag = g_timeout_add (del, (GSourceFunc) servlist_cycle_cb, serv);
			else
				servlist_connect (serv->server_session, net, TRUE);

			return TRUE;
		}
	}

	return FALSE;
}

ircserver *
servlist_server_find (ircnet *net, char *name, int *pos)
{
	GSList *list = net->servlist;
	ircserver *serv;
	int i = 0;

	while (list)
	{
		serv = list->data;
		if (strcmp (serv->hostname, name) == 0)
		{
			if (pos)
				*pos = i;
			return serv;
		}
		i++;
		list = list->next;
	}

	return NULL;
}

/* find a network (e.g. (ircnet *) to "FreeNode") from a hostname
   (e.g. "irc.eu.freenode.net") */

ircnet *
servlist_net_find_from_server (char *server_name)
{
	GSList *list = network_list;
	GSList *slist;
	ircnet *net;
	ircserver *serv;

	while (list)
	{
		net = list->data;

		slist = net->servlist;
		while (slist)
		{
			serv = slist->data;
			if (strcasecmp (serv->hostname, server_name) == 0)
				return net;
			slist = slist->next;
		}

		list = list->next;
	}

	return NULL;
}

ircnet *
servlist_net_find (char *name, int *pos, int (*cmpfunc) (const char *, const char *))
{
	GSList *list = network_list;
	ircnet *net;
	int i = 0;

	while (list)
	{
		net = list->data;
		if (cmpfunc (net->name, name) == 0)
		{
			if (pos)
				*pos = i;
			return net;
		}
		i++;
		list = list->next;
	}

	return NULL;
}

ircserver *
servlist_server_add (ircnet *net, char *name)
{
	ircserver *serv;

	serv = malloc (sizeof (ircserver));
	memset (serv, 0, sizeof (ircserver));
	serv->hostname = strdup (name);

	net->servlist = g_slist_append (net->servlist, serv);

	return serv;
}

void
servlist_server_remove (ircnet *net, ircserver *serv)
{
	free (serv->hostname);
	free (serv);
	net->servlist = g_slist_remove (net->servlist, serv);
}

static void
servlist_server_remove_all (ircnet *net)
{
	ircserver *serv;

	while (net->servlist)
	{
		serv = net->servlist->data;
		servlist_server_remove (net, serv);
	}
}

void
servlist_net_remove (ircnet *net)
{
	GSList *list;
	server *serv;

	servlist_server_remove_all (net);
	network_list = g_slist_remove (network_list, net);

	if (net->nick)
		free (net->nick);
	if (net->nick2)
		free (net->nick2);
	if (net->user)
		free (net->user);
	if (net->real)
		free (net->real);
	if (net->pass)
		free (net->pass);
	if (net->autojoin)
		free (net->autojoin);
	if (net->command)
		free (net->command);
	if (net->nickserv)
		free (net->nickserv);
	if (net->comment)
		free (net->comment);
	if (net->encoding)
		free (net->encoding);
	if (net->sasl_user)
		free (net->sasl_user);
	if (net->sasl_pass)
		free (net->sasl_pass);

	g_free (net->name);
	g_free (net);

	/* for safety */
	list = serv_list;
	while (list)
	{
		serv = list->data;
		if (serv->network == net)
			serv->network = NULL;
		list = list->next;
	}
}

ircnet *
servlist_net_add (char *name, char *comment, int prepend)
{
	ircnet *net;

	net = g_new0(ircnet, 1);
	net->name = g_strdup (name);
/*	net->comment = strdup (comment);*/
	net->flags = FLAG_CYCLE | FLAG_USE_GLOBAL | FLAG_USE_PROXY;

	if (prepend)
		network_list = g_slist_prepend (network_list, net);
	else
		network_list = g_slist_append (network_list, net);

	return net;
}

static int
servlist_load (void)
{
	FILE *fp;
	char buf[2050];
	int len;
	char *tmp;
	ircnet *net = NULL;

	fp = xchat_fopen_file ("servlist_.conf", "r", 0);
	if (!fp)
		return FALSE;

	while (fgets (buf, sizeof (buf) - 2, fp))
	{
		len = strlen (buf);
		buf[len] = 0;
		buf[len-1] = 0;	/* remove the trailing \n */
		if (net)
		{
			switch (buf[0])
			{
			case 'a':
				net->sasl_user = strdup (buf + 2);
				break;
			case 'A':
				net->sasl_pass = strdup (buf + 2);
				break;
			case 'I':
				net->nick = strdup (buf + 2);
				break;
			case 'i':
				net->nick2 = strdup (buf + 2);
				break;
			case 'U':
				net->user = strdup (buf + 2);
				break;
			case 'R':
				net->real = strdup (buf + 2);
				break;
			case 'P':
				net->pass = strdup (buf + 2);
				break;
			case 'J':
				net->autojoin = strdup (buf + 2);
				break;
			case 'C':
				if (net->command)
				{
					/* concat extra commands with a \n separator */
					tmp = net->command;
					net->command = malloc (strlen (tmp) + strlen (buf + 2) + 2);
					strcpy (net->command, tmp);
					strcat (net->command, "\n");
					strcat (net->command, buf + 2);
					free (tmp);
				} else
					net->command = strdup (buf + 2);
				break;
			case 'F':
				net->flags = atoi (buf + 2);
				break;
			case 'D':
				net->selected = atoi (buf + 2);
				break;
			case 'E':
				net->encoding = strdup (buf + 2);
				break;
			case 'S':	/* new server/hostname for this network */
				servlist_server_add (net, buf + 2);
				break;
			case 'B':
				net->nickserv = strdup (buf + 2);
				break;
			}
		}
		if (buf[0] == 'N')
			net = servlist_net_add (buf + 2, /* comment */ NULL, FALSE);
	}
	fclose (fp);

	return TRUE;
}

void
servlist_init (void)
{
	if (!network_list)
		servlist_load();
}

/* check if a charset is known by Iconv */
int
servlist_check_encoding (char *charset)
{
	GIConv gic;
	char *c;

	c = strchr (charset, ' ');
	if (c)
		c[0] = 0;

	if (!strcasecmp (charset, "IRC")) /* special case */
	{
		if (c)
			c[0] = ' ';
		return TRUE;
	}

	gic = g_iconv_open (charset, "UTF-8");

	if (c)
		c[0] = ' ';

	if (gic != (GIConv)-1)
	{
		g_iconv_close (gic);
		return TRUE;
	}

	return FALSE;
}

static int
servlist_write_ccmd (char *str, void *fp)
{
	return fprintf (fp, "C=%s\n", (str[0] == '/') ? str + 1 : str);
}


int
servlist_save (void)
{
	FILE *fp;
	char buf[256];
	ircnet *net;
	ircserver *serv;
	GSList *list;
	GSList *hlist;
	int first = FALSE;

	snprintf (buf, sizeof (buf), "%s/servlist_.conf", get_xdir_fs ());
	if (access (buf, F_OK) != 0)
		first = TRUE;

	fp = xchat_fopen_file ("servlist_.conf", "w", 0);
	if (!fp)
		return FALSE;

	if (first)
		chmod (buf, 0600);

	fprintf (fp, "v="PACKAGE_VERSION"\n\n");

	list = network_list;
	while (list)
	{
		net = list->data;

		fprintf (fp, "N=%s\n", net->name);
		if (net->sasl_user)
			fprintf (fp, "a=%s\n", net->sasl_user);
		if (net->sasl_pass)
			fprintf (fp, "A=%s\n", net->sasl_pass);
		if (net->nick)
			fprintf (fp, "I=%s\n", net->nick);
		if (net->nick2)
			fprintf (fp, "i=%s\n", net->nick2);
		if (net->user)
			fprintf (fp, "U=%s\n", net->user);
		if (net->real)
			fprintf (fp, "R=%s\n", net->real);
		if (net->pass)
			fprintf (fp, "P=%s\n", net->pass);
		if (net->autojoin)
			fprintf (fp, "J=%s\n", net->autojoin);
		if (net->nickserv)
			fprintf (fp, "B=%s\n", net->nickserv);
		if (net->encoding && strcasecmp (net->encoding, "System") &&
			 strcasecmp (net->encoding, "System default"))
		{
			fprintf (fp, "E=%s\n", net->encoding);
			if (!servlist_check_encoding (net->encoding))
			{
				snprintf (buf, sizeof (buf), _("Warning: \"%s\" character set is unknown. No conversion will be applied for network %s."),
							 net->encoding, net->name);
				fe_message (buf, FE_MSG_WARN);
			}
		}

		if (net->command)
			token_foreach (net->command, '\n', servlist_write_ccmd, fp);

		fprintf (fp, "F=%d\nD=%d\n", net->flags, net->selected);

		hlist = net->servlist;
		while (hlist)
		{
			serv = hlist->data;
			fprintf (fp, "S=%s\n", serv->hostname);
			hlist = hlist->next;
		}

		if (fprintf (fp, "\n") < 1)
		{
			fclose (fp);
			return FALSE;
		}

		list = list->next;
	}

	fclose (fp);
	return TRUE;
}
