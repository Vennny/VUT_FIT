
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                              Radek Hranický, 2014-2018
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {
	if (ptrht)	//kdyz ukazatel existuje 
		for (int i = 0; i < HTSIZE; i++) 
			(*ptrht)[i] = NULL;
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {
	if (!ptrht || !key)
		return NULL;			//chyba

	tHTItem *tmp = (*ptrht)[hashCode(key)]; // prvni adresa

	while (tmp){	//dokud existuje prvek, hledame 
		if (!strcmp(tmp->key, key))	//pokud najdeme prvek s klicem, vracime ho
			return tmp;

		tmp = tmp->ptrnext;			//pokud prvek ma jiny klic, bereme dalsi
	}

	return NULL;
}

/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {

	tHTItem *new = htSearch(ptrht, key);

	if(new){			//prvek se stejnym klicem nalezen
		new->data = data;
	}
	else{			//vkladame novy prvek
		if ( !(new = (tHTItem*)malloc(sizeof(tHTItem)) ))	//alokace
			return;		//chyba pri alokaci

		new->key = key;
		new->data = data;

		int i = hashCode(key);	//najdeme index klice a zjistime jestli je volny

		if(!(*ptrht)[i])		//seznam na indexu prazdny
			new->ptrnext = NULL;

		else
			new->ptrnext = (*ptrht)[i];	//seznam neni prazdny, odkazujeme na prvek

		(*ptrht)[i] = new;		//prvek na zacatek
	}
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {

	tHTItem *tmp;
	
	if ( (tmp = htSearch(ptrht, key)))
		return &tmp->data;
	
	return NULL; 
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {

	if (!ptrht)
		return;

 	tHTItem *last = NULL;	//posledni prvek

 	int i = hashCode(key);	//index klice
 	tHTItem *tmp = (*ptrht)[i];

 	while(tmp){

 		if(!(strcmp(key, tmp->key))){		//splnena podminka = nalezen prvek

 			if (!last)						//neni zadny posledni prvek pred nalezenym
 				(*ptrht)[i] = tmp->ptrnext;

 			else
 				last->ptrnext = tmp->ptrnext;	//posledni polozku propojime s nadchazejici

 			free(tmp);
 			return;
 		}

 		last = tmp; 		//stavajici polozka se stava posledni polozkou
 		tmp = tmp->ptrnext; //do stavajici ulozime dalsi 
 	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {

	if (!ptrht)
		return;

	for (int i = 0; i < HTSIZE; i++){	//prochazime tabulku

		while ((*ptrht)[i]){		//prochazime prvky na indexu
			tHTItem *tmp = (*ptrht)[i];
			(*ptrht)[i] = tmp->ptrnext;
			free(tmp);
		}
	}
}
