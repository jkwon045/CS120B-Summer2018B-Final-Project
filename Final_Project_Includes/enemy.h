#ifndef ENEMY_H
#define ENEMY_H

#include <avr/io.h>
#include <stdlib.h>
#include "io.c"
#include "obstacle.h"

#define MAX_ENEMIES 3
#define ENEMY_CHAR '#'
#define INVALID_ENEMY_INDEX 34
//Second row offset is 16, defined in obstacle.h

//counter for how long enemy should stay still/move in ticks
#define second3 15
unsigned char speed = second3;
unsigned char enemyticks = 0;
unsigned char enemyUpdate = 1;

//defining movement directions for enemies

enum enemy_move_states {enemy_move_init, enemy_move_wait, enemy_move_move, enemy_move_shift}; //Probably want a 300 ms period, basically slower than user input


static char enemyarray[MAX_ENEMIES] = { INVALID_ENEMY_INDEX };
unsigned char num_enemy = 0;

void initENEMY(void) {
	unsigned char genENEMY = rand() % MAX_ENEMIES;
	unsigned char location, save; //Save is a boolean to indicate whether or not it should be saved.
	unsigned char j = 0;

	PORTB = genENEMY;
	for (unsigned char i = 0; i < MAX_ENEMIES; i++){
		enemyarray[i] = INVALID_ENEMY_INDEX;
	}

	for (unsigned char i = 0; i < MAX_ENEMIES; i++) {
		location = ((rand() % 15)+3+SECOND_ROW_OFFSET); // Gives value from 19 - 33
	
		save = 1;
		for (unsigned char k = 0; k < num_enemy; k++) {
			if (enemyarray[k] == location) save = 0;
		}
		for (unsigned char k = 0; k < MAX_OBJECTS; k++) {
			if (objarray[k] == location) save = 0; 
		}
		if (save) {
			enemyarray[j++] = location;
			
		}
	}
	num_enemy = j;
}

//Should shift objects, then shift enemies, otherwise enemies end up overwritten by objects
void shiftEnemies(void) {
	for (unsigned char i = 0; i < num_enemy; i++) {
		if ((enemyarray[i] != INVALID_ENEMY_INDEX) && ((enemyarray[i] - 1) < 17)) {
			enemyarray[i] = INVALID_ENEMY_INDEX;
			num_enemy--;
		}
		else {
			unsigned char move = 1;
			for (unsigned char j = 0; j < MAX_OBJECTS; j++) {
				if (enemyarray[i] == objarray[j]) {
					move = 0;
				}
			}
			if (move) enemyarray[i] = enemyarray[i] - 1;
		}	
	}
}

//Call after shifting enemies
void generateNewEnemy(void) {
	unsigned char gen = rand() % 5; //20% chance to generate an enemy

	if (gen == 0 && num_enemy < MAX_ENEMIES) {
		unsigned char i = 0;

		for (; i < MAX_ENEMIES && enemyarray[i] != INVALID_ENEMY_INDEX; i++) {}
		objarray[i] = INVALID_ENEMY_INDEX - 1;
		num_enemy++;
	}
}

void enemyMove(unsigned char index, char direction) {
	if (enemyarray[index] == INVALID_ENEMY_INDEX) {
		return;
	}

	unsigned char moveInvalid = 0;
	
	if (((enemyarray[index] + direction) == INVALID_ENEMY_INDEX) || (enemyarray[index] + direction) == 16) { //16 is the index of the last element of the first row
		moveInvalid = 1;
	}
	
	for (unsigned char i = 0; i < MAX_OBJECTS; i++) {
		//If the enemy is going to move off into a direction that isn't within the valid list of areas where it can move
		//If the enemy is going to move into an obstacle
		if ((objarray[i] != INVALID_OBJ_INDEX) && ((enemyarray[index] + direction) == objarray[i])) {
			moveInvalid = 1;
		}
	}
	//Checking for stacking of enemies, and prevents from moving if the direction causes an enemy to stack onto another
	for (unsigned char i = 0; i < MAX_ENEMIES; i++) {
		if ( (i != index) && ((enemyarray[index] + direction) == enemyarray[i])) {
			moveInvalid = 1;
		}
	}
	if (!moveInvalid) {
		enemyarray[index] += direction;
	}
}

//State machine for enemy movement
int enemy_tick(int state) {
	switch (state) {
		case enemy_move_init:
			state = enemy_move_wait;
			enemyUpdate = 0;
			break;
		case enemy_move_wait:
			if (enemyticks == speed) {
				state = enemy_move_move;
				enemyticks = 0;
			}
			else {
				state = enemy_move_wait;
				enemyticks++;
			}
			break;
		case enemy_move_move:
			state = enemy_move_wait;
			enemyUpdate = 0;
			break;
		case enemy_move_shift:
			state = enemy_move_wait;
			enemyUpdate = 0;
			break;
		default:
			state = enemy_move_init;
			break;
	}

	switch (state) {
		case enemy_move_init:
			initENEMY();
			enemyUpdate = 1;
			break;
		case enemy_move_wait:
			break;
		case enemy_move_move:
			/*for (unsigned char i = 0; i < MAX_ENEMIES; i++) {
				if (enemyarray[i] != INVALID_ENEMY_INDEX) {
					unsigned char chanceMove = rand()%2;
					if( chanceMove ){
						unsigned char direction = (rand() % 2 == 0) ? -1 : 1;
						enemyMove(i, direction);
					}
				}
			}*/
			
			enemyMove(0, 1);
			enemyMove(1, -1);
			enemyUpdate = 1;
			break;
		case enemy_move_shift:
			shiftEnemies();
			generateNewEnemy();
			enemyUpdate = 1;
			break;
		default:
			break;
	}

	return state;
}
#endif
