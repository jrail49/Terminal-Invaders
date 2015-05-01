// gcc -lncurses -o pocs main.c

 /***************\
/*** #INCLUDES ***\
\*****************/
#include <ncurses.h>

 /**************\
/*** #DEFINES ***\
\****************/
#define TICKRATE 100

#define WORLD_WIDTH 50
#define WORLD_HEIGHT 20
 
#define SNAKEY_LENGTH 40

 /***********\
/*** ENUMS ***\
\*************/
enum direction {
	UP,
	DOWN,
	RIGHT,
	LEFT
};

 /*************\
/*** STRUCTS ***\
\***************/
typedef struct spart {
	int x;
	int y;
} snakeypart;


 /****************\
/*** PROTOTYPES ***\
\******************/
int move_snakey(WINDOW *win, int direction, snakeypart snakey[]);

 /***************\
/*** FUNCTIONS ***\
\*****************/
//////////////////////////////////////////////////////////////////////
// main
int main(int argc, char *argv[]) 
{
 
 	/**
 	* ncurses sets up a root window, stdscr, which backdrops the 
 	* current terminal display.
 	*/
    WINDOW *snakeys_world;
    int offsetx, offsety;
    /*** NEW CODE ***/
    int i, ch;
    /*** END OF NEW CODE ***/
 
 	// Prepare the terminal for curses mode, allocates memory for 
 	// stdscr.
    initscr();

    /*** NEW ***/
    initscr();
    noecho();
    cbreak();
    timoeut(TICKRATE);
    keypad(stdscr, TRUE);

    printw("PieceOfCakeSnake v. 1.0 - Press x to quit...");	// display text at top of screen.
    /*** END OF NEW CODE ***/

    // Put the buffer on the screen.
    refresh();
 
 	// COLS and LINES are environment variables that hold the current
 	// width and hight of the terminal. 
    offsetx = (COLS - WORLD_WIDTH) / 2;
    offsety = (LINES - WORLD_HEIGHT) / 2;
 
 	/*** Draw a boarder around the window ***/
 	// create the window
    snakeys_world = newwin(WORLD_HEIGHT,	// height
                           WORLD_WIDTH,		// width
                           offsety,			// height buffer
                           offsetx);		// width buffer
    box(snakeys_world, 0 , 0);				// draw the box
 
    /*** NEW CODE ***/
    snakeypart snakey[SNAKEY_LENGTH];

    int sbegx = (WORLD_WIDTH - SNAKEY_LENGTH) / 2;
    int sbegy = (WORLD_HEIGHT - 1) / 2;

    for (i = 0; i < SNAKEY_LENGTH; ++i){
    	snakey[i].x = sbegx + i;
    	snakey[i].y = sbegy;
    }

    int cur_dir = RIGHT;

    while ((ch = getch()) != 'x'){
    	move_snakey(snakeys_world, cur_dir, snakey);
    	if (ch != ERR){	// Not sure where ERR came from
    		// set the snakes current direction to what the user 
    		// entered.
    		switch(ch) {
    			case KEY_UP:
    				cur_dir = UP;
    				break;
    			case KEY_DOWN:
    				cur_dir = DOWN;
    				break;
    			case KEY_RIGHT:
    				cur_dir = RIGHT;
    				break;
    			case KEY_LEFT:
    				cur_dir = LEFT;
    				break;
    			default:
    				break;
    		}
    	}
    }
    /*** END OF NEW CODE ***/

/*** Removed ***
    wrefresh(snakeys_world);	// Same as refresh() but only works on
    							// child windows. 
 
 	// pause program until until keyboard input is received. 
    getch();
*** End of Removal ***/

 	// memory deallocation and returns the terminal to it's former 
 	// state
    delwin(snakeys_world);
    
    endwin();
 
    return 0;
}

//////////////////////////////////////////////////////////////////////
// move_snakey
// Input:
// Output:
int move_snakey(WINDOW *won, int direction, snakeypart snakey[])
{

}








