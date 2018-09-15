#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
using namespace std;

void runBellmanFord(int** graph, int numNodes, int fd);

bool debug = false; 
#define FIFO_FILE "fifo_twoway"

int main(int argc, char* argv[]){
	printf("FIFO_SERVER STARTED\n");
	unsigned int microseconds = 3000000;
	int fd;
	char readbuf[80];
	char end[10];
	int to_end;
	int read_bytes;

	mkfifo(FIFO_FILE, S_IFIFO|0640);

	int** graph;
	int counter = 0;
	int numNodes = 0;
	int numValues = 0;
	int total = 0;
	while(1){
		fd = open(FIFO_FILE, O_RDWR);
		read_bytes = read(fd, readbuf, sizeof(readbuf));
		readbuf[read_bytes] = '\0';
		//cout << "Received input: " << readbuf << endl;
		int inputSize = (int)strlen(readbuf);
		int to_end = strcmp(readbuf, end);

		string s(readbuf);
		for (int i = 0; i < s.size(); i++){
			if (s[i] == ',') {
				if (counter == 0){
					numNodes++;
				}
				s.erase(i, 1);
				s.insert(i, " ");
			} else if (s[i] == '*'){
				s.erase(i, 1);
				s.insert(i, "-1");
			}
		}
		if (counter == 0) {
			cout << "Awaiting new graph from client..." << endl;
			numNodes++;
			total = numNodes * numNodes;
		}
		//cout << "Number of nodes is " << numNodes << endl;
		//cout << "total values expected: " << total << endl;

		if (counter == 0){
			//delete[] graph;
			graph = new int*[numNodes];
			for (int i = 0; i < numNodes; i++){	
				graph[i] = new int[numNodes];
			}
		}

		stringstream stream;
		stream << s;
		for (int j = 0; j < numNodes; j++) {
			//cout << "adding value to graph[" << counter << "][" << j << "]" << endl; 
			numValues++;
			stream >> graph[counter][j];
		}
		//cout << "total values received: " << numValues << endl;

		//cout << "Graph is now: " << endl;
		// for (int x = 0; x < numNodes; x++){
  //   		cout << "[ ";
  //   		for (int y = 0; y < numNodes; y++){
  //   			cout << graph[x][y] << " ";
  //   		}
  //   		cout << "]" << endl;
  //   	}

		counter++;

		if (numValues == total){
			cout << "Graph received from client...running BellmanFord algorithm" << endl;
			runBellmanFord(graph, numNodes, fd);
			//usleep(microseconds);
			total = 0;
			numValues = 0;
			counter = 0;
			numNodes = 0;
		}
	}

	exit(1);
}

void runBellmanFord(int** graph, int numNodes, int fd){
	int distancesArray[numNodes];
	int pathArray[numNodes];
	int i;
	for (i = 0; i < numNodes; i++){
		distancesArray[i] = INT_MAX;
	}
	for (i = 0; i < numNodes; i++){
		pathArray[i] = -1;
	}

	distancesArray[0] = 0;
	pathArray[0] = 0;

	int iter;
	for (iter = 1; iter < numNodes-1; iter++) {
		if (debug) { cout << "####ITERATION " << iter << "####" << endl;}
		bool updated = false;
		for (int i = 0; i < numNodes; i++){
			for (int j = 0; j < numNodes; j++){
				if (j == i) {
					continue;
				}

				if (distancesArray[i] == INT_MAX) {
					if (debug) { cout << "Don't know how to get to " << i << " yet, skipping." << endl; }
					break;
				}

				if (debug) { cout << "Checking distance from " << i << " to " << j << endl; }
				if (graph[i][j] != -1){
					int distanceToNode = graph[i][j];
					if (distancesArray[i] < INT_MAX) {
						distanceToNode+= distancesArray[i];
					}

					if (debug) { cout << "distance is " << distanceToNode << endl; }
					if (distanceToNode < distancesArray[j]) {
						if (debug) {cout << distanceToNode << " is less than " << distancesArray[j] << " ...updating" << endl; }
						updated = true;
						distancesArray[j] = distanceToNode; 
						pathArray[j] = i;
					} else {
						if (debug) {cout << distanceToNode << " is not less than " << distancesArray[j] << " ...ignoring" << endl; }
					}
				} else {
					if (debug) {cout << "No path from " << i << " to " << j << endl;}
				}
			}
		}
		if (!updated){
			break;
		}
	}

	string s = "";
	stringstream ss;
	i = 0;
	int dist;
	for (i = 0; i < numNodes; i++){
		if (i!=0){
			s.append(",");
			cout << ",";
		}
		if (distancesArray[i] == INT_MAX){
			s.append("no_path");
			cout << "no_path";
		} else {
			ss << distancesArray[i];
			s.append(ss.str());
			ss.str(string());
			cout << distancesArray[i];
		}
	}
	s.append("\n");
	cout << endl;

	int backwardsPath[numNodes];
	for (i = 0; i < numNodes; i++){
		for (int j = 0; j < numNodes; j++){
			backwardsPath[j] = -1;
		}

		int currentNode = i;
		//cout << "inserting " << currentNode << " at index " << 0 << endl;
		backwardsPath[0] = currentNode;
		int index = 1;
		bool noreach = false;
		while (pathArray[currentNode] != 0){
			if (pathArray[currentNode] == -1){
				noreach = true;
				backwardsPath[index] = -2;
				//cout << "Current node cannot be reached." << endl;
				break;
			}
			//cout << "Current node is " << currentNode << endl;
			//cout << "Arrived through " << pathArray[currentNode] << endl;
			//cout << "inserting " << pathArray[currentNode] << " at index " << index << endl;
			backwardsPath[index] = pathArray[currentNode];
			currentNode = pathArray[currentNode];
			index++;
		}
		if (i != 0 && !noreach){
			//cout << "inserting " << 0 << " at index " << index << endl;
			backwardsPath[index] = 0;
		} 

		//cout << "BACKWARD_PATH: ";
		for (int z = 0; z < numNodes; z++){
			//cout << backwardsPath[z] << " ";
		}
		//cout << endl;

		for (int x = numNodes -1; x >= 0; x--){
			//cout << "x is " << x << endl;
			if (backwardsPath[x] == -1){
				continue;
			}
			if (backwardsPath[x] == -2) {
				s.append("no path to");
				ss << i;
				s.append(ss.str());
				ss.str(string());
				s.append("\n");
				cout << "no path to " << i << endl;
				break;
			}
			if (x == 0){
				ss << backwardsPath[x];
				s.append(ss.str());
				ss.str(string());
				s.append("\n");
				cout << backwardsPath[x] << endl;
			} else {
				ss << backwardsPath[x];
				s.append(ss.str());
				ss.str(string());
				s.append("->");
				cout << backwardsPath[x] << "->";
			}
		}
	}
	s.append("Iteration: ");
	ss << iter;
	s.append(ss.str());
	ss.str(string());
	s.append("\n\n");
	cout << "Iteration: " << iter << endl;

	int n = s.length(); 
	char char_array[n+1];
	strcpy(char_array, s.c_str()); 
	int stringlen = strlen(char_array);
	char_array[stringlen - 1] = '\0';
	cout << "Sending message back to client" << endl;
	write(fd, char_array, strlen(char_array));
	usleep(100);
}
