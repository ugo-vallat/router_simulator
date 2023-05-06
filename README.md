# router_simulator

Ce projet est une bibliothèque qui permet de réaliser des opérations complexes avec des nombres complexes.

## Installation
### Installation

Pour installer la bibliothèque, vous pouvez exécuter les commandes suivantes :

```bash
$ git clone https://github.com/mon-utilisateur/nom-de-la-bibliotheque.git
$ cd nom-de-la-bibliotheque
$ make
$ make install
```

Ces commandes vont cloner le dépôt GitHub de la bibliothèque, se placer dans le répertoire de la bibliothèque, compiler les fichiers source et installer la bibliothèque dans votre système.

## Utilisation

Pour utiliser la bibliothèque dans votre propre projet, vous devez inclure le fichier d'en-tête `nom-de-la-bibliotheque.h` dans votre code source, puis lier votre programme à la bibliothèque en utilisant l'option `-lnom-de-la-bibliotheque` lors de la compilation.

Voici un exemple de code qui utilise la bibliothèque pour calculer la somme de deux nombres complexes :

```c
#include <stdio.h>
#include <nom-de-la-bibliotheque.h>

int main() {
    complex_t a = {1.0, 2.0};
    complex_t b = {3.0, 4.0};
    complex_t c = add_complex(a, b);
    printf("(%g + %gi) + (%g + %gi) = (%g + %gi)\n", a.real, a.imag, b.real, b.imag, c.real, c.imag);
    return 0;
}
```

Pour compiler ce code, vous pouvez utiliser la commande suivante :

```bash
$ gcc -o mon-programme mon-programme.c -lnom-de-la-bibliotheque
```

Cette commande va compiler le fichier `mon-programme.c`, lier le programme à la bibliothèque `nom-de-la-bibliotheque` et générer un exécutable `mon-programme`.

## Documentation

La documentation de la bibliothèque est disponible dans le fichier `doc/nom-de-la-bibliotheque.html`. Vous pouvez ouvrir ce fichier dans votre navigateur web pour accéder à la documentation.

## Licence

Cette bibliothèque est distribuée sous la licence MIT. Veuillez consulter le fichier `LICENSE` pour plus d'informations sur les conditions d'utilisation de la bibliothèque.