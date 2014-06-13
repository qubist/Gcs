#include "test_framework.h"

void test(){
	spec_is_str_equal(getCommandHelp("move"),"Usage: move <X value> <Y value> [Z value]\nMoves relative to your current position. If a Z value is left out, gcc will assume the current Z position.");
	spec_is_str_equal(getCommandHelp("fish"),"\"fish\" is not a command");
	spec_is_str_equal(getCommandHelp("moveto"),"Usage: moveto <X coordinate> <Y coordinate> [Z coordinate]\nMoves to an absolute position. If a Z coordinate is left out, gcc will assume the current Z position.");
//	Command *cp = findCommand("move");
//	char *n = (*cp).name;
//	spec_is_str_equal(n,"move");
////	n = (*cp).help_text;
//	n = cp->help_text; //this is that ^
//	spec_is_str_equal(n,"blah");
	spec_is_ptr_equal(findCommand("fish"), (void *)NIL);
		
}

int main(){
	test();
	reportTests();
}