/*
	A manufacturing company tracks supplier relationships to assess supply chain risks. 
	
	They need to understand:

	Which suppliers can ultimately affect each product (direct and indirect dependencies)

	How disruption at one supplier could cascade through the supply chain

	Which supplier relationships create the most vulnerability

	The complete dependency matrix for risk planning

	Given supplier relationship data, design an algorithm to compute all possible supply chain dependencies and analyze systemic risks.
*/

/*
	graf ponderat orientat
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

// we ll use warshall algorithm to compute the transitive closure of the graph

void warshall(Graph_T graph) {
	int W[MAX_NODES][MAX_NODES];
	// 1 -> path, 0 -> no path

	for (int i = 0; i < graph.numberOfNodes; i++) {
		for (int j = 0; j < graph.numberOfNodes; j++) {
			W[i][j] = (graph.adjacencyMatrix[i][j] != 0 ? 1 : 0);
		}
	}

	for (int k = 0; k < graph.numberOfNodes; k++) {
		for (int i = 0; i < graph.numberOfNodes; i++) {
			for (int j = 0; j < graph.numberOfNodes; j++) {
				if (W[i][j] == 0) {
					W[i][j] = W[i][k] && W[k][j];
				}
			}
		}
	}

	for (int i = 0; i < graph.numberOfNodes; i++) {
		for (int j = 0; j < graph.numberOfNodes; j++) {
			printf("%d ", W[i][j]);
		}
		printf("\n");
	}

	printf("Risks for each node:\n");
	for (int i = 0; i < graph.numberOfNodes; i++) {
		int riskCount = 0;
		for (int j = 0; j < graph.numberOfNodes; j++) {
			if (W[i][j] == 1 && i != j) {
				riskCount++;
			}
		}

		if (riskCount > 0) {
			printf("Node %d can be affected by %d other nodes.\n", i, riskCount);
		}
		else {
			printf("Node %d has no risks.\n", i);
		}
	}

	printf("How many risks provoke each node:\n");
	for (int i = 0; i < graph.numberOfNodes; i++) {
		int riskCount = 0;
		for (int j = 0; j < graph.numberOfNodes; j++) {
			if (W[j][i] == 1 && i != j) {
				riskCount++;
			}
		}
		if (riskCount > 0) {
			printf("Node %d can cause %d risks.\n", i, riskCount);
		}
		else {
			printf("Node %d does not cause any risks.\n", i);
		}
	}
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

	warshall(graph);

	return 0;
}