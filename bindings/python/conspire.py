import gobject
import dbus
from dbus.mainloop.glib import DBusGMainLoop

class conspire:
	def command_signal_handler():

	def __init__(self):
		DBusGMainLoop(set_as_default=True)
		self.bus = dbus.SessionBus()
		self.proxy = bus.get_object('org.atheme.conspire', '/org/atheme/conspire')
		self.interface = dbus.Interface(proxy, 'org.atheme.conspire')


	cmd_sinks = []
	cmd_pads = []

	def register_command_pad(word, callback, help_text="Undocumented"):
		cpad = {"cmd": word, "callback": callback}
		self.cmd_pads.append(cpad)
		self.interface.HookCommand(word, 0, help_text, 0)

	def register_command_sink(word, callback, help_text="Undocumented"):
		csink = {"cmd": word, "callback": callback}
		self.cmd_sinks.append(csink)
		self.interface.HookCommand(word, 0, help_text, 1)

	def run():
		gobject.MainLoop().run()

	
