#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"gol.h"

//Check argument for generations is an integer
int generation_is_integer(char* generation){
	for(int i=0; generation[i] != '\0'; i++){
		if (!isdigit(generation[i])){
			return 0;
		}
	}
	return 1;
}


int main(int argc, char *argv[]){
	struct universe v;
	int stats = 0;
	int generations = 5;
	int (*alive_func_ptr)(struct universe *, int, int);
	int input_counter = 0;
	int output_counter = 0;
	int generation_counter = 0;
	FILE *infile;
	FILE *outfile;
	char* input_name;
	char* output_name;
	struct universe *ptr = (struct universe *) malloc(sizeof(struct universe));
	alive_func_ptr = will_be_alive;
	if (ptr==NULL){
		fprintf(stderr, "Error: malloc error!\n");
		return 1;
	}
	v = *ptr;
	for(int j=1; j<argc; j++){
		char *arg = argv[j];
		if (arg[0] == '-'){
			if (arg[2]){
				//check command is not longer than 1 character
				fprintf(stderr, "Error: invalid option %s provided!\n", arg);
				return 1;
			}
			switch(arg[1]){
				case 'i':
					// check argument provided
					if (!argv[j+1]){
						fprintf(stderr, "Error: no -i argument provided!\n");
						return 1;
					}
					if (!input_counter){
						infile = fopen(argv[j+1], "r");
						// check filename is an existing file
						if (!infile){
							fprintf(stderr, "Error: invalid input file provided!\n");
							return 1;
						}
						input_name = argv[j+1];
						input_counter = 1;
					}else{
						// check for conflicting filename arguments
						if (strcmp(argv[j+1], input_name)){
							fprintf(stderr, "Error: conflicting -i arguments provided!\n");
							return 1;			
						}
			
					}
					j++;
					break;
					
				case 'o':
					// check argument provided
					if (!argv[j+1]){
						fprintf(stderr, "Error: no -o argument provided!\n");
						return 1;
					}
					if (!output_counter){
						outfile = fopen(argv[j+1], "w");
						output_name = argv[j+1];
						output_counter = 1;
					}else{
						// check for conflicting filename arguments
						if (strcmp(argv[j+1], output_name)){
							fprintf(stderr, "Error: conflicting -o arguments provided!\n");
							return 1;
						}
					}
					j++;
					break;

				case 'g':
					// check argument provided and argument is not negative
					if (!argv[j+1] || argv[j+1][0] == '-'){
						fprintf(stderr, "Error: invalid -g argument specified!\n");
						return 1;
					}
					// check argument is an integer > 0
					if (!generation_is_integer(argv[j+1])){
						fprintf(stderr, "Error: -g argument is not an integer!\n");
						return 1;	
					}
					// check for conflicting arguments
					if (generation_counter && generations != atoi(argv[j+1])){
						fprintf(stderr, "Error: conflicting arguments provided for -g!\n");
						return 1;		
					} 
					generations = atoi(argv[j+1]);
					generation_counter++;
					j++;
					break;

				case 's':
					stats = 1;
					break;

				case 't':
					alive_func_ptr = will_be_alive_torus;
					break;

				default:
					// check for invalid command line option
					fprintf(stderr, "Error: invalid option provided!\n");
					return 1;
			}
		}else{
			// check no other arguments entered
			fprintf(stderr, "Error: invalid argument provided!\n");
			return 1;
		}	
	}

	if (input_counter){
		read_in_file(infile,&v);
	}else{
  		read_in_file(stdin,&v);
	}
	for(int i = 0; i < generations; i++){
  		evolve(&v,alive_func_ptr);
	}
	if (output_counter){
	 	write_out_file(outfile,&v);
	}else{
  		write_out_file(stdout,&v);
		fflush(stdout);
	}
	if (stats){
		print_statistics(&v);
	}
	free(ptr);
	return 0;
}
