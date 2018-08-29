#ifndef ENEMY_H
#define ENEMY_H

#include <avr/io.h>
#include <stdlib.h>
#include "io.c"
#include "obstacle.h"
#include "move_task.h"

#define MAX_ENEMIES 3
#define ENEMY_CHAR 1
#define INVALID_ENEMY_INDEX 34
//Second row offset is 16, defined in obstacle.h

unsigned char enemy_death_flag = 0;
unsigned char enemy_num_died = MAX_ENEMIES;

//Flag set by move task
unsigned char shift_enemy_flag = 0;

//counter for how long enemy should stay still/move in ticks
#define second3 15
unsigned char speed = second3;
unsigned char enemyticks = 0;
unsigned char enemyUpdate = 1;

//defining movement directions for enemies

enum enemy_move_states {enemy_move_init, enemy_move_wait, enemy_move_move, enemy_move_shift}; //Probably want a 300 ms period, basically slower than user input


static char enemyarray[MAX_ENEMIES] = { INVALID_ENEMY_INDEX };
unsigned char num_enemy = 0;

void checkEnemyHit();

void initENEMY(void) {
	unsigned char genENEMY = rand() % MAX_ENEMIES;
	unsigned char location, save; //Save is a boolean to indicate whether or not it should be saved.
	unsigned char j = 0;
	
	for (unsigned char i = 0; i < MAX_ENEMIES; i++){
		enemyarray[i] = INVALID_ENEMY_INDEX;
	}

	for (unsigned char i = 0; i < genENEMY; i++) {
		location = ((rand() % 15)+3+SECOND_ROW_OFFSET); // Gives value from 19 - 33
	
		save = 1;
		for (unsigned char k = 0; k < MAX_ENEMIES; k++) {
			
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
	for (unsigned char i = 0; i < MAX_ENEMIES; i++) {
		unsigned char temp = enemyarray[i];
		if (temp == INVALID_ENEMY_INDEX){
			continue;
		}
		if ((temp-1) < 17) {
			enemyarray[i] = INVALID_ENEMY_INDEX;
			num_enemy--;
		}
		else {
			enemyarray[i] = temp - 1;
		}	
	}
}

//Call after shifting enemies
void generateNewEnemy(void) {
	unsigned char gen = rand()%5; //20% chance to generate an enemy
	
	if (gen == 0 && num_enemy < MAX_ENEMIES) {
		unsigned char i = 0;

		for (; i < MAX_ENEMIES && enemyarray[i] != INVALID_ENEMY_INDEX; i++) {}
		enemyarray[i] = INVALID_ENEMY_INDEX - 1;
		num_enemy++;
	}
}

void enemyMove(unsigned char index, char direction) {
	if (enemyarray[index] == INVALID_ENEMY_INDEX) {
		return;
	}
	
	unsigned char temp = enemyarray[index] + direction;
	
	if (temp == INVALID_ENEMY_INDEX) { //16 is the index of the last element of the first row
		return;
	}
	
	if (temp == 15){
		enemyarray[index] = 34;
		num_enemy--;
		return;
	}
	
	for (unsigned char i = 0; i < MAX_OBJECTS; i++) {
		//If the enemy is going to move off into a direction that isn't within the valid list of areas where it can move
		//If the enemy is going to move into an obstacle
		if ((objarray[i] != INVALID_OBJ_INDEX) && (temp == objarray[i])) {
			return;
		}
	}
	//Checking for stacking of enemies, and prevents from moving if the direction causes an enemy to stack onto another
	for (unsigned char i = 0; i < MAX_ENEMIES; i++) {
		if (i == index || enemyarray[i] == INVALID_ENEMY_INDEX) {
			continue;
		}
		
		if((temp) == enemyarray[i]){
			return;
		}
	}
	
	enemyarray[index] += direction;
}

void removeEnemy(unsigned char index){
	num_enemy--;
	enemyarray[index] = INVALID_ENEMY_INDEX;
	
}

//State machine for enemy movement
int enemy_tick(int state) {
	switch (state) {
		case enemy_move_init:
			state = enemy_move_wait;
			break;
		case enemy_move_wait:
			if (enemyticks == speed) {
				state = enemy_move_move;
				enemyticks = 0;
			}
			else if (shift_enemy_flag){
				state = enemy_move_shift;
			}
			else {
				state = enemy_move_wait;
				enemyticks++;
			}
			break;
		case enemy_move_move:
			state = enemy_move_wait;
			break;
		case enemy_move_shift:
			state = enemy_move_wait;
			break;
		default:
			state = enemy_move_init;
			break;
	}

	switch (state) {
		case enemy_move_init:
			break;
		case enemy_move_wait:
			if(enemy_death_flag){
				removeEnemy(enemy_num_died);
				enemy_death_flag = 0;
			}
			break;
		case enemy_move_move:
			for (unsigned char i = 0; i < MAX_ENEMIES; i++) {
				if (enemyarray[i] != INVALID_ENEMY_INDEX) {
					unsigned char chanceMove = rand()%2;
					if( chanceMove ){
						unsigned char direction = (rand() % 2 == 0) ? -1 : 1;
						enemyMove(i, direction);
					}
				}
			}
			checkEnemyHit();
			break;
		case enemy_move_shift:
			shiftEnemies();
			generateNewEnemy();
			shift_enemy_flag = 0;
			break;
		default:
			break;
	}
	return state;
}
#endif
