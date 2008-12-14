command_register("JSTEST", "JSTEST", 0, function (word, word_eol) {
	print("Hello World!");
});

command_register("JSSPAM", "JSSPAM", 0, function (word, word_eol) {
	command("say test from javascript");
});
