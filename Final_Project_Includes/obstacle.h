#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <avr/io.h>
#include <stdlib.h>
#include "io.c"

#define MAX_OBJECTS 4
#define OBJ_CHAR 0xFF
#define INVALID_OBJ_INDEX 33
#define SECOND_ROW_OFFSET 16

//Array to hold locations of obstacles
char objarray[MAX_OBJECTS] = { INVALID_OBJ_INDEX }; //18 indicates there's no object initialized
unsigned char num_objs = 0;

void initOBJ(void) {
	unsigned char numObj = rand() % MAX_OBJECTS;
	unsigned char location;
	unsigned char j = 0, save;

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
	for (unsigned char i = 0; i < num_objs; i++) {
		if (objarray[i] != INVALID_OBJ_INDEX && ((objarray[i] - 1) < 1)) {
			objarray[i] = INVALID_OBJ_INDEX;
			num_objs--;
		}
		else {
			objarray[i]--;
		}
	}	
}

//Call only after shifting objects
void generateNewObj(void) {
	unsigned char gen = rand() % 6; //16.7% chance to generate a new object

	if (gen == 0 && num_objs < MAX_OBJECTS) {
		unsigned char i = 0;
		for (; i < MAX_OBJECTS && objarray[i] != INVALID_OBJ_INDEX; i++) {}
		objarray[i] = INVALID_OBJ_INDEX-1;
		num_objs++;
	}
}

#endif