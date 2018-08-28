#ifndef DISPLAY_H
#define DISPLAY_H

#include <avr/io.h>
#include "enemy.h"
#include "obstacle.h"
#include "move_task.h"
#include "io.c"

//Place objects, then enemies, then character
enum disp_states {disp_init, disp_update};

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

int disp_tick(int state) {
	switch (state) {
		case disp_init:
			state = disp_update;
			break;
		case disp_update:
			state = disp_update;
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
		default:
			break;
	}

	return state;		
}

#endif