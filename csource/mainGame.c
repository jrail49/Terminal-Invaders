// gcc -lncurses -o pocs main.c

/*** #INCLUDES ***/
#include <ncurses.h>

/*** #DEFINES ***/
#define TICKRATE 100

#define WORLD_WIDTH 50
#define WORLD_HEIGHT 20
 
#define SNAKEY_LENGTH 40


/*** ENUMS ***/
enum direction {
	UP,
	DOWN,
	RIGHT,
	LEFT
};

/*** STRUCTS ***/
typedef struct spart {
	int x;
	int y;
} snakeypart;

/*** PROTOTYPES ***/
int move_snakey(WINDOW *win, int direction, snakeypart snakey[]);
char nextSpartDir(snakeypart current, snakeypart next);

/*** FUNCTIONS ***/
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
    noecho();	/* 	Preven the terminal from printing user Input 	*/
    cbreak();	/*	Disables line buggering and feeds input directly 
    				to the program. This makes the character input 
    				respond immediately and not wait untill a new 
    				line is entered.	*/
    timeout(TICKRATE);	/*	Sets and input delay in milliseconds for 
    						stdscr which is applied during input with 
    						getch() and sibling functions.  */
    					/*	If there is not user input with in the 
    						timer interval then getch() return swith 
    						value ERR.	*/
    keypad(stdscr, TRUE);	/*	Enables special character input such 
    							as arrow keys.	*/

	//	display text at the current curser location. At time of start
	//	up this is at 0,0 (the top left of the screen).
    printw("PieceOfCakeSnake v. 1.0 - Press x to quit...");	

    //	Put the buffer on the screen.
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
// Comments:
// This entire function is all "NEW CODE".
int move_snakey(WINDOW *win, int direction, snakeypart snakey[])
{
	// Clear child windows
	wclear(win);


	// draw the snake from tail to head. 
	for (int i = 0; i < SNAKEY_LENGTH - 1; ++i){

		// direction of next segment
		char ch = nextSpartDir(snakey[i], snakey[i+1]);

		snakey[i] = snakey[i + 1];
		//	Move the curser to y,x in the window win and add the 
		//	character '#'.

		mvwaddch(win, snakey[i].y, snakey[i].x, ch);
	}

	int x = snakey[SNAKEY_LENGTH - 1].x;
	int y = snakey[SNAKEY_LENGTH - 1].y;
	switch (direction) {
		case UP:
			y - 1 == 0 ? y = WORLD_HEIGHT - 2 : --y;
			break;
		case DOWN:
			y + 1 == WORLD_HEIGHT - 1 ? y = 1 : ++y;
			break;
		case RIGHT:
			x + 1 == WORLD_WIDTH - 1 ? x = 1 : ++x;
			break;
		case LEFT:
			x - 1 == 0 ? x = WORLD_WIDTH - 2 : --x;
			break;
		default:
			break;
	}

	snakey[SNAKEY_LENGTH - 1].x = x;
	snakey[SNAKEY_LENGTH - 1].y = y;

	//mvwaddch(win, y, x, '#');
	wmove(win, y, x);
	box(win, 0, 0);

	wrefresh(win);

	return 0;
}

char nextSpartDir(snakeypart current, snakeypart next)
{
	int xdiff = current.x - next.x;
	int ydiff = current.y - next.y;

	if (xdiff < 0)
	{
		return '>';
	}
	else if (xdiff > 0){
		return '<';
	}

	if (ydiff < 0){
		return 'v';
	}
	else if (ydiff > 0){
		return '^';
	}

	return '#';
}








