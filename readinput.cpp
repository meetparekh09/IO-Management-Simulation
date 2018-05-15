/*
	Module Name: readinput.cpp
	Description: Reads the input from file and stores all the request in 'requests' list
*/
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <list>
#include "data_structures.h"

/*************************** imported from simulate.cpp ***************************/
extern std::list<Request*> requests;


void readInput(char *filename) {
	/*
		Function Name: readInput
		Arguments: char *filename: path to input file
		Returns: void
		Description: reads input from file specified and initializes requests list
	*/
	char *line = new char[100]; // buffer
	int arrival_time, track_required;
	std::fstream file;
	int curr_req_id = 0;



	// open file
	file.open(filename, std::ios_base::in);

	// read and process it
	while(file.getline(line, 100)) {
		if(strlen(line) == 0 || line[0] == '#') {
			continue;
		}
		sscanf(line, "%d %d", &arrival_time, &track_required);
		Request *request = new Request(curr_req_id, arrival_time, track_required, CREATED);
		requests.push_back(request);
		curr_req_id++;
	}
}