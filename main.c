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

static void print_game_state() 
{
	printf("\n===================");
	for (int y = 0; y < 9; y++) {
		if (y % 3 == 0) {
			// Box Spacer
			printf("\n");
		}
		for (int x = 0; x < 9; x++) {
			printf("%d ", game_state[y][x]);

			// Box spacer
			if ((x+1) % 3 == 0) {
				printf(" ");
			}
		}
		printf("\n");
	}
	printf("===================\n");
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


/* 
 * It will go over every empty
 * tile and reduce its entropy base on the tiles in the 
 * row, column, and box that it belongs to.
 */
static void update_tile_entropy() 
{
	// For each tile
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {

			// If tile is empty
			if (game_state[y][x] == 0) {
				
				// Compare by row
				for (int i = 0; i < 9; i++) {
					if (game_state[y][i] != 0) {
						remove_entropy(game_state[y][i], y, x);
					}
				}

				// Compare by column
				for (int i = 0; i < 9; i++) {
					if (game_state[i][x] != 0) {
						remove_entropy(game_state[i][x], y, x);
					}
				}

				// Compare by box
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

static void write_in_tile()
{
	// Find the tile with the lowest entropy
	is_unsolved = 0;
	int curr_low_count = 10;
	int curr_low_y;
	int curr_low_x;

	// For each tile
	for (int y = 0; y < 9; y++) {
		for (int x = 0; x < 9; x++) {

			// Find the empty tile with the lowest entropy
			if (tile_entropy_count[y][x] < curr_low_count && game_state[y][x] == 0) {
				curr_low_count = tile_entropy_count[y][x];
				curr_low_y = y;
				curr_low_x = x;
				 
				is_unsolved++;
			}
		}
	}

	// Lowest entropy tiles now collapses into a random possibility
	if(is_unsolved) {

		int possible[9] = {0};
		int possibility_count = 0;
		// Go through each possibility of tile's entropy
		for (int i = 0; i < 9; i++) {
			// If possibility is possible
			if (tile_entropy[curr_low_y][curr_low_x][i] != 0) {
				// Creates array where all the possibilities are on the left
				// filled with zeros on the right ex [1, 4, 5, 0, 0, 0, 0, 0, 0]
				possible[possibility_count] = tile_entropy[curr_low_y][curr_low_x][i];
				possibility_count++;
			}
		}

		/*
		 * If there are no possibilities for empty tile
		 * meaning an unsolvable position.
		 * just reset and retry for now cause I don't 
		 * want to make a whole history backtracking system right now.
		 */
		if (possibility_count == 0) {
			set_state_to_starting();
			set_board_entropy_to_max();
			attempts++;
			return;
		}

		int chosen_number = possible[rand() % possibility_count];

		// Store new number in game state
		game_state[curr_low_y][curr_low_x] = chosen_number;
	} 
}


int main() 
{
	set_state_to_starting();
	set_board_entropy_to_max();

	// Initiates for rand function
	srand(time(NULL));

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
