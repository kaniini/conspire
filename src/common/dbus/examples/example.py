#! /usr/bin/python

import dbus

bus = dbus.SessionBus()
proxy = bus.get_object('org.atheme.conspire', '/')
conspire = dbus.Interface(proxy, 'org.atheme.conspire')
path = conspire.Connect ("example.py",
		         "Python example",
		         "Example of a D-Bus client written in python",
		         "1.0")

channels = conspire.ListGet ("channels")
while conspire.ListNext (channels):
	name = conspire.ListStr (channels, "channel")
	print "------- " + name + " -------"
	conspire.SetContext (xchat.ListInt (channels, "context"))
	conspire.EmitPrint ("Channel Message", ["John", "Hi there", "@"])
	users = conspire.ListGet ("users")
	while conspire.ListNext (users):
		print "Nick: " + xchat.ListStr (users, "nick")
	conspire.ListFree (users)
conspire.ListFree (channels)

print conspire.Strip ("\00312Blue\003 \002Bold!\002", -1, 1|2)

