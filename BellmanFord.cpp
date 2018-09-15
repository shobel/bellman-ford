#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>
#include <stdio.h>
using namespace std;

void runBellmanFord(int** graph, int numNodes);

bool debug = false; 

int main(int argc, char* argv[]){

	if (!argv[1]){
		cout << "Input file required" << endl;
		exit(1);
	}

	string fileString = argv[1];
	if (fileString.find(".csv") == string::npos && fileString.find(".txt") == string::npos) {
		cout << "Input file must be in csv or txt format" << endl;
		exit(1);
	}

	ifstream in;
	in.open(argv[1]);

	int** graph;
	string s;
	int numNodes = 0;
	int counter = 0;
	while (getline(in, s)) {
		if (debug){ cout << "Line " << counter << " is: " << s << endl; }
		if (counter == 0){
			//first line of file: we will end up iterating over this line twice, first to get number of nodes and then to populate graph
			for (int i = 0; i < s.size(); i++){
				if (s[i] == ','){
					numNodes++;
				}
			}
			numNodes++;

			//now we know how many nodes there are, we can init the graph
			graph = new int*[numNodes];
			for (int i = 0; i < numNodes; i++){
				graph[i] = new int[numNodes];
			}
		}

		//graph has been initialized, add data to graph  
		for (int i = 0; i < s.size(); i++){

			if (s[i] == ',') {
				s.erase(i, 1);
				s.insert(i, " ");
			} else if (s[i] == '*'){
				s.erase(i, 1);
				s.insert(i, "-1");
			}
		}

		if (debug){cout << "Parsed line: " << s << endl;}
		stringstream stream;
		stream << s;
		for (int j = 0; j < numNodes; j++) {
			//if (debug) { cout << "graph[" << counter << "][" << j << "] is " << stream << endl; }
			stream >> graph[counter][j];
		}

		counter++;
    }

    if (debug) {
    	for (int x = 0; x < numNodes; x++){
    		cout << "[ ";
    		for (int y = 0; y < numNodes; y++){
    			cout << graph[x][y] << " ";
    		}
    		cout << "]" << endl;
    	}
	}

    if (counter == 0){
    	cout << "File was empty" << endl;
    	exit(1);
    }

    runBellmanFord(graph, numNodes);
}

void runBellmanFord(int** graph, int numNodes){
	int distancesArray[numNodes];
	int pathArray[numNodes];
	for (int i = 0; i < numNodes; i++){
		distancesArray[i] = INT_MAX;
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

	stringstream stream;
  	stream << "N" << numNodes << "solution.txt";
	char *y = new char[stream.str().length() + 1]; 
	strcpy(y, stream.str().c_str());

  	ofstream solutionFile;
	solutionFile.open(y,fstream::out | ios::app);
	int i = 0;
	for (i = 0; i < numNodes; i++){
		if (i==0){
			cout << distancesArray[i];
			solutionFile << distancesArray[i];
		} else {
			cout << "," << distancesArray[i];
			solutionFile << "," << distancesArray[i];
		}
	}
	solutionFile << "\n";
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
		while (pathArray[currentNode] != 0){
			//cout << "Current node is " << currentNode << endl;
			//cout << "Arrived through " << pathArray[currentNode] << endl;
			//cout << "inserting " << pathArray[currentNode] << " at index " << index << endl;
			backwardsPath[index] = pathArray[currentNode];
			currentNode = pathArray[currentNode];
			index++;
		}
		if (i != 0){
			//cout << "Adding 0" << endl;
			//cout << "inserting " << 0 << " at index " << index << endl;
			backwardsPath[index] = 0;
		}

		for (int x = numNodes -1; x >= 0; x--){
			if (backwardsPath[x] == -1){
				continue;
			}
			if (x == 0){
				cout << backwardsPath[x] << endl;
				solutionFile << backwardsPath[x] << "\n";
			} else {
				cout << backwardsPath[x] << "->";
				solutionFile << backwardsPath[x] << "->";
			}
		}
	}
	cout << "Iteration: " << iter << endl;
	solutionFile << "Iteration: " << iter << "\n";
	solutionFile.close();
}
