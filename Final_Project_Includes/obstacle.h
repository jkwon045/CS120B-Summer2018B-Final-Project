#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <avr/io.h>
#include <stdlib.h>
#include "io.c"

#define MAX_OBJECTS 4
#define OBJ_CHAR 0xFF
#define INVALID_OBJ_INDEX 33
#define SECOND_ROW_OFFSET 16

//Flag set by move_task
unsigned char shift_obj_flag = 0;

//Array to hold locations of obstacles
char objarray[MAX_OBJECTS]; //18 indicates there's no object initialized
unsigned char num_objs = 0;

enum obj_state {obj_init, obj_wait, obj_shift};

void initOBJ(void) {
	unsigned char numObj = rand() % MAX_OBJECTS;
	unsigned char location;
	unsigned char j = 0, save;
	for( unsigned char i = 0; i < MAX_OBJECTS; i++){
		objarray[i] = INVALID_OBJ_INDEX;
	}

	for (unsigned char i = 0; i < numObj; i++) {
		location = rand()%16;
		save = 1;
		for (unsigned char k = 0; k < j; k++) {
			if (objarray[k] == location) save = 0;
		}
		if(save) objarray[j++] = location + 2 + SECOND_ROW_OFFSET; //Save off the object if it's not a repeat
	}
	num_objs = j;
}

void shiftObjects(void) {
	unsigned char n = 1;
	for (unsigned char i = 0; i < MAX_OBJECTS; i++) {
		unsigned char temp = objarray[i];
		if (temp == INVALID_OBJ_INDEX ) continue;
		if (temp - 1 < 17) {
			objarray[i] = INVALID_OBJ_INDEX;
			num_objs--;
		}
		else {
			objarray[i] = temp - 1;
		}
		LCD_Cursor(n++);
		LCD_WriteData((objarray[i]/10)+'0');
		LCD_Cursor(n++);
		LCD_WriteData((objarray[i]%10)+'0');
	}	
}

//Call only after shifting objects
void generateNewObj(void) {
	if(num_objs >= MAX_OBJECTS) return;
	
	unsigned char gen = rand() % 6; //16.7% chance to generate a new object
	if (gen == 0) {
		unsigned char i = 0;
		unsigned char temp = objarray[i];
		for (; i < MAX_OBJECTS && temp != INVALID_OBJ_INDEX; i++) {
			temp = objarray[i];
		}
		objarray[i] = INVALID_OBJ_INDEX-1;
		num_objs++;
	}
}

int obj_tick(int state){
	
	switch(state){
		case obj_init:
			state = obj_wait;
			break;
		case obj_wait:
			if(shift_obj_flag){
				state = obj_shift;
			}
			break;
		case obj_shift:
			state = obj_wait;
			break;
		default:
			state = obj_init;
			break;
	}
	
	switch(state){
		case obj_init:
			initOBJ();
			break;
		case obj_wait:
			break;
		case obj_shift:
			shiftObjects();			
			generateNewObj();
			shift_obj_flag = 0;
			break;
		default:
			break;
	}
	
	return state;
}

#endif