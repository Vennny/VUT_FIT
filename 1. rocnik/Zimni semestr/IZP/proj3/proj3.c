	/**
 * 3. projekt IZP 2017/18
 * Vaclav Trampeska - xtramp00
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

int premium_case = 0;																						//inicializace globalni promenne pro premiove reseni

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

	c->size = 0;
	if (cap > 0)
	{		
		c->obj = malloc(cap * sizeof(struct obj_t)); 														// alokovani pameti pro pole objektu
		c->capacity = cap;																					// ulozeni kapacity
		return;
	}
																											// kdyz se alokace nepodari, nebo kdyz je kapacita 0
	c->capacity = 0;
	c->obj = NULL;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
	free(c->obj);
	init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
	if((c->size) >= (c->capacity))																				//kdyz se objekt do skhluku nevejde, tak rozsirujeme
		if(resize_cluster(c, c->capacity + CLUSTER_CHUNK) == NULL)												//zvetseni kapacity shluku
		{
			fprintf(stderr,"Alokace byla neuspesna\n");															//chybove hlaseni
		}

	c->obj[c->size] = obj;																						//vlozeni objektu
	c->size++;																									//zvetseni velikosti
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

	for (int i = 0; i < c2->size; i++)																			//pridavani objektu z druheho shluku do prvniho
		append_cluster(c1, c2->obj[i]);

	sort_cluster(c1);																							//serazeni shluku
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

	clear_cluster(&carr[idx]);																					//odstraneni objektu shluku

	for (int i = idx; i < narr - 1; i++)																		//posúnuti vsech shluku
		carr[i] = carr[i + 1];

	return narr - 1;																							//vraceni nove velikosti
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

	return sqrtf((o1->x - o2->x) * (o1->x - o2->x) + (o1->y - o2->y) * (o1->y - o2->y));						//matematicky vypocet vzdalenosti vychazejici z pythagorovy vety
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float result;																								//inicializace promenne, do ktere dosadime vysledek

    if (premium_case == 0)																						//metoda Unweighted pair-group average
    {
		float sum = 0;																							//inicializace promenne pro soucet vzdalenosti

		for (int i = 0; i < c1->size; i++)																		//dva for cykly prochazi vsechny objekty
		{
			for(int j = 0; j < c2->size; j++)
			{
				sum += obj_distance(&c1->obj[i], &c2->obj[j]);													//pricteni vzdalenosti
			}
		}

		int count = c1->size * c2->size;																		//pocet pruchodu
		result = sum/count;																						//nacteni vysledku
	}

	else if (premium_case == 1)																					//metoda nejblizsiho souseda
	{
		float minDistance = INT_MAX;																			//inicializace minimalni velikosti
		float distance;																							

		for (int i = 0; i < c1->size; i++)																		//dva for cykly prochazi vsechny objekty
		{
			for(int j = 0; j < c2->size; j++)
			{
				distance = obj_distance(&c1->obj[i], &c2->obj[j]);												//nacteni vzdalenosti do promenne

				if (distance < minDistance)																		//kdyz je vzdalenost mensi nez dosavadni minimum
					minDistance = distance;																		//vzdalenost bude novym minimem
			}
		}

		result = minDistance;																					//nacteni minimalni vzdalenosti do vysledku
	}

	else if (premium_case == 2)																					//metoda nejvzdalenejsiho souseda
	{
		float maxDistance = INT_MIN;																			//inicializace maximalni vzdalenosti
		float distance;

		for (int i = 0; i < c1->size; i++)																		//dva for cykly prochazi vsechny objekty
		{
			for(int j = 0; j < c2->size; j++)
			{
				distance = obj_distance(&c1->obj[i], &c2->obj[j]);												//nacteni vzdalenosti do promenne

				if (distance > maxDistance)																		//kdyz je vzdalenost vetsi nez dosavadni maximum
					maxDistance = distance;																		//vzdalenost bude novym maximem
			}
		}

		result = maxDistance;																					//nacteni maximalni vzdalenosti do promenne
	}

	return result;																								//funkce vraci vysledek zvolene metody
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float minDistance = INT_MAX;																				//inicializace minimalni vzdalenosti na maximum
    float distance;

	for (int i = 0; i < narr; i++)																				//dva for cykly prochazi vsechny kombinace shluku
	{
		for (int j = i + 1; j < narr; j++)
		{
			distance = cluster_distance(&carr[i], &carr[j]);													//nacteni vzdalenosti shluku

			if (distance < minDistance)																			//pokud je vzdalenost mensi nez dosavadni minum
			{
				minDistance = distance;																			//nacteni noveho minima
				*c1 = i;																						//ulozeni indexu obou shluku
				*c2 = j;
			}
		}
	}
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    
	FILE *file;
	int clusterQuantity;
	
	if ((file = fopen(filename, "r")) == NULL)																	//otevreni souboru a nacteni do promenne
	{
		fprintf(stderr, "Nepovedene otevreni souboru \"%s\"\n", filename);										//nepovede otevreni vyvola chybove hlaseni
		return -1;																								//vraceni zaporne hodnoty
	}
 
	if (fscanf(file, "count=%d", &clusterQuantity) != 1)														//nacteni prvniho radku
	{
		fprintf(stderr, "Prvni radek souboru musi byt formatu \"count=\"pocetShluku\" \" \n");					//nepovedene nacteni vyvola chybove hlaseni
		fclose(file);																							//zavreni souboru
		return -1;																								//vraceni zaporne hodnoty
	}

	if (clusterQuantity < 1)																					//pokud je nactena hodnota mensi nez 1
	{
		fprintf(stderr, "Pocet shluku musi byt vetsi nez 0\n");													//chybove hlaseni
		fclose(file);																							//uzavreni souboru
		return -1;																								//vraceni zaporne hodnoty
	}

	*arr = malloc(sizeof(struct cluster_t) * clusterQuantity);													//alokovani pameti pro clustery

	for (int i = 0; i < clusterQuantity; i++)																	//prochazeni radku
	{
		int id, x, y;

		if (fscanf(file, "%d %d %d", &id, &x, &y) != 3)															//nacteni ID a souradnic
		{																										//nepovede nacteni vyvola chybove hlaseni
			fprintf(stderr, "Format na radku %d je chybny. Spravny format je \"id x y\". Vsechna cisla musi byt cela cisla\n", i+2);
			fclose(file);																						//uzavreni souboru

			for(int j = 0; j < i; j++)																			//uvolneni vsech shluku
			{
				clear_cluster(&(*arr)[j]);
			}

			free(*arr);
			return -1;																							//vraceni zaporne hodnoty
		}

		if (x < 0 || x > 1000 || y < 0 || y > 1000)																//pokud souradnice neodpovidaji intervalu
		{
			fprintf(stderr, "Souradnice na radku %d musi byt vetsi nez 0 a mensi rovno 1000\n", i+2);			//chybove hlaseni
			fclose(file);																						//uzavreni souboru

			for(int j = 0; j < i; j++)																			//uvolneni vsech shluku
			{
				clear_cluster(&(*arr)[j]);
			}
			
			free(*arr);
			return -1;																							//vraceni zaporne hodnoty
		}

		struct cluster_t cluster;																				//vytvoreni shluku
		init_cluster(&cluster, 1);																				//inicializace shluku

		struct obj_t object = {id, x, y};																		//nacteni id a souradnic do objektu
		append_cluster(&cluster, object);																		//nacteni objektu do shluku

		(*arr)[i] = cluster;																					//nacteni shluku do navratove hodnoty
	}

	fclose(file);																								//uzavreni souboru
	return clusterQuantity;																						//vraceni poctu shluku
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[]) {
    struct cluster_t *clusters;

	int clustersQuantity = 0;
	int clustersRequired;

	if (argc == 2)																								//pokud neni zadan volitelny argument konecneho poctu shluku
	{
		clustersRequired = 1;																					//konecny pocet shluku bude 1
	}
	else if (argc == 3 || argc == 4)																			//pokud jsou zadany volitelne argumenty pro konecny pocet a metodu
	{
		char *endptr;
		clustersRequired = strtol(argv[2], &endptr, 10);														//nacteni konecneho poctu shluku do promenne

		if (strlen(endptr) > 0)																					//kontrola jestli je pocet cislo
		{
			fprintf(stderr, "Zadany pocet neni platne cislo\n");
			return EXIT_FAILURE;																				//neuspesne ukonceni programu
		}

		if (clustersRequired < 1)																				//pokud je zadany pocet mensi nez 1
		{
			fprintf(stderr, "Pocet musi byt vetsi nez 0\n");													//chybove hlaseni
			return EXIT_FAILURE;																				//neuspesne ukonceni programu
		}

		if (argc == 4)																							//pokud je zadana i metoda
		{
			if (strcmp(argv[3], "--avg") == 0)																	//zjisteni vybrane metody
			{
				premium_case = 0;
			}
			else if (strcmp(argv[3], "--min") == 0)
			{
				premium_case = 1;
			}
			else if (strcmp(argv[3], "--max") == 0)
			{
				premium_case = 2;
			}
			else																								//pokud byla metoda vybrana spatne
			{
				fprintf(stderr, "Spatne zadana specifikace metody\n");											//chybove hlaseni
				return EXIT_FAILURE;																			//neuspesne ukonceni programu
			}
		}
	}
	else 																										//pokud jsou argumenty zadany spatne
	{
		fprintf(stderr,"Spatne zadane argumenty\n");															//chybove hlaseni
		return (EXIT_FAILURE);																					//neuspesne ukonceni programu
	}

	clustersQuantity = load_clusters(argv[1], &clusters);														//nacteni shluku a ulozeni jejich poct do promenne

	if (clustersQuantity == -1)																					//hodnota -1 znaci chybu ve funkci load_clusters
		return EXIT_FAILURE;																					//neuspesne ukonceni programu

	if (clustersRequired > clustersQuantity)																	//pokud je zadany pocet shluku vetsi nez jejich pocet v souboru
	{
		fprintf(stderr, "Zadany pocet musi byt mensi nez pocet shluku v souboru - %d\n", clustersQuantity);		//chybove hlaseni

		for(int i = 0; i < clustersQuantity; i++)																//uvolneni shluku
		clear_cluster(&clusters[i]);

		free(clusters);
		return EXIT_FAILURE;																					//neuspesne ukonceni programu
	}

	int i1 = 0;
	int i2 = 0;
	while(clustersRequired < clustersQuantity) 																	//cyklus dokud zadany pocet shluku a pocet shluku nebude stejny
	{
		find_neighbours(clusters, clustersQuantity, &i1, &i2);													//hledani nejblizsich shluku
		merge_clusters(&clusters[i1], &clusters[i2]);																//pridani druheho shluku do prvniho
		clustersQuantity = remove_cluster(clusters, clustersQuantity, i2);										//odstraneni druheho shluku a nacteni noveho poctu shluku
	}

	print_clusters(clusters, clustersQuantity);																	//vypis shluku

	for(int i = 0; i < clustersRequired; i++)																	//uvolneni vsech shluku
	{
		clear_cluster(&clusters[i]);
	}

	free(clusters);

	return (EXIT_SUCCESS);																						//uspesne ukonceni programu
}