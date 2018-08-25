#ifndef MOVE_TASK_H
#define MOVE_TASK_H

#include <avr/io.h>
#include "io.c"
#include "analog_stick.h"

#define USER_CHAR '@'
#define EMPTY ' '
#define START_SPACE 17

unsigned char jump_flag = 0;
unsigned char characterpos = START_SPACE; //Start the character at the bottom left 
enum move_sm {move_init, move_wait, move_movement, move_jump, move_fall};

unsigned char move_character(unsigned char pos, unsigned char direction) {
	LCD_Cursor(pos);
	LCD_WriteData(EMPTY);

	switch (direction) {
	case UP:
		pos = ((pos - 16) < 0) ? pos : pos - 16;
		jump_flag = 1;
		break;
	case DOWN:
		pos = ((pos + 16) > 32) ? pos : pos + 16;
		jump_flag = 0;
		break;
	case RIGHT:
		if (pos < 17) {
			pos = ((pos + 1) < 17) ? pos + 1 : pos;
		}
		else {
			pos = ((pos + 1) < 33) ? pos + 1 : pos;
		}
		break;
	case LEFT:
		if (pos > 16) {
			pos = ((pos - 1) < 17) ? pos : pos - 1;
		}
		else {
			pos = ((pos - 1) < 1) ? pos : pos - 1;
		}
		break;
	default:
		pos = pos;
		break;
	}

	LCD_Cursor(pos);
	LCD_WriteData(USER_CHAR);
	LCD_Cursor_Off();
	return pos;

}

int move_tick(int state) {
	unsigned char yval = readYAxis();
	unsigned char xval = readXAxis();
	switch (state) {
	case move_init:
		state = move_wait;
		break;
	case move_wait:
		if (yval == xval) { //In this case they both return NONE which indicates no movement
			state = move_wait;
		}
		else if(yval == UP){
			state = move_jump;
		}
		else if (xval){
			state = move_movement;
		}
		break;
	case move_movement:
		state = move_wait;
		break;
	case move_jump:
		state = move_fall;
		break;
	case move_fall:
		state = move_wait;
		break;
	default:
		state = move_init;
		break;
	}

	switch (state) {
	case move_init:
		characterpos = START_SPACE;
		LCD_Cursor(characterpos);
		LCD_WriteData(USER_CHAR);
		LCD_Cursor_Off();
		break;
	case move_wait:
		break;
	case move_movement:
		characterpos = move_character(characterpos, xval);
		break;
	case move_jump:
		characterpos = move_character(characterpos, UP);
		characterpos = move_character(characterpos, xval);
		break;
	case move_fall:
		characterpos = move_character(characterpos, DOWN);
		characterpos = move_character(characterpos, xval);
	default:
		break;
	}

	return state;

}

#endif