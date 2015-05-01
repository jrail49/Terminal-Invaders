
/*** Includes ***/
#include <ncurses.h>

/*** Defines ***/
#define TICKRATE 100

/*** Globals ***/
static int WORLD_WIDTH = 50;
static int WORLD_HIGHT = 20;

/*** Prototypes ***/


/**
Function:	main
Input:	width and height of the world.
Output: None
Comments:	if the user only enters one varable, make the world a
			box.
Status:	Incomplete
*/
int main(int argc, char *argv[])
{
	// Local varables
	WINDOW *world;
	int offsetx, offsety;

	//	Prepare the terminal for curses mode, allocates memory for 
	//	stdcsr
	initscr();
	//	Refresh the original terminal
	refresh();

	/*
	//	Disable terminal echo
	noecho();
	//	Disable line buffering and feed input directly to program
	cbreak();
	//	Set timeout in milliseconds
	timeout(TICKRATE);
	//	Enable special characters
	keypad(stdscr, TRUE);

	*/
	
	//	Display title of applications in the top right hand corner 
	//	with version number. 
	printw("spaceInvaders v. 1.0 - Press ESC to quit.");

	//	Get offset of game box inside of window
	offsetx = (COLS - WORLD_WIDTH) / 2;
	offsety = (LINES - WORLD_HIGHT) / 2;

	//	Create world window
	world = newwin(WORLD_HIGHT,		// width
				   WORLD_WIDTH,		// height
				   offsety,			// vertical buffer
				   offsetx);		// horizontal buffer
	
	//	Draw the game box
	box(world, 0, 0);

	//	Put the buffer on the screen
	wrefresh(world);

	//	Get character input from user
	getch();

	//	Deallocate memory and return the terminal to its former state
	delwin(world);
	endwin();

	return 0;
}



// EOF //