#ifndef MOVE_TASK_H
#define MOVE_TASK_H

#include <avr/io.h>
#include "io.c"
#include "analog_stick.h"
#include "enemy.h"
#include "obstacle.h"

#define USER_CHAR '@'
#define EMPTY ' '
#define START_SPACE 17

unsigned char jump_flag = 0;
unsigned char user_death_flag = 0;
unsigned char char_update = 0;
unsigned char num_enemy_killed = 0;
unsigned char on_obstacle = 0;
unsigned char objCollision = 0;
//flags to scroll screen after user hits a certain point

unsigned char characterpos = START_SPACE; //Start the character at the bottom left 
enum move_sm {move_init, move_wait, move_movement, move_jump, move_fall,  move_on_obstacle, move_obstacle_movement};

void checkObstacleCollision(unsigned char);

unsigned char move_character(unsigned char pos, unsigned char direction) {
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
		checkObstacleCollision(pos+1);
		if (pos < 17) {
			if(!objCollision){
				pos = ((pos + 1) < 11) ? pos + 1 : pos;
			}
		}
		else {
			if(!objCollision){
				pos = ((pos + 1) < 27) ? pos + 1 : pos;
			}
		}
		objCollision = 0;
		break;
	case LEFT:
		checkObstacleCollision(pos-1);
		if (pos > 16) {
			if(!objCollision){
				pos = ((pos - 1) < 17) ? pos : pos - 1;
			}
		}
		else {
			if(!objCollision){
				pos = ((pos - 1) < 1) ? pos : pos - 1;
			}
		}
		objCollision = 0;
		break;
	default:
		pos = pos;
		break;
	}

	return pos;

}

void checkEnemyHit(void){
	for ( unsigned char i = 0; i < MAX_ENEMIES; i++){
		if(enemyarray[i] == INVALID_ENEMY_INDEX){
			continue;
		}
		if (characterpos == enemyarray[i]){
			user_death_flag = 1;
		}
	}
}

void checkObstacleCollision(unsigned char pos){
	for ( unsigned char i = 0; i < MAX_OBJECTS; i++ ){
		if (objarray[i] == INVALID_OBJ_INDEX){
			continue;
		}
		if (pos == objarray[i] ){
			objCollision = 1;
			return;
		}
	}
	objCollision = 0;
}

void checkStomp(void){
	for ( unsigned char i = 0; i < MAX_ENEMIES; i++){
		if(enemyarray[i] == INVALID_ENEMY_INDEX) continue;
		if(characterpos == enemyarray[i]){
			enemy_death_flag = 1;
			enemy_num_died = i;
			num_enemy_killed++;
		}
	}
}

void checkOnObstacle(void){
	for ( unsigned char i = 0; i < MAX_OBJECTS; i++ ){
		if ( objarray[i] == INVALID_OBJ_INDEX ) continue;
		if ( objarray[i] == (characterpos + 16)){
			on_obstacle = 1;
			return;
		}
	}
	on_obstacle = 0;
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
			if(!on_obstacle){
				state = move_fall;
			} else {
				state = move_on_obstacle;
			}
			break;
		case move_fall:
			state = move_wait;
			break;
		case move_on_obstacle:
			if(xval && on_obstacle){
				state = move_obstacle_movement;
			}
			 else if (on_obstacle) {
				state = move_on_obstacle;
			}
			else {
				state = move_fall;
			}
			break;
		case move_obstacle_movement:
			if(on_obstacle){
				state = move_on_obstacle;
			} else {
				state = move_fall;
			}
			break;
		default:
			state = move_init;
			break;
	}

	switch (state) {
		case move_init:
			characterpos = START_SPACE;
			break;
		case move_wait:
			break;
		case move_movement:
			characterpos = move_character(characterpos, xval);
			if(characterpos >= 26){
				shift_enemy_flag = 1;
				shift_obj_flag = 1;
				characterpos--;
			}
			checkEnemyHit();
			break;
		case move_jump:
			characterpos = move_character(characterpos, UP);
			characterpos = move_character(characterpos, xval);
			checkOnObstacle();
			if(characterpos >= 26){
				shift_enemy_flag = 1;
				shift_obj_flag = 1;
				characterpos--;
			}
			else if(characterpos < 17 && characterpos >= 9){
				shift_enemy_flag = 1;
				shift_obj_flag = 1;
				characterpos--;
			}
			break;
		case move_fall:
			characterpos = move_character(characterpos, DOWN);
			characterpos = move_character(characterpos, xval);
			checkStomp();
			if(characterpos >= 26){
				shift_enemy_flag = 1;
				shift_obj_flag = 1;
				characterpos--;
			}
			break;
		case move_on_obstacle:
			checkOnObstacle();
			break;
		case move_obstacle_movement:
			characterpos = move_character(characterpos, xval);
			checkOnObstacle();
			if(!on_obstacle){
				characterpos = move_character(characterpos, DOWN);
				checkStomp();
			}
			if(characterpos >= 26){
				shift_enemy_flag = 1;
				shift_obj_flag = 1;
				characterpos--;
			}
			else if(characterpos < 17 && characterpos >= 9){
				shift_enemy_flag = 1;
				shift_obj_flag = 1;
				characterpos--;
			}
			break;
		default:
			break;
	}
	
	PORTB = state;
	return state;
}
#endif