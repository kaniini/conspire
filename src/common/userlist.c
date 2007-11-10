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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xchat.h"
#include "modes.h"
#include "fe.h"
#include "notify.h"
#include "xchatc.h"
#include "util.h"

#if 0
static int
nick_cmp_az_ops (server *serv, struct User *user1, struct User *user2)
{
	unsigned int access1 = user1->access;
	unsigned int access2 = user2->access;
	int pos;

	if (access1 != access2)
	{
		for (pos = 0; pos < USERACCESS_SIZE; pos++)
		{
			if ((access1&(1<<pos)) && (access2&(1<<pos)))
				break;
			if ((access1&(1<<pos)) && !(access2&(1<<pos)))
				return -1;
			if (!(access1&(1<<pos)) && (access2&(1<<pos)))
				return 1;
		}
	}

	return serv->p_cmp (user1->nick, user2->nick);
}
#endif

/*
 insert name in appropriate place in linked list. Returns row number or:
  -1: duplicate
*/

static gboolean
userlist_insertname (session *sess, struct User *newuser)
{
	/* comparator may have changed due to CASEMAPPING, so set a new one. --nenolod */
	if (!sess->userdict)
		sess->userdict = mowgli_dictionary_create(sess->server->p_cmp);
	else
		mowgli_dictionary_set_comparator_func(sess->userdict, sess->server->p_cmp);

	return mowgli_dictionary_add(sess->userdict, newuser->nick, newuser) != NULL;
}

void
userlist_set_away (struct session *sess, char *nick, unsigned int away)
{
	struct User *user;

	user = userlist_find (sess, nick);
	if (user)
	{
		if (user->away != away)
		{
			user->away = away;
			/* rehash GUI */
			fe_userlist_rehash (sess, user);
		}
	}
}

int
userlist_add_hostname (struct session *sess, char *nick, char *hostname,
							  char *realname, char *servername, unsigned int away)
{
	struct User *user;

	user = userlist_find (sess, nick);
	if (user)
	{
		if (!user->hostname && hostname)
			user->hostname = strdup (hostname);
		if (!user->realname && realname)
			user->realname = strdup (realname);
		if (!user->servername && servername)
			user->servername = strdup (servername);

		if (prefs.showhostname_in_userlist || user->away != away)
		{
			user->away = away;
			fe_userlist_rehash (sess, user);
		}
		user->away = away;
		return 1;
	}
	return 0;
}

static void
free_user(struct User *user)
{
	if (user->realname)
		free (user->realname);
	if (user->hostname)
		free (user->hostname);
	if (user->servername)
		free (user->servername);
	free (user);
}

static void
free_user_cb(mowgli_dictionary_elem_t *elem, gpointer data)
{
	struct User *user = (struct User *) elem->data;

	free_user(user);
}

void
userlist_free (session *sess)
{
	mowgli_dictionary_destroy(sess->userdict, free_user_cb, NULL);

	sess->userdict = NULL;
	sess->me = NULL;

	sess->ops = 0;
	sess->hops = 0;
	sess->voices = 0;
	sess->total = 0;
}

void
userlist_clear (session *sess)
{
	fe_userlist_clear (sess);
	userlist_free (sess);
	fe_userlist_numbers (sess);
}

struct User *
userlist_find (struct session *sess, char *name)
{
	if (sess->userdict)
		return mowgli_dictionary_retrieve(sess->userdict, name);

	return NULL;
}

struct User *
userlist_find_global (struct server *serv, char *name)
{
	struct User *user;
	session *sess;
	GSList *list = sess_list;
	while (list)
	{
		sess = (session *) list->data;
		if (sess->server == serv)
		{
			user = userlist_find (sess, name);
			if (user)
				return user;
		}
		list = list->next;
	}
	return 0;
}

static void
update_counts (session *sess, struct User *user, char prefix,
					int level, int offset)
{
	switch (prefix)
	{
	case '@':
		user->op = level;
		sess->ops += offset;
		break;
	case '%':
		user->hop = level;
		sess->hops += offset;
		break;
	case '+':
		user->voice = level;
		sess->voices += offset;
		break;
	}
}

void
userlist_update_mode (session *sess, char *name, char mode, char sign)
{
	int access;
	int offset = 0;
	int level;
	char prefix;
	struct User *user;

	user = userlist_find (sess, name);
	if (!user)
		return;

	/* which bit number is affected? */
	access = mode_access (sess->server, mode, &prefix);

	if (sign == '+')
	{
		level = TRUE;
		if (!(user->access & (1 << access)))
		{
			offset = 1;
			user->access |= (1 << access);
		}
	} else
	{
		level = FALSE;
		if (user->access & (1 << access))
		{
			offset = -1;
			user->access &= ~(1 << access);
		}
	}

	/* now what is this users highest prefix? e.g. @ for ops */
	user->prefix[0] = get_nick_prefix (sess->server, user->access);

	/* update the various counts using the CHANGED prefix only */
	update_counts (sess, user, prefix, level, offset);

	fe_userlist_rehash (sess, user);
	fe_userlist_numbers (sess);
}

int
userlist_change (struct session *sess, char *oldname, char *newname)
{
	struct User *user = userlist_find (sess, oldname);

	if (user)
	{
		mowgli_dictionary_delete(sess->userdict, oldname);

		g_strlcpy (user->nick, newname, NICKLEN);

		mowgli_dictionary_add(sess->userdict, user->nick, user);

		fe_userlist_rehash (sess, user);
		fe_userlist_numbers (sess);

		return 1;
	}

	return 0;
}

int
userlist_remove (struct session *sess, char *name)
{
	struct User *user;

	user = userlist_find (sess, name);
	if (!user)
		return FALSE;

	if (user->voice)
		sess->voices--;
	if (user->op)
		sess->ops--;
	if (user->hop)
		sess->hops--;
	sess->total--;
	fe_userlist_numbers (sess);
	fe_userlist_remove (sess, user);

	if (user == sess->me)
		sess->me = NULL;

	mowgli_dictionary_delete(sess->userdict, name);
	free_user (user);

	return TRUE;
}

void
userlist_add (struct session *sess, char *name, char *hostname)
{
	struct User *user;
	int prefix_chars;
	gboolean ret;
	unsigned int acc;

	acc = nick_access (sess->server, name, &prefix_chars);

	notify_set_online (sess->server, name + prefix_chars);

	user = malloc (sizeof (struct User));
	memset (user, 0, sizeof (struct User));

	user->access = acc;

	/* assume first char is the highest level nick prefix */
	if (prefix_chars)
		user->prefix[0] = name[0];

	/* add it to our linked list */
	if (hostname)
		user->hostname = strdup (hostname);
	g_strlcpy (user->nick, name + prefix_chars, NICKLEN);
	/* is it me? */
	if (!sess->server->p_cmp (user->nick, sess->server->nick))
		user->me = TRUE;

	ret = userlist_insertname(sess, user);
	if (ret == FALSE)
	{
		free_user(user);
		return;
	}	

	sess->total++;

	/* most ircds don't support multiple modechars infront of the nickname
      for /NAMES - though they should. */
	while (prefix_chars)
	{
		update_counts (sess, user, name[0], TRUE, 1);
		name++;
		prefix_chars--;
	}

	if (user->me)
		sess->me = user;

	fe_userlist_insert (sess, user, 0, FALSE);
	fe_userlist_numbers (sess);
}

void
userlist_rehash (session *sess)
{
	struct User *user;
	mowgli_dictionary_iteration_state_t state;

	MOWGLI_DICTIONARY_FOREACH(user, &state, sess->userdict)
		fe_userlist_rehash(sess, user);
}

GSList *
userlist_flat_list (session *sess)
{
	GSList *list = NULL;
	struct User *user;
	mowgli_dictionary_iteration_state_t state;

	MOWGLI_DICTIONARY_FOREACH(user, &state, sess->userdict)
		list = g_slist_prepend(list, user);

	return g_slist_reverse(list);
}

GList *
userlist_double_list(session *sess)
{
	GList *list = NULL;
	struct User *user;
	mowgli_dictionary_iteration_state_t state;

	MOWGLI_DICTIONARY_FOREACH(user, &state, sess->userdict)
		list = g_list_prepend(list, user);

	return list;
}
