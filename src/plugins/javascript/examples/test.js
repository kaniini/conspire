command_register("JSTEST", "JSTEST", 0, function (word, word_eol) {
	print("Hello World!");
});

command_register("JSSPAM", "JSSPAM", 0, function (word, word_eol) {
	command("say test from javascript");
});

var ConnectSignal = function (session, host, ip, data) {
	print("Type of window: " + session.type + "!");
	print("Connecting: " + host + " (" + ip + ")");
}

signal_attach("server connect", ConnectSignal);
signal_attach("server connect", function () {
	signal_disconnect("server connect", ConnectSignal);
});
