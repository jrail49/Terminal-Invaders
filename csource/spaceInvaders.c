
/*** Includes ***/
#include <ncurses.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
//#include <iostream>

/*** Defines ***/
#define TICKRATE 100
#define TRUE 1
#define FALSE 0

/*** Enums ***/
enum userInput {
	LEFT,
	RIGHT,
	FIRE,
	DO_NOTHING
};

/*** Structs ***/
typedef struct weapon{
	int x;
	int y;
	char bulletChar;
	struct weapon *nextBullet;
	int active;
} bullet;

typedef struct enemy{
	int x;
	int y;
	char ship;
	int health;
	bullet *firstBullet;
	bullet *lastBullet;
} commonEnemy;

typedef struct user{
	int x;
	int y;
	char ship;
	bullet *firstBullet;
	bullet *lastBullet;
} player;

typedef struct eFire{
	bullet *firstBullet;
	bullet *lastBullet;
} alienFire;

/*** Globals ***/
static int WORLD_WIDTH = 50;
static int WORLD_HEIGHT = 30;
static int numberOfEnemies = 5;
static commonEnemy invaders[];
static int once = 1;
static alienFire *enemyFire;// = malloc(sizeof(struct weapon));
static int updateGame = TRUE;
static int hit = FALSE;
static int state = 0;



/*** Prototypes ***/
int move_player(WINDOW *win, int userInput, player *p);
int updateInvaders(WINDOW *win, commonEnemy invaders[]);
void drawPlayer(WINDOW *win, player *p);
void drawInvaders(WINDOW *win);
void updateWorld(WINDOW *win, int userInput, player *p);
void createEnemies();
void fire(player *p);
int gameOver();
int playerGotHit(player *p);
void loserScreen(WINDOW *win);
void winnerScreen(WINDOW *win);
void restartGame(player *p);


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
	
	//	Disable terminal echo
	noecho();
	//	Disable line buffering and feed input directly to program
	cbreak();
	//	Set timeout in milliseconds
	timeout(TICKRATE);
	//	Enable special characters
	keypad(stdscr, TRUE);

	int input;

	//	Get offset of game box inside of window
	//	Display title of applications in the top right hand corner 
	//	with version number. 
	printw("spaceInvaders v. 1.0\nPress 'q' to quit, 'r' to restart\n");
	
	if (COLS < WORLD_WIDTH)
	{
		printw("Window is to thin, please make the window wider.\n");
	}
	else if (LINES < WORLD_HEIGHT)
	{
		printw("Window is to short, please make the window taller.\n");
	}
	else {
		printw("'Prress Up' and 'Space' to fire");
	}




	//	Refresh the original terminal
	refresh();

	offsetx = (COLS - WORLD_WIDTH) / 2;
	offsety = (LINES - WORLD_HEIGHT) / 2;

	//	Create world window
	world = newwin(WORLD_HEIGHT,		// width
				   WORLD_WIDTH,		// height
				   offsety,			// vertical buffer
				   offsetx);		// horizontal buffer
	box(world, 0, 0);

	//	draw the player
	player p1;

	enemyFire = malloc(sizeof(struct eFire));
	enemyFire->firstBullet = 0;
	enemyFire->lastBullet = 0;

	restartGame(&p1);
		
	int ship_command = DO_NOTHING;
	while((input = getch()) != 'q'){	
		if (input == 'r'){
			wclear(world);
			restartGame(&p1);
			wrefresh(world);
			continue;
		}
		// Clear child windows
		wclear(world);

		// draw everything
		updateWorld(world, ship_command, &p1);
		if(input != ERR){
			switch(input){
				case KEY_LEFT:
					ship_command = LEFT;
					break;
				case KEY_RIGHT:
					ship_command = RIGHT;
					break;
				case KEY_UP:
					ship_command = FIRE;
				case ' ':
					ship_command = FIRE;
				default:
					break;
			}
		} else {
			ship_command = DO_NOTHING;
		}

		// draw game box
		box(world, 0, 0);

		// refresh the window
		wrefresh(world);
	}

	//	Get character input from user
	//getch();

	//	Deallocate memory and return the terminal to its former state
	delwin(world);
	endwin();

	return 0;
}

void restartGame(player *p){

	p->x = WORLD_WIDTH/2;
	p->y = WORLD_HEIGHT - 2;
	p->ship = '#';
	p->firstBullet = 0;
	p->lastBullet = 0;

	struct weapon *node;
	struct weapon *node2;
	struct weapon *freeThis;
	node = enemyFire->firstBullet;
	node2 = enemyFire->lastBullet;
	while(node){
		freeThis = node;
		node = node->nextBullet;
		free(freeThis);
	}


	enemyFire->firstBullet = 0;
	enemyFire->lastBullet = 0;
	
	createEnemies();
	updateGame = TRUE;
	hit = FALSE;
	state = 0;
}

int gameOver(){
	int boudycount = 0;

	if (state != 0){
		return state;
	}

	for (int i = 0; i < numberOfEnemies; ++i)
	{
		if (invaders[i].health <= 0){
			boudycount++;
		}
		if (invaders[i].y == (WORLD_HEIGHT))
		{
			state = -1;
			break;
		}
	}

	if (boudycount == (numberOfEnemies)){
		state = 1;
	}

	return state;
}

void winnerScreen(WINDOW *win){
	const char loserText[] = {'Y','o','u',' ','W','i','n','.'};
	const int textLength = 8;
	const int startX = (WORLD_WIDTH/2 - textLength/2 -1);
	const int startY = (WORLD_HEIGHT/2);
	static int currentChar = -1;

	const int threashold = 3;
	static int speed = 1;
	static int wait = 0;

	
	if ((wait + speed) > threashold){
		wait = 0;
		currentChar++;
		return;
	}
	
	int i = 0;
	for (i = 0; i < currentChar; ++i){
		mvwaddch(win, startY, startX+i, loserText[i]);
	}

	if (currentChar < textLength){
		currentChar += 1;
	}
}

void loserScreen(WINDOW *win){
	const char loserText[] = {'Y', 'o', 'u', ' ', 'L', 'o', 's','t','.'};
	const int textLength = 9;
	const int startX = (WORLD_WIDTH/2 - textLength/2 -1);
	const int startY = (WORLD_HEIGHT/2);
	static int currentChar = -1;
	
	const int threashold = 3;
	static int speed = 1;
	static int wait = 0;

	
	if ((wait + speed) > threashold){
		wait = 0;
		currentChar++;
		return;
	}
	int i = 0;
	for (i = 0; i < currentChar; ++i){
		mvwaddch(win, startY, startX+i, loserText[i]);
	}

	if (currentChar < textLength){
		currentChar += 1;
	}
}

int playerGotHit(player *p){

	if (hit){
		return hit;
	}

	struct weapon *node;
	node = enemyFire->firstBullet;
	if (node != 0){
		while(node != 0){
			// determine if a bullet hit the player
			if (p->y == node->y){
				if (p->x == node->x){
					hit = TRUE;
					updateGame = FALSE;
					return hit;
				}
			}
			node = node->nextBullet;
		}
	}

	return hit;
}

void fire(player *p){

	struct weapon *newbullet;
	newbullet = malloc(sizeof(struct weapon));
	newbullet->x = p->x;
	newbullet->y = p->y-1;
	newbullet->bulletChar = '*';
	newbullet->active = TRUE;

	// tell this bullet it is the last bullet
	newbullet->nextBullet = 0;

	if (p->firstBullet == 0){
		//	If there are no other bullets currently being tracked, set 
		//	this bullet to be the first and the last bullet.
		p->firstBullet = newbullet;
		p->lastBullet = newbullet;
	}
	else {
		//	If there is another bullet being tracked, update the last
		//	bullet to be this new bullet. 
		p->lastBullet->nextBullet = newbullet;
		p->lastBullet = newbullet;
	}
}

int move_player(WINDOW *win, int userInput, player *p)
{
	switch (userInput){
		case LEFT:
			p->x - 1 == 0 ? p->x = WORLD_WIDTH - 2 : --p->x;
			break;
		case RIGHT:
			p->x + 1 == WORLD_WIDTH - 1 ? p->x = 1 : ++p->x;
			break;
		case FIRE:
			fire(p);
			break;
		default:
			break;
	}
	return 0;
}

void drawPlayerFire(WINDOW *win, player *p){
	// move all of the bullets. 
	if (p->firstBullet != 0)
	{		
		struct weapon *node;
		node = p->firstBullet;
		//	Update the remaining bullets;
		while(node){
			mvwaddch(win, node->y, node->x, node->bulletChar);
			node = node->nextBullet;
		}
		
	}
}

void drawPlayer(WINDOW *win, player *p)
{
	wmove(win, p->y, p->x);
}

void updatePlayerFire(player *p)
{
	// move all of the bullets. 
	if (p->firstBullet != 0)
	{
		
		struct weapon *node;
		struct weapon *freeThis;
		node = p->firstBullet;
		//	Free all the memory of bullets untill we find a bullet 
		//	that is still active. 
		while ((node->y <= 0)){
			if (node->nextBullet == 0){
				free(node);
				p->firstBullet = 0;
				break;
			}

			p->firstBullet = node->nextBullet;
			freeThis = node;
			node = node->nextBullet;
			free(freeThis);
		}
		
		//	Update the remaining bullets;
		while(node){
			node->y -= 1; 
			node = node->nextBullet;
		}		
	}
}

void updateEnemyFire(WINDOW *win){
	if (enemyFire->firstBullet != 0)
	{	
		struct weapon *node;
		struct weapon *freeThis;
		node = enemyFire->firstBullet;
		//	Free all the memory of bullets untill we find a bullet 
		//	that is still active. 
		while ((node->y <= 0)){
			if (node->nextBullet == 0){
				free(node);
				enemyFire->firstBullet = 0;
				break;
			}

			enemyFire->firstBullet = node->nextBullet;
			freeThis = node;
			node = node->nextBullet;
			free(freeThis);
		}
		
		//	Update the remaining bullets;
		while(node){
			node->y += 1; 
			node = node->nextBullet;
		}
	}
}

void drawEnemyFire(WINDOW *win){
	if(enemyFire->firstBullet != 0){
		struct weapon *node;
		node = enemyFire->firstBullet;
		while(node){
			mvwaddch(win, node->y, node->x, node->bulletChar);
			node = node->nextBullet;
		}
	}
}

void invaderFire(int index)
{



	struct weapon *newbullet;
	newbullet = malloc(sizeof(struct weapon));
	newbullet->x = invaders[index].x;
	newbullet->y = invaders[index].y+1;
	newbullet->bulletChar = '|';
	newbullet->active = TRUE;

	newbullet->nextBullet = 0;
	
	if (enemyFire->firstBullet == 0){
		//	If there are no other bullets currently being tracked, set 
		//	this bullet to be the first and the last bullet.
		enemyFire->firstBullet = newbullet;
		enemyFire->lastBullet = newbullet;
	}
	else {
		//	If there is another bullet being tracked, update the last
		//	bullet to be this new bullet. 
		enemyFire->lastBullet->nextBullet = newbullet;
		enemyFire->lastBullet = newbullet;
	}
}

void move_invaders(WINDOW *win)
{
	static const int speed = 2;
	static const int threashold = 3;
	static int wait = 0;
	static int moveX = -1;
	static int moveY = 0;

	// wait untill the ship can move.
	if ((wait = wait + speed) < threashold){
		return;
	}
	else{
		wait = 0;
	}

	// move the invaders
	if (invaders[0].x == 1 && moveX == -1){
		moveX = +1;	
		for (int i = 0; i < numberOfEnemies; ++i){
			invaders[i].y += 1;
		}
		return;
	}
	else if((invaders[numberOfEnemies-1].x == WORLD_WIDTH-2) && moveX == +1){
		moveX = -1;
		for (int i = 0; i < numberOfEnemies; ++i){
			invaders[i].y += 1;
		}
		return;
	}
	else {
		for (int i = 0; i < numberOfEnemies; ++i){
			invaders[i].x += moveX;
		}
		return;
	}

	return;
}

void drawInvaders(WINDOW *win)
{
	for(int i = 0; i < numberOfEnemies; ++i){
		if (invaders[i].health > 0){
			mvwaddch(win, invaders[i].y, invaders[i].x, invaders[i].ship);
			mvwaddch(win, invaders[i].y-1, invaders[i].x, '_');
		}
		else if (invaders[i].health != INT_MIN){
			mvwaddch(win, invaders[i].y, invaders[i].x, invaders[i].ship);
		}
	}
}

void updateInvadersShip(){
	int dead = -3;
	//srand(time(NULL));
	static const int time = 3;
	static int wait = 0;
	static int r = -1;

	if (wait++ > time){
		r = rand() % numberOfEnemies;
		wait = 0;
	}
		

	for(int i = 0; i < numberOfEnemies; ++i){
		if (invaders[i].health != INT_MIN){
			if (invaders[i].health >= 2){
				if (i == r) invaderFire(i);
				continue;
			}
			else if (invaders[i].health == 1){
				invaders[i].ship = 'v';
				if (i == r) invaderFire(i);
				continue;
			}
			if (invaders[i].health == 0){
				invaders[i].ship = 'v';
			}
			else if (invaders[i].health == -1){
	 			invaders[i].ship = '#';
			}
			else if (invaders[i].health == -2){
				invaders[i].ship = '+';
			}
			else if (invaders[i].health == dead){
				invaders[i].ship = '-';
			}
			else if (invaders[i].health < dead){
				invaders[i].health = INT_MIN;
				invaders[i].ship = ' ';
				continue;
			}	
			// update invader death state
			invaders[i].health--;
		}
	}
}

void damageInvader(player *p){

	struct weapon *node;
	node = p->firstBullet;
	int i = 0;
	if (node != 0){
		while(node != 0){
			for (i = 0; i < numberOfEnemies; ++i){
				if (invaders[i].y == node->y){
					if (invaders[i].x == node->x){
						if (invaders[i].health > 0){
							invaders[i].health -= 1;
						}
					}
				}
			}
			node = node->nextBullet;
		}
	}

}

void updateWorld(WINDOW *win, int userInput, player* p){	

	if (updateGame){
		// damage enemies if hit
		damageInvader(p);

		// move the enemies;
		move_invaders(win);

		// update enemy fire
		updateEnemyFire(win);

		// update player fire
		updatePlayerFire(p);

		// update
		updateInvadersShip();

		// update the players location
		move_player(win, userInput, p);

	}

	// draw enemies
	drawInvaders(win);
	
	// draw bullets
	drawEnemyFire(win);
	drawPlayerFire(win, p);

	// Check to see if the game is over. 
	int over;
	int winner = gameOver();
	if ((over = playerGotHit(p))){
		loserScreen(win);
	}
	else if (winner == 1){
		winnerScreen(win);
	}
	else if (winner == -1){
		loserScreen(win);
	}

	drawPlayer(win, p);

}

void createEnemies(){
	//commonEnemy invaders[numberOfEnemies];

	int xbuf = WORLD_WIDTH/(numberOfEnemies+1);

	for(int i = 0; i < numberOfEnemies; ++i){
		struct enemy newInvader;
		newInvader.x = xbuf + xbuf*i;
		newInvader.y = 3;
		newInvader.ship = 'V';
		newInvader.health = 2;
		invaders[i] = newInvader;
	}

}



// EOF //