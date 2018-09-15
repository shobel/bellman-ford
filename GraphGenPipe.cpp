#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <fstream>

using namespace std;

#define FIFO_FILE "fifo_twoway"

int main(int argc, char* argv[]){

	unsigned int microseconds = 3000000;

	int fd;
	int end_process;
	int stringlen;
	char readbuf[80];
	int read_bytes;
	printf("FIFO_CLIENT STARTED\n");
	fd = open(FIFO_FILE, O_CREAT|O_RDWR);

	srand(time(NULL));
	int costMin = 1;
	int costMax = 100;

	if (!argv[1]){
		cout << "Error: first argument must provide the number of nodes." << endl;
		exit(1);
	}

	if (argv[2] != NULL) {
		microseconds = stoul(argv[2]);
	}

	int numNodes = atoi(argv[1]);
	//cout << "Number of nodes: " << numNodes << endl;

	int graph[numNodes][numNodes];
	int iteration = 0;
	while (1) {

	for (int i = 0; i < numNodes; i++){
		for (int j = 0; j < numNodes; j++){
			if (i == j) {
				graph[i][j] = 0;
				continue;
			} 

			int connected = rand()%(2);
			if (connected){
				int cost = rand()%(costMax-costMin + 1) + costMin;
				//cout << "cost is " << cost << endl;
				graph[i][j] = cost;
			} else {
				graph[i][j] = -1;
			}
		}
	}

	cout << "Sending random graph to server for processing:" << endl;

	string s;
	for (int i = 0; i < numNodes; i++){
		s = "";
		for (int j = 0; j < numNodes; j++) {
			if (graph[i][j] == -1){
				s.append("*");
				cout << "*";
			} else {
				stringstream ss;
				ss << graph[i][j];
				s.append(ss.str());
				cout << graph[i][j];
			}

			if (j != numNodes-1){
				s.append(",");
				cout << ",";
			}
		}
		cout << endl;

		int n = s.length(); 
		char char_array[n+1];
		strcpy(char_array, s.c_str()); 
		int stringlen = strlen(char_array);
		char_array[stringlen - 1] = '\0';

		//cout << "length of string sent is " << stringlen << endl;
		//cout << "Sending string over: " << s << endl;

		write(fd, char_array, strlen(char_array));
		usleep(100);

	}
	read_bytes = read(fd, readbuf, sizeof(readbuf));
	readbuf[read_bytes] = '\0';

    stringstream stream;
  	stream << "graph-" << iteration << "-solution.txt";
	char *y = new char[stream.str().length() + 1]; 
	strcpy(y, stream.str().c_str());

	cout << "Received solution from server. Writing to file " <<  stream.str() << endl;

  	ofstream solutionFile;
	solutionFile.open(y,fstream::out);
	solutionFile << readbuf;
	solutionFile.close();

	iteration++;
	usleep(microseconds);
	}
}
