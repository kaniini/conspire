import conspire

def printtest(word, word_eol, userdata, sess):
	conspire.prnt("HELLLOOOO WORLD")
conspire.command_register("pytest", "pytest", 0, printtest)

