/*
 * Soubor:  proj2.c
 * Datum:   2017/11/9
 * Autor:   Vennny
 * Projekt: Iteracni vypocty, projekt č. 2 pro předmět IZP
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define IMPLICITNI_VYSKA 1.5 //ze zadani
#define POCET_ITERACI 8 //odchylka se dostane na presnost na 10 desetinnych mist maximalne na 8. iteraci

void vypis_help()  
{//Vypise napovedu
	printf("\nProgram se spousti v nasledujici podobe:\n./proj --help\n./proj2 --tan A N M\n./proj2 [-c X] -m A [B]\n\n"
		"'--help' zpusobi, ze program vytiskne napovedu pouzivani programu a skonci.\n\n"
		"'--tan' porovna presnosti vypoctu tangens uhlu A (v radianech) mezi volanim tan z matematicke knihovny, a vypoctu tangens pomoci \nTaylorova polynomu a zretezeneho zlomku."
		"Argumenty N a M udavaji, ve kterych iteracich iteracniho vypoctu ma porovnani probihat.\n0 < N <= M < 14n\n\n"
		"'-m' vypocita a zmeri vzdalenosti.\nUhel alfa je dan argumentem A v radianech. Program vypocita a vypise vzdalenost mereneho objektu. 0 < A <= 1.4 < pi/2."
		"\nPokud je zadan, uhel beta; udava argument B v radianech. Program vypocita a vypise i vysku mereneho objektu. 0 < B <= 1.4 < pi/2"
		"\nArgument -c nastavuje vysku mericiho pristroje c pro vypocet. Vyska c je dana argumentem X (0 < X <= 100). Argument je volitelny\n- implicitni vyska je 1.5 metru."
		"\n\n");
}

void je_cislo(char *endptr, double cislo) 
{//kontrola, jestli je argument cislo, funkce strtod a strtol vraci ukazatel na prvni znak za posledni nalezenou cislici v argumentu
	if (strlen(endptr) > 0 || fabs(isinf(cislo)) || isnan(cislo)) //pokud je tedy delka vetsi nez nula, cislo to neni
	{
		fprintf(stderr, "Zadany argument neni cislo\n"); //vypiseme chybu na stderr
		exit(0); //konec programu
	}
}

void kontrola_uhlu(double uhel)
{//kontrola, jestli uhel odpovida zadani
	if(uhel <= 0 || uhel > 1.4)
	{ //kdyz ne, vypiseme chybu na stderr a ukoncime program
 		fprintf(stderr,"Uhel musi byt vetsi nez 0 a maximalne 1.4\n");
 		exit(0);
 	}
}
double taylor_tan(double x,unsigned int n)
{// vypocet tan pomoci Taylorova polynomu
    double citatel[] = {1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604}; // zadane hodnoty citatele
    double jmenovatel[] = {1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375}; // zadane hodnoty jmenovatele
   
    double vysledek = 0;   // inicializace promenne pro vysledek
    double x_squared=x*x;	// promenna pro x^2 

    for(unsigned i=0; i<n; i++)  // cyklus pro vypocet Taylorova polynomu
    {
        vysledek += x * citatel[i] / jmenovatel[i]; 
        x *= x_squared;
    }
    return vysledek;  // vracime hodnotu funkce
}

double cfrac_tan(double x, unsigned int n) 
{// vypocet tan pomoci zretelnych zlomku
	double zlomek = 0.0; // inicializace zlomku
	 	
	for(unsigned int i = n-1; i >= 1; i--) // cyklus pocita od posledniho zlomku a zastavi pred posledni iteraci
	{
		zlomek = x*x / (2*i+1 - zlomek); 
	}

	double vysledek = x/(1.0 - zlomek); //posledni iterace

	return vysledek; // vracime hodnotu funkce
}

void vypis_tan(char *argv[])
{
	char *endptr;
	double uhel = strtod(argv[2], &endptr); // argument dosazen do double promenne
	je_cislo(endptr,uhel); //kontrola, jestli je cislo
	//kontrola_uhlu(uhel);// kontrola, jestli velikost uhlu odpovida zadani

	int interval_1 = strtol(argv[3], &endptr,10);
	je_cislo(endptr,interval_1);
 	int interval_2 = strtol(argv[4], &endptr,10);// argument dosazen do integer promenne
	je_cislo(endptr,interval_2);

	if(interval_1 <= 0 || interval_1 > 13 || interval_2 <= 0 || interval_2 > 13) //kontrola intevalu, cisla nesmi byt mensi rovno 0 a vetsi nez 13
	{ 
 		fprintf(stderr,"Zadany interval musi byt z intervalu (0;13>\n"); // pokud cisla nevyhovuji, vypise se chyba na stderr
 		exit(0); // konec programu
 	}	

 	if(interval_1>interval_2) //osetreni aby prvni cislo intervalu nebylo vetsi nez druhe
 	{
 		fprintf(stderr,"Prvni cislo z intervalu nemuze byt vetsi nez to druhe\n"); //vypis chyby na stderr
 		exit(0); //konec programu
 	}	

	while (interval_1<=interval_2) // cyklus projede iterace z uzivatelem zadaneho intervalu
	{
		double vysledek_taylor = taylor_tan(uhel, interval_1); // nacteni vysledku Taylorova polynomu
		double vysledek_tan = tan(uhel);	// nacteni vysledku matematicke funkce
		double vysledek_cfrac = cfrac_tan(uhel, interval_1); // nacteni vysledku zretelnych zlomku
		// vypis vysledku a jejich odchylek
		printf("%d %e %e %e %e %e %e\n", interval_1, vysledek_tan, vysledek_taylor, fabs(vysledek_tan - vysledek_taylor), vysledek_tan, vysledek_cfrac, fabs(vysledek_tan - vysledek_cfrac) );
		interval_1++;
	}
}

double vypocet_delky(double alfa, double vyska_bodu) 
{// vypocet vzdalenosti pozorovatele od objektu, parametry: uhel alfa a vyska pozorovatele
	double delka = vyska_bodu / cfrac_tan(alfa, POCET_ITERACI);	// vypocet pomoci metody zretelnych zlomku
	return delka; // vracime vzdalenost pozorovatele od objektu
}

double vypocet_vysky(double beta, double vyska_bodu, double delka)
{	 // vypocet vysky objektu, parametry: uhel beta, vyska pozorovatele a vypocitana vzdalenost pozorovatele od objektu
	double vyska = cfrac_tan(beta,POCET_ITERACI) * delka + vyska_bodu; // vypocet pomoci metody zretelnych zlomku, musime pricist vysku pozorovatele
	return vyska;	// vracime hodnotu vysky objektu
}

void vypocty_mereni(int argc, char *argv[], int dalsich_argumentu, double vyska_pozorovani) // funkce se spusti pokud je prvnim argumentem -m nebo -c
{
	if (argc == (3 + dalsich_argumentu)) // spusti se, pokud neni zadany uhel beta
	{
		char *endptr;
		double alfa = strtod(argv[2 + dalsich_argumentu], &endptr); //prirazeni uhlu alfa

		je_cislo(endptr,alfa); // kontrola, jestli je cislo
		kontrola_uhlu(alfa);// kontrola, jestli velikost uhlu odpovida zadani

		printf("%.10e\n", vypocet_delky(alfa, vyska_pozorovani)); // zavolani fuknce na vypocet vzdalenosti pozorovatele od objektu a jeji vypsani
	}

	else if(argc == (4 + dalsich_argumentu)) //spusti se, pokud je zadany i uhel beta
	{
		char *endptr;
		double alfa = strtod(argv[2 + dalsich_argumentu], &endptr);// prirazeni uhlu alfa

		je_cislo(endptr,alfa);// kontrola, jestli je cislo
		kontrola_uhlu(alfa);// kontrola, jestli velikost uhlu odpovida zadani

		double beta = strtod(argv[3 + dalsich_argumentu], &endptr); // prirazeni uhlu beta

		je_cislo(endptr,beta);// kontrola, jestli je cislo
		kontrola_uhlu(beta);// kontrola, jestli velikost uhlu odpovida zadani

		double delka = vypocet_delky(alfa, vyska_pozorovani); // zavolani fuknce na vypocet vzdalenosti pozorovatele od objektu
		double vyska_objektu = vypocet_vysky(beta, vyska_pozorovani, delka); // zavolani fuknce na vypocet vysky objektu

		printf("%.10e\n%.10e\n", delka, vyska_objektu); //  vypsani vysledku
	}

	else // pokud jsou argumenty nespravne zadany, vypisujeme chybu na stderr, doporucime uzivateli napovedu a ukoncime program
	{
		fprintf(stderr, "Nespravny pocet argumentu. Pro napovedu napiste ./proj2 --help\n");
		exit(0);
	}
}

int main(int argc, char *argv[])
{
	if (strcmp(argv[1],"--help") == 0) // kdyz je prvni argment --help
    {
    	vypis_help(); //zavolani funkce na vypis napovedy
    }
	
	else if (strcmp(argv[1],"--tan") == 0) //kdyz je prvni argument --tan
	{
		if (argc == 5) //kontrola spravneho poctu argumentu
			vypis_tan(argv); //zavolame funkci na vypocet a vypis tan

		else //pokud je nespravny pocet argumentu, vypiseme chybu na stderr, doporucime uzivateli napovedu a ukoncime program
		{
			fprintf(stderr, "Nespravny pocet argumentu. Pro napovedu napiste ./proj2 --help\n");
			return EXIT_FAILURE;
		}
	}

	else if(strcmp(argv[1],"-m") == 0) //pokud je prvni argument -m a tedy vyska neni uzivatelem zadana
	{
		int plus_argumentu = 0; //ve funkci nebudeme pricitavat zadne cislo pro spravne nalezeni uhlu alfa a beta
		vypocty_mereni(argc,argv, plus_argumentu, IMPLICITNI_VYSKA); // zavolani funkce pro vypocet a vypis vzdalenosti pozorovatele od objektu a vysky objektu, vyska je defaultni 1.5
	}

	else if(strcmp(argv[1],"-c") == 0) //pokud je prvni argument -c a tedy je zadana vyska pozorovani
    {	
    		int plus_argumentu = 2; // v tomto pripade mame o 2 vice argumentu nez bez zadani vysky

    		char *endptr;
    		double vyska_pozorovani = strtod(argv[2], &endptr); //prirazeni vysky
    		je_cislo(endptr,vyska_pozorovani); //kontrola, jestli je cislo

    		if(vyska_pozorovani <= 0 || vyska_pozorovani > 100) //ze zadani nesmi byt vyska mensi rovno 0 a vetsi nez 100, pokud je mimo tento interval, je podminka if splnena
    		{ 
    			fprintf(stderr, "Vyska musi byt z intervalu (0;100> \n");//vypiseme chybu na stderr
    			return EXIT_FAILURE; // konec programu
    		}
    		
   	    	vypocty_mereni(argc,argv, plus_argumentu, vyska_pozorovani); // zavolani funkce pro vypocet a vypis vzdalenosti pozorovatele od objektu a vysky objektu
    } 

	else  // pokud uzivatel zada uplne jine argumenty
	{
		fprintf(stderr, "Spatne zadane argumenty. Pro napovedu napiste ./proj2 --help"); //vypis chyby na stderr, doporucime uzivateli napovedu
		return EXIT_FAILURE; // konec programu vyhodnoceny jako neuspesny
	}

    return EXIT_SUCCESS;
}
