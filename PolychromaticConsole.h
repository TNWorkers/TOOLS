#ifndef POLYCHROMATICCONSOLE
#define POLYCHROMATICCONSOLE

enum COLOUR_CHOICE {BLACK, RED, GREEN, BROWN, BLUE, MAGENTA, CYAN, GRAY, YELLOW, WHITE};

string TCOLOR (COLOUR_CHOICE COLOUR_input)
{
	if      (COLOUR_input == BLACK)  {return "\033[22;0m";}
	else if (COLOUR_input == RED)    {return "\033[22;31m";}
	else if (COLOUR_input == GREEN)  {return "\033[22;32m";}
	else if (COLOUR_input == BROWN)  {return "\033[22;33m";}
	else if (COLOUR_input == BLUE)   {return "\033[22;34m";}
	else if (COLOUR_input == MAGENTA){return "\033[22;35m";}
	else if (COLOUR_input == CYAN)   {return "\033[22;36m";}
	else if (COLOUR_input == GRAY)   {return "\033[22;37m";}
	else if (COLOUR_input == YELLOW) {return "\033[01;33m";}
	else if (COLOUR_input == WHITE)  {return "\033[01;37m";}
}

#endif
