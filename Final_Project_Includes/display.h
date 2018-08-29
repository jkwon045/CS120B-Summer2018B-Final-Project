#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>
#include "enemy.h"
#include "obstacle.h"
#include "move_task.h"
#include "io.c"
#define MAXDIGITS 5

//Place objects, then enemies, then character
enum disp_states {disp_init, disp_update, disp_death_screen, disp_play_again};

char msg[16] = "Score: ";
char highScoreMsg[16] = "High Score: ";

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
	while(msg[j] != '\0' && j < (16 - MAXDIGITS)) j++;
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

int disp_tick(int state) {
	switch (state) {
		case disp_init:
			state = disp_update;
			break;
		case disp_update:
			if(user_death_flag){
				state = disp_death_screen;
				clearScreen();
				createMessage(score);
				createHighScoreMessage(highScore);
				LCD_WriteString(1, (const unsigned char*)msg);
				LCD_WriteString(17, (const unsigned char*)highScoreMsg);
			} else {
				state = disp_update;
			}
			break;
		case disp_death_screen:
			state = disp_death_screen;
			break;
		case disp_play_again:
			break;
		default:
			state = disp_init;
			break;
	}

	switch (state) {
		case disp_init:
			break;
		case disp_update:
			clearScreen();
			insertEnemy();
			insertObject();
			insertCharacter();
			break;
		case disp_death_screen:
			if(score < highScore){
				highScore = score;
				saveScore(highScore);
			}
			break;
		case disp_play_again:
			break;
		default:
			break;
	}

	return state;		
}



#endif