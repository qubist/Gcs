void testHelp(){
	spec_is_string_equal(getCommandHelp("move"),"Usage: move <X> <Y> [Z]\nMoves relative to your current position.");
	spec_is_string_equal(getCommandHelp("moveto"),"Usage: moveto <X> <Y> [Z]\nMoves to an absolute position.");
	Command *cp = findCommand("move");
	char *n = (*cp).name;
	spec_is_string_equal(n,"move");
//	n = (*cp).help_text;
	n = cp->help_text; //this is that ^
	spec_is_string_equal(n,"blah");
	spec_is_ptr_equal(findCommand("fish"), NIL);
		
}