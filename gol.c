#include <stdio.h>
#include <stdlib.h>
#include "gol.h"

//TODO store in array and realloc after every row, fscanf stdin

void read_in_file(FILE *infile, struct universe *u){
	printf("%d\n", u);
	char c;
	int rows = 0;
	int columns = 0;
	int temp = 0;
	int cells_alive = 0;
	int counter = 0;
	char* ptr;
	//u = (struct universe *) malloc(sizeof(struct universe));
	printf("%d\n", u);
	if (u==NULL){
		fprintf(stderr, "Malloc error!");
		exit(1);
	}
	printf("Hi");
	u->grid = (char *) malloc(sizeof(char) * 512);
	if (u->grid==NULL){
		fprintf(stderr, "Malloc error!");
		exit(1);
	}
	printf("Hi");
	while((c = getc(infile)) != EOF && temp <=512){
		if (c == '\n'){
			rows += 1;
			if (columns == 0){
				columns = temp;
				temp = 0;

			}else{
				if (columns != temp){
					fprintf(stderr, "Number of columns is not constant!");
					exit(1);
				}
				columns = temp;
				temp = 0;
			}
			ptr = realloc(u->grid, columns * (rows+1)* sizeof(char));
			if (ptr==NULL){
				fprintf(stderr, "Realloc error!");
				exit(1);
			}
			u->grid = ptr;

		}else{
			u->grid[counter] = c;
			counter ++;
			temp += 1;
			if (c == '*'){
				cells_alive += 1;
			}
		}
	}
	if (columns > 512){
		fprintf(stderr, "Too many columns!");
		exit(1);
	}
	u->rows = rows;
	u->columns = columns;
	u->generations = 1;
	u->cells_alive = cells_alive;
	u->percent_alive = (double) cells_alive / (rows * columns)*100;
	u->running_average = (double) cells_alive / (rows * columns)*100;
	printf("%d", u->cells_alive);
	return;
}

void write_out_file(FILE *outfile, struct universe *u){
	if (outfile){
		for(int i = 0; i < u->rows; i++){
			for(int j = 0; j < u->columns; j++){
				fprintf(outfile, "%c", u->grid[j + i * u->columns]);
			}
			fprintf(outfile, "\n");
		}
	}else{
		for(int i = 0; i < u->rows; i++){
			for(int j = 0; j < u->columns; j++){
				fprintf(stdout, "%c", u->grid[j + i * u->columns]);
			}
			fprintf(stdout, "\n");
		}
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
	int alive;
	alive_counter += (row < u->rows - 1) ? is_alive(u, column, row + 1) : 0;
	alive_counter += (row > 0) ? is_alive(u, column, row - 1) : 0;
	alive_counter += (column < u->columns - 1) ? is_alive(u, column + 1, row) : 0;
	alive_counter += (column > 0) ? is_alive(u, column - 1, row) : 0;
	alive_counter += (row < u->rows - 1 && column < u->columns - 1) ? is_alive(u, column + 1, row + 1) : 0;
	alive_counter += (row < u->rows - 1 && column > 0) ? is_alive(u, column - 1, row + 1) : 0;
	alive_counter += (row > 0 && column < u->columns - 1) ? is_alive(u, column + 1, row - 1) : 0;
	alive_counter += (row > 0 && column > 0) ? is_alive(u, column - 1, row - 1) : 0;
	switch (is_alive(u, column, row)){
		case 0:
			if (alive_counter == 3){
				alive = 1;
			}else{
				alive = 0;
			}
		case 1:
			if (alive_counter == 2 || alive_counter == 3){
				alive = 1;
			}else{
				alive = 0;
			}
	}
	return alive;
}

int will_be_alive_torus(struct universe *u,  int column, int row){
	int alive_counter = 0;
	int alive;
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
				alive = 1;
			}else{
				alive = 0;
			}
		case 1:
			if (alive_counter == 2 || alive_counter == 3){
				alive = 1;
			}else{
				alive = 0;
			}
	}
	return alive;
}

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row)){
	printf("Hi %d", u->columns);
	int cells_alive = 0;
	for(int i=0; i < u->rows; i++){
		for(int j=0; j < u->columns; j++){
			if (rule(u,j,i) == 1){
				u->grid[j + i * u->columns] = '*';
				cells_alive += 1;
			}else{
				u->grid[j + i * u->columns] = '.';
			}
		}
	}
	u -> cells_alive = cells_alive;
	u -> percent_alive = (double) cells_alive / (u -> columns * u -> rows);
	u -> running_average = (double) (u -> running_average * u -> generations + u -> percent_alive) / (u -> generations + 1);
	u -> generations += 1;
}

void print_statistics(struct universe *u){
	printf("%.3f%% of cells currently alive", u->percent_alive);
	printf("%.3f%% of cells alive on average", u->running_average);
}

int main(int argc, char *argv[]){
	struct universe v;

	int stats = 0;
	int generations = 5;
	int (*alive_func_ptr)(struct universe *, int, int);
	alive_func_ptr = will_be_alive;
	FILE *infile;
	FILE *outfile;
	for(int j=1; j<argc; j++){
		char *arg = argv[j];
		if (arg[0] == '-'){
			switch(arg[1]){
				case 'i':
					printf("%c\n", arg[1]);
					infile = fopen(argv[j+1], "r");
					printf("%s\n", argv[j+1]);
					j++;
					break;
				case 'o':
					printf("%c\n", arg[1]);
					outfile = fopen(argv[j+1], "w");
					j++;
					break;
				case 'g':
					printf("g\n");
					generations = atoi(argv[j+1]);
					j++;
					break;
				case 's':
					printf("s\n");
					stats = 1;
					break;
				case 't':
					printf("t\n");
					alive_func_ptr = will_be_alive_torus;
					break;
				default:
					break;
			}
		}
		
	}
	read_in_file(infile,&v);
	printf("%d", v.cells_alive);
	for(int i = 0; i < generations; i++){
  		evolve(&v,alive_func_ptr);
	}
  	write_out_file(outfile,&v);
	if (stats){
		print_statistics(&v);
	}
	return 0;
}
