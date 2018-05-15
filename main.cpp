/*
	Module Name: main.cpp
	Description: Entry point of program. Acts as the controller for the program
*/
#include <unistd.h>
#include <stdio.h>

/*************************** imported from readinput.cpp ***************************/
extern void readInput(char *filename);

/*************************** imported from simulate.cpp ***************************/
extern void simulate();


/**************************** Global Variables ****************************/
char algo; //holds the algorithm to be implemented
bool verbose; //whether verbose option is selected or not
bool print_queue; //whether to print IO queue

int main(int argc, char *argv[]) {
	/*
		Function Name: main
		Arguments:
			int argc: number of command line arguments
			char *argv[]: string array containing all the command line arguments
		Returns: int: program exit status
		Description: entry point of program and also its controlledr
	*/
	
	int opt; //option character in command line argument

	while((opt = getopt(argc, argv, "qvs:")) != -1) {
		switch(opt) {
		//get the scheduler algorithm to be implemented
		case 's':
			if(optarg != NULL) algo = optarg[0];
			break;
		case 'v':
			verbose = true;
			break;
		case 'q':
			print_queue=true;
			break;
		default:
			printf("Invalid Option\n");
		}
	}


	// read the input file and store all IO requests in requests list
	readInput(argv[optind]);

	// simulate the IO requests
	simulate();
}