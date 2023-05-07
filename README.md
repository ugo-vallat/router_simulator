
# router_simulator

Ce projet est un simulateur de routeur permettant de voir les échanges de tables de routage.

# Installation

Pour tester le simulateur, vous pouvez fork le projet puis le cloner sur votre machine. Compilez le programme avec make et puis lancez-le :

```bash
$ ./router_simulator/ROUTER/SOURCE_CODE
$ make
$ ./test_router
```

# Utilisation

Des jeux de test sont déjà présent dans le dossier SET_OF_TEST, copiez les fichiés .txt du set que vous désirez et mettez les dans le dossier INPUT puis lancer le programme avec `./test_router`.

Les fichiers de configuration se décomposent en 2 parties :

## config_network.txt

Configuration générale du réseau avec dans l'ordre, nombre de routeurs, liste des routeurs (numéro + ip virtuel + port physique utilisé) et les connexions entre les routeurs.

Exemple : 
```txt
2
1 - 1.1.1.1 - 20000
2 - 2.2.2.2 - 20001

1 - 2
```

## config_routeur_n.txt

Table de routage initiale du routeur n. Contient le nombre d'entrées suivi de la liste des sous-réseaux auxquels il est relié (ip sous réseau + masque + ip routeur + poids ).
Théoriquement, ip routeur doit correspondre à l'ip donné dans le fichié config_network.txt et le poids vaut 1, mais l'utilisateur est libre de rentrer ce qu'il désire comment valeur (en respectant le format ipv4 et de poids < 1000).

Exemple :
```txt
2
10.1.1.1 28 1.1.1.1 1
10.2.2.2 32 1.1.1.1 1
```

## Sorties

Les tables de routage après la fin de l'exécution du programme sont ronvoyées vers leur fichier respctif (table_router_n.txt) dans le dossier OUTPUT.
Chaque fichier contient le nombre d'entrées de la table suivi d'un tableau comportant :
    - ip du sous-réseau (avec le masque appliqué !)
    - masque du sous réseau
    - ip du prochain routeur
    - poids

Exemple :
```txt
size = 4
____________________________________________________
    IP NETWORK   | MASK|    IP ROUTER    | WEIGHT 
        10.1.0.0 |  20 |         1.1.1.1 |   1 
        10.2.2.2 |  32 |         1.1.1.1 |   1 
        20.1.1.0 |  30 |         2.2.2.2 |   2 
        20.2.2.0 |  24 |         2.2.2.2 |   2 
```

## Notes

Dans les fichiers `router.c` et `test_router.c` se trouvent des constantes pouvant être modifiées (mais déconseillé) :

```c
/*----------------- dans router.c -----------------*/
#define TIME_READ_MAX 5 //temps de blocage max des récepteurs et émetteurs
#define TIME_TO_LIVE_MAX 15 //durée de vie max de tous les processus
#define NB_RECEIV_MAX 1024 // nombre max de packects reçus par le récepteur
#define NB_TRANSMITE_MAX 1024 // nombre max de packects envoyés par l'emmetteur
#define NB_MODIF_TABLE_MAX 1024 // nombre max de modifications de la table 

/*----------------- dans test_router.c -----------------*/
#define NUMBER_ROUTER_MAX 8 // nombre maximum de routeurs
#define LOCALHOST "127.0.0.1" // adresse localhost

```


# Licence

Ce programme est distribuée sous la licence GNU. (volez pas le code svp :) )