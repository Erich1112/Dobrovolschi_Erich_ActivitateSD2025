
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//trebuie sa folositi fisierul masini.txt
//sau va creati un alt fisier cu alte date

struct StructuraMasina {
	int id;
	int nrUsi;
	float pret;
	char* model;
	char* numeSofer;
	unsigned char serie;
};
typedef struct StructuraMasina Masina;

typedef struct Nod nod;
struct Nod {
	Masina info;
	nod* next;
};

struct HashTable {
	int dim;
	nod** vector;
};
typedef struct HashTable HashTable;

Masina citireMasinaDinFisier(FILE* file) {
	char buffer[100];
	char sep[3] = ",\n";
	fgets(buffer, 100, file);
	char* aux;
	Masina m1;
	aux = strtok(buffer, sep);
	m1.id = atoi(aux);
	m1.nrUsi = atoi(strtok(NULL, sep));
	m1.pret = atof(strtok(NULL, sep));
	aux = strtok(NULL, sep);
	m1.model = malloc(strlen(aux) + 1);
	strcpy_s(m1.model, strlen(aux) + 1, aux);

	aux = strtok(NULL, sep);
	m1.numeSofer = malloc(strlen(aux) + 1);
	strcpy_s(m1.numeSofer, strlen(aux) + 1, aux);

	m1.serie = *strtok(NULL, sep);
	return m1;
}

void afisareMasina(Masina masina) {
	printf("Id: %d\n", masina.id);
	printf("Nr. usi : %d\n", masina.nrUsi);
	printf("Pret: %.2f\n", masina.pret);
	printf("Model: %s\n", masina.model);
	printf("Nume sofer: %s\n", masina.numeSofer);
	printf("Serie: %c\n\n", masina.serie);
}

void afisareListaMasini(nod *cap) {
	while (cap != NULL)
	{
		afisareMasina(cap->info);
		cap = cap->next;
	}
}

void adaugaMasinaInLista(nod** cap, Masina masinaNoua) {
	nod* temp = (nod*)malloc(sizeof(nod));
	temp->info = masinaNoua;
	temp->next = NULL;
	if ((*cap) == NULL)
	{
		(*cap) = temp;
	}
	else
	{
		nod* aux = *cap;
		while (aux->next != NULL)
		{
			aux = aux->next;
		}
		aux->next = temp;
	}
}


HashTable initializareHashTable(int dimensiune) {
	HashTable ht;
	ht.dim = dimensiune;
	ht.vector = (nod**)malloc(sizeof(nod*) * dimensiune);
	for (int i = 0; i < dimensiune; i++)
	{
		ht.vector[i] = NULL;
	}
	return ht;
}

int calculeazaHash(const char* cheieNumeSofer, int dimensiune) {
	if (!dimensiune && dimensiune < 0)
	{
		return -1;
	}
	unsigned int suma = 0;
	for (int i = 0; i < strlen(cheieNumeSofer); i++)
	{
		suma += (int)cheieNumeSofer[i];
	}
	suma %= dimensiune;
	return suma;
}

void inserareMasinaInTabela(HashTable hash, Masina masina) {
	int pozitie = calculeazaHash(masina.numeSofer, hash.dim);
	if (hash.vector[pozitie] == NULL)
	{
		//nu avem coliziune
		adaugaMasinaInLista(&(hash.vector[pozitie]), masina);
	}
	else
	{
		//avem coliziune
		adaugaMasinaInLista(&(hash.vector[pozitie]), masina);
	}
}

HashTable citireMasiniDinFisier(const char* numeFisier, int dimensiune) {
	FILE* f = fopen(numeFisier,"r");
	HashTable hash = initializareHashTable(dimensiune);
	while (!feof(f));
	{
		Masina masina = citireMasinaDinFisier(f);
		inserareMasinaInTabela(hash, masina);
	}
	fclose(f);
	return hash;
}

void afisareTabelaDeMasini(HashTable ht) {
	for (int i = 0; i < ht.dim; i++)
	{
		printf("suntem la lantul %d\n", i + 1);
		afisareListaMasini(&ht.vector[i]);
	}
}

void dezalocareTabelaDeMasini(HashTable *ht) {
	//sunt dezalocate toate masinile din tabela de dispersie
	if (ht == NULL || ht->vector == NULL)
	{
		return;
	}
	for (int i = 0; i < ht->dim; i++) {
		nod* current = ht->vector[i];
		while (current != NULL) {
			nod* temp = current;
			current = current->next;
			if (temp->info.model != NULL)
			{
				free(temp->info.model);
			}
			if (temp->info.numeSofer != NULL)
			{
				free(temp->info.numeSofer);
			}
			free(temp);
		}
	}
	free(ht->vector);
	ht->vector = NULL;
	ht->dim = 0;

}

float* calculeazaPreturiMediiPerClustere(HashTable ht, int* nrClustere) {
	if (ht.vector == NULL || ht.dim <= 0) {
		*nrClustere = 0;
		return NULL;
	}

	float* medii = (float*)malloc(sizeof(float) * ht.dim);
	*nrClustere = 0;

	for (int i = 0; i < ht.dim; i++) {
		float suma = 0.0f;
		int count = 0;
		nod* current = ht.vector[i];

		while (current != NULL) {
			suma += current->info.pret;
			count++;
			current = current->next;
		}

		if (count > 0) {
			medii[*nrClustere] = suma / count;
			(*nrClustere)++;
		}
	}

	if (*nrClustere == 0) {
		free(medii);
		return NULL;
	}

	return medii;
}

Masina getMasinaDupaCheie(HashTable ht, const char * numeSofer) {
	Masina m;
	m.id = -1;
	int pozitie = calculeazaHash(numeSofer, ht.dim);
	if (pozitie < 0 || pozitie >= ht.dim || ht.vector[pozitie] == NULL)
	{
		return m;
	}
	nod* aux = ht.vector[pozitie];
	while (aux != NULL && strcmp(aux->info.numeSofer, numeSofer) != 0)
	{
		aux = aux->next;
	}
	if (aux != NULL)
	{
		m.numeSofer = (char*)malloc(strlen(numeSofer) + 1);
		strcpy(m.numeSofer, numeSofer);
		m.model = (char*)malloc(strlen(aux->info.model) + 1);
		strcpy(m.model, aux->info.numeSofer);
		m.id = aux->info.id;
		m.nrUsi = aux->info.nrUsi;
		m.pret = aux->info.pret;
		m.serie = aux->info.serie;
	}
	return m;
}

int main() {

	HashTable hash;
	hash = citireMasiniDinFisier("masina.txt", 5);
	afisareTabelaDeMasini(hash);
	int nrClustere = 0;
	float* medii = calculeazaPreturiMediiPerClustere(hash, &nrClustere);
	if (medii != NULL) {
		printf("\nPreturi medii pe clustere:\n");
		for (int i = 0; i < nrClustere; i++) {
			printf("Cluster %d: %.2f\n", i + 1, medii[i]);
		}
		free(medii);
	}

	dezalocareTabelaDeMasini(&hash);
	return 0;
}
