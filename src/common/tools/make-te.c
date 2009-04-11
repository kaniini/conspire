#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char name[512];
	char num[512];
	char def[512];
	char args[512];
	char buf[512];
	char *defines[512];
  	int i = 0;

	printf("/* this file is auto generated, edit textevents.in instead! */\n\nconst struct text_event te[] = {\n");
	while(fgets(name, sizeof(name), stdin))
	{
		name[strlen(name)-1] = 0;
		fgets(def, sizeof(def), stdin);
		def[strlen(def)-1] = 0;
		fgets(args, sizeof(args), stdin);
		args[strlen(args)-1] = 0;
		fgets(buf, sizeof(buf), stdin);

		printf("\n{\"%s\", %d, \n\"%s\"},\n",
			 name, atoi(args), def);

		defines[i] = strdup (num);
		i++;
	}

	printf("};\n");
	
	return 0;
}
