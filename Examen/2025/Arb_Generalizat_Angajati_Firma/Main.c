#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "PrimFiu_FrateDreapta.h"

TTree readFile2(FILE* fin, TTree arbore) {
	TKey cheie = 0;
	TNodeRef primFiu = 0;
	TNodeRef frateDreapta = 0;

	while (fscanf(fin, "%hhu %hhu %hhu", &cheie, &primFiu, &frateDreapta) == 3) {
		printf("Values read: %hhu %hhu %hhu\n", cheie, primFiu, frateDreapta);

		arbore = insertNodePrimFiuFrateDreapta(arbore, primFiu, frateDreapta, cheie);
	}

	return arbore;
}

void printLeaves(TTree arbore) {
	TNodeRef root = getRoot(arbore);

	if (root == 0) {
		printf("Arborele este gol.\n");
		return;
	}

	printf("Angajati fara subalterni : \n");

	for (TNodeRef i = 1; i <= arbore.size; i++) {
		if (arbore.nodes[i].primFiu == 0) { // daca nu are copii
			printf("Angajat: %d\n", arbore.nodes[i].key);
		}
	}
	
	printf("\n");
}

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE* fin = fopen(argv[1], "r");
	if (fin == NULL) {
		perror("Failed to open file");
		return 1;
	}

	TTree arbore = { 0 };
		
	arbore = InitTree(arbore);
	arbore = readFile2(fin, arbore);
	fclose(fin);

	printArray(arbore);
	printf("\n\n");
	printLeaves(arbore);

	return 0;
}