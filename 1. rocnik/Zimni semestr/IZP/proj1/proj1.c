/*
 * Soubor:  proj1.c
 * Datum:   2017/11/5
 * Autor:   Václav Trampeška - xtramp00, xtramp00@stud.fit.vutbr.cz
 * Projekt: Práce s textem, projekt č. 1 pro předmět IZP
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_MEST 43
#define MAX_DELKA 101

void Nacti_mesta(int max_delka, char mesto[][max_delka], int *pocet_mest)
{
	char znak;
	int delka_mesta = 0;
	while((znak = toupper(getchar()))!=EOF)	   //bere znaky dokud není EOF a prevede je na velka pismena
		{
			if (znak != '\n')     //dokud znak není konec radku
			{
				mesto[*pocet_mest][delka_mesta] = znak; //uklaáda znaky do města
				delka_mesta++;	
				if (delka_mesta > MAX_DELKA) // ošetření maximální délky města
				{
					fprintf(stderr, "Delka nektereho mesta presahla maximalni hodnotu %d\n", MAX_DELKA); //vypsání chyby na stderr
					exit(0);  // ukončení programu
				}
			}

			else	// když je znak konec řádku, ukončíme město a přesuneme se na další
			{
				mesto[*pocet_mest][delka_mesta]='\0';
				delka_mesta = 0;
				(*pocet_mest)++;
			}

			if (*pocet_mest > MAX_MEST) //ošetření maximalního počtu měst
			{
				fprintf(stderr, "Pocet mest presahl maximalni hodnotu %d\n" , MAX_MEST);  //vypsání chyby na stderr
				exit(0);    // ukončení programu
			} 
		}

		(*pocet_mest)++;
}

void Nacti_argument(char *argument, char *argv[], int delka_argumentu) // načítá argument do pole a mění na velká písmena
{
	int i=0;

	while(i<delka_argumentu) 
	{
		argument[i] = toupper(argv[1][i]);
		i++;
	}
	argument[delka_argumentu]='\0';
}

void Uloz_vysledek(int max_delka, char mesto[][max_delka], char *vysledek, int i, int delka_argumentu, int *pocet_vysledku) // ukládá nalezené shody do pole výsledků
{
	if (strchr(vysledek,mesto[i][delka_argumentu])==NULL)     //kontroluje, zda je v poli vysledků už zapsany stejný znak, nebo ne
	{
		vysledek[*pocet_vysledku]=mesto[i][delka_argumentu];   //pokud se v poli ještě znak nenachází, zapíše ho
		(*pocet_vysledku)++;    
		vysledek[*pocet_vysledku]='\0';	
	}

}

void Bubblesort_vysledku(char *vysledek, int pocet_vysledku)  // abecedně seřadíme pole výsledků
{
    for(int i = 0; i < pocet_vysledku - 1; i++)
    {
        for(int j = 0; j < pocet_vysledku - i-1; j++)
        {
            if(vysledek[j+1] < vysledek[j])
            {
                char pom = vysledek[j + 1];
                vysledek[j + 1] = vysledek[j];
                vysledek[j] = pom;
            }   
        }   
    }   
}    
           
void Tisk_vysledku(int max_delka, char mesto[][max_delka], char *vysledek, int nalezeno, int nalezeno_pozice, int argc)
{
	if (nalezeno==1)     //když nalezneme právě jednu adresu
		printf("Found: %s\n", mesto[nalezeno_pozice]); 

	else if(nalezeno==0 && argc!=1) //když nenalezneme žádnou adresu 
		printf("Not found\n");

	else
		printf("Enable: %s\n",vysledek); //když nalezneme více než jednu adresu
}

int main(int argc, char *argv[])
{ 
	char mesto[MAX_MEST][MAX_DELKA];		
	int pocet_mest = 0;
	int nalezeno = 0;
	int nalezeno_pozice= 0;
	
	Nacti_mesta(MAX_DELKA, mesto, &pocet_mest);  // volání funkce na načtení měst do pole
	
	char vysledek[pocet_mest];  //deklarace pole výsledků až po načtení měst, protože potřebujeme jejich počet
	int pocet_vysledku=0;

	if(argc==1)    // toto se vykoná, pokud nezadáme argument
	{  
		for (int i=0; i<pocet_mest; i++)
		{	
			int delka_argumentu = 0;
			Uloz_vysledek(MAX_DELKA, mesto, vysledek,i, delka_argumentu, &pocet_vysledku);
		}
	}

	else if(argc>2)    // toto se vykoná, pokud zadáme více než jeden argument
	{
		fprintf(stderr, "Muzete zadat maximalne jeden argument.\n");
		return EXIT_FAILURE;  //v tomto případě ukončíme program
	}

	else   //toto se vykoná, pokud je zadaný přesně jeden argument
	{   
		int delka_argumentu= strlen(argv[1]);   //  načtení délky argumentu
		char argument[delka_argumentu];   // deklarace pole pro argument 

		Nacti_argument(argument, argv, delka_argumentu);  // volání funkce pro načtění argumentu

		for (int i=0; i<pocet_mest;i++)    //cyklus tady prochází všechna města
		{
			if(strncmp(argument,mesto[i],delka_argumentu)==0)   //hledá shodu argumentu v poli mest
			{
				Uloz_vysledek(MAX_DELKA, mesto, vysledek,i, delka_argumentu, &pocet_vysledku); // volání funkce na uložení výsledků

				nalezeno_pozice=i;    //pozice nalezené adresy
				nalezeno++;     //počet nalezench
			}
		}	
	}

	Bubblesort_vysledku(vysledek,pocet_vysledku);  // volání funkce na abecední seřazení výsledků

	Tisk_vysledku(MAX_DELKA, mesto, vysledek, nalezeno, nalezeno_pozice, argc);  // volání funkce na výtisk výsledků

	return 0;
}