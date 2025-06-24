#ifndef __ARBOREB__
#define __ARBOREB__

#define N   2
#define NN (N*2)

typedef int TipCheie;

struct Nod
{
	TipCheie cheie;
	struct Pagina* p;
	int contor;
};
typedef struct Nod Nod;

struct Pagina
{
	int m;
	struct Pagina* p0;
	Nod e[NN + 1];
};

typedef struct Pagina Pagina;
Pagina* insereaza(Pagina* pag, TipCheie x, Nod* nod);
void afisare(Pagina* arbore, int nivel);
Pagina* suprima(Pagina* pag, TipCheie x, Nod* nod);

extern int nr_scindari;
extern int nr_pasi_cautare;
extern int inaltime;

#endif __ARBOREB__
