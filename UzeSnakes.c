/*
 *  UzeSnakes
 *  Copyright (C) 2010 Flávio Zavan
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/tileset.inc"
#include "data/patches.inc"
#include "data/snake60.inc"
#include "data/snake30.inc"

#define NUM_NON_FONT_TILES (61-22)
#define NUMERALSOFFSET 48
#define LETTEROFFSET 52
#define PLAYEROFFSET 9
#define MENUITEMS 5
#define MAZES 4
#define OPTIONSX 20
#define OPTIONSY 15
#define MARKERX 18
#define VALUESX 35
#define MODES 4
#define STARTINGSIZE 3
#define DEFAULTDELAY 3
#define DEFAULTSIZE 30
#define DEFAULTTIME 2
#define MAXSIZE 500
#define MAXTIME 10
#define MINSIZE 4
#define MINTIME 1
#define SNAKE 0
#define EMPTY 4
#define WALL 5
#define MEAT 6
#define REVERSE 7
#define MONEY 8
#define FAST 9
#define SLOW 10
#define INVERT 11
#define CUT 12
#define BORDER 1
#define ROOMS 2
#define HALLWAY 3

//Important tiles
#define EMPTY_TILE 1
#define WALL_TILE 2
#define HEAD_TILE 3
#define BODY_TILE 7
#define TAIL_TILE 8
#define MEAT_TILE 21
#define REVERSE_TILE 22
#define MONEY_TILE 23
#define FAST_TILE 24
#define SLOW_TILE 25
#define INVERT_TILE 26
#define CUT_TILE 27


//Strings
//@ is the copyright symbol and ? is Á
const char strCopyright0[] PROGMEM = "@ FL?VIO ZAVAN 2010";
const char strCopyright1[] PROGMEM = "LICENSED UNDER GNU GPL V3";
const char strCopyright2[] PROGMEM = "MEDIA UNDER CC ASA 3 UNPORTED";
const char strCopyright3[] PROGMEM = "MUSIC BY IDERALDO LUIZ DA SILVA";
const char strCopyright4[] PROGMEM = "V 1 03";
const char strMenu[MENUITEMS][11] PROGMEM = {{"1 PLAYER"},{"2 PLAYERS:"},
	{"DELAY:"},{"MAZE:"},{"SOUND:"}};
const char strMazes[MAZES][8] PROGMEM = {{"NONE"},{"BORDERS"},{"ROOMS"},
	{"HALLWAY"}};
const char strModes[MODES][8] PROGMEM = {{"NORMAL"},{"SPEEDUP"},{"GROWTH"},
	{"TIMED"}};
const char strSound[4][14] PROGMEM = {{"NO SOUND"},{"MUSIC ONLY"},{"SFX ONLY"}
	,{"MUSIC AND SFX"}};
const char strMinutes[] PROGMEM = "MINUTES:";
const char strSize[] PROGMEM = "SIZE:";
const char strP1[] PROGMEM = "PLAYER 1:";
const char strP2[] PROGMEM = "PLAYER 2:";
const char strGO[] PROGMEM = "GAME OVER";
const char str1won[] PROGMEM = "PLAYER 1 WON";
const char str2won[] PROGMEM = "PLAYER 2 WON";
const char strDraw[] PROGMEM = "DRAW";
const char strTime[] PROGMEM = "TIME:      :";
const char strPaused[] PROGMEM = "PAUSED";
const char strResume[] PROGMEM = "RESUME";
const char strQuit[] PROGMEM = "QUIT";

//Used to convert the data in the array to tiles
const unsigned char convert[] = {0,0,0,0,EMPTY_TILE,WALL_TILE,
	MEAT_TILE,REVERSE_TILE,MONEY_TILE,FAST_TILE,SLOW_TILE,
	INVERT_TILE,CUT_TILE};

//Globals
int btnHeld[2] = {0,0};
int btnPressed[2] = {0,0};
int btnPrev[2] = {0,0};
unsigned char chosenDelay = DEFAULTDELAY;
unsigned char maze = 0;
unsigned int size = DEFAULTSIZE;
unsigned char minutes = DEFAULTTIME;
unsigned char grid[28][22];
unsigned char sound = 3;

struct snakeStruct{
	unsigned int size, score;
	unsigned char state, delay, counter, direction, timer;
	char headx, heady, tailx, taily, nextx, nexty;
	int grow;
};
struct snakeStruct snake[2];

void usPrint(unsigned char x, unsigned char y, const char *string){
	unsigned char i = 0;
	char c=pgm_read_byte(&(string[i++]));
	do {
		//Numerals and :
		if (c == 32){
			SetTile(x++,y,0);
		}
		else if (c < 59){
			c = c-NUMERALSOFFSET;
			SetFont(x++,y,c);
		}
		//?, @ and letters
		else {
			c = c-LETTEROFFSET;
			SetFont(x++,y,c);
		}
		c=pgm_read_byte(&(string[i++]));
	} while (c != 0);
}

void usPrintNum(char x, char y, unsigned int number){
	unsigned char p = x + 5;
	do{
		SetFont(p--,y,number%10);
		number /= 10;
	}while (number > 0);
	for(;p >= x; p--){
		SetTile(p,y,0);
	}
}

void drawMarker(unsigned char y, unsigned char size){
	Fill(MARKERX,OPTIONSY,1,size,0);
	SetTile(MARKERX,y+OPTIONSY,MEAT_TILE);
}

unsigned char menu(){
	StopSong();
	if(sound & 1){
		StartSong(snake30);
	}
	ClearVram();
	unsigned long randomSeed = 0;
	//Print the standard text
	DrawMap2(7,4,uze);
	DrawMap2(24,4,sna);
	DrawMap2(39,4,kes);
	DrawMap2(21,11,logo);
	usPrint(22,21,strCopyright0);
	usPrint(18,22,strCopyright1);
	usPrint(16,23,strCopyright2);
	usPrint(15,24,strCopyright3);
	usPrint(27,25,strCopyright4);
	//Print the options
	for (unsigned char i = 0; i < MENUITEMS; i++){
		usPrint(OPTIONSX,OPTIONSY+i,strMenu[i]);
	}
	unsigned char marker = 0;
	unsigned char mode = 0;
	drawMarker(marker, MENUITEMS);
	//Print mode, speed and maze
	usPrint(VALUESX,OPTIONSY+1,strModes[mode]);
	usPrintNum(VALUESX,OPTIONSY+2,chosenDelay);
	usPrint(VALUESX,OPTIONSY+3,strMazes[maze]);
	usPrint(VALUESX,OPTIONSY+4,strSound[sound]);
	while (1){
		WaitVsync(1);
		randomSeed++;
		btnHeld[0] = ReadJoypad(0);
		btnPressed[0] = btnHeld[0] & (btnHeld[0] ^ btnPrev[0]);
		if (btnPressed[0] & BTN_UP){
			marker += MENUITEMS-1;
		}
		else if (btnPressed[0] & BTN_DOWN){
			marker++;
		}
		//1 player
		else if (btnPressed[0] & BTN_B && marker == 0){
			srandom(randomSeed);
			return 0;
		}
		//2 players
		else if (btnPressed[0] & BTN_B && marker == 1){
			srandom(randomSeed);
			return 1+mode;
		}
		//Change options
		else if (btnPressed[0] & BTN_RIGHT){
			Fill(VALUESX,OPTIONSY+marker,15,1,0);
			if (marker == 1){
				mode++;
				mode %= MODES;
				usPrint(VALUESX,OPTIONSY+1,strModes[mode]);
			}
			else if (marker == 2){
				chosenDelay++;
				chosenDelay %= 32;
				usPrintNum(VALUESX,OPTIONSY+2,chosenDelay);
			}
			else if (marker == 3){
				maze++;
				maze %= MAZES;
				usPrint(VALUESX,OPTIONSY+3,strMazes[maze]);
			}
			else if (marker == 4){
				sound++;
				sound %= 4;
				usPrint(VALUESX,OPTIONSY+4,strSound[sound]);
				if (sound & 1){
					StartSong(snake30);
				}
				else {
					StopSong();
				}
			}
		}
		else if (btnPressed[0] & BTN_LEFT){
			Fill(VALUESX,OPTIONSY+marker,15,1,0);
			if (marker == 1){
				mode--;
				mode %= MODES;
				usPrint(VALUESX,OPTIONSY+1,strModes[mode]);
			}
			else if (marker == 2){
				chosenDelay--;
				chosenDelay %= 32;
				usPrintNum(VALUESX,OPTIONSY+2,chosenDelay);
			}
			else if (marker == 3){
				maze--;
				maze %= MAZES;
				usPrint(VALUESX,OPTIONSY+3,strMazes[maze]);
			}
			else if (marker == 4){
				sound--;
				sound %= 4;
				usPrint(VALUESX,OPTIONSY+4,strSound[sound]);
				if (sound & 1){
					StartSong(snake30);
				}
				else {
					StopSong();
				}
			}
		}
		marker %= MENUITEMS;
		drawMarker(marker, MENUITEMS);
		btnPrev[0] = btnHeld[0];
	}
}

unsigned char getPos(unsigned char x, unsigned char y){
	if (x & 1){
		return grid[x/2][y] & 15;
	}
	else{
		return grid[x/2][y] >> 4;
	}
}

void setPos(unsigned char x, unsigned char y, unsigned char value){
	if (x & 1){
		grid[x/2][y] = (grid[x/2][y] & 240) + value;
	}
	else{
		grid[x/2][y] = (grid[x/2][y] & 15) + (value << 4);
	}
}

void drawField(unsigned char players){
	for(unsigned char y = 0; y < 22; y++){
		for(unsigned char x = 0; x < 56; x++){
			if (getPos(x,y) > 3){
				SetTile(x+2,y+4,convert[getPos(x,y)]);
			}
		}
	}
	//Snakes
	for(unsigned char i = 0; i < players; i++){
		//Head
		SetTile(snake[i].headx+2,snake[i].heady+4,
			HEAD_TILE+getPos(snake[i].headx,snake[i].heady)
			+PLAYEROFFSET*i);
		//Body
		char x = snake[i].tailx;
		char y = snake[i].taily;
		for(unsigned char n = 0; n < snake[i].size - 2; n++){
			switch (getPos(x,y)){
				case 0:
					y--;
					break;
	
				case 1:
					x++;
					break;
	
				case 2:
					y++;
					break;
	
				case 3:
					x--;
					break;
			}
			if(x < 0){
				x = 55;
			}
			else if(x > 55){
				x = 0;
			}
			else if(y > 21){
				y = 0;
			}
			else if(y < 0){
				y = 21;
			}
			SetTile(x+2,y+4,BODY_TILE+PLAYEROFFSET*i);
		}
		//Tail
		SetTile(snake[i].tailx+2,snake[i].taily+4,
			TAIL_TILE+getPos(snake[i].tailx,snake[i].taily)
			+PLAYEROFFSET*i);
	}
}

void placeSnake(unsigned char players, unsigned char i, unsigned char delay){
	snake[i].size = 3;
	snake[i].delay = delay;
	snake[i].state = 0;
	snake[i].counter = 0;
	snake[i].grow = 0;
	snake[i].score = 0;
	snake[i].timer = 0;
	Fill(2+28*i,3,5,1,0);
	if (players == 1){
		snake[0].headx = 15;
		snake[0].heady = 11;
		snake[0].tailx = 13;
		snake[0].taily = 11;
		setPos(15,11,1);
		setPos(14,11,1);
		setPos(13,11,1);
		snake[0].nextx = 16;
		snake[0].nexty = 11;
		snake[0].direction = 1;
	}
	else {
		unsigned char hx, hy, tx, ty, d, nx, ny, mx, my;
		if (i == 0){
			hx = 15;
			hy = 11;
			tx = 13;
			ty = 11;
			d = 1;
			nx = 16;
			ny = 11;
			mx = 14;
			my = 11;
		}
		else {
			hx = 41;
			hy = 12;
			tx = 43;
			ty = 12;
			d = 3;
			nx = 40;
			ny = 12;
			mx = 42;
			my = 12;
			if(maze == HALLWAY){
				hy = 10;
				ty = 10;
				ny = 10;
				my = 10;
			}
		}
		//If the standard position isn't good, try a new one
		//Also, max size is 500
		//There will always be enough room
		while(getPos(hx,hy) != EMPTY || getPos(tx,ty) != EMPTY ||
			getPos(mx,my) != EMPTY || getPos(nx,ny) != EMPTY){
			hx++;
			tx++;
			mx++;
			nx++;
			if(hx > 53){
				if(i == 0){
					hx = 2;
					tx = 0;
					mx = 1;
					nx = 3;
				}
				else {
					hx = 1;
					tx = 3;
					mx = 2;
					nx = 0;
				}
				hy++;
				ty++;
				my++;
				ny++;
				if(hy > 21){
					hy = 0;
					my = 0;
					ny = 0;
					ty = 0;
				}
			}
		}
		setPos(hx,hy,d);
		setPos(mx,my,d);
		setPos(tx,ty,d);
		snake[i].nextx = nx;
		snake[i].nexty = ny;
		snake[i].headx = hx;
		snake[i].heady = hy;
		snake[i].tailx = tx;
		snake[i].taily = ty;
		snake[i].direction = d;
	}
}

void placeMeat(){
	unsigned char x = random() % 56;
	unsigned char y = random() % 22;
	while(getPos(x,y) != EMPTY){
		x++;
		if (x > 55){
			x = 0;
			y++;
		}
		if (y > 21){
			y = 0;
		}
	}
	setPos(x,y,MEAT);
	SetTile(x+2,y+4,MEAT_TILE);
}

void placeItem(unsigned char delay){
	unsigned char x = random() % 56;
	unsigned char y = random() % 22;
	unsigned char i = random() % 6 + 7;
	while(getPos(x,y) != EMPTY){
		x++;
		if (x > 55){
			x = 0;
			y++;
		}
		if (y > 21){
			y = 0;
		}
	}
	//Can't go faster than zero
	if(delay == 0 && i == FAST){
		i = SLOW;
	}
	setPos(x,y,i);
	SetTile(x+2,y+4,convert[i]);
}

void clearSnake(unsigned int l){
	char rx = snake[l].tailx;
	char ry = snake[l].taily;
	for(unsigned char i = 0; i < snake[l].size; i++){
		switch (getPos(rx,ry)){
			case 0:
				setPos(rx,ry,EMPTY);
				ry = (ry + 21) % 22;
				break;

			case 1:
				setPos(rx,ry,EMPTY);
				rx = (rx + 1) % 56;
				break;

			case 2:
				setPos(rx,ry,EMPTY);
				ry = (ry + 1) % 22;
				break;

			case 3:
				setPos(rx,ry,EMPTY);
				rx = (rx + 55) % 56;
				break;
		}
	}
}

void reverseSnake(unsigned char i){
	char nx = snake[i].tailx;
	char ny = snake[i].taily;
	//Real variables for setting and getting pos
	char ox, oy, x, y, rx, ry, rnx, rny;
	ox = 0;
	oy = 0;
	rnx = nx;
	rny = ny;
	for(unsigned char n = 0; n < snake[i].size; n++){
		x = nx;
		y = ny;
		rx = rnx;
		ry = rny;
		switch (getPos(rx,ry)){
			case 0:
				rny =  (rny + 21) % 22;
				ny--;
				break;
	
			case 1:
				rnx =  (rnx + 1) % 56;
				nx++;
				break;
	
			case 2:
				rny =  (rny + 1) % 22;
				ny++;
				break;
	
			case 3:
				rnx =  (rnx + 55) % 56;
				nx--;
				break;
		}
		//Invert the direction of the tail
		if (n == 0){
			setPos(rx,ry,(getPos(rx,ry)+2) % 4);
		}
		else {
			unsigned char v = 0;
			if(ox > x){
				v = 1;
			}
			else if(oy > y){
				v = 2;
			}
			else if(ox < x){
				v = 3;
			}
			setPos(rx,ry,v);
		}
		ox = x;
		oy = y;
	}
	//Switch head and tail
	snake[i].tailx ^= snake[i].headx;
	snake[i].headx ^= snake[i].tailx;
	snake[i].tailx ^= snake[i].headx;
	snake[i].taily ^= snake[i].heady;
	snake[i].heady ^= snake[i].taily;
	snake[i].taily ^= snake[i].heady;
	//Fix direction
	snake[i].direction = getPos(snake[i].headx,snake[i].heady);
	//Fix next
	switch (snake[i].direction){
		case 0:
			snake[i].nextx = snake[i].headx;
			snake[i].nexty = snake[i].heady - 1;
			break;

		case 1:
			snake[i].nextx = snake[i].headx + 1;
			snake[i].nexty = snake[i].heady;
			break;

		case 2:
			snake[i].nextx = snake[i].headx;
			snake[i].nexty = snake[i].heady + 1;
			break;

		case 3:
			snake[i].nextx = snake[i].headx - 1;
			snake[i].nexty = snake[i].heady;
			break;
	}
}

bool pause(unsigned char i){
	Fill(MARKERX-1,OPTIONSY-3,10,6,0);
	usPrint(MARKERX,OPTIONSY-2,strPaused);
	usPrint(OPTIONSX,OPTIONSY,strResume);
	usPrint(OPTIONSX,OPTIONSY+1,strQuit);
	bool opt = 0;
	drawMarker(opt,2);
	while(ReadJoypad(i) != BTN_B){
		if(ReadJoypad(i) & BTN_UP && opt == 1){
			opt = 0;
			drawMarker(opt,2);
		}
		else if(ReadJoypad(i) & BTN_DOWN && opt == 0){
			opt = 1;
			drawMarker(opt,2);
		}
	}
	while(ReadJoypad(i));
	return opt;
}

void game(unsigned char players, unsigned char mode){
	StopSong();
	if(sound & 1){
		StartSong(snake60);
	}
	//Draw everything
	ClearVram();
	//Initialize all variables
	for(unsigned char y = 0; y < 22; y++){
		for(unsigned char x = 0; x < 56; x++){
			setPos(x,y,EMPTY);
		}
	}
	//Add maze if needed
	switch (maze){
		case BORDER :
			for(unsigned char i = 0; i < 56; i++){
				setPos(i,0,WALL);
				setPos(i,21,WALL);
			}
			for(unsigned char i = 0; i < 22; i++){
				setPos(0,i,WALL);
				setPos(55,i,WALL);
			}
			break;

		case ROOMS :
			for(unsigned char i = 0; i < 56; i++){
				setPos(i,10,WALL);
			}
			for(unsigned char i = 0; i < 22; i++){
				setPos(28,i,WALL);
			}
			break;

		case HALLWAY:
			for(unsigned char i = 0; i < 10; i++){
				setPos(i,9,WALL);
				setPos(10,i,WALL);
				setPos(i+11,9,WALL);
				setPos(21,i,WALL);
				setPos(i+46,12,WALL);
				setPos(45,i+12,WALL);
				setPos(i+35,12,WALL);
				setPos(34,i+12,WALL);
			}
			break;
			
	}
	unsigned char delay = chosenDelay;
	unsigned char won = 0;
	unsigned char itemTimer = 0;
	char frames = 0;
	char seconds = 0;
	if(mode == 1){
		seconds = 10;
		usPrintNum(20,3,seconds);
	}
	else if(mode == 2){
		usPrintNum(20,3,size);
	}
	char mins = minutes;
	placeSnake(players,0,delay);
	usPrint(4,2,strP1);
	if(players == 2){
		usPrint(32,2,strP2);
		placeSnake(players,1,delay);
	}
	placeMeat();
	drawField(players);
	//Game loop
	while(!won){
		WaitVsync(1);
		//Update scores
		if (mode != 2){
			usPrintNum(13,2,snake[0].score);
		}
		else {
			usPrintNum(13,2,snake[0].size);
		}
		if(players == 2){
			if (mode != 2){
				usPrintNum(41,2,snake[1].score);
			}
			else {
				usPrintNum(41,2,snake[1].size);
			}
		}
		//Handle timer events
		if(itemTimer == 3){
			placeItem(delay);
			itemTimer++;
		}
		frames++;
		if(frames == 60){
			frames = 0;
			if(mode == 3){
				seconds--;
			}
		}
		if(mode == 3){
			if(seconds < 0){
				seconds = 59;
				mins--;
			}
			usPrint(14,3,strTime);
			usPrintNum(19,3,mins);
			usPrintNum(26,3,seconds);
			if(mins == 0 && seconds == 0){
				if(snake[0].score > snake[1].score){
					won = 1;
				}
				else if(snake[0].score < snake[1].score){
					won = 2;
				}
				else {
					won = 3;
				}
				//Just in case someone hits a wall
				//During the last iteration
				break;
			}
		}
		//Check for victory in growth mode
		if(mode == 2){
			if (snake[0].size == size){
				won = 1;
				break;
			}
			else if (snake[1].size == size){
				won = 2;
				break;
			}
		}
		//Go faster in speedup mode
		if(mode == 1){
			if(frames == 0 && delay > 0){
				seconds--;
				usPrintNum(20,3,seconds);
			}
			if(seconds == 0){
				if(sound & 2){
					TriggerFx(4,0xff,1);
				}
				seconds = 10;
				delay--;
				if(snake[0].timer == 0){
					snake[0].delay = delay;
				}
				if(snake[1].timer == 0){
					snake[1].delay = delay;
				}
			}
		}
		//Read controllers and move snakes
		for(unsigned char i = 0; i < players; i++){
			btnHeld[i] = ReadJoypad(i);
			btnPressed[i] = btnHeld[i] & (btnHeld[i] ^ btnPrev[i]);
			
			//Fix the timer
			if(frames == 0 && snake[i].timer > 0){
				snake[i].timer--;
				usPrintNum(1+28*i,3,snake[0].timer);
				if(snake[i].timer== 0){
					snake[i].delay = delay;
					snake[i].state = 0;
					Fill(2+28*i,3,5,1,0);
				}
			}
			//Turn
			if(btnPressed[i] & BTN_UP && ((getPos(
				snake[i].headx, snake[i].heady) != 2 &&
				!snake[i].state)||(getPos(snake[i].headx,
				snake[i].heady) != 0 &&	snake[i].state))){
				if(snake[i].state){
					snake[i].nexty = snake[i].heady + 1;
					snake[i].direction = 2;
				}
				else {
					snake[i].nexty = snake[i].heady - 1;
					snake[i].direction = 0;
				}
				snake[i].nextx = snake[i].headx;
			}
			else if(btnPressed[i] & BTN_DOWN && ((getPos(
				snake[i].headx, snake[i].heady) != 0 &&
				!snake[i].state)||(getPos(snake[i].headx,
				snake[i].heady) != 2 &&	snake[i].state))){
				if(snake[i].state){
					snake[i].nexty = snake[i].heady - 1;
					snake[i].direction = 0;
				}
				else {
					snake[i].nexty = snake[i].heady + 1;
					snake[i].direction = 2;
				}
				snake[i].nextx = snake[i].headx;
			}
			else if(btnPressed[i] & BTN_LEFT && ((getPos(
				snake[i].headx, snake[i].heady) != 1 &&
				!snake[i].state)||(getPos(snake[i].headx,
				snake[i].heady) != 3 &&	snake[i].state))){
				if(snake[i].state){
					snake[i].nextx = snake[i].headx + 1;
					snake[i].direction = 1;
				}
				else {
					snake[i].nextx = snake[i].headx - 1;
					snake[i].direction = 3;
				}
				snake[i].nexty = snake[i].heady;
			}
			else if(btnPressed[i] & BTN_RIGHT && ((getPos(
				snake[i].headx, snake[i].heady) != 3 &&
				!snake[i].state)||(getPos(snake[i].headx,
				snake[i].heady) != 1 &&	snake[i].state))){
				if(snake[i].state){
					snake[i].nextx = snake[i].headx - 1;
					snake[i].direction = 3;
				}
				else {
					snake[i].nextx = snake[i].headx + 1;
					snake[i].direction = 1;
				}
				snake[i].nexty = snake[i].heady;
			}
			else if(btnPressed[i] & BTN_START){
				if(pause(i)){
					won = 4;
					break;
				}
				else {
					drawField(players);
				}
			}
			//Useful for debugging
			/*else if(btnPressed[i] & BTN_B){
				if (size >= 6){
					snake[i].grow -= 3;
				}
				else {
					snake[i].grow -= 
						snake[i].size - 3;
				}
			}
			else if(btnPressed[i] & BTN_Y){
				reverseSnake(i);
			}
			else if(btnPressed[i] & BTN_A){
				snake[i].grow++;
			}*/

			//Move
			if(snake[i].counter >= snake[i].delay){
				//Fix head
				setPos(snake[i].headx,snake[i].heady,
					snake[i].direction);
				//Fix nexts
				if(snake[i].nextx > 55){
					snake[i].nextx = 0;
				}
				else if(snake[i].nextx < 0){
					snake[i].nextx = 55;
				}
				if(snake[i].nexty > 21){
					snake[i].nexty = 0;
				}
				else if(snake[i].nexty < 0){
					snake[i].nexty = 21;
				}
				//Check for objects
				unsigned char object = getPos(snake[i].nextx,
					snake[i].nexty);
				//Die when hitting a wall or snake
				if(object < 4 || object == WALL){
					//End game if normal or speedup
					if (mode < 2){
						won = 2-i;
					}
					//Make a new snake
					else {
						if(sound & 2){
							TriggerFx(2,0xff,1);
						}
						clearSnake(i);
						placeSnake(players,i,delay);
						drawField(players);
					}
					break;
				}
				//Meat
				else if(object == MEAT){
					if(sound & 2){
						TriggerFx(3,0xff,1);
					}
					snake[i].grow++;
					placeMeat();
					snake[i].score++;
					if(itemTimer < 3){
						itemTimer++;
					}
				}
				//Money
				else if(object == MONEY){
					if(sound & 2){
						TriggerFx(3,0xff,1);
					}
					snake[i].grow += 3;
					snake[i].score += 5;
					itemTimer = 0;
				}
				//Scissors
				else if(object == CUT){
					if(sound & 2){
						TriggerFx(3,0xff,1);
					}
					if (snake[i].size >= 6){
						snake[i].grow -= 3;
					}
					else {
						snake[i].grow -= 
							snake[i].size - 3;
					}
					snake[i].score += 5;
					itemTimer = 0;
				}
				//Invert
				else if(object == INVERT){
					if(sound & 2){
						TriggerFx(3,0xff,1);
					}
					snake[i].timer = 10;
					snake[i].state = 1;
					snake[i].score += 5;
					itemTimer = 0;
					usPrintNum(1+28*i,3,snake[0].timer);
				}
				//Fast
				else if(object == FAST){
					if(sound & 2){
						TriggerFx(3,0xff,1);
					}
					snake[i].timer = 10;
					snake[i].delay /= 2;
					snake[i].score += 5;
					itemTimer = 0;
					usPrintNum(1+28*i,3,snake[0].timer);
				}
				//Slow
				else if(object == SLOW){
					if(sound & 2){
						TriggerFx(3,0xff,1);
					}
					snake[i].timer = 10;
					if (snake[i].delay == 0){
						snake[i].delay++;
					}
					else {
						snake[i].delay *= 2;
					}
					snake[i].score += 5;
					itemTimer = 0;
					usPrintNum(1+28*i,3,snake[0].timer);
				}
				//Reverse happens after the snake moved
				//Start moving
				snake[i].counter = 0;
				setPos(snake[i].nextx,snake[i].nexty,
					snake[i].direction);
				//Put body where the head was
				SetTile(snake[i].headx+2,snake[i].heady+4,
					BODY_TILE+PLAYEROFFSET*i);
				snake[i].headx = snake[i].nextx;
				snake[i].heady = snake[i].nexty;
				//Place new head
				SetTile(snake[i].headx+2,snake[i].heady+4,
					HEAD_TILE+snake[i].direction+
					PLAYEROFFSET*i);
				//New next
				switch(snake[i].direction){
					case 0 :
						snake[i].nexty--;
						break;

					case 1 :
						snake[i].nextx++;
						break;

					case 2 :
						snake[i].nexty++;
						break;

					case 3 :
						snake[i].nextx--;
						break;
				}
				//Only move tail if not growing
				if (snake[i].grow > 0){
					snake[i].size++;
				}
				else if(snake[i].grow < 0){
					snake[i].size += snake[i].grow;
				}
				snake[i].grow--;
				while(snake[i].grow < 0){
					//Remove old tail
					SetTile(snake[i].tailx+2,
						snake[i].taily+4,
						EMPTY_TILE);
					switch(getPos(snake[i].tailx,
						snake[i].taily)){
						case 0:
							setPos(snake[i].tailx,
								snake[i].taily--
								,EMPTY);
							break;

						case 1:
							setPos(snake[i].tailx++,
								snake[i].taily,
								EMPTY);
							break;

						case 2:
							setPos(snake[i].tailx,
								snake[i].taily++
								,EMPTY);
							break;

						case 3:
							setPos(snake[i].tailx--,
								snake[i].taily,
								EMPTY);
							break;
					}
					//Fix tails
					if(snake[i].tailx > 55){
						snake[i].tailx = 0;
					}
					else if(snake[i].tailx < 0){
						snake[i].tailx = 55;
					}
					if(snake[i].taily > 21){
						snake[i].taily = 0;
					}
					else if(snake[i].taily < 0){
						snake[i].taily = 21;
					}
					//Place new tail
					SetTile(snake[i].tailx+2,
						snake[i].taily+4,
						TAIL_TILE+getPos(
						snake[i].tailx, snake[i].taily)
						+PLAYEROFFSET*i);
					snake[i].grow++;
				}
				//Reverse happens after the snake moved
				if(object == REVERSE){
					if(sound & 2){
						TriggerFx(3,0xff,1);
					}
					reverseSnake(i);
					drawField(players);
					snake[i].score += 5;
					itemTimer = 0;
				}
			}
			snake[i].counter++;
			btnPrev[i] = btnHeld[i];
		}
	}
	//Game ended, show winner
	if(won < 4){
		if(sound & 2){
			TriggerFx(5,0xff,1);
		}
		drawField(players);
		if(players == 1){
			usPrint(26,11,strGO);
		}
		else {
			if(won == 1){
				usPrint(24,11,str1won);
			}
			else if(won == 2){
				usPrint(24,11,str2won);
			}
			else {
				usPrint(26,11,strDraw);
			}
		}
		while(ReadJoypad(0) != BTN_START);
	}
}

unsigned int requestValue(unsigned int value, unsigned int max,
	unsigned int min){
	usPrintNum(VALUESX,OPTIONSY,
		value);
	while(ReadJoypad(0));
	do{
		WaitVsync(1);
		btnHeld[0] = ReadJoypad(0);
		btnPressed[0] = btnHeld[0] & (btnHeld[0] ^ btnPrev[0]);
		if(btnPressed[0] & BTN_RIGHT){
				value++;
		}
		else if(btnPressed[0] & BTN_LEFT){
				value--;
		}
		else if(btnHeld[0] & BTN_UP){
				value++;
		}
		else if(btnHeld[0] & BTN_DOWN){
				value--;
		}
		if (value > max){
			value = min;
		}
		else if (value < min){
			value = max;
		}
		usPrintNum(VALUESX,OPTIONSY,
			value);
		btnPrev[0] = btnHeld[0];
	}while(btnPressed[0] != BTN_B);
return value;
}

int main(){
	//Do all the basic stuff
	InitMusicPlayer(patches);
	SetMasterVolume(0xb0);
	SetTileTable(tileset);
	SetFontTilesIndex(NUM_NON_FONT_TILES);
	ClearVram();
	//Intro
	DrawMap2(16,11,uze);
	DrawMap2(31,11,box);
	WaitVsync(30);
	TriggerFx(6,0xff,1);
	WaitVsync(90);
	//Main loop
	while (1){
		switch (menu()) {
			case 0 :
				//Start one player game
				game(1,0);
				break;

			case 1 :
				//2 player normal game
				game(2,0);
				break;

			case 2 :
				//2 player speedup game
				game(2,1);
				break;
				
			case 3 :
				//2 player growth game
				//Read the size
				ClearVram();
				usPrint(OPTIONSX,OPTIONSY,strSize);
				size = requestValue(size,MAXSIZE,MINSIZE);
				game(2,2);
				break;

			case 4 :
				//2 player timed game
				//Read the time
				ClearVram();
				usPrint(OPTIONSX,OPTIONSY,strMinutes);
				usPrintNum(VALUESX,OPTIONSY,
					minutes);
				minutes = requestValue(minutes,MAXTIME,MINTIME);
				game(2,3);
				break;
		}
	}
} 
