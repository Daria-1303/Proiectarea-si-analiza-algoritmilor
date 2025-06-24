#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DEBUG 1
#define ALPHABET_SIZE 26

int pagini_create = 0;

typedef struct ArboreRegasire{
    int final_cuvant;
    struct ArboreRegasire *copii[ALPHABET_SIZE];
}ArboreRegasire_T;

ArboreRegasire_T *creazaNod(){
    ArboreRegasire_T *nod =  (ArboreRegasire_T*) malloc(sizeof(ArboreRegasire_T));

    if(nod == NULL){
        perror("Eroare la alocarea memoriei pentru nod.\n");
        return NULL;
    }

    nod->final_cuvant = 0;

    for(int i = 0; i < ALPHABET_SIZE; i++){
        nod->copii[i] = NULL;
    }

    return nod;
}

/*
* @brief Adauga un cuvant in arborele de regasire.
* @param radacina Pointer la radacina arborelui de regasire.
* @param cuvant Cuvantul de adaugat in arbore.
*/

void adaugaCuvant(ArboreRegasire_T* radacina, const char *cuvant){
    ArboreRegasire_T *nod_curent = radacina;

    // Parcurgem fiecare caracter din cuvant
    for(int i = 0; cuvant[i]; i++){
        // calculam indexul in functie de caracter
        int index = cuvant[i] - 'a';

        if(index < 0 || index >= ALPHABET_SIZE){
            fprintf(stderr, "Caracter invalid: %c in cuvantul %s\n", cuvant[i], cuvant);
            return;
        }

        // Daca nodul copil nu exista, il cream
        if(nod_curent->copii[index] == NULL){
            nod_curent->copii[index] = creazaNod();

            pagini_create++;
        }

        // Mergem la nodul copil
        nod_curent = nod_curent->copii[index];
    }

    // Marcam nodul curent ca fiind final de cuvant
    nod_curent->final_cuvant = 1;
}

void elibereazaArbore(ArboreRegasire_T *radacina){
    if(radacina == NULL) return;

    // Eliberam recursiv toti copiii
    for(int i = 0; i < ALPHABET_SIZE; i++){
        elibereazaArbore(radacina->copii[i]);
    }

    // Eliberam nodul curent
    free(radacina);
}

void dfsPrint(ArboreRegasire_T* radacina, char *buffer, int nivel){
    if(radacina == NULL){
        return;
    }

    if(radacina->final_cuvant == 1){
        buffer[nivel] = '\0'; // Terminam stringul
        printf("%s\n", buffer); // Afisam cuvantul
    }

    for(int i = 0; i < ALPHABET_SIZE; i++){
        // verificam daca avem litere in noua lista
        if(radacina->copii[i] != NULL){
            // Adaugam litera curenta in buffer
            buffer[nivel] = 'a' + i; // Convertim indexul inapoi la caracter
            // Apelam recursiv pentru copilul curent
            dfsPrint(radacina->copii[i], buffer, nivel + 1);
        }
    }
}

/**
 * @brief calculeaza lungimea medie a cuvintelor din arbore
*/

void lungimeMedieCuvinte(ArboreRegasire_T *radacina, int nivel, int *numar_cuvinte, int *lungime_totala){
    if(radacina->final_cuvant == 1){
        *numar_cuvinte = *numar_cuvinte + 1;
        *lungime_totala = *lungime_totala + nivel;
    }   

    for(int i = 0; i < ALPHABET_SIZE; i++){
        if(radacina->copii[i] != NULL){
            lungimeMedieCuvinte(radacina->copii[i], nivel + 1, numar_cuvinte, lungime_totala);
        }
    }
}

/**
 * @brief o functie care calculeaza inaltimea arborelui
*/

/**
 * @brief o functie care calculeaza nr de pagini din arbore
 */





int main(int argc, char **argv){
    if(argc != 2){
        perror("Utilizare: ./p <fisier_intrare>\n");
        return 1;
    }

    FILE *fisier = fopen(argv[1], "r");
    if(fisier == NULL){
        perror("Eroare la deschiderea fisierului.\n");
        return 1;
    }

    ArboreRegasire_T *radacina = creazaNod();

    if(radacina == NULL){
        fclose(fisier);
        return 1;
    }

    char cuvant[100];

    while(fscanf(fisier, "%s", cuvant) == 1){
        // Convertim cuvantul la litere mici
        for(int i = 0; cuvant[i]; i++){
            cuvant[i] = tolower(cuvant[i]);
        }

        adaugaCuvant(radacina, cuvant);
    }

    fclose(fisier);

    char buffer[100];
    printf("Cuvintele din arborele de regasire sunt:\n");
    dfsPrint(radacina, buffer, 0);


    int numar_cuvinte = 0;
    int lungime_totala = 0;

    lungimeMedieCuvinte(radacina, 0, &numar_cuvinte, &lungime_totala);

    if(numar_cuvinte > 0){
        double lungime_medie = (double)lungime_totala / numar_cuvinte;
        printf("Lungimea medie a cuvintelor este: %.2f\n", lungime_medie);
    } else {
        printf("Nu exista cuvinte in arbore.\n");
    }


    elibereazaArbore(radacina);
    if(DEBUG){
        printf("Arborele de regasire a fost eliberat.\n");
    }

    return 0;
}