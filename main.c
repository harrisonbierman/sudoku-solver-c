#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct Coords {
	int y;
	int x;
}Coords;

static size_t cooldown = 0;

static int tile_entropy[9][9][9];
static int tile_entropy_count[9][9];
static int is_unsolved = 1;

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

static void collaps_tile(int collaps_to, int y, int x) 
{
	for (int i = 0; i < 9; i++) {
		if (tile_entropy[y][x][i] != collaps_to) {
			tile_entropy[y][x][i] = 0;
		}
	}
}

static void remove_entropy(int to_remove, int y, int x)
{
	for (int i = 0; i < 9; i++) {
		if (tile_entropy[y][x][i] == to_remove) {
			tile_entropy[y][x][i] = 0;
			tile_entropy_count[y][x] -= 1;	
		}
	}

}

static void update_tile_entropy() 
{
	// reset entropy for all tiles back to max and 
	// then this reductive function will get to back to where it should be
	// not the most elegant solution but it should work for now
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			for (int z = 0; z < 9; z++) {
				tile_entropy[y][x][z] = z + 1;
			}
		}
	}

	// reset the count back to 9
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			tile_entropy_count[y][x] = 9;
		}
	}

	// check for collapsed tiles
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			if (game_state[y][x] != 0) {
				collaps_tile(game_state[y][x], y, x);
			}
		}
	}

	// for each tile
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {

			// if tile is empty
			if (game_state[y][x] == 0) {
				
				// update by row
				for (int i = 0; i < 9; i++) {
					if (game_state[y][i] != 0) {
						remove_entropy(game_state[y][i], y, x);
					}
				}

				// update by column
				for (int i = 0; i < 9; i++) {
					if (game_state[i][x] != 0) {
						remove_entropy(game_state[i][x], y, x);
					}
				}

				// update by box
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
	printf("\n=====================\n");
	for (int y = 0; y < 9; y++) {
		printf("\n");
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
	}
	printf("\n=====================\n");
}

static void set_state_to_starting() 
{
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			game_state[y][x] = starting_state[y][x];
		}
	}
}

static void write_in_number()
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
			return;
		}

		int chosen_number = possible[rand() % count];
		// store new number in game state
		game_state[curr_low_y][curr_low_x] = chosen_number;

	} else {
		printf("\nPuzzle Solved\n");
		print_game_state();
	}
}

int main() 
{


	
	set_state_to_starting();

	srand(time(NULL));

	/*
	 * all tiles start with their 
	 * entropy being all 1-9 states
	 */
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			for (int z = 0; z < 9; z++) {
				tile_entropy[y][x][z] = z + 1;
			}
		}
	}

	// the count of possible states all start at 9
	// keeps track as quick reference for picking
	// the tile with the lowest entropy 
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {
			tile_entropy_count[y][x] = 9;
		}
	}


	time_t seconds;

	printf("\n\n Unsolved Puzzle\n");
	print_game_state();

	while(is_unsolved){

		seconds = time(NULL);

		if (seconds % 2 == 0) {
			if (cooldown == 0) {
				print_game_state();
				cooldown = 10000;
			} 
			cooldown--;
		}

		update_tile_entropy();
		write_in_number();
	}
}
