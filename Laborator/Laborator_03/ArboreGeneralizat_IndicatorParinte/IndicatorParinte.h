#include <stdio.h>
#include <stdint.h>

#define MAXNODES 100

typedef uint8_t TKey;

typedef struct {
	uint8_t parent;		// parintele nodului	(in fisierul data.txt) -> a doua valoare din linie
	TKey key;			// valoarea nodului		(in fisierul data.txt) -> prima valoare din linie
}TNode;

typedef uint8_t TNodeRef;

typedef struct {
	TNode nodes[MAXNODES];
	uint8_t size; //tree size = number of nodes in the tree	
}TTree;

TTree InitTree(TTree a);

TTree InsertNode(TTree a, TNodeRef  parent, TKey key);

void printArray(TTree a);		// printare arbore

void printNode(TNode node, char* message);	// printare nod

TNodeRef getRoot(TTree a);

TNodeRef parent(TTree a, TNodeRef noderef);	

TKey nodeKey(TTree a, TNodeRef noderef);

TNode nodeByRef(TTree a, TNodeRef noderef);

TNodeRef firstChild(TTree a, TNodeRef noderef);

TNodeRef rightSibling(TTree a, TNodeRef noderef);

TTree deleteNode(TTree a, TNodeRef noderef);



void preOrderRecursiv(TTree arbore, TNodeRef radacinaCurenta);
void postOrderRecursiv(TTree arbore, TNodeRef radacinaCurenta);
void inOrderRecursiv(TTree arbore, TNodeRef radacinaCurenta);