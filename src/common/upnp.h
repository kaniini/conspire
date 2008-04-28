/* Conspire
 * Copyright (C) 2008 William Pitcock
 * Portions by Thomas Bernard <http://miniupnp.free.fr>
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

#ifndef __CONSPIRE__LIBCONSPIRE__UPNP_H__GUARD
#define __CONSPIRE__LIBCONSPIRE__UPNP_H__GUARD

void upnp_init(void);
void upnp_add_redir(const char * addr, int port);
void upnp_rem_redir(int port);

#endif
