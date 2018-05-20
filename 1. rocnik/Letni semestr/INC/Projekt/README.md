# Přístupový terminál

Moje řešení slouží pouze pro inspiraci. Hodnoceno 18/20. (-2b zpráva)

## Zadání

1. Seznamte se s přípravkem FITKit a způsobem připojení jeho periférií, zejména
klávesnice a LCD displeje.
2. Prostudujte si zdrojové kódy projektu v jazyce VHDL.
3. Navrhněte řídicí jednotku (konečný automat) jednoduchého přístupového terminálu.
4. Navrženou řídicí jednotku implementujte v jazyce VHDL a ověřte její funkčnost
přímo na přípravku FITKit.

## Implementace
Funkce obvodu je následující:
- Vstupy z klávesnice jsou pravidelně testovány pomocí řadiče klávesnice (Keyboard
Controller). Jakmile je detekován stisk některé klávesy, nastaví řadič na svém výstupu
KEY jeden z 16-ti signálů odpovídající číslu stisknuté klávesy (signály KEY(0..9)
odpovídají klávesám 0..9, signály KEY(10..13) odpovídají klávesám A..D, signál
KEY(14) odpovídá klávese "*", signál KEY(15) odpovídá klávese "#").
- Výstupní signály z klávesnice jsou dále připojeny ke konečnému automatu (FSM),
který ovládá zbývající části obvodu. Automat sleduje posloupnost stisknutých kláves,
v průběhu zadávání kódu vypisuje na LCD displeji znak "*" a po potvrzení kódu
klávesou "#" vypíše na displej zprávu "Pristup povolen" nebo "Pristup odepren".
Opětovným stisknutím klávesy "#" přechází obvod opět do stavu čekajícího na vstupní
kód.
- Textové zprávy "Pristup povolen" resp. "Pristup odepren" jsou uloženy ve dvou
paměťových modulech typu ROM. Každý z modulů obsahuje 16 osmi-bitových
položek obsahujících jednu ze zpráv.
- V okamžiku potvrzení vstupního kódu klávesou "#", aktivuje automat Clock Enable
signál 4-bitového čítače (COUNTER), který je připojen na adresové vstupy
paměťových modulů a způsobí tak vyčtení jejich obsahu na výstup.
- Výstupy paměťových modulů jsou dále připojeny na dvou-vstupý multiplexor
(MX_MEM). V případě, že byl kód správně zadán, potom automat vybere skrze tento
multiplexor zprávu "Přístup povolen", v opačném případě vybere zprávu "Pristup
odepren".
- Výstup multiplexoru (MX_MEM) je připojen na další dvou-vstupý multiplexor
(MX_LCD). Tento multiplexor přepíná na svůj výstup buď zprávu uloženou v
paměťových modulech nebo znak "*" podle toho, zda se přístupový terminál nachází
ve stavu zadávání kódu nebo ve stavu vypisujícím výstupní zprávu.
- Výstup multiplexoru (MX_LCD) je připojen na datový vstup řadiče LCD displeje
(LCD DISPLAY CONTROLLER) a reprezentuje znaky, které se budou zobrazovat na
displeji. Řadič je ovládán pomocí dvou signálů WRITE a CLEAR. Aktivace signálu
WRITE způsobí zápis znaku na displeji, zatímco aktivace signálu CLEAR displej
vymaže a připraví na zápis nové sekvence znaků

## Odevzdání

- Soubor fsm.vhd se zdrojovým kódem konečného automatu.
- Soubor accterm.bin, obsahující konfiguraci pro FPGA čip. Tento soubor naleznete
v podadresáři build.
- Soubor zprava.pdf s výstupní zprávou (ve formátu PDF) obsahující následující
informace:
  - Jméno a příjmení, login, přístupové kódy
  - Graf přechodu konečného automatu
  - Seznam výstupů s identifikací, zda se jedná o Mealyho nebo Moorovy výstupy.
  - Ukázku výstupní zprávy naleznete v příloze č. 1.
  
Rozsah zprávy nesmí překročit jednu stranu formátu A4
