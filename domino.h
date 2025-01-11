#include "joueurs.h"
#include <stdio.h>
#include <stdlib.h>

// Vérifie si un domino peut être placé à gauche ou à droite du plateau.
bool verificationPlacement(Domino domino_place, Plateau *plateau, bool a_gauche) {
    // Si le plateau est vide, le placement est toujours valide.
    if (plateau == NULL) {
        return true;
    }

    if (a_gauche) {
        // Vérifie si la valeur droite du domino à placer correspond à la valeur gauche du premier domino du plateau.
        return domino_place.v_droite == plateau->domino.v_gauche;
    } else {
        // Parcourt le plateau jusqu'au dernier domino.
        Plateau *actuel = plateau;
        while (actuel->suivant != NULL) {
            actuel = actuel->suivant;
        }
        // Vérifie si la valeur gauche du domino à placer correspond à la valeur droite du dernier domino du plateau.
        return domino_place.v_gauche == actuel->domino.v_droite;
    }
}

// Place un domino à gauche du plateau.
void placerDomino_Gauche(Domino domino, Plateau **plateau) {
    // Vérifie si le placement à gauche est valide.
    bool valide = verificationPlacement(domino, *plateau, true);
    if (valide) {
        // Alloue de la mémoire pour un nouveau domino sur le plateau.
        Plateau nouveau = (Plateau)malloc(sizeof(Plateau));
        nouveau->domino = domino;
        nouveau->precedent = NULL;
        nouveau->suivant = *plateau;

        // Si le plateau n'est pas vide, met à jour le pointeur précédent du premier domino existant.
        if (*plateau != NULL) {
            (*plateau)->precedent = nouveau;
        }

        // Met à jour la tête du plateau pour pointer vers le nouveau domino.
        *plateau = nouveau;
        printf("\nLe domino a été placé correctement !\n");
    } else {
        // Message si le domino ne peut pas être placé.
        printf("\nLe domino n'est pas valide !\n");
    }
}

// Place un domino à droite du plateau.
void placerDomino_Droite(Domino domino, Plateau **plateau) {
    // Si le plateau est vide, place le domino à gauche (équivalent d'initialisation).
    if (*plateau == NULL) {
        placerDomino_Gauche(domino, plateau);
        return;
    }

    // Vérifie si le placement à droite est valide.
    bool valide = verificationPlacement(domino, *plateau, false);
    if (valide) {
        // Parcourt le plateau jusqu'au dernier domino.
        Plateau *actuel = *plateau;
        while (actuel->suivant != NULL) {
            actuel = actuel->suivant;
        }

        // Alloue de la mémoire pour un nouveau domino à la fin du plateau.
        Plateau nouveau_droit = (Plateau)malloc(sizeof(Plateau));
        nouveau_droit->domino = domino;
        nouveau_droit->suivant = NULL;
        nouveau_droit->precedent = actuel;

        // Relie le dernier domino existant au nouveau.
        actuel->suivant = nouveau_droit;
        printf("\nLe domino a été placé correctement !\n");
    } else {
        // Message si le domino ne peut pas être placé.
        printf("\nLe domino n'est pas valide !\n");
    }
}

