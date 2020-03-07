#include <stdio.h>
#include <stdlib.h>
#include "gol.h"


void read_in_file(FILE *infile, struct universe *u){
	char c;
	int rows = 0;
	int columns = 0;
	int temp = 0;
	int cells_alive = 0;
	int counter = 0;
	char* ptr;

	u->grid = (char *) malloc(sizeof(char) * 512);

	if (u->grid==NULL){
		fprintf(stderr, "Error: malloc error!\n");
		exit(1);
	}	

	//check for EOF and if number of columns is too large (>512)
	while((c = getc(infile)) != EOF && temp <=512){
		if (c == '\n'){
			rows += 1;
			if (columns == 0){
				columns = temp;
				temp = 0;

			}else{
				// check number of columns is the same for each row
				if (columns != temp){
					fprintf(stderr, "Error: number of columns in input is not constant!\n");
					exit(1);
				}

				columns = temp;
				temp = 0;
			}
			// increase memory for universe if needed
			ptr = realloc(u->grid, columns * (rows+1)* sizeof(char));
			// check memory reallocation was successful
			if (ptr==NULL){
				fprintf(stderr, "Error: realloc error!\n");
				exit(1);
			}

			u->grid = ptr;

		}else{
			// check each character in file is valid
			if (c != '.' && c != '*'){
				fprintf(stderr, "Error: invalid character in input!");
				exit(1);
			}
			u->grid[counter] = c;
			counter ++;
			temp += 1;
			// update number of cells alive if character is *
			if (c == '*'){
				cells_alive += 1;
			}
		}
	}
	// if number of columns is too large return error
	if (temp > 512){
		fprintf(stderr, "Error: input has over 512 columns!\n");
		exit(1);
	}
	// set universe variables
	u->rows = rows;
	u->columns = columns;
	u->generations = 1;
	u->cells_alive = cells_alive;
	u->percent_alive = (double) cells_alive / (rows * columns)*100;
	u->running_average = (double) cells_alive / (rows * columns)*100;

	return;
}

void write_out_file(FILE *outfile, struct universe *u){
	for(int i = 0; i < u->rows; i++){
		for(int j = 0; j < u->columns; j++){
			fprintf(outfile, "%c", u->grid[j + i * u->columns]);
		}
		fprintf(outfile, "\n");
	}
}

int is_alive(struct universe *u, int column, int row){
	int idx = column + row * u->columns;

	if (u->grid[idx]=='*'){
		return 1;
	}else{
		return 0;
	}

}

int will_be_alive(struct universe *u, int column, int row){
	int alive_counter = 0;
	// check if neighbours are alive
	alive_counter += (row+1 < u->rows) ? is_alive(u, column, row + 1) : 0;
	alive_counter += (row -1 > 0) ? is_alive(u, column, row - 1) : 0;
	alive_counter += (column +1 < u->columns) ? is_alive(u, column + 1, row) : 0;
	alive_counter += (column -1 > 0) ? is_alive(u, column - 1, row) : 0;
	alive_counter += ((row +1 < u->rows) && (column + 1 < u->columns)) ? is_alive(u, column + 1, row + 1) : 0;
	alive_counter += ((row -1 > 0) && (column -1 > 0)) ? is_alive(u, column - 1, row - 1) : 0;
	alive_counter += ((row -1 > 0) && (column +1 < u->columns - 1)) ? is_alive(u, column + 1, row - 1) : 0;
	alive_counter += ((row +1 < u->rows) && (column -1 > 0)) ? is_alive(u, column - 1, row + 1) : 0;

	switch (is_alive(u, column, row)){
		case 0:
			if (alive_counter == 3){
				return 1;
			}else{
				return 0;
			}

		default:
			if (alive_counter == 2 || alive_counter == 3){
				return 1;
			}else{
				return 0;
			}
	}
}

int will_be_alive_torus(struct universe *u,  int column, int row){
	int alive_counter = 0;
	// check if neighbours are alive
	alive_counter += is_alive(u, column, (row + 1) % u->rows);
	alive_counter += is_alive(u, column, (row - 1 + u->rows) % u->rows);
	alive_counter += is_alive(u, (column + 1) % u->columns, row);
	alive_counter += is_alive(u, (column - 1 + u->columns) % u->columns, row);
	alive_counter += is_alive(u, (column + 1) % u->columns, (row + 1) % u->rows);
	alive_counter += is_alive(u, (column - 1 + u->columns) % u->columns, (row + 1) % u->rows);
	alive_counter += is_alive(u, (column + 1) % u->columns, (row - 1 + u->rows) % u->rows);
	alive_counter += is_alive(u, (column - 1 + u->columns) % u->columns, (row - 1 + u->rows) % u->rows);

	switch (is_alive(u, column, row)){
		case 0:
			if (alive_counter == 3){
				return 1;
			}else{
				return 0;
			}

		default:
			if (alive_counter == 2 || alive_counter == 3){
				return 1;
			}else{
				return 0;
			}
	}
}

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)){
	// declare new_grid for storing new values to avoid changing state of universe during for loop
	char new_grid[u->columns * u->rows];
	int cells_alive = 0;
	// write new char value to corresponding entry in new_grid
	for(int i = 0; i < u->rows; i++){
		for(int j = 0; j < u->columns; j++){
			if (rule(u,j,i)){
				new_grid[j + i * u->columns] = '*';
				cells_alive += 1;
			}else{
				new_grid[j + i * u->columns] = '.';
			}
		}
	}
	// copy values from new_grid to respective position in universe grid
	for(int i = 0; i < u->rows; i++){
		for(int j = 0; j < u->columns; j++){
			u->grid[j + i * u->columns] = new_grid[j + i * u->columns];
		}
	}
	// update universe variables
	u -> cells_alive = cells_alive;
	u -> percent_alive = (float) cells_alive / (float) (u -> columns * u -> rows) * 100;
	u -> running_average = (float) (u -> running_average * u -> generations + u -> percent_alive) / (float) (u -> generations + 1);
	u -> generations += 1;
}

void print_statistics(struct universe *u){
	printf("%.3f%% of cells currently alive\n", u->percent_alive);
	printf("%.3f%% of cells alive on average\n", u->running_average);
}

