/*
	Module Name: simulate.cpp
	Description: Simulates the IO requests as per specified algorithm.
*/
#include <stdio.h>	
#include <list>
#include "data_structures.h"

/*************************** imported from main.cpp ***************************/
extern char algo;
extern bool verbose, print_queue;
extern void print_request_list();


/*************************** global variables ***************************/
int curr_head_location;
std::list<Request*> requests;


/*************************** function declarations ***************************/
bool is_request_active();
Request* new_request_arrived(int curr_time);
void print_requests();
double get_avg_turnaround_time();
double get_avg_wait_time();
int get_max_wait_time();


void simulate() {
	/*
		Function Name: simulate
		Arguments: void
		Returns: void
		Description: simulates the IO requests as per specified scheduling algorithm.
	*/

	// variables for storing state and info of simulation
	int curr_time = 0, tot_movement = 0;
	Request *curr_request = NULL;
	Scheduler *sched = NULL;

	// select scheduler as per option specified
	if(algo == 'i') {
		sched = new FIFOScheduler();
	} else if(algo == 'j') {
		sched = new SSTFScheduler();
	} else if(algo == 's') {
		sched = new LookScheduler();
	} else if(algo == 'c') {
		sched = new CLookScheduler();
	} else if(algo == 'f') {
		sched = new FLookScheduler();
	}


	// start simulation
	// if there is any active request in request list then keep on simulating
	while(is_request_active()) { 
		// check whether new request has arrived
		Request *request = new_request_arrived(curr_time);

		// if yes then add it to IO queue
		if(request != NULL) {
			if(verbose)
				printf("%d: %d add %d\n", curr_time, request->request_id, request->track_required);
			sched->add_request(request);
			request->state = READY;
		}

		// if any request is running currently then do its processing
		if(curr_request != NULL) {

			// if header is away from the track required then move it accordingly
			if(curr_head_location < curr_request->track_required) {
				curr_head_location++;
				tot_movement++;
			} else if(curr_head_location > curr_request->track_required) {
				curr_head_location--;
				tot_movement++;
			}

			// if header is on the track required then fininsh the request
			// also do the corresponding accounting calculations
			if(curr_head_location == curr_request->track_required) {
				curr_request->end_time = curr_time;
				curr_request->turn_around_time = curr_time - curr_request->arrival_time;
				curr_request->state = COMPLETE;
				if(verbose)
					printf("%d: %d finish %d\n", curr_time, curr_request->request_id, curr_request->turn_around_time);
				curr_request = NULL;
			}
		}


		// if there is no request going on currently then get new one from IO queue
		if(curr_request == NULL) {
			curr_request = sched->get_next_request();

			// if there is request pending in queue then process it.
			if(curr_request != NULL) {
				if(print_queue) {
					printf("\n\n");
					sched->print_queue();
					printf("\n\n");
				}
				if(verbose) {
					printf("%d: %d issue %d %d\n", curr_time, curr_request->request_id, curr_request->track_required, curr_head_location);
				}
				// calculate accounting for wait time and start time
				curr_request->start_time = curr_time;
				curr_request->wait_time = curr_time - curr_request->arrival_time;

				// if we get requests for the same track header is on 
				// then keep on completing it
				while(curr_head_location == curr_request->track_required) {
					// accounting to be performed when request is completed
					curr_request->end_time = curr_time;
					curr_request->turn_around_time = curr_time - curr_request->arrival_time;
					curr_request->state = COMPLETE;


					if(verbose)
						printf("%d: %d finish %d\n", curr_time, curr_request->request_id, curr_request->turn_around_time);

					// again get new request and repeat the process
					curr_request = sched->get_next_request();
					if(curr_request == NULL) {
						break;
					} else {
						if(verbose) {
							printf("%d: %d issue %d %d\n", curr_time, curr_request->request_id, curr_request->track_required, curr_head_location);
						}
						curr_request->start_time = curr_time;
						curr_request->wait_time = curr_time - curr_request->arrival_time;
					}
				}
			}
		}

		// finally increment the time
		curr_time++;
	}

	// print the requests and their corresponding information
	print_requests();

	// print the summary
	printf("SUM: %d %d %.2lf %.2lf %d\n", curr_time-1, tot_movement, get_avg_turnaround_time(),get_avg_wait_time() , get_max_wait_time());
}

bool is_request_active() {
	/*
		Function Name: is_request_active
		Arguments: void
		Returns: bool
		Description: checks whether there are any requests in list which are active
	*/

	// iterate through the list to find atleast one request that is active
	// active means either CREATED or in queue i.e. READY or 
	// currently being processed i.e. RUNNING
	std::list<Request*>::iterator it;
	for (it = requests.begin(); it != requests.end(); ++it){
		// if one such request found then return true
    	if((*it)->state == CREATED || (*it)->state == READY || (*it)->state == RUNNING)
    		return true;
	}

	// else there is no such request
	return false;
}


void print_requests() {
	/*
		Function Name: print_requests
		Arguments: void
		Returns: void
		Description: prints the request info required at the end
	*/
	std::list<Request*>::iterator it;
	for (it = requests.begin(); it != requests.end(); ++it){
    	(*it)->print_request(); // calls the print_requests function of Request class
	}
}


Request* new_request_arrived(int curr_time) {
	/*
		Function Name: new_request_arrived
		Arguments: int curr_time
		Returns: Request*: new request arrived at this time else give NULL
		Description: gives the request that arrived at this time else returns NULL
	*/
	std::list<Request*>::iterator it;
	for (it = requests.begin(); it != requests.end(); ++it){	
		// if request's arrival time is current time then return it.
    	if((*it)->arrival_time == curr_time) {
    		return (*it);
    	}
    	if((*it)->arrival_time > curr_time) {
    		break;
    	}
	}
	// else return NULL
	return NULL;
}


double get_avg_turnaround_time() {
	/*
		Function Name: get_avg_turnaround_time
		Arguments: void
		Returns: double: average turnaround time of all requests
		Description: calculates average turnaround time of all requests
	*/
	double turn_around_time = 0;
	std::list<Request*>::iterator it;
	for (it = requests.begin(); it != requests.end(); ++it){
    	turn_around_time += (*it)->turn_around_time;
	}
	turn_around_time = turn_around_time/requests.size();
	return turn_around_time;
}

double get_avg_wait_time() {
	/*
		Function Name: get_avg_wait_time
		Arguments: void
		Returns: double: average wait time of all requests
		Description: calculates average wait time of all requests
	*/
	double wait_time = 0;
	std::list<Request*>::iterator it;
	for (it = requests.begin(); it != requests.end(); ++it){
    	wait_time += (*it)->wait_time;
	}
	wait_time = wait_time/requests.size();
	return wait_time;
}

int get_max_wait_time() {
	/*
		Function Name: get_max_wait_time
		Arguments: void
		Returns: int: maximum time a request had to wait
		Description: finds out the maximum time a request had to wait
	*/
	int max_wait_time = 0;
	std::list<Request*>::iterator it;
	for (it = requests.begin(); it != requests.end(); ++it){
    	if((*it)->wait_time > max_wait_time) {
    		max_wait_time = (*it)->wait_time;
    	}
	}
	return max_wait_time;
}