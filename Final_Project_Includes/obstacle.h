#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <avr/io.h>
#include <stdlib.h>
#include "io.c"

#define MAX_OBJECTS 4
#define OBJ_CHAR 0xFF

unsigned char objarray[MAX_OBJECTS];
unsigned char num_objs = 0;

void initOBJ(void) {
	unsigned char numObj = rand() % MAX_OBJECTS;
	unsigned char location;
	unsigned char j = 0, save;

	for (unsigned char i = 0; i < numObj; i++) {
		location = rand()%16;
		if ( (location + 2) > 1 ){ // starting location is leftmost area, so don't spawn obj there
			save = 0;
			for (unsigned char k = 0; k < j; k++) {
				if (objarray[k] == location) {
					save = 1;
				}
			}
			if(save) objarray[j++] = location; //Save off the object if it's not a repeat
		}
	}
	num_objs = j;
}



#endif