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

#include "javascript.h"

enum ConspireSession_propids {
	ConspireSession_PROP_LASTNICK = -1,
	ConspireSession_PROP_TYPE = -2,
};

static JSBool
ConspireSession_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	session *sess;
	JSObject *proto = JS_GetPrototype(cx, obj);

	if(!JSVAL_IS_INT(id))
		return JS_TRUE;
	if(!proto || JS_GET_CLASS(cx, proto) != &ConspireSession_class)
		return JS_TRUE;

	sess = (session *)JS_GetPrivate(cx, obj);
	if(sess) {
		switch(JSVAL_TO_INT(id)) {
		case ConspireSession_PROP_LASTNICK:
			{
				JSString *lastnick = JS_NewStringCopyZ(cx, sess->lastnick);
				*vp = STRING_TO_JSVAL(lastnick);
			}
			break;
		case ConspireSession_PROP_TYPE:
			{
				*vp = INT_TO_JSVAL(sess->type);
			}
			break;
		}
	}

	return JS_TRUE;
}

JSClass ConspireSession_class = {
	"ConspireSession", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub, JS_PropertyStub, ConspireSession_getProperty, JS_PropertyStub,
	JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSPropertySpec ConspireSession_props[] = {
	{ "lastnick",	ConspireSession_PROP_LASTNICK,	JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT,	NULL,	NULL },
	{ "type",		ConspireSession_PROP_TYPE,		JSPROP_READONLY | JSPROP_SHARED | JSPROP_PERMANENT,	NULL,	NULL },
	{ NULL,			0,								0,													NULL,	NULL },
};

JSObject *
cjs_session_initialize(JSContext *cx, JSObject *global)
{
	/* XXX: For now this (mostly) is just a stub. --impl */
	return JS_InitClass(cx, global, NULL, &ConspireSession_class, NULL, 0, ConspireSession_props, NULL, NULL, NULL);
}
