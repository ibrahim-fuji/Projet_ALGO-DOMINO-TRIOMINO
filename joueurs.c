#include "joueurs.h"
#include "do.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool verificationPlacement(Domino domino_place, Plateau *plateau, bool a_gauche) {
    if (plateau == NULL) {
        return true;
    }

    if (a_gauche) {
        return domino_place.v_droite == plateau->domino.v_gauche;
    } else {
        Plateau *actuel = plateau;
        while (actuel->suivant != NULL) {
            actuel = actuel->suivant;
        }
        return domino_place.v_gauche == actuel->domino.v_droite;
    }
}

void placerDomino_Gauche(Domino domino, Plateau **plateau) {
    bool valide = verificationPlacement(domino, *plateau, true);
    if (valide) {
        Plateau* nouveau = (Plateau*)malloc(sizeof(Plateau*));
        nouveau->domino = domino;
        nouveau->precedent = NULL;
        nouveau->suivant = *plateau;

        if (*plateau != NULL) {
            (*plateau)->precedent = nouveau;
        }

        *plateau = nouveau;
//        printf("\nLe domino a été placé correctement !\n");
    } else {
//        printf("\nLe domino n'est pas valide !\n");
    }
}

void placerDomino_Droite(Domino domino, Plateau **plateau) {
    if (*plateau == NULL) {
        placerDomino_Gauche(domino, plateau);
        return;
    }

    bool valide = verificationPlacement(domino, *plateau, false);
    if (valide) {
        Plateau *actuel = *plateau;
        while (actuel->suivant != NULL) {
            actuel = actuel->suivant;
        }

        Plateau* nouveau_droit = (Plateau*)malloc(sizeof(Plateau*));
        nouveau_droit->domino = domino;
        nouveau_droit->suivant = NULL;
        nouveau_droit->precedent = actuel;

        actuel->suivant = nouveau_droit;
//        printf("\nLe domino a été placé correctement !\n");
    } else {
//        printf("\nLe domino n'est pas valide !\n");
    }
}
