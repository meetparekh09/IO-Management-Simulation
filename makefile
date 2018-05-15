iosched: main.cpp data_structures.h readinput.cpp simulate.cpp
	g++ -o iosched main.cpp data_structures.h readinput.cpp simulate.cpp

clean:
	rm iosched