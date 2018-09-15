# bellman-ford

Compilation steps: run the command `makeAll.sh` OR
run the following individual commands:
g++ BellmanFord.cpp -o BellmanFord
g++ BellmanFordPipe.cpp -o BellmanFordPipe
g++ GraphGen.cpp -o GraphGen
g++ GraphGenPipe.cpp -o GraphGenPipe

Additional Information: 

The BellmanFord executable will write the solution to stdout as well as to a
file with the naming convention "N{num_nodes}solution.txt". The GraphGen
executable will also write the random graph to stdout as well as to a file
with the naming convention "N{num_nodes}-graph.txt".

For the pipe processes, the client is the random graph generator and the
server is the bellmanford algorithm process. The graph generator needs to 
be run with the number of nodes provided as the first and only argument 
(i.e. ./GraphGen 5) and both processes output relevant information to stdout.

The graph generator process will send the graph over the pipe to the bellmanford process. 
The bellmanford process will find the solution to the graph and send it back over the pipe
to the graph generator who writes the solution to a file named "graph-{#}-solution.txt".

The bellmanford process should be started first because it waits for the graph generator to start.
I believe the processes should be started from the same working directory so that they can 
properly share the pipe file.

When no path is found, the distances array (first output line) and path list in 
the BellmanFord and BellmanFordPipe outputs will show "no path".

There is an intentional delay each time the client sends the random graph to 
the server. 
