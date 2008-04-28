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

#include "xchat.h"
#include "xchatc.h"
#include "upnp.h"
#include "text.h"

#include "upnp/miniwget.h"
#include "upnp/miniupnpc.h"
#include "upnp/upnpcommands.h"

static struct UPNPUrls urls = {};
static struct IGDdatas data = {};

void
upnp_init(void)
{
	struct UPNPDev *devlist;
	struct UPNPDev *dev;
	char *descXML;
	int descXMLsize = 0;

	memset(&urls, 0, sizeof(struct UPNPUrls));
	memset(&data, 0, sizeof(struct IGDdatas));

	g_print("I: initializing UPNP support, please wait...\n");
	devlist = upnpDiscover(2000, NULL, NULL);

	if (devlist)
	{
		dev = devlist;
		while (dev)
		{
			if (strstr (dev->st, "InternetGatewayDevice"))
				break;
			dev = dev->pNext;
		}
		if (!dev)
			dev = devlist; /* defaulting to first device */

		g_print("I: Found UPNP device [desc='%s' st='%s']\n", dev->descURL, dev->st);

		descXML = miniwget(dev->descURL, &descXMLsize);
		if (descXML)
		{
			parserootdesc (descXML, descXMLsize, &data);
			free (descXML); descXML = 0;
			GetUPNPUrls (&urls, &data, dev->descURL);
		}
		freeUPNPDevlist(devlist);
	}

	g_print("I: UPNP initialization completed.\n");
}

void
upnp_add_redir(const char * addr, int port)
{
	gchar port_str[16];

	if(!urls.controlURL)
		return;

	g_snprintf(port_str, 16, "%d", port);

	if (UPNP_AddPortMapping(urls.controlURL, data.servicetype,
	                        port_str, port_str, addr, 0, "TCP"))
	{
		g_print("warning: AddPortMapping(%s, %s, %s) failed\n", port_str, port_str, addr);
	}
}

void
upnp_rem_redir(int port)
{
	gchar port_str[16];

	if(!urls.controlURL)
		return;

	g_snprintf(port_str, 16, "%d", port);
	UPNP_DeletePortMapping(urls.controlURL, data.servicetype, port_str, "TCP");
}
