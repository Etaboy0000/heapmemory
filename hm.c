#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include "hm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>



// Taille des métadonnées d'un bloc
#define BLOCK_SIZE sizeof(struct block_libre)

// Structure d'un bloc libre
struct block_libre {
    int taille;                 // Taille du bloc
    struct block_libre* suivant; // Pointeur vers le bloc suivant
    int libre;                  // Indicateur: 1 = libre, 0 = utilisé
};


char* zone = NULL;  // début de la zone mémoire
struct block_libre* liste_blocs_libres = NULL;  // liste chaînée des blocs libres

// Fonction d'initialisation de la mémoire
void init() {
    if (zone == NULL) {
        zone = sbrk(0);  // obtenir l'adresse actuelle du tas, de facon absolu
        if (sbrk(1024 * 1024) == (void*)-1) {  // allouer 1 Mo au tas dans un premier temps
            perror("Erreur lors de l'initialisation de la mémoire");
            return;
        }
        liste_blocs_libres = (struct block_libre*)zone;
        liste_blocs_libres->taille = 1024 * 1024 - BLOCK_SIZE;
        liste_blocs_libres->suivant = NULL;
        liste_blocs_libres->libre = 1;
    }
}

// Fonction pour trouver un bloc libre assez grand
struct block_libre* trouver_bloc(int taille) {
    struct block_libre* current = liste_blocs_libres;
    while (current != NULL) {
        if (current->libre && current->taille >= taille) {
            return current;
        }
        current = current->suivant;
    }
    return NULL;  // Aucun bloc libre trouvé
}

// Fonction pour diviser un bloc libre s'il est plus grand que nécessaire
void diviser_bloc(struct block_libre* bloc, int taille) {
    struct block_libre* nouveau_bloc = (void*)((char*)bloc + BLOCK_SIZE + taille);
    nouveau_bloc->taille = bloc->taille - taille - BLOCK_SIZE;
    nouveau_bloc->suivant = bloc->suivant;
    nouveau_bloc->libre = 1;

    bloc->taille = taille;
    bloc->suivant = nouveau_bloc;
    bloc->libre = 0;
}

// Fonction d'allocation de mémoire
void* mon_malloc(int taille) {
    if (!zone) {
        init();  // Initialise la mémoire si elle ne l'est pas encore
    }

    struct block_libre* bloc = trouver_bloc(taille);
    if (!bloc) {
        return NULL;  // Pas assez de mémoire disponible
    }

    if (bloc->taille > taille + BLOCK_SIZE) {
        diviser_bloc(bloc, taille);  // Divise le bloc s'il est trop grand
    } else {
        bloc->libre = 0;  // Marque le bloc comme utilisé
    }

    return (void*)((char*)bloc + BLOCK_SIZE);  // Retourne la partie utilisable
}

// Fonction de libération de mémoire
void mon_free(void* ptr) {
    if (!ptr) return;

    struct block_libre* bloc = (struct block_libre*)((char*)ptr - BLOCK_SIZE);  // Reviens aux métadonnées
    bloc->libre = 1;  // Marque le bloc comme libre

    // TODO: Fusionner les blocs libres adjacents pour éviter la fragmentation , update pour plus tard.
}
//utiliser gdb
