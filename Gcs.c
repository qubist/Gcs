#include <stdio.h>
#include <signal.h>
#include <string.h>

// #define TESTING

#ifdef TESTING
#include "test_framework.h"
#endif

#define LINE_SIZE 1000
#define NIL 0
#define PARAM_SIZE 5
#define PARAMNUM 4
FILE *fp;

// global declarations
int printgcode = 1;
// declaring the Command struct with name, help text, and function to call
typedef struct{
	char *name;
	char *help_text;
	void (*fn)(char *p1,char *p2,char *p3);
}Command;

char b[255], params[PARAMNUM][PARAM_SIZE];
int done=0;

// macro vars
char macroname[40];
int macrorecording=0;

// functions that are used in main

// string equals function
int streq(s1,s2)
char s1[],s2[];
{
	int f=0;
	int e;
	while((e = (s1[f]==s2[f])) && (s1[f]!=NIL && s2[f]!=NIL))
	f++;
	return e;
}

// function for sending GCODE to printer
void sendg(gcode)
char gcode[];
{
	if(gcode[0]!=0) {
		if (fp != 0) fprintf(fp, "%s\n", gcode);
		if (printgcode == 1) printf("%s\n", gcode);
	}
}

int isNumNull(t)
char t[];
{
	int f=0;
	while(t[f]!=0) {
		if(!(t[f]>='0' && t[f]<='9')) return 0;
		f++;
	}
	return 1;
}

// function for creating GCODE
void xyzCode(x, y, z, g)
char x[], y[], z[], g[];
{
	char b[255];
	if(isNumNull(x) && isNumNull(y) && isNumNull(z)){
		b[0]=0;
		// if params 1, 2, and 3 are there, input all of them
		if(x[0]!=0 && y[0]!=0 && z[0]!=0)
		sprintf(b,"%s X%s Y%s Z%s", g, x, y, z);
		// if params 1 and 2 are actual things and not param 3, don't send Z
		else if(x[0]!=0 && y[0]!=0)
		sprintf(b,"%s X%s Y%s", g, x, y);
		// if params 1 or 2 aren't there, return an error and print the "wrong" GCODE
		else printf("This command requires at least two parameters\n");
		sendg(b);
	}
	else printf("Invalid parameter(s)\n");
}

char *getCommandHelp(char*) ;

// command functions
void homeFn(char *p1,char *p2,char *p3) {
	sendg("G28");
}
void moveFn(char *p1,char *p2,char *p3) {
	xyzCode(p1, p2, p3, "G91\nG1");
}
void movetoFn(char *p1,char *p2,char *p3) {
	xyzCode(p1, p2, p3, "G90\nG1");
}
void laseronFn(char *p1,char *p2,char *p3) {
	sendg("M42 S255 P12");
}
void laseroffFn(char *p1,char *p2,char *p3) {
	sendg("M42 S0 P12");
}
void setspeedFn(char *p1,char *p2,char *p3) {
	sprintf(b, "G1 F%s", params[0]);
	sendg(b);
}
void recordFn(char *p1,char *p2,char *p3) {
	if (macrorecording == 0 && *p1 == 0) {
		printf("This command requires one parameter\n");
	}
	else if (macrorecording == 0) {
		macrorecording=1;
		strcpy(macroname, params[0]);
	}
	else macrorecording=0;
}
void togglegcodeFn(char *p1,char *p2,char *p3) {
	if (printgcode == 1) printgcode = 0;
	else printgcode = 1;
	if (printgcode == 1) printf("GCODE on\n");
	else printf("GCODE off\n");
}
void quitFn(char *p1,char *p2,char *p3) {
	done=1;
	if (fp !=0) fclose(fp);
}

#define NUM_COMMANDS 10
Command commands[] ;

void helpFn(char *p1,char *p2,char *p3) {
	if(*p1 == 0) {
		printf("Usage: help [command]\nCommands:\n");
		for(int i = 0; i<NUM_COMMANDS; i++){
			printf("  %s\n", commands[i].name);
		}
		printf("Consider yourself helped.\n");
	}
	else
	printf("%s", getCommandHelp(p1));
	// printf("TEST PARAMS 0:%s", p1);

}
// Look up there ^^^! NUMCOMMANDS is above helpFn!
// command array
Command commands[] = {
	{"home", "Usage: home \nHomes the printer (sends G28).\n", &homeFn},
	{"move", "Usage: move <X value> <Y value> [Z value]\nMoves relative to your current position. If a Z value is left out, Gcs will assume the current Z position.\n", &moveFn/*or "moveFn"*/},
	{"moveto", "Usage: moveto <X coordinate> <Y coordinate> [Z coordinate]\nMoves to an absolute position. If a Z coordinate is left out, Gcs will assume the current Z position.\n", &movetoFn},
	{"laseron", "Usage: laseron\nTurns the laser on (hidden).\n", &laseronFn},
	{"laseroff", "Usage: laseroff\nTurns the laser off (hidden).\n", &laseroffFn},
	{"setspeed", "Usage: setspeed <speed>\nSets the speed of movement.\n", &setspeedFn},
	{"record", "Usage: record <macro name>\nStarts recording a macro.\n", &recordFn},
	{"togglegcode", "Usage: togglegcode\nToggles showing GCODE when sending it to printer.\n", &togglegcodeFn},
	{"quit", "Usage: quit\nQuits.\n", &quitFn},
	{"help", "Usage: help [command]\nShows a command list (if there are no parameters) or shows the help text of a command.\n", &helpFn},
	// {"", "Usage: \n.", &Fn},
};

Command *findCommand(char *command){
	for(int i = 0; i<NUM_COMMANDS; i++){
		if(streq(command, commands[i].name)){
			return &commands[i];
		}
	}
	return NIL;
}

char buffer[255];
char *getCommandHelp(char *command){
	Command *cp = findCommand(command);
	if (cp == NIL) {
		sprintf(buffer,"\"%s\" is not a command\n",command);
		return buffer;
	}
	else return cp->help_text;
}

#ifdef TESTING
#include "test.h"
#else

int main(){
	char c,buffer[LINE_SIZE],command[15];
	int f=0, p, squid, i;

	printf("Gcs version 0.0.1 created by Will Harris-Braun, last updated 8/14/2017.\n");

	fp=fopen("/dev/ttyACM0", "w");
	if(fp==0){
		fp=fopen("/dev/ttyACM1", "w");
	}

	if (fp==0) {
		printf("Warning: Unable to connect to printer!\nWarning: Running in unconnected mode\n");
		//    return(1);
	}

	// the loop
	while(done==0){
		// print macro name into prompt
		if(macrorecording==1){
			printf("%s >", macroname);
		}
		// print prompt
		else putchar('>');
		// get a line from terminal as a str
		f=0;
		while((c = getchar()) !='\n') {
			buffer[f] = c;
			f++;
		}
		buffer[f]=NIL;
		// parse command from buffer
		f=0;
		while(buffer[f]!=' ' && buffer[f]!=NIL) {
			command[f]=buffer[f];
			f++;
		}
		command[f]=NIL;
		// parse params from buffer
		squid=0;
		for(i=0; i<PARAMNUM; params[i++][0]=0);
		while(buffer[f]!=NIL){
			p=0;
			f++;
			while(buffer[f]!=' ' && buffer[f]!=NIL) {
				params[squid][p]=buffer[f];
				f++; p++;
			}
			params[squid][p]=NIL;
			squid++;
		}

		Command *cp = findCommand(command);
		if (cp != NIL) {
			(*cp->fn)(params[0],params[1],params[2]);
		}
		else {

			// GCODE command
			if (buffer[0]=='G') {
				sendg(buffer);
			}
			// command not found
			else printf("Command not found\n");

			// printf("%s\n",buffer);
		}


		f=0;
	}

}

#endif
