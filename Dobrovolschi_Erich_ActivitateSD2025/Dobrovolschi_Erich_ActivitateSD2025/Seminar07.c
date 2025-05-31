#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Masina
{
	int id;
	int nrUsi;
	float pret;
	char* numeSofer;
	char* model;
	char serie;
};
typedef struct Masina Masina;

struct Heap
{
	int nrElemente;
	int lungime;
	Masina* masini;
};

typedef struct Heap Heap;

Masina citireMasinaDinFisier(FILE* numefisier)
{
	char buffer[100];
	char sep[3] = ",\n";
	fgets(buffer, 100, numefisier);
	char* aux;
	Masina m;
	aux = strtok(buffer, sep);
	m.id = atoi(aux);
	m.nrUsi = atoi(strtok(NULL, sep));
	m.pret = atof(strtok(NULL, sep));
	aux = strtok(NULL, sep);
	m.model = malloc(strlen(aux) + 1);
	strcpy_s(m.model, strlen(aux) + 1, aux);

	aux = strtok(NULL, sep);
	m.numeSofer = malloc(strlen(aux) + 1);
	strcpy_s(m.numeSofer, strlen(aux) + 1, aux);

	m.serie = *strtok(NULL, sep);
	return m;
}

void afisareMasina(Masina masina)
{
	printf("Id: %d\n", masina.id);
	printf("nrUsi: %d\n", masina.nrUsi);
	printf("Pret: %.2f\n", masina.pret);
	printf("Model: %s\n", masina.model);
	printf("Nume Sofer: %s\n", masina.numeSofer);
	printf("Serie %c\n", masina.serie);
}

Heap initializare(int lungime)
{
	Heap heap;
	heap.lungime = lungime;
	heap.nrElemente = 0;
	heap.masini = (Masina*)malloc(sizeof(Masina) * lungime);
	return heap;
}

void filtreazaHeap(Heap heap, int pozitieNod)
{
	int stanga = 2 * pozitieNod + 1;
	int dreapta = 2 * pozitieNod + 2;
	int maxim = pozitieNod;

	if (stanga < heap.nrElemente && heap.masini[stanga].id > heap.masini[maxim].id)
	{
		maxim = stanga;
	}
	if (dreapta < heap.nrElemente && heap.masini[dreapta].id > heap.masini[maxim].id)
	{
		maxim = dreapta;
	}
	if (maxim != pozitieNod)
	{
		Masina temp;
		temp = heap.masini[pozitieNod];
		heap.masini[pozitieNod] = heap.masini[maxim];
		heap.masini[maxim] = temp;
		if (maxim <= (heap.nrElemente - 2) / 2)
		{
			filtreazaHeap(heap, maxim);
		}
	}

}

Heap citireHeapDeMasiniDinFisier(const char* numeFisier)
{
	FILE* f = fopen(numeFisier, "r");
	Heap heap = initializare(10);
	while (!feof(f))
	{
		Masina m;
		m = citireMasinaDinFisier(f);
		heap.masini[heap.nrElemente] = m;
		heap.nrElemente++;
	}

	for (int i = (heap.nrElemente - 1) / 2; i >= 0; i--)
	{
		filtreazaHeap(heap, i);
	}
	fclose(f);
	return heap;
}

void afiseazaHeap(Heap heap)
{
	for (int i = 0; i < heap.nrElemente; i++)
	{
		afisareMasina(heap.masini[i]);
	}
}

void afiseazaHeapAscuns(Heap heap)
{
	for (int i = 0; i < heap.lungime; i++)
	{
		afisareMasina(heap.masini[i]);
	}
}

Masina extrageMasina(Heap* heap)
{
	Masina m;
	m = heap->masini[0];
	heap->masini[0] = heap->masini[heap->nrElemente - 1];
	heap->masini[heap->nrElemente - 1] = m;
	heap->nrElemente--;
	for (int i = (heap->nrElemente - 2) / 2; i >= 0; i--)
	{
		filtreazaHeap(*heap, i);
	}
	return m;
}

void dezalocareHeap(Heap* heap)
{
	for (int i = 0; i < heap->lungime; i++)
	{
		free(heap->masini[i].model);
		free(heap->masini[i].numeSofer);
	}
	free(heap->masini);
	heap->masini = NULL;
	heap->lungime = 0;
	heap->nrElemente = 0;
}

int main()
{
	Heap heap = citireHeapDeMasiniDinFisier("masini_arbor.txt");
	afiseazaHeap(heap);
	Masina m;
	m = extrageMasina(&heap);
	afisareMasina(m);
	return 0;
}