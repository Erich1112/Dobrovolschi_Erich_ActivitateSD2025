
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

struct Nod
{
	Masina info;
	struct Nod* dr;
	struct Nod* st;
};
typedef struct Nod Nod;

Masina citireMasinaDinFisier(FILE* file) {
	char buffer[100];
	char sep[3] = ",\n";
	fgets(buffer, 100, file);
	char* aux;
	Masina m1;
	aux = strtok(buffer, sep);
	m1.id = atoi(aux);
	m1.nrUsi = atoi(strtok(NULL, sep));
	m1.pret= atof(strtok(NULL, sep));
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


void adaugaMasinaInArbore(Nod** nod, Masina masinaNoua) {
	if ((*nod) != NULL)
	{
		if (masinaNoua.id > (*nod)->info.id)
		{
			adaugaMasinaInArbore(&(*nod)->dr, masinaNoua);
		}
		else if (masinaNoua.id < (*nod)->info.id)
		{
			adaugaMasinaInArbore(&(*nod)->st, masinaNoua);
		}
	}
	else
	{
		(*nod) = (Nod*)malloc(sizeof(Nod));
		(*nod)->info = masinaNoua;
		(*nod)->st = NULL;
		(*nod)->dr = NULL;
	}
}

void* citireArboreDeMasiniDinFisier(const char* numeFisier) {
	FILE* f = fopen(numeFisier, "r");
	Nod* nod = NULL;
	while (!feof(f))
	{
		adaugaMasinaInArbore(&nod, citireMasinaDinFisier(f));
	}
	fclose(f);
	return nod;
}

void afisarePreOrdineRSD(Nod* radacina) {
	if (radacina != NULL)
	{
		afisareMasina(radacina->info);
		afisarePreOrdineRSD(radacina->st);
		afisarePreOrdineRSD(radacina->dr);
	}
}


void afisareInOrdineSRD(Nod* radacina) {
	if (radacina != NULL)
	{
		afisareInOrdineSRD(radacina->st);
		afisareMasina(radacina->info);
		afisareInOrdineSRD(radacina->dr);
	}
}

void afisarePostOrdineSDR(Nod* radacina) {
	if (radacina != NULL) {
		afisarePostOrdineSDR(radacina->st);
		afisarePostOrdineSDR(radacina->dr);
		afisareMasina(radacina->info);
	}
}
//void afisareInOrdineSRD(Nod* radacina) {
//	if (radacina != NULL)
//	{
//		afisareInOrdineSRD(radacina->st);
//		afisareMasina(radacina->info);
//		afisareInOrdineSRD(radacina->dr);
//	}
//}

void afisareMasiniDinArbore(Nod* radacina) {
	printf("=== Afisare PRE-ORDINE (RSD) ===\n");
	afisarePreOrdineRSD(radacina);
	printf("=== Afisare IN-ORDINE (SRD) ===\n");
	afisareInOrdineSRD(radacina);
	printf("=== Afisare POST-ORDINE (SDR) ===\n");
	afisarePostOrdineSDR(radacina);
}

void dezalocareArboreDeMasini(Nod* radacina) {
	if (radacina != NULL) {
		dezalocareArboreDeMasini(radacina->st);
		dezalocareArboreDeMasini(radacina->dr);
		free(radacina->info.model);
		free(radacina->info.numeSofer);
		free(radacina);
	}
}

Masina getMasinaByID(Nod *radacina, int id) {
	Masina m;
	m.id = -1;
	if (radacina != NULL) {
		if (id < radacina->info.id) {
			return getMasinaByID(radacina->st, id);
		}
		else {
			if (id > radacina->info.id) {
				return getMasinaByID(radacina->dr, id);
			}
			else {
				return radacina->info;
			}
		}

	}
	return m;
}

int determinaNumarNoduri(Nod * radacina) {
	int nrNoduri = 0;
	if (radacina != NULL) {
		nrNoduri += determinaNumarNoduri(radacina->st);
		nrNoduri += determinaNumarNoduri(radacina->dr);
		nrNoduri += 1;
	}
	return nrNoduri;
}

int calculeazaInaltimeArbore(Nod* radacina) {
	if (radacina == NULL) {
		return 0;
	}
	int hSt = calculeazaInaltimeArbore(radacina->st);
	int hDr = calculeazaInaltimeArbore(radacina->dr);
	return (hSt > hDr ? hSt : hDr) + 1;
}

float calculeazaPretTotal(Nod* radacina) {
	if (radacina == NULL) {
		return 0.0f;
	}
	float suma = radacina->info.pret;
	suma += calculeazaPretTotal(radacina->st);
	suma += calculeazaPretTotal(radacina->dr);
	return suma;
}

float calculeazaPretulMasinilorUnuiSofer(Nod* radacina, const char* numeSofer) {
	if (radacina == NULL) {
		return 0.0f;
	}
	float suma = 0.0f;
	if (strcmp(radacina->info.numeSofer, numeSofer) == 0) {
		suma += radacina->info.pret;
	}
	suma += calculeazaPretulMasinilorUnuiSofer(radacina->st, numeSofer);
	suma += calculeazaPretulMasinilorUnuiSofer(radacina->dr, numeSofer);
	return suma;
}

int main() {

	Nod* rad = citireArboreDeMasiniDinFisier("masini_arbor.txt");
	afisarePreOrdineRSD(rad);
	printf("Masinile\n");
	afisareInOrdineSRD(rad);
	printf("\nMasina gasita: ");
	afisareMasina(getMasinaByID(rad, 6));
	printf("\nnrnoduri:%d\n", determinaNumarNoduri(rad));

	/*printf("Afisare arbore:\n");
	afisareMasiniDinArbore(rad);*/

	printf("Incaltimea Arbore:\n",calculeazaInaltimeArbore(rad));
	printf("Pret total:\n", calculeazaPretTotal(rad));
	printf("Pretul masinii soferului Gigel este:\n", calculeazaPretulMasinilorUnuiSofer(rad, "Gigel"));
	dezalocareArboreDeMasini(rad);
	return 0;
}
