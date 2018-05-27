# Zadání projektu č. 2

Mé rešení slouží pouze k inspiraci. Hodnoceno 30/30.

Vytvořte stránky pro imaginární zařízení v jedné z následujících oblastí podle vlastního uvážení:

Stavební firma
Realitní kancelář
Zvolte takový název zařízení, aby se neshodoval s názvem žádné existující organizace, kterou lze na WWW již nalézt (viz. Google).

Navrhněte vhodnou informační architekturu webu. Web musí mít nejméně tři hlavní informační sekce (např. základní informace, služby, kontakt), každá bude tvořena nejméně dvěma dokumenty (např. rodinné domy, byty). Web bude disponovat:

Úvodní stránkou obsahující obecné informace a sekci s aktualitami (upoutávky, akce apod.)
Vhodně navrženou globální navigací, která je dostupná ze všech dokumentů
Lokální (kontextovou) navigací v každé sekci, která obsahuje více dokumentů. Jedná se o odkazy na jiné dokumenty ve stejné sekci, nikoliv o odkazy v rámci jednoho dokumentu.
Stálým záhlavím a zápatím
Mapou webu přístupnou z každé stránky (např. odkaz v zápatí)

## Grafické zpracování

Navrhněte grafický styl stránek, který odpovídá zaměření daného subjektu. Návrh zahrnuje minimálně odpovídající barevné schéma (lze použít i různé pomocné nástroje) a obrázkové logo použitelné v záhlaví stránky (může se jednat i o stylizovaný text). Kvalita grafického návrhu není rozhodujícím kritériem hodnocení projektů, v extrémních případech ale může hodnocení ovlivnit (oběma směry).
Obě varianty zadání skýtají rozsáhlé využití fotografií pro dosažení atraktivního vzhledu. Pokuste se to využít. Současně mějte ale na paměti praktické aspekty:
Načítání obrázků může trvat déle, případně se nemusí podařit. Dbejte na to, aby text webu byl čitelný, i když se obrázky nezobrazí (např. je-li obrázek použitý jako pozadí pod textem, použijte současně i odpovídající barevné pozadí).
Obrázek na pozadí obecně snižuje čitelnost textu. Věnujte pozornost tomu, aby byl text dostatečně kontrastní.
Připravte odpovídající CSS styl stránek.
Navíc připravte tiskovou verzi stylu, která se použije při tisku stránky z prohlížeče (lze zkontrolovat v náhledu pro tisk). Tisková verze nebude obsahovat navigaci, pouze obsah.

## Zvláštní požadavky

Na úvodní stránce implementujte pomocí JavaScriptu jednu oblast (box), která bude při prvním zobrazení skrytá a místo ní bude zobrazen pouze její titulek, ikona apod. Po kliknutí na ikonu se zobrazí obsah oblasti a dalším kliknutím se opět skryje. Pomocí cookies implementujte ukládání stavu těchto boxů tak, aby uživatel viděl při návratu na stránku boxy v takovém stavu (zobrazené nebo nezobrazené), v jakém je zanechal. Typické použití: část nabídky nebo doplňkové informace ve sloupci s nabídkou, podrobnější kontaktní údaje, podrobnější informace k údajům v textu.

## Pokyny ke zpracování

Výchozí dokument se bude jmenovat index.html
Texty použité na stránkách si vymyslete. V místech, kde lze předpokládat delší souvislý text (profil apod.) je možno použít generátor výplňkového textu (Lorem Ipsum, Pseudo-čeština). Použijte dostatečné množství textu formátovaného v souladu s předpokládaným obsahem tak, aby bylo patrné, jak bude stránka ve výsledku vypadat. Nepoužívejte souvislý text ze stránek třetích stran.
Požadavky na technické zpracování
Stránky realizujte pomocí HTML 5 a CSS. Použití experimentálních rozšíření CSS lze použít za podmínky, že se stránka korektně zobrazí v prohlížečích zmíněných níže.
Kód stránek musí být validní při kontrole validátorem W3C.
Respektujte obecná pravidla přístupnosti webových stránek.
Veškeré navigační prvky musí být použitelné i bez zapnuté podpory JavaScriptu v prohlížeči. Skrývací box požadovaný výše musí být v takovém případě trvale zobrazen.
Používejte sémantické značkování (záhlaví, sekce, nadpisy, seznamy apod.) tak, aby byl web bez problémů srozumitelný i bez podpory CSS.
Nepoužívejte značky a atributy označené ve specifikaci jako zavržené nebo zastaralé.
Tabulky používejte pouze k prezentaci tabulárních dat, nikoliv k realizaci vizuálního rozložení stránky.
Umístění stránek
Vytvořené stránky umístěte na libovolný server přístupný z Internetu (např. fakultní, některý freehosting, ...).
Titulek webu (záhlaví) bude fungovat jako odkaz na zvolené umístění webu (t.j. na hlavní stránku absolutním URL).
Veškerý kód musí být vlastním dílem studenta. Při návrhu vzhledu je možno se inspirovat existujícími stránkami, nelze však převzít žádný kód z cizích webů ani z volně dostupných knihoven. Totéž platí i pro JavaScriptový kód. Využití obecných JS knihoven (např. JQuery) je povoleno. CSS frameworky (např. Bootstrap) nejsou povoleny. Přestože se frameworky v praxi často využívají, cílem projektu je ukázat vlastní zvládnutí technologií probíraných v rámci ITW.

## Odevzdání

Projekt se odevzdává jako jeden archiv ZIP, který obsahuje všechny soubory webu. Jméno odevzdávaného souboru musí být vaslogin.zip. Tento soubor se odevzdá přes IS FIT. Pokud narazíte na limit velikosti odevzdávaného souboru (cca. 2 MB), odevzdejte v archivu pouze kód bez obrázků a dalších dat. Verzi dostupnou na WWW však samozřejmě nechte kompletní.

Projekt musí být funkční minimálně v prohlížečích Internet Explorer nebo Edge, Firefox a Chrome v aktuálních verzích (běžně dostupných přes automatické aktualizace).

Datum odevzdání: 2.5.2018
Hodnocení: max. 30 bodů
Hodnotit se bude splnění všech výše uvedených kritérií, kvalita zpracování a použitelnost pro daný účel. Samotné splnění všech kritérií neznamená automaticky plný počet bodů. Plného počtu bodů mohou dosáhnout pouze stránky evidentně propracované a použitelné pro daný účel.
