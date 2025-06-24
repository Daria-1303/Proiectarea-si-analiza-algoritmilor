/*
	A university library uses a computerized catalog system to organize millions of books. 
	
	You receive a dump of their index structure in the following format:

	[m=5] "Fiction"|["Adventure","Biography"]|["History","Mathematics","Physics"]|["Art","Chemistry"]|["Drama","Fantasy"]|["Geology","Medicine"]|["Poetry","Science","Technology","Zoology"]

	The format represents book categories where m=5 indicates the maximum number of categories per node, 
	
	quoted strings are internal node categories, and bracketed lists are leaf collections.

	Your task is to:

		Parse and validate this catalog structure
		Implement a search function to find which section contains a given book category
		Calculate the maximum number of comparisons needed to find any category
		Determine if the structure is optimally balanced for search operations
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "arboreB.h"    // definește Pagina, Nod, insereaza, afisare, cautare, suprima

// Externi din bibliotecă
extern int nr_scindari;
extern int nr_pasi_cautare;
// extern int inaltime;   <- eliminat, calculăm înălțimea manual

// --- Funcții auxiliare pentru parsare și validare ---

static char* trim(char* s) {
    while (*s && isspace((unsigned char)*s)) s++;
    char* end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
    return s;
}

static char** split(const char* str, char delim, int* out_count) {
    char* tmp = strdup(str);
    int count = 1;
    for (char* p = tmp; *p; p++) if (*p == delim) count++;
    char** tokens = malloc(count * sizeof(char*));
    int idx = 0;
    char* saveptr;
    char delim_str[2] = { delim, '\0' };
	char* token = strtok_r(tmp, delim_str, &saveptr);
    while (token) {
        tokens[idx++] = strdup(token);
		token = strtok_r(NULL, delim_str, &saveptr);
    }
    *out_count = idx;
    free(tmp);
    return tokens;
}

static bool validate_leaf_list(char* content) {
    return strchr(content, '"') != NULL;
}

static bool verificaEchilibruRec(Pagina* pag, int nivel, int* nivel_frunza) {
    if (!pag) return true;
    if (pag->p0 == NULL) {
        if (*nivel_frunza < 0)
            *nivel_frunza = nivel;
        return nivel == *nivel_frunza;
    }
    if (!verificaEchilibruRec(pag->p0, nivel + 1, nivel_frunza))
        return false;
    for (int i = 1; i <= pag->m; i++) {
        if (!verificaEchilibruRec(pag->e[i].p, nivel + 1, nivel_frunza))
            return false;
    }
    return true;
}

static bool esteOptEchilibrat(Pagina* root) {
    int nivel_frunza = -1;
    return verificaEchilibruRec(root, 0, &nivel_frunza);
}

// Calculează înălțimea B-arborelui (număr de nivele)
static int calculeazaInaltime(Pagina* root) {
    int h = 0;
    Pagina* p = root;
    while (p) {
        h++;
        p = p->p0;
    }
    return h;
}

// Calculează numărul maxim de comparații: inaltime * ceil(log2(m+1))
static int calculeazaComparatiiMax(Pagina* root) {
    if (!root) return 0;
    int m = root->m;
    int comp_nod = 0;
    int p = 1;
    while (p < m + 1) { p <<= 1; comp_nod++; }
    int h = calculeazaInaltime(root);
    return h * comp_nod;
}

// Simplă funcție hash din string în TipCheie
static TipCheie codCategorie(const char* s) {
    unsigned long h = 5381;
    int c;
    while ((c = *s++)) h = ((h << 5) + h) + c;
    return (TipCheie)(h & 0x7FFFFFFF);
}

int main(void) {
    const char* dump =
        "[m=5] "
        "\"Fiction\"|[\"Adventure\",\"Biography\"]|"
        "[\"History\",\"Mathematics\",\"Physics\"]|"
        "[\"Art\",\"Chemistry\"]|[\"Drama\",\"Fantasy\"]|"
        "[\"Geology\",\"Medicine\"]|"
        "[\"Poetry\",\"Science\",\"Technology\",\"Zoology\"]";

    int m_limit;
    if (sscanf(dump, "[m=%d]", &m_limit) != 1) {
        fprintf(stderr, "Eroare: format invalid, nu am găsit m=...\n");
        return EXIT_FAILURE;
    }
    printf("Valoarea maximă m per nod: %d\n", m_limit);

    const char* rest = strchr(dump, ']');
    if (!rest) return EXIT_FAILURE;
    rest++;

    int part_count;
    char** parts = split(rest, '|', &part_count);

    Pagina* root = NULL;
    for (int i = 0; i < part_count; i++) {
        char* tok = trim(parts[i]);
        if (tok[0] == '"') {
            size_t L = strlen(tok);
            if (tok[L - 1] == '"') {
                tok[L - 1] = '\0';
                char* key = tok + 1;
                root = insereaza(root, codCategorie(key), NULL);
            }
        }
        else if (tok[0] == '[') {
            size_t L = strlen(tok);
            if (tok[L - 1] == ']') {
                tok[L - 1] = '\0';
                char* inner = tok + 1;
                if (!validate_leaf_list(inner)) continue;
                int cnt;
                char** cats = split(inner, ',', &cnt);
                for (int j = 0; j < cnt; j++) {
                    char* c = trim(cats[j]);
                    size_t LL = strlen(c);
                    if (LL > 1 && c[0] == '"' && c[LL - 1] == '"') {
                        c[LL - 1] = '\0';
                        char* name = c + 1;
                        root = insereaza(root, codCategorie(name), NULL);
                    }
                    free(cats[j]);
                }
                free(cats);
            }
        }
        free(parts[i]);
    }
    free(parts);

    printf("\nStructura B-arbore (nivel: chei):\n");
    afisare(root, 0);

    char buf[128];
    printf("\nIntrodu categoria de căutat: ");
    if (fgets(buf, sizeof(buf), stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
        nr_pasi_cautare = 0;
        int found = cautare(root, codCategorie(buf));
        printf(found ? "Categoria '%s' găsită în %d pași\n"
            : "Categoria '%s' NU a fost găsită\n",
            buf, nr_pasi_cautare);
    }

    int h = calculeazaInaltime(root);
    int maxComp = calculeazaComparatiiMax(root);
    printf("\nÎnălțimea arborelui: %d nivele\n", h);
    printf("Număr maxim comparații: %d (nod root cu %d chei, binary-search ~%d pași/nod)\n",
        maxComp, root ? root->m : 0, (root ? (int)ceil(log2(root->m + 1)) : 0));

    printf(esteOptEchilibrat(root)
        ? "Structura este optim echilibrată (toate frunzele la același nivel)\n"
        : "Structura NU este optim echilibrată\n");

    return EXIT_SUCCESS;
}
