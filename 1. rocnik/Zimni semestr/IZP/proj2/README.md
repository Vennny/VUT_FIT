# Projekt 2 - Iterační výpočty
Mé rešení slouží pouze k inspiraci. Hodnoceno 5,23/7.

## Motivační příklad
Mějme přístroj obsahující sensor náklonu a procesor umožňující jednoduché operace nad čísly s plovoucí řádovou čárkou. Přístroj chce uživatel používat pro měření vzdálenosti a výšky velkých objektů (budova, komín, strom).

![Diagram](img/Diagram.png?raw=true "Diagram")

## Zadání projektu
Implementujte funkce nutné pro výpočet vzdálenosti a výšky pomocí úhlu náklonu měřeného přístroje. Výpočet proveďte pouze pomocí matematických operací +,-,*,/. Implementujte výpočet vzdálenosti a výšky měřeného objektu.

### Překlad a odevzdání zdrojového souboru
Odevzdání: Program implementujte ve zdrojovém souboru `proj2.c`. Zdrojový soubor odevzdejte prostřednictvím informačního systému.

Překlad: Program překládejte s následujícími argumenty:
```
$ gcc -std=c99 -Wall -Wextra -Werror proj2.c -lm -o proj2
```

### Syntax spuštění
Program se spouští v následující podobě:

```
./proj2 --help
```
```
./proj2 --tan A N M
```
```
./proj2 [-c X] -m A [B]
```

Argumenty programu:

- --help způsobí, že program vytiskne nápovědu používání programu a skončí.

- --tan porovná přesnosti výpočtu tangens úhlu A (v radiánech) mezi voláním tan z matematické knihovny, a výpočtu tangens pomocí Taylorova polynomu a zřetězeného zlomku. Argumenty N a M udávají, ve kterých iteracích iteračního výpočtu má porovnání probíhat. 0 < N <= M < 14

- -m vypočítá a změří vzdálenosti.

  - Úhel α (viz obrázek) je dán argumentem A v radiánech. Program vypočítá a vypíše vzdálenost měřeného objektu. 0 < A <= 1.4 < π/2.
  - Pokud je zadán, úhel β udává argument B v radiánech. Program vypočítá a vypíše i výšku měřeného objektu. 0 < B <= 1.4 < π/2
  - Argument -c nastavuje výšku měřicího přístroje c pro výpočet. Výška c je dána argumentem X (0 < X <= 100). Argument je volitelný - implicitní výška je 1.5 metrů.
  
### Implementační detaily
Je zakázané použít funkce z matematické knihovny. Jedinou výjimkou jsou funkce:

```
#include <math.h>
double tan(double x); // pro účely porovnání výpočtů
int isinf(x);
int isnan(x);
double fabs(double x);
```
a konstanty NAN a INF. Ve všech výpočtech používejte typ `double`. Uvažujte přesnost na 10 míst. Počet iterací v iteračních výpočtech odvoďte.

#### Implementace tangens
Funkci tangens implementujte dvakrát a to pomocí Taylorova polynomu a zřetězených zlomků.

##### 1. podúkol
Tangens pomocí Taylorova polynomu implementujte ve funkci s prototypem:

```
double taylor_tan(double x, unsigned int n);
```
kde x udává úhel a n rozvoj polynomu (počet členů). Taylorův polynom pro funkci tangens vypadá následovně:

![Taylor_tan](img/Taylor_tan.png?raw=true "Taylor_tan")

přičemž řada čitatelů ve zlomcích koeficientů je zde a řada jmenovatelů ve zlomcích koeficientů je [zde](https://oeis.org/A002430). Ve svém programu použijte pouze 13 prvních koeficientů.


