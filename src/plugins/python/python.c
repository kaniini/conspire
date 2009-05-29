// Pyhon.h Must be included first
#include "Python.h"
#include <glib.h>

#include "common/plugin.h"
#include "common/xchat.h"
#include "common/signal_factory.h"
#include "common/server.h"
#include "common/text.h"
#include "common/cfgfiles.h"
#include "common/outbound.h"
 
mowgli_list_t context_stack = {};
mowgli_dictionary_t *command_stack = NULL;
session *gSess;

CommandResult
py_callback(session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{
	// grab the callback object from the dictionary	
	PyObject *callback = mowgli_dictionary_retrieve(command_stack, word[1]);
	// TODO: have it pass down word, word_eol, userdata and sess, for now, they do nothing	
	gSess = sess;
	PyObject_CallFunction(callback, "(iiii)",NULL, NULL,NULL,NULL);
	return CMD_EXEC_OK;	
}

static PyObject *
py_conspire_command_register(PyObject *self, PyObject *args)
{
	char *name = NULL;
	char *desc = NULL;
	int priority = 0;
	PyObject *callback;
	// parse the args sent to the function
	PyArg_ParseTuple(args, "ssiO", &name, &desc, &priority, &callback);
	if(!PyCallable_Check(callback))
	{
		PyErr_SetString(PyExc_TypeError, "Callback can not be called");		
		return NULL;	
	}	
	mowgli_dictionary_add(command_stack, name, callback);
	// callback the functionname provided	
	command_register(name, desc, priority, py_callback);
		
	return Py_None;
}

static PyObject *
py_conspire_print(PyObject *self, PyObject* args)
{	
	char *text = NULL;	
	PyArg_ParseTuple(args, "s", &text);
	PrintTextf(gSess, "%s",text);
	return Py_None;
}

static PyMethodDef py_conspire_functions[] = 
{
	{"command_register", py_conspire_command_register, METH_VARARGS, 
	 "Create a new command in conspire."},
	{"print", py_conspire_print, METH_VARARGS,
	 "Print out to the current context."},
	{NULL, NULL, 0, NULL}        /* Sentinel */		
};

CommandResult
cmd_pyload(session *sess, gchar *tbuf, gchar *word[], gchar *word_eol[])
{		
	FILE *pyFile;
	// word[2] should contain the script that is going to be run	
	if (!word[2]) { 
		PrintTextf(sess, "*\tUsage: /pyload file.py");
		return CMD_EXEC_FAIL;
	}	
	pyFile = fopen(word[2], "r");
	if(!pyFile)
	{
		PrintTextf(sess, "*\tCan not open file: %s Check that it exists, and you have permissions to read it.", word[2]);
		return CMD_EXEC_FAIL;	
	}	 		

	// TODO: Have stdout go to current sess
	// Start executing the file
	if(PyRun_SimpleFile(pyFile, word[2]) != 0)
	{
		PrintTextf(sess, "*\tAn error has occured while trying to load %s", word[2]);
		return CMD_EXEC_FAIL;			
	}
	PrintTextf(sess, "*\tSuccessfully Loaded %s", word[2]);
	fclose(pyFile);
	
	return CMD_EXEC_OK;
}



gboolean
init(Plugin *p)
{
	PyObject* module;	
	char *argv[] = {"<conspire>", 0};	

	command_stack = mowgli_dictionary_create(g_ascii_strcasecmp);	
	// TODO: add PYRELOAD, PYUNLOAD, PYLIST, and integrate it into conspire plugin manager	
	
	// initialize the python environment		
	Py_Initialize();
	Py_SetProgramName("conspire");	
	PySys_SetArgv(1, argv);	
	module = Py_InitModule("conspire", py_conspire_functions); 
		
	command_register("PYLOAD", "Loads a python script", 0, cmd_pyload);
	return TRUE;
}

gboolean
fini(Plugin *p)
{
	command_remove_handler("PYLOAD", cmd_pyload);
	Py_Finalize();
	return TRUE;
}

PLUGIN_DECLARE("python", PACKAGE_VERSION, 
	"Python plugin for conspire",
	"Darren Blaber", init, fini);
