// vrem sa afisam cel mai lung cuvant si cel mai scurt cuvant
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26

// --- Nodul unui Trie ---
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool is_end_of_word;
} TrieNode;

// --- Creează un nod nou ---
TrieNode* createNode(void) {
    TrieNode* node = malloc(sizeof(TrieNode));
    if (!node) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    node->is_end_of_word = false;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        node->children[i] = NULL;
    return node;
}

// --- Inserează un cuvânt în Trie ---
void insert(TrieNode* root, const char* word) {
    TrieNode* p = root;

    for (int i = 0; word[i]; i++) {
		int idx = word[i] - 'a'; // return 0 - 25 pentru 'a' - 'z'
		if (idx < 0 || idx >= ALPHABET_SIZE) continue; // ignoră caracterele nevalide

		if (!p->children[idx]) // dacă nu există nodul pentru caracterul curent
            p->children[idx] = createNode();

		// Mergi la nodul corespunzător caracterului curent
        p = p->children[idx];
    }
    p->is_end_of_word = true;
}

// --- Parcurgere DFS și printare a cuvintelor stocate --- 
void dfsPrint(TrieNode* node, char buffer[], int depth) {
	if (node->is_end_of_word) {// dacă este sfârșitul unui cuvânt
		buffer[depth] = '\0';  // termină șirul de caractere
        printf("%s\n", buffer);
    }

	// Parcurge copiii nodului curent
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            buffer[depth] = 'a' + i;
            dfsPrint(node->children[i], buffer, depth + 1);
        }
    }
}

// --- Eliberează memoria Trie-ului ---
void freeTrie(TrieNode* node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        freeTrie(node->children[i]);
    free(node);
}

// cel mai lung cuvant

void findLongestWord(TrieNode* node, char buffer[], int depth, char** longestWord, int* maxLength) {
	if (node->is_end_of_word) {
		buffer[depth] = '\0'; // termină șirul de caractere
		if (depth > *maxLength) {	// dacă lungimea curentă este mai mare decât cea maximă găsită
			*maxLength = depth;
			*longestWord = strdup(buffer); // alocă și copiază cuvântul
		}
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			buffer[depth] = 'a' + i;
			findLongestWord(node->children[i], buffer, depth + 1, longestWord, maxLength);
		}
	}
}

// cel mai scurt cuvant

void findShortestWord(TrieNode* node, char buffer[], int depth, char** shortestWord, int* minLength) {
	if (node->is_end_of_word) {
		buffer[depth] = '\0'; // termină șirul de caractere
		if (depth < *minLength) { // dacă lungimea curentă este mai mică decât cea minimă găsită
			*minLength = depth;
			*shortestWord = strdup(buffer); // alocă și copiază cuvântul
		}
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			buffer[depth] = 'a' + i;
			findShortestWord(node->children[i], buffer, depth + 1, shortestWord, minLength);
		}
	}
}

// ultim cuvant

void findLastWord(TrieNode* node, char buffer[], int depth, char** lastWord) {
	if (node->is_end_of_word) {
		buffer[depth] = '\0'; // termină șirul de caractere
		*lastWord = strdup(buffer); // alocă și copiază cuvântul
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			buffer[depth] = 'a' + i;
			findLastWord(node->children[i], buffer, depth + 1, lastWord);
		}
	}
}

// prim cuvant 

void findFirstWord(TrieNode* node, char buffer[], int depth, char** firstWord) {
	if (node->is_end_of_word) {
		buffer[depth] = '\0'; // termină șirul de caractere
		*firstWord = strdup(buffer); // alocă și copiază cuvântul
		return; // ieși din funcție după găsirea primului cuvânt
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			buffer[depth] = 'a' + i;
			findFirstWord(node->children[i], buffer, depth + 1, firstWord);
			if (*firstWord) return; // dacă s-a găsit primul cuvânt, ieși din funcție
		}
	}
}

// lungimea medie a cuvintelor din trie

void computeStats(TrieNode* node, int depth, int* totalWords, int* totalLength) {
	if (node->is_end_of_word) {
		(*totalWords)++;
		(*totalLength) += depth;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			computeStats(node->children[i], depth + 1,totalWords, totalLength);
		}
	}
}

// cuvintele care au a doua litera 'r'

void findWordsWithSecondR(TrieNode* node, char buffer[], int depth) {
	if (node->is_end_of_word && depth >= 1 && buffer[1] == 'r') {
		buffer[depth] = '\0'; // termină șirul de caractere
		printf("%s\n", buffer);
	}
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (node->children[i]) {
			buffer[depth] = 'a' + i;
			findWordsWithSecondR(node->children[i], buffer, depth + 1);
		}
	}
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: ./p filename\n");
		return 1;
	}

	FILE* file = fopen(argv[1], "r");
	if (!file) {
		perror("fopen");
		return 1;
	}

	TrieNode* root = createNode();
	char word[100]; // presupunem că cuvintele nu depășesc 99 de caractere
	while (fscanf(file, "%99s", word) == 1) { // citește cuvinte din fișier
		insert(root, word);
	}
	fclose(file);

	char buffer[100]; // buffer pentru cuvinte
	dfsPrint(root, buffer, 0); // parcurge și afișează cuvintele
	

	char* longestWord = NULL;
	int maxLength = 0;
	findLongestWord(root, buffer, 0, &longestWord, &maxLength);
	if (longestWord) {
		printf("Cel mai lung cuvant este: %s (lungime: %d)\n", longestWord, maxLength);
		free(longestWord); // eliberează memoria alocată pentru cuvântul cel mai lung
	}
	else {
		printf("Nu s-au găsit cuvinte.\n");
	}


	char* shortestWord = NULL;
	int minLength = 100; // presupunem că cuvintele nu depășesc 99 de caractere
	findShortestWord(root, buffer, 0, &shortestWord, &minLength);
	if (shortestWord) {
		printf("Cel mai scurt cuvant este: %s (lungime: %d)\n", shortestWord, minLength);
		free(shortestWord); // eliberează memoria alocată pentru cuvântul cel mai scurt
	}
	else {
		printf("Nu s-au găsit cuvinte.\n");
	}


	char* lastWord = NULL;
	findLastWord(root, buffer, 0, &lastWord);
	if (lastWord) {
		printf("Ultimul cuvant este: %s\n", lastWord);
		free(lastWord); // eliberează memoria alocată pentru ultimul cuvânt
	}
	else {
		printf("Nu s-au găsit cuvinte.\n");
	}

	char* firstWord = NULL;
	findFirstWord(root, buffer, 0, &firstWord);
	if (firstWord) {
		printf("Primul cuvant este: %s\n", firstWord);
		free(firstWord); // eliberează memoria alocată pentru primul cuvânt
	}
	else {
		printf("Nu s-au găsit cuvinte.\n");
	}

	int totalWords = 0, totalLength = 0;
	computeStats(root, 0, &totalWords, &totalLength);
	if (totalWords > 0) {
		double avg = (double)totalLength / totalWords;
		printf("Lungimea medie: %.2f\n", avg);
	}

	
	char* secondRWords = NULL;
	printf("Cuvintele care au a doua litera 'r':\n");
	findWordsWithSecondR(root, buffer, 0);
	free(secondRWords); // eliberează memoria alocată pentru cuvintele cu a doua litera 'r'

	char* suggestions = NULL;
	printf("Sugestii pentru cuvântul 'armata':\n");
	findSuggestions(root, buffer, 0, "armta");


	freeTrie(root); // eliberează memoria Trie-ului
	printf("Program succes\n");

    return 0;
}
