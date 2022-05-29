# Implementační dokumentace k 1. projektu IPK 2021/2022
**Jméno a příjmení: Marina Kravchuk**
**Login: xkravc02**
___
Implementace serveru v jazyce C.
Príkaz `./hinfosvc PORT` spustí server na zadaném portu.

Komunikace se serverem je možná pomocí webového prohlížeče, nástroji typu wget a curl, a taky příkazu GET

`GET http://servername:12345/hostname` - vrací síťové jméno počítače
`GET http://servername:12345/cpu-name` - vrací informaci o procesoru
`GET http://servername:12345/load`     - vrací aktuální zátěž

Omezení projektu:
-   V případě interní chyby serveru (chyba alokace, chyba vytvoření soketu, chyba bindu a podobně) se klientovi vždy odpoví, že je problém
-   Při větším množství argumentů (>2) se program ukončí a vypíše vzorek správného použití na standartní chybový vystup
-   Je-li port zadán mimo interval (0-65535), program se vypíše chybu