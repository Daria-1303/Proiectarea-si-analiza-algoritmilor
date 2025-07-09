#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26   // ajustați dacă acceptați spații sau alte caractere
#define TOP_K         10

// --- Structuri de date ---
typedef struct Suggestion {
    const char* query;    // pointer în tabelă globală de stringuri
    int         freq;
} Suggestion;

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool              is_end_of_query;
    int               freq;               // valid numai dacă is_end_of_query==true

    Suggestion        topSuggestions[TOP_K];
    int               topCount;           // cât de multe sugestii avem populat (≤TOP_K)
} TrieNode;

// --- Prototipuri ---
TrieNode* createNode(void);
void      updateTopK(Suggestion arr[], int* count, const char* q, int f);
void      insertQuery(TrieNode* root, const char* q, int f);
int       autocomplete(TrieNode* root, const char* prefix, Suggestion out[TOP_K]);
void      freeTrie(TrieNode* node);
int       indexOf(char c);

// --- Implementări ---

// Creează un nod gol
TrieNode* createNode(void) {
    TrieNode* node = malloc(sizeof * node);
    if (!node) { perror("malloc"); exit(EXIT_FAILURE); }
    node->is_end_of_query = false;
    node->freq = 0;
    node->topCount = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        node->children[i] = NULL;
    return node;
}

// Returnează indicele din 0..ALPHABET_SIZE-1 pentru un caracter
int indexOf(char c) {
    return c - 'a';  // presupune doar litere mici a–z
}

// Încorporare / actualizare a unui tuple (q,f) în vectorul de top-K
void updateTopK(Suggestion arr[], int* count, const char* q, int f) {
    // 1) caut dacă există deja
    for (int i = 0; i < *count; i++) {
        if (strcmp(arr[i].query, q) == 0) {
            arr[i].freq = f;              // actualizez frecvența
            // apoi "bubble down/up" pentru a menține sortarea descrescătoare
            for (int j = i; j > 0 && arr[j].freq > arr[j - 1].freq; j--) {
                Suggestion tmp = arr[j];
                arr[j] = arr[j - 1];
                arr[j - 1] = tmp;
            }
            return;
        }
    }
    // 2) dacă nu există și e loc
    if (*count < TOP_K) {
        arr[*count].query = q;
        arr[*count].freq = f;
        (*count)++;
        // inserez la final și apoi urc ca un heap
        for (int j = *count - 1; j > 0 && arr[j].freq > arr[j - 1].freq; j--) {
            Suggestion tmp = arr[j];
            arr[j] = arr[j - 1];
            arr[j - 1] = tmp;
        }
        return;
    }
    // 3) dacă nu e loc și frecvența e mai mare decât a 10-a
    if (f > arr[*count - 1].freq) {
        arr[*count - 1].query = q;
        arr[*count - 1].freq = f;
        // urc noul element
        for (int j = *count - 1; j > 0 && arr[j].freq > arr[j - 1].freq; j--) {
            Suggestion tmp = arr[j];
            arr[j] = arr[j - 1];
            arr[j - 1] = tmp;
        }
    }
}

// Inserează un query și actualizează toate nodurile din drumul său
void insertQuery(TrieNode* root, const char* q, int f) {
    TrieNode* p = root;
    for (int i = 0; q[i]; i++) {
        int idx = indexOf(q[i]);
        if (idx < 0 || idx >= ALPHABET_SIZE) continue;
        if (!p->children[idx])
            p->children[idx] = createNode();
        p = p->children[idx];
        updateTopK(p->topSuggestions, &p->topCount, q, f);
    }
    p->is_end_of_query = true;
    p->freq = f;
}

// Returnează în out[0..return-1] cele TOP_K sugestii pentru prefix
int autocomplete(TrieNode* root, const char* prefix, Suggestion out[TOP_K]) {
    TrieNode* p = root;
    for (int i = 0; prefix[i]; i++) {
        int idx = indexOf(prefix[i]);
        if (idx < 0 || idx >= ALPHABET_SIZE) return 0;
        if (!p->children[idx]) return 0;
        p = p->children[idx];
    }
    for (int i = 0; i < p->topCount; i++)
        out[i] = p->topSuggestions[i];
    return p->topCount;
}

// Eliberează memoria trie-ului
void freeTrie(TrieNode* node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        freeTrie(node->children[i]);
    free(node);
}

// --- Demonstrație în main ---
int main(void) {
    // 1) Bulk load inițial
    const char* queries[] = {
        "how to cook", "how to code", "how to train your dragon",
        "how to tie a tie", "how to lose weight", "home workout",
        "home remedies", "holiday destinations", "hollywood news",
        "hollow knight", "hollow man"
    };
    int freqs[] = {
        1500, 2300, 500, 1200, 900, 800, 600, 400, 700, 300, 200
    };
    int N = sizeof(queries) / sizeof(queries[0]);

    TrieNode* root = createNode();
    for (int i = 0; i < N; i++)
        insertQuery(root, queries[i], freqs[i]);

    // 2) Interogări de test
    char prefix[100];
    Suggestion out[TOP_K];

    while (true) {
        printf("\nPrefix (enter gol pentru stop): ");
        if (!fgets(prefix, sizeof prefix, stdin)) break;
        prefix[strcspn(prefix, "\r\n")] = '\0';
        if (prefix[0] == '\0') break;

        int k = autocomplete(root, prefix, out);
        if (k == 0) {
            printf("  (fără sugestii)\n");
        }
        else {
            printf("Sugestii pentru \"%s\":\n", prefix);
            for (int i = 0; i < k; i++) {
                printf("  %d. %s (%d căutări)\n",
                    i + 1, out[i].query, out[i].freq);
            }
        }
    }

    freeTrie(root);
    return 0;
}
