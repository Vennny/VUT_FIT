# 1. Úloha IOS (2018)

Mé rešení slouží pouze k inspiraci. Hodnoceno 15/15, ok testů 18/21.


## Popis úlohy

Cílem úlohy je vytvořit skript (tzv. wrapper), který bude spouštět textový editor. Skript si bude pamatovat, které soubory byly v jakém adresáři prostřednictvímskriptu wedi editovány. Pokud bude skript spuštěn bez parametrů, vybere skript soubor, který má být editován.

## Specifikace chování skriptu

### JMÉNO

wedi - wrapper textového editoru s možností automatického výběru souboru

### POUŽITÍ

```
wedi SOUBOR
wedi [ADRESÁŘ]
wedi -m [ADRESÁŘ]
wedi -l [ADRESÁŘ]
wedi -b|-a DATUM [ADRESÁŘ]
```

### POPIS

- Pokud byl skriptu zadán soubor, bude editován.
- Pokud zadaný argument odpovídá existujícímu adresáři, skript z daného adresáře vybere soubor pro editaci. Výběr souboru je následující.
   - Pokud bylo v daném adresáři editováno skriptem více souborů, vybere se soubor, který byl pomocí skriptu editován jako poslední. Editací souboru se myslí třeba i prohlížení jeho obsahu pomocí skriptu (tj. není nutné, aby byl soubor změněn).   
   - Pokud byl zadán argument `-m`, vybere se soubor, který byl pomocí skriptu editován nejčastěji.  
- Pokud nebyl v daném adresáři editován ještě žádný soubor, jedná se o chybu.
- Pokud nebyl zadán adresář, předpokládá se aktuální adresář.
- Skript dokáže také zobrazit seznam všech souborů (argument `-l`), které byly v daném adresáři editovány.
- Pokud byl zadán argument `-b` resp. `-a` (before, after), skript zobrazí seznam souborů, které byly editovány před resp. po zadaném datu v čase 00:00:00.0 včetně. DATUM je formátu `YYYY-MM-DD`.

### NASTAVENÍ A KONFIGURACE

- Skript si pamatuje informace o svém spouštění v souboru, který je dán proměnnou `WEDI_RC`. Formát souboru není specifikován.
  - Pokud není proměnná nastavena, jedná se o chybu.
  - Pokud soubor na cestě dané proměnnou `WEDI_RC` neexistuje, soubor bude vytvořen včetně cesty k danému souboru (pokud i ta neexistuje).
- Skript spouští editor, který je nastaven v proměnné `EDITOR`. Pokud není proměnná `EDITOR` nastavená, respektuje proměnnou `VISUAL`. Pokud ani ta není nastavená, použije se příkaz `vi`.


### NÁVRATOVÁ HODNOTA

Skript vrací úspěch v případě úspěšné operace nebo v případě úspěšné editace. Pokud editor vrátí chybu, skript vrátí stejný chybový návratový kód. Interní chyba skriptu bude doprovázena chybovým hlášením.

### POZNÁMKY

- Skript nebere v potaz soubory, se kterými dříve počítal a které jsou nyní smazané.
- Při rozhodování relativní cesty adresáře je doporučené používat reálnou cestu (realpath). Důvod např.:

```
$ wedi .
$ wedi ‘pwd‘
```

## Implementační detaily

- Skript by měl mít v celém běhu nastaveno `POSIXLY_CORRECT=yes`.
- Skript by měl běžet na všech běžných shellech (dash, ksh, bash). Můžete použít GNU rozšíření pro `sed` či `awk`. Jazyk Perl nebo Python povolen není.
- Skript by měl ošetřit i chybový případ, že na daném stroji utilita `realpath` není dostupná.
Referenční stroj neexistuje. Skript musí běžet na běžně dostupných OS GNU/Linux a <pre>*BSD</pre>. Ve školním prostředí máte k dispozici počítače laboratořích (CentOS), stroj merlin (CentOS) a eva (FreeBSD). *Pozor, na stroji merlin je shell /bin/ksh symbolický odkaz na bash (tj. nechová se jako Korn shell jako na obvyklých strojích)*.
- Skript nesmí používat dočasné soubory. Povoleny jsou dočasné souborynepřímo tvořené příkazem `sed` (např. argument `sed -i`).

## Příklady použití

Následující příklady předpokládají, že skript `wedi` je dostupný v jedné z cest vproměnné `PATH`.

1. Editace různých souborů:

```
$ export WEDI_RC=$HOME/.config/wedirc
$ date
Mon 2 Mar 14:45:24 CET 2018
$ wedi ~/.bashrc
$ wedi ~/.ssh/config
$ wedi ~/.local/bin/wedi
$ wedi ~/.bashrc
$ wedi ~/.indent.pro
$ date
Wed 4 Mar 19:51:02 CET 2018
$ wedi ~/.bashrc
$ wedi ~/.vimrc
```

2. Opětovná editace:

```
$ cd ~/.ssh
$ wedi
... spustí se editace souboru ~/.ssh/config
$ wedi ~
... spustí se editace souboru ~/.vimrc
$ cd
$ wedi -m
... spustí se editace souboru ~/.bashrc
```

3. Zobrazení seznamu editovaných souborů:
```
$ wedi -l $HOME
.bashrc
.indent.pro
.vimrc
$ wedi -b 2018-03-04 $HOME
.bashrc
.indent.pro
$ wedi -a 2018-03-03 $HOME
.bashrc
.vimrc
```
