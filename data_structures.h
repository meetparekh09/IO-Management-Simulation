/*
	Module Name: data_structures.h
	Description: Defines all the data structures and schedulers used in the program
*/
#include <stdio.h>
#include <list>

/*************************** imported from main.cpp ***************************/
extern int curr_head_location;

#ifndef STATE_H
#define STATE_H

enum State {CREATED, READY, RUNNING, COMPLETE}; // defines state of the request

#endif

#ifndef REQUEST_H
#define REQUEST_H

class Request {
	/*
		Class Name: Request
		Description: defines a IO request with its corresponding information
	*/
public:
	int request_id;
	int arrival_time;
	int start_time;
	int end_time;
	int track_required;
	int wait_time;
	int turn_around_time;
	State state;


	/*************************** Constructor ***************************/
	Request(int request_id, int arrival_time, int track_required, State state) {
		this->request_id = request_id;
		this->arrival_time = arrival_time;
		this->track_required = track_required;
		this->state = state;
	}


	void print_request() {
		/*
			Function Name: print_request
			Arguments: void
			Returns: void
			Description: prints the information of request in the required format
		*/
		printf("%5d: %5d %5d %5d\n", request_id, arrival_time, start_time, end_time);
	}
};


#endif

#ifndef SCHEDULER_H
#define SCHEDULER_H


class Scheduler {
	/*
		Class Name: Scheduler
		Description: defines a parent virtual Scheduler class
	*/
public:

	/*************************** Virtual Function Definitions ***************************/
	virtual Request *get_next_request() {
		/*
			Function Name: get_next_request
			Arguments: void
			Returns: Request*: request to be processed next
			Description: gives the next request to be processed from the queue as per scheduling algorithm
		*/
		return NULL;
	}
	virtual void add_request(Request *request) {
		/*
			Function Name: add_request
			Arguments: Request *request: request to be inserted in queue
			Returns: void
			Description: inserts the new request in the queue
		*/
		return;
	}
	virtual void print_queue() {
		/*
			Function Name: print_queue
			Arguments: void
			Returns: void
			Description: prints all the requests of the queue
		*/
		return;
	}

};


#endif


#ifndef FIFO_SCHEDULER_H
#define FIFO_SCHEDULER_H

class FIFOScheduler : public Scheduler {
	std::list<Request*> queue;
public:
	Request* get_next_request() {
		/*
			Function Name: get_next_request
			Arguments: void
			Returns: Request*: request to be processed next
			Description: gives the next request to be processed from the queue as per FIFO algorithm
		*/
		if(queue.size() == 0) {
			return NULL;
		}
		Request *request = queue.front(); // get the first request and then return it.
		queue.pop_front();
		return request;
	}
	
	void add_request(Request *request) {
		/*
			Function Name: add_request
			Arguments: Request *request: request to be inserted in queue
			Returns: void
			Description: inserts the new request in the queue
		*/
		queue.push_back(request);
	}

	void print_queue() {
		/*
			Function Name: print_queue
			Arguments: void
			Returns: void
			Description: prints all the requests of the queue
		*/
		std::list<Request*>::iterator it;
		for (it = queue.begin(); it != queue.end(); ++it){
    		printf("%d: %d %d\n", (*it)->request_id, (*it)->arrival_time, (*it)->track_required);
		}
	}

};

#endif

#ifndef SSTF_SCHEDULER_H
#define SSTF_SCHEDULER_H


class SSTFScheduler: public Scheduler {
	std::list<Request*> queue;
public:
	void add_request(Request *request) {
		/*
			Function Name: add_request
			Arguments: Request *request: request to be inserted in queue
			Returns: void
			Description: inserts the new request in the queue
		*/
		queue.push_back(request);
	}

	Request* get_next_request() {
		/*
			Function Name: get_next_request
			Arguments: void
			Returns: Request*: request to be processed next
			Description: gives the next request to be processed from the queue as per SSTF algorithm
		*/
		if(queue.size() == 0) {
			return NULL;
		}
		std::list<Request*>::iterator it;

		// initialize the minimum seek time and corresponding request
		int min_seek = get_seek_time(queue.front());
		Request *request = queue.front();

		// find out the minimum seek time and the corresponding request
		for(it = queue.begin(); it != queue.end(); it++) {
			int seek = get_seek_time((*it));
			if(seek < min_seek) {
				min_seek = seek;
				request = (*it);
			}
		}

		// remove request from the queue and return it
		queue.remove(request);
		return request;
	}

	void print_queue() {
		/*
			Function Name: print_queue
			Arguments: void
			Returns: void
			Description: prints all the requests of the queue
		*/
		std::list<Request*>::iterator it;
		for (it = queue.begin(); it != queue.end(); ++it){
    		printf("%d: %d %d\n", (*it)->request_id, (*it)->arrival_time, (*it)->track_required);
		}
	}

	int get_seek_time(Request *request) {
		/*
			Function Name: get_seek_time
			Arguments: Request *request
			Returns: int
			Description: calculates seek time to move header from current location to the track required by the request
		*/
		if(curr_head_location > request->track_required) {
			return curr_head_location - request->track_required;
		} else {
			return request->track_required - curr_head_location;
		}
	}
};


#endif


#ifndef LOOK_SCHEDULER_H
#define LOOK_SCHEDULER_H

class LookScheduler : public Scheduler {
	std::list<Request*> queue;
	bool forward_direction; // holds the direction in which to move the header
public:
	LookScheduler() {
		forward_direction = true;
	}

	void add_request(Request *request) {
		/*
			Function Name: add_request
			Arguments: Request *request: request to be inserted in queue
			Returns: void
			Description: inserts the new request in the queue
		*/
		queue.push_back(request);
	}

	Request* get_next_request() {
		/*
			Function Name: get_next_request
			Arguments: void
			Returns: Request*: request to be processed next
			Description: gives the next request to be processed from the queue as per LOOK algorithm
		*/
		if(queue.size() == 0) {
			return NULL;
		}
		std::list<Request*>::iterator it;
		int min_seek = -1;
		Request *request = NULL;

		// check for all the requests in one direction
		for(it = queue.begin(); it != queue.end(); it++) {
			if(forward_direction && (*it)->track_required < curr_head_location) {
				continue;
			}
			if(!forward_direction && (*it)->track_required > curr_head_location) {
				continue;
			}
			int seek_time = get_seek_time((*it));
			if(min_seek == -1) {
				min_seek = seek_time;
				request = (*it);
			} else if(seek_time < min_seek) {
				min_seek = seek_time;
				request = (*it);
			}
		}

		// if any found then return it else change the direction
		if(request != NULL) {
			queue.remove(request);
			return request;
		} else {
			forward_direction = !forward_direction;
		}

		// now check for the same in another direction
		for(it = queue.begin(); it != queue.end(); it++) {
			if(forward_direction && (*it)->track_required < curr_head_location) {
				continue;
			}
			if(!forward_direction && (*it)->track_required > curr_head_location) {
				continue;
			}
			int seek_time = get_seek_time((*it));
			if(min_seek == -1) {
				min_seek = seek_time;
				request = (*it);
			} else if(seek_time < min_seek) {
				min_seek = seek_time;
				request = (*it);
			}
		}


		queue.remove(request);
		return request;
	}

	void print_queue() {
		/*
			Function Name: print_queue
			Arguments: void
			Returns: void
			Description: prints all the requests of the queue
		*/
		std::list<Request*>::iterator it;
		for (it = queue.begin(); it != queue.end(); ++it){
    		printf("%d: %d %d\n", (*it)->request_id, (*it)->arrival_time, (*it)->track_required);
		}
	}

	int get_seek_time(Request *request) {
		/*
			Function Name: get_seek_time
			Arguments: Request *request
			Returns: int
			Description: calculates seek time to move header from current location to the track required by the request
		*/
		if(curr_head_location > request->track_required) {
			return curr_head_location - request->track_required;
		} else {
			return request->track_required - curr_head_location;
		}
	}
};


#endif


#ifndef CLOOK_SCHEDULER_H
#define CLOOK_SCHEDULER_H

class CLookScheduler : public Scheduler {
	std::list<Request*> queue;
public:
	void add_request(Request *request) {
		/*
			Function Name: add_request
			Arguments: Request *request: request to be inserted in queue
			Returns: void
			Description: inserts the new request in the queue
		*/
		queue.push_back(request);
	}

	Request* get_next_request() {
		/*
			Function Name: get_next_request
			Arguments: void
			Returns: Request*: request to be processed next
			Description: gives the next request to be processed from the queue as per CLOOK algorithm
		*/
		if(queue.size() == 0) {
			return NULL;
		}
		std::list<Request*>::iterator it;
		int min_seek = -1;
		Request *request = NULL;

		// check for minimum seek time request in forward direction
		for(it = queue.begin(); it != queue.end(); it++) {
			if((*it)->track_required < curr_head_location) {
				continue;
			}
			int seek_time = get_seek_time((*it));
			if(min_seek == -1) {
				min_seek = seek_time;
				request = (*it);
			} else if(seek_time < min_seek) {
				min_seek = seek_time;
				request = (*it);
			}
		}

		// if found one then return it
		if(request != NULL) {
			queue.remove(request);
			return request;
		}

		// otherwise check for other minimum seek time request in backward direction
		for(it = queue.begin(); it != queue.end(); it++) {
			int seek_time = (*it)->track_required;
			if(min_seek == -1) {
				min_seek = seek_time;
				request = (*it);
			} else if(seek_time < min_seek) {
				min_seek = seek_time;
				request = (*it);
			}
		}
		queue.remove(request);
		return request;
	}

	void print_queue() {
		/*
			Function Name: print_queue
			Arguments: void
			Returns: void
			Description: prints all the requests of the queue
		*/
		std::list<Request*>::iterator it;
		for (it = queue.begin(); it != queue.end(); ++it){
    		printf("%d: %d %d\n", (*it)->request_id, (*it)->arrival_time, (*it)->track_required);
		}
	}

	int get_seek_time(Request *request) {
		/*
			Function Name: get_seek_time
			Arguments: Request *request
			Returns: int
			Description: calculates seek time to move header from current location to the track required by the request
		*/
		if(curr_head_location > request->track_required) {
			return curr_head_location - request->track_required;
		} else {
			return request->track_required - curr_head_location;
		}
	}
};


#endif

#ifndef FLOOK_SCHEDULER_H
#define FLOOK_SCHEDULER_H

class FLookScheduler : public Scheduler {
	std::list<Request*> queue1;
	std::list<Request*> queue2;

	bool forward_direction;
public:
	FLookScheduler() {
		forward_direction = true;
	}

	void add_request(Request *request) {
		/*
			Function Name: add_request
			Arguments: Request *request: request to be inserted in queue
			Returns: void
			Description: inserts the new request in the queue
		*/
		queue2.push_back(request);
	}

	Request* get_next_request() {
		/*
			Function Name: get_next_request
			Arguments: void
			Returns: Request*: request to be processed next
			Description: gives the next request to be processed from the queue as per FLOOK algorithm
		*/

		// if running queue is empty then switch it
		if(queue1.size() == 0) {
			queue1 = queue2;
			queue2.clear();
		}

		// if other queue is also empty then return NULL
		if(queue1.size() == 0) {
			return NULL;
		}


		// rest of the process is repeatition of LOOK
		std::list<Request*>::iterator it;
		int min_seek = -1;
		Request *request = NULL;
		for(it = queue1.begin(); it != queue1.end(); it++) {
			if(forward_direction && (*it)->track_required < curr_head_location) {
				continue;
			}
			if(!forward_direction && (*it)->track_required > curr_head_location) {
				continue;
			}
			int seek_time = get_seek_time((*it));
			if(min_seek == -1) {
				min_seek = seek_time;
				request = (*it);
			} else if(seek_time < min_seek) {
				min_seek = seek_time;
				request = (*it);
			}
		}
		if(request != NULL) {
			queue1.remove(request);
			return request;
		} else {
			forward_direction = !forward_direction;
		}
		for(it = queue1.begin(); it != queue1.end(); it++) {
			if(forward_direction && (*it)->track_required < curr_head_location) {
				continue;
			}
			if(!forward_direction && (*it)->track_required > curr_head_location) {
				continue;
			}
			int seek_time = get_seek_time((*it));
			if(min_seek == -1) {
				min_seek = seek_time;
				request = (*it);
			} else if(seek_time < min_seek) {
				min_seek = seek_time;
				request = (*it);
			}
		}
		queue1.remove(request);
		return request;
	}

	void print_queue() {
		/*
			Function Name: print_queue
			Arguments: void
			Returns: void
			Description: prints all the requests of the queue
		*/
		std::list<Request*>::iterator it;
		for (it = queue1.begin(); it != queue1.end(); ++it){
    		printf("%d: %d %d\n", (*it)->request_id, (*it)->arrival_time, (*it)->track_required);
		}
	}

	int get_seek_time(Request *request) {
		/*
			Function Name: get_seek_time
			Arguments: Request *request
			Returns: int
			Description: calculates seek time to move header from current location to the track required by the request
		*/
		if(curr_head_location > request->track_required) {
			return curr_head_location - request->track_required;
		} else {
			return request->track_required - curr_head_location;
		}
	}
};

#endif