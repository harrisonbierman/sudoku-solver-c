#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int tile_entropy[9][9][9];
static int tile_entropy_count[9][9];
static int is_unsolved = 1;

static int attempts = 0;

static int game_state [9][9] =  {
						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 },
						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 },
						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 },

						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 },
						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 },
						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 },

						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 },
						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 },
						 { 0, 0, 0,  0, 0, 0,  0, 0, 0 }
						};

static int starting_state_easy [9][9] = {
						 { 5, 2, 0,  0, 0, 0,  1, 3, 0 },
						 { 3, 0, 0,  7, 0, 0,  9, 0, 0 },
						 { 0, 9, 8,  5, 0, 0,  6, 0, 2 },

						 { 0, 8, 3,  0, 9, 0,  2, 0, 5 },
						 { 0, 0, 0,  8, 1, 3,  0, 0, 0 },
						 { 9, 0, 1,  0, 5, 0,  3, 8, 0 },

						 { 8, 0, 0,  0, 0, 6,  7, 2, 0 },
						 { 0, 0, 7,  0, 0, 9,  0, 0, 4 },
						 { 0, 4, 2,  0, 0, 0,  0, 9, 1 }
						};

static int starting_state [9][9] =  {
						 { 0, 8, 0,  0, 4, 0,  0, 0, 2 },
						 { 9, 0, 0,  8, 0, 0,  0, 0, 0 },
						 { 6, 5, 0,  0, 0, 0,  9, 0, 3 },

						 { 0, 6, 0,  3, 0, 0,  5, 4, 0 },
						 { 0, 0, 0,  4, 0, 6,  0, 0, 0 },
						 { 0, 9, 3,  0, 0, 7,  0, 1, 0 },

						 { 5, 0, 8,  0, 0, 0,  0, 2, 1 },
						 { 0, 0, 0,  0, 0, 4,  0, 0, 5 },
						 { 2, 0, 0,  0, 7, 0,  0, 6, 0 }
						};

static void remove_entropy(int to_remove, int y, int x)
{
	for (int i = 0; i < 9; i++) {
		if (tile_entropy[y][x][i] == to_remove) {
			tile_entropy[y][x][i] = 0;
			tile_entropy_count[y][x] -= 1;	
		}
	}

}



/* 
 * This function will first look at the game state and 
 * for every tile that has been written into, it will 
 * collapse its entropy. Then it will go over every empty
 * tile and reduce its entropy base on the tiles in the 
 * row, column, and box that it belongs to.
 */
static void update_tile_entropy() 
{
	// for each tile
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {

			// if tile is empty
			if (game_state[y][x] == 0) {
				
				// compare by row
				for (int i = 0; i < 9; i++) {
					if (game_state[y][i] != 0) {
						remove_entropy(game_state[y][i], y, x);
					}
				}

				// compare by column
				for (int i = 0; i < 9; i++) {
					if (game_state[i][x] != 0) {
						remove_entropy(game_state[i][x], y, x);
					}
				}

				// compare by box
				// I found this neat trick to snap up or down by 3 tiles
				// so the respective tile only compares with the box it is
				// associated with
				for (int box_y = (y/3)*3; box_y < ((y/3)*3)+3; box_y++) {
					for (int box_x = (x/3)*3; box_x < ((x/3)*3)+3; box_x++) {
						if (game_state[box_y][box_x] != 0) {
							remove_entropy(game_state[box_y][box_x], y, x);
						}
					}
				}
			}
		}
	}
}


static void print_game_state() 
{
	printf("\n===================");
	for (int y = 0; y < 9; y++) {
		if (y % 3 == 0) {
			printf("\n");
		}
		for (int x = 0; x < 9; x++) {
			printf("%d ", game_state[y][x]);

			// box spacer
			if ((x+1) % 3 == 0) {
				printf(" ");
			}
		}
		printf("\n");
	}
	printf("===================\n");
}

static void set_state_to_starting() 
{
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			game_state[y][x] = starting_state[y][x];
		}
	}
}

static void set_board_entropy_to_max()
{
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
				tile_entropy_count[y][x] = 9;
			for (int z = 0; z < 9; z++) {
				tile_entropy[y][x][z] = z + 1;
			}
		}
	}
}

static void write_in_tile()
{
	// find the tile with the lowest entropy
	is_unsolved = 0;
	int curr_low_num = 10;
	int curr_low_y;
	int curr_low_x;

	// for each tile
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {

			// if tile is lower than the previous low tile and the tile is not written into
			if (tile_entropy_count[y][x] < curr_low_num && game_state[y][x] == 0) {
				curr_low_num = tile_entropy_count[y][x];
				curr_low_y = y;
				curr_low_x = x;
				 
				// if there are no tiles that are zero, unsolved would be false;
				is_unsolved++;
			}
		}
	}

	// now we will chose from entropy list of chosen tile 
	// and collapse to a random possibility
	if(is_unsolved) {

		int possible[9] = {0};
		int count = 0;
		// go through each possibility on tiles entropy list
		for (int i = 0; i < 9; i++) {
			// if possibility is possible
			if (tile_entropy[curr_low_y][curr_low_x][i] != 0) {
				// creates array where all the possibilities are on the left
				// filled with zeros on the right ex [1, 4, 5, 0, 0, 0, 0, 0, 0]
				possible[count] = tile_entropy[curr_low_y][curr_low_x][i];
				count++;
			}
		}
		// if there are no possibilities for empty tile
		// meaning an unsolvable position.
		// just reset and retry for now cause I don't 
		// want to make a whole history backtracking system right now.
		if (count == 0) {
			set_state_to_starting();
			set_board_entropy_to_max();
			attempts++;
			return;
		}

		int chosen_number = possible[rand() % count];
		// store new number in game state
		game_state[curr_low_y][curr_low_x] = chosen_number;

	} 

}

int main() 
{
	set_state_to_starting();
	set_board_entropy_to_max();

	// initiates for rand function
	srand(time(NULL));

	/*
	 * all tiles start with their 
	 * entropy being all 1-9 states
	 * tiles possible states [1, 2, 3, 4, 5, 6, 7, 8, 9]
	 */

	printf("\n  Unsolved Puzzle");
	print_game_state();

	while(is_unsolved){
		update_tile_entropy();
		write_in_tile();
	}

	printf("\n   Solved Puzzle");
	print_game_state();
	printf("\n Solve Attemps: %d", attempts);
}
