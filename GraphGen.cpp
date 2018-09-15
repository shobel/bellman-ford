#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <stdio.h>
using namespace std;

int main(int argc, char* argv[]){

	srand(time(NULL));
	int costMin = 1;
	int costMax = 100;

	if (!argv[1]){
		cout << "Error: first argument must provide the number of nodes." << endl;
		exit(1);
	}

	int numNodes = atoi(argv[1]);
	//cout << "Number of nodes: " << numNodes << endl;

	int graph[numNodes][numNodes];
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

	stringstream stream;
  	stream << "N" << numNodes << "-graph.txt";
	char *y = new char[stream.str().length() + 1]; 
	strcpy(y, stream.str().c_str());
	ofstream graphFile;
	graphFile.open(y,fstream::out | ios::app);

	for (int i = 0; i < numNodes; i++){
		for (int j = 0; j < numNodes; j++) {
			if (graph[i][j] == -1){
				cout << "*";
				graphFile << "*";
			} else {
				cout << graph[i][j];
				graphFile << graph[i][j];
			}

			if (j != numNodes-1){
				cout << ",";
				graphFile << ",";
			}
		}
		cout << endl;
		graphFile << "\n";
	}
}
