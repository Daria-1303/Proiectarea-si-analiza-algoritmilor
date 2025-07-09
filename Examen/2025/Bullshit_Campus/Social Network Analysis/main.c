/*
A social media platform wants to analyze user connections to understand community formation. 
Given friendship data between users, the platform needs to:

	Identify isolated groups of friends who have no connections to other groups  
	Determine the size of the largest friend circle
	Find users who could serve as bridges between different communities
	Calculate how many separate social clusters exist in the network
	You are provided with a dataset of user friendships where each friendship is bidirectional. 4
	
	Design an algorithm to analyze these social connections and answer queries about community structure.
*/

#include <stdio.h>
#include <stdlib.h>

// ----------------
// Defines
// ----------------

#define DEBUG 0

#define MAX_NODES 50

// ----------------
// Structures
// ----------------

typedef struct Graph {
	int numberOfNodes;
	int adjacencyMatrix[MAX_NODES][MAX_NODES];
}Graph_T;

// ----------------
// Function 
// ----------------

void initGraph(Graph_T* graph) {
	memset(graph, 0, sizeof(Graph_T));
}

void readFromFile(Graph_T* graph, FILE* fin) {
	for (int i = 0; i < graph->numberOfNodes; i++) {
		for (int j = 0; j < graph->numberOfNodes; j++) {
			if (fscanf(fin, "%d", &graph->adjacencyMatrix[i][j]) != 1) {
				perror("Error reading adjacency matrix\n");
				exit(1);
			}
		}
	}
}
void printGraph(const Graph_T* graph) {
	for (int i = 0; i < graph->numberOfNodes; i++) {
		for (int j = 0; j < graph->numberOfNodes; j++) {
			printf("%d ", graph->adjacencyMatrix[i][j]);
		}
		printf("\n");
	}
}

void dfsRecursive(Graph_T graph, int start, int* visited, int *size) {
	visited[start] = 1;
	printf("Visited node: %d\n", start);

	for (int i = 0; i < graph.numberOfNodes; i++) {
		if (graph.adjacencyMatrix[start][i] == 1 && visited[i] == 0) {
			*size += 1; // Increment size for each node visited
			dfsRecursive(graph, i, visited, size);
		}
	}
}

int isolatedGroups(Graph_T graph, int sizeIsolatedGroups[MAX_NODES]) {
	int visited[MAX_NODES] = {0};
	int numIsolatedGroups = 0;
	

	for (int i = 0; i < graph.numberOfNodes; i++) {
		if (visited[i] == 0) {
			numIsolatedGroups++;
			int size = 1;
			dfsRecursive(graph, i, visited, &size);
			sizeIsolatedGroups[numIsolatedGroups - 1] = size; // Store the size of the isolated group
		}
	}

	return numIsolatedGroups;
}

int largestFriendCircle(int size[]) {
	int max = 0;

	for (int i = 0; i < MAX_NODES; i++) {
		if (size[i] == 0) {
			break;
		}
		if (size[i] > max) {
			max = size[i];
		}
	}

	return max;
}

// merge pe fiecare muchie in afara de cea legata de nodul j
void dfsBridge(Graph_T graph, int start, int* visited, int j) {
	visited[start] = 1;
	//printf("Visited node: %d\n", start);

	for (int i = 0; i < graph.numberOfNodes; i++) {
		if (graph.adjacencyMatrix[start][i] == 1 && visited[i] == 0 && j != i) {
			dfsBridge(graph, i, visited, j);
		}
	}
}

int bridgePoints(Graph_T graph, int numIsolatedGroups) {
	int visited[MAX_NODES] = { 0 };
	int count = 0;

	for (int j = 0; j < graph.numberOfNodes; j++) {
		for (int i = 0; i < graph.numberOfNodes;i++) {
			if (visited[i] == 0 && i != j) {
				count++;
				dfsBridge(graph, i, visited, j);
			}
		}

		if (count != numIsolatedGroups) {
			printf("Node %d could be a bridge point\n", j);
		}
		count = 0; // Reset count for the next iteration
		memset(visited, 0, sizeof(visited)); // Reset visited for the next iteration
	}
}



int main(int argc, char** argv) {
	if (argc < 2) {
		perror("Incorrect ussage : ./main <input_file>\n");
		return 1;
	}

	FILE* fin = fopen(argv[1], "r");

	if (!fin) {
		perror("Error opening file\n");
		return 1;
	}

	Graph_T graph;
	initGraph(&graph);

	if (fscanf(fin, "%d", &graph.numberOfNodes) != 1) {
		perror("Error reading number of nodes\n");
		fclose(fin);
		return 1;
	}

	readFromFile(&graph, fin);

	if (DEBUG) {
		printf("Adjacency Matrix:\n");
		printGraph(&graph);
	}

	int visited[MAX_NODES] = { 0 };
	int sizeIsolatedGroups[MAX_NODES] = { 0 };
	int size;
	if(DEBUG) {
		printf("DFS Traversal:\n");
		dfsRecursive(graph, 0, visited, size);
	}

	int numIsolatedGroups = isolatedGroups(graph, sizeIsolatedGroups);
	if (DEBUG) {
		printf("Number of isolated groups: %d\n", numIsolatedGroups);
	}

	if (DEBUG) {
		printf("Sizes of isolated groups:\n");
		for (int i = 0; i < numIsolatedGroups; i++) {
			printf("%d ", sizeIsolatedGroups[i]);
		}
		printf("\n");
	}

	int largestCircle = largestFriendCircle(sizeIsolatedGroups);
	printf("Largest friend circle size: %d\n", largestCircle);

	printf("Number of isolated groups: %d\n", numIsolatedGroups);

	bridgePoints(graph, numIsolatedGroups);
	fclose(fin);

	return 0;
}