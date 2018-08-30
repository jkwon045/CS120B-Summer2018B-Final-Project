#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>
#include "enemy.h"
#include "obstacle.h"
#include "move_task.h"
#include "io.c"
#define MAXDIGITS 5
#define START_BUTTON ~PINB & 0x01
#define RESET_BUTTON (~PINB & 0x02) >> 1
#define sec5 25

//Place objects, then enemies, then character
enum disp_states {disp_init, disp_menu, disp_filter_start, disp_filter_reset, disp_update, disp_death_screen, disp_play_again};

char msg[16] = "Score: ";
char highScoreMsg[16] = "High Score: ";
unsigned char disp_counter = 0;
unsigned char start = 0;
char menu[16] = "Press to start!";
char playagain[16] = "Play Again?";
unsigned long timer_cnt = 0;
unsigned char seeded = 0;

void insertObject(void) {
	for (unsigned char i = 0; i < MAX_OBJECTS; i++) {
		if (objarray[i] != INVALID_OBJ_INDEX) {
			LCD_Cursor(objarray[i]);
			LCD_WriteData(OBJ_CHAR);
		}
	}
	LCD_Cursor_Off();
}

void insertEnemy(void) {
	for (unsigned char i = 0; i < MAX_ENEMIES; i++) {
		if (enemyarray[i] != INVALID_ENEMY_INDEX && enemyarray[i] < 33 && enemyarray[i] > 16) {
			LCD_Cursor(enemyarray[i]);
			LCD_WriteData(ENEMY_CHAR);
		}
	}
	
	LCD_Cursor_Off();
}

void insertCharacter(void) {
	LCD_Cursor(characterpos);
	LCD_WriteData(USER_CHAR);
	LCD_Cursor_Off();
}

void clearScreen(void){
	for( unsigned i = 1; i < 33; i++){
		LCD_Cursor(i);
		LCD_WriteData(' ');
	}
}

void createMessage(unsigned char score){
	unsigned char scoreString[MAXDIGITS] = {};
	unsigned char j = 0;
	while(msg[j] != '\0' && j < 7) j++;
	
	short i = 0;
	if (score == 0 ) {
		msg[j] = '0';
		msg[j+1] = '\0';
		return;
	}
	while(score > 0 ){
		scoreString[i] = score%10 + '0';
		score = score/10;
	}
	
	for(; i >= 0; i--){
		msg[j++] = scoreString[i];
	}
	msg[j] = '\0';
}

void createHighScoreMessage(unsigned char score){
	unsigned char scoreString[MAXDIGITS] = {};

	unsigned char j = 0;
	while(highScoreMsg[j] != '0' && j < (16 - MAXDIGITS) ) j++;
	short i = 0;
	if (score == 0 ){
		highScoreMsg[j] = '0';
		highScoreMsg[j+1] = '\0';
		return;
	}
	while(score > 0 ){
		scoreString[i] = score%10 + '0';
		score = score/10;
	}
	
	for(; i >= 0; i--){
		highScoreMsg[j++] = scoreString[i];
	}
	highScoreMsg[j] = '\0';
}

void setResetFlags(void){
	enemy_reset_flag = 1;
	obj_reset_flag = 1;
	char_reset_flag = 1;
}

void setStartFlags(void){
	obj_start = 1;
	enemy_start = 1;
	char_start = 1;
	if(!seeded){
		srand(timer_cnt);
		seeded = 1;
	}
}

int disp_tick(int state) {
	switch (state) {
		case disp_init:
			state = disp_menu;
			LCD_DisplayString(1, (const unsigned char*)menu);
			break;
		case disp_menu:
			timer_cnt++;
			if(START_BUTTON){
				state = disp_filter_start;
			}
			else if (RESET_BUTTON){
				state = disp_filter_reset;
			}
			break;
		case disp_filter_start:
			if(START_BUTTON){
				state = disp_update;
				setStartFlags();
			}
			else{
				state = disp_menu;
			}
			break;
		case disp_filter_reset:
			if(RESET_BUTTON){
				state = disp_init;
				setResetFlags();
				score = 0;
				user_death_flag = 0;
			}
			else{
				state = disp_menu;
			}
			break;
		case disp_update:
			timer_cnt++;
			if(RESET_BUTTON){
				state = disp_filter_reset;
			}
			else if(user_death_flag){
				state = disp_death_screen;
				clearScreen();
				if(score > highScore){
					highScore = score;
					saveScore(highScore);
				}
				createMessage(score);
				createHighScoreMessage(highScore);
				LCD_WriteString(1, (const unsigned char*)msg);
				LCD_WriteString(17, (const unsigned char*)highScoreMsg);
			} else {
				state = disp_update;
			}
			break;
		case disp_death_screen:
			timer_cnt++;
			if(RESET_BUTTON){
				state = disp_filter_reset;
			}
			else if ( disp_counter >= sec5){
				state = disp_play_again;
				LCD_DisplayString(1, (const unsigned char*)playagain);
				disp_counter = 0;
			}
			break;
		case disp_play_again:
			timer_cnt++;
			if(RESET_BUTTON){
				state = disp_filter_reset;
			}
			else if ( START_BUTTON ) {
				state = disp_filter_reset;
				setResetFlags();
				score = 0;
				user_death_flag = 0;
			}
			else if ( disp_counter >= sec5){
				LCD_WriteString(1, (const unsigned char*)msg);
				LCD_WriteString(17, (const unsigned char*)highScoreMsg);
				 state = disp_death_screen;
				 disp_counter = 0;
			}
			break;
		default:
			state = disp_init;
			break;
	}

	switch (state) {
		case disp_init:
			break;
		case disp_menu:
			break;
		case disp_filter_start:
			break;
		case disp_filter_reset:
			break;
		case disp_update:
			clearScreen();
			insertEnemy();
			insertObject();
			insertCharacter();
			break;
		case disp_death_screen:
			disp_counter++;
			break;
		case disp_play_again:
			disp_counter++;
			break;
		default:
			break;
	}

	return state;		
}



#endif