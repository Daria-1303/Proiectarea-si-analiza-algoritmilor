/*
	A city planning department needs to connect all neighborhoods to essential utilities (water, electricity, gas) at minimum cost. 
	
	They have cost estimates for running utility lines between different areas, including:

	Installation costs that vary based on terrain and distance

	The requirement that all neighborhoods must be connected (directly or indirectly)

	Budget constraints requiring the most economical solution

	The need to ensure no redundant connections that don't reduce total cost

	Given the cost matrix for connecting different neighborhoods, design an algorithm to determine the minimum cost to connect the entire city to utilities. 
	
	Compare different approaches and analyze which is more efficient for this specific application.
*/

/*
	-> graf ponderat, conex
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

// ----------------
// Defines
// ----------------

#define ENABLE_DEBUG 1

#define MAX_NODES 50

// ----------------
// Structures
// ----------------

typedef struct Graph {
	int numberOfNodes;
	int adjacencyMatrix[MAX_NODES][MAX_NODES];
}Graph_T;


// ----------------
// Functions
// ----------------

void initGraph(Graph_T* graph) {
	memset(graph, 0, sizeof(Graph_T));
}

void readFromFile(Graph_T* graph, FILE* fin) {
	if (fscanf(fin, "%d", &graph->numberOfNodes) != 1) {
		perror("Error reading number of nodes\n");
		exit(1);
	}

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
	printf("-----------------\n");
	if (graph->numberOfNodes <= 0) {
		printf("Graph is empty.\n");
		return;
	}
	printf("Number of nodes: %d\n", graph->numberOfNodes);
	printf("Adjacency Matrix:\n");
	for (int i = 0; i < graph->numberOfNodes; i++) {
		for (int j = 0; j < graph->numberOfNodes; j++) {
			printf("%d ", graph->adjacencyMatrix[i][j]);
		}
		printf("\n");
	}
	printf("-----------------\n");
}

void primAlgorithm(Graph_T graph, int startNode) {
	int visited[MAX_NODES] = { 0 };

	int totalCost = 0;

	visited[startNode] = 1;

	printf("Prim's Algorithm starting from node %d:\n", startNode);

	// we need to find the minimum edge that connects a visited node to an unvisited node for a (numberOfNodes - 1) times
	for (int i = 0; i < (graph.numberOfNodes - 1); i++) {
		int minCostPerEdge = INT_MAX;
		int u = -1;  // the node from which we will take the edge
		int v = -1;  // the node to which we will take the edge

		// we search for the minimum edge that connects a visited node to an unvisited node
		for (int j = 0; j < graph.numberOfNodes; j++) {
			// we verify if the node j is visited
			if (visited[j] == 1) {
				// we will search in the V nodes for the minimum edge that connects a visited node to an unvisited node
				for (int k = 0; k < graph.numberOfNodes; k++) {
					if (visited[k] == 0 && graph.adjacencyMatrix[j][k] != 0) {
						// we verify if the edge is smaller than the minimum cost found so far
						if (graph.adjacencyMatrix[j][k] < minCostPerEdge) {
							minCostPerEdge = graph.adjacencyMatrix[j][k];
							u = j;  // the node from which we will take the edge
							v = k;  // the node to which we will take the edge
						}
					}
				}
			}
		}


		visited[v] = 1;  // mark the node v as visited
		totalCost += minCostPerEdge;  // add the cost of the edge to the total cost
		printf("Connecting node %d to node %d with cost %d\n", u, v, minCostPerEdge);
	}

	printf("Total cost to connect the city: %d\n", totalCost);
}

void kruskalAlgorithm(Graph_T graph) {
	int S[MAX_NODES] = { 0 };

	int totalCost = 0;

	for (int i = 0; i < graph.numberOfNodes; i++) {
		S[i] = i;  // initialize the set S with the nodes
	}

	for (int i = 0; i < (graph.numberOfNodes - 1); i++) {
		int minCostPerEdge = INT_MAX;
		int u = -1;  // the node from which we will take the edge
		int v = -1;  // the node to which we will take the edge

		for (int j = 0; j < graph.numberOfNodes; j++) {
			for (int k = 0; k < graph.numberOfNodes; k++) {
				if (graph.adjacencyMatrix[j][k] != 0 && S[j] != S[k]) {
					// we verify if the edge is smaller than the minimum cost found so far
					if (graph.adjacencyMatrix[j][k] < minCostPerEdge) {
						minCostPerEdge = graph.adjacencyMatrix[j][k];
						u = j;  // the node from which we will take the edge
						v = k;  // the node to which we will take the edge
					}
				}
			}
		}
		
		for (int j = 0; j < graph.numberOfNodes; j++) {
			if (S[j] == S[v]) {
				S[j] = S[u];  // merge the sets
			}
		}

		printf("Connecting node %d to node %d with cost %d\n", u, v, minCostPerEdge);
		totalCost += minCostPerEdge;  // add the cost of the edge to the total cost
	}

	printf("Total cost to connect the city: %d\n", totalCost);
}

// ----------------
// MAIN function
// ----------------
int main(int argc, char** argv) {
	if (argc < 2) {
		perror("Usage: ./program <input_file>\n");
		return 1;
	}

	FILE* fin = fopen(argv[1], "r");

	if (!fin) {
		perror("Error opening file\n");
		return 1;
	}

	Graph_T graph;
	initGraph(&graph);

	readFromFile(&graph, fin);
	fclose(fin);

	if (ENABLE_DEBUG) {
		printGraph(&graph);
	}

	// Queries

	printf("PRIM:\n");
	primAlgorithm(graph, 0);  // Start Prim's algorithm from node 0

	printf("\nKRUSKAL:\n");
	kruskalAlgorithm(graph);  // Run Kruskal's algorithm on the graph

	return 0;
}