#include "domino.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

DominoNode* initialiserDomino() {
    DominoNode *tete = NULL;
    int i, j;

    for (i = 0; i <= 6; i++) {
        for (j = i; j <= 6; j++) {
            DominoNode *nouveau = (DominoNode*)malloc(sizeof(DominoNode));
            nouveau->domino_current.v_gauche = i;
            nouveau->domino_current.v_droite = j;
            nouveau->domino_suivant = tete;
            tete = nouveau;
        }
    }
    return tete;
}

Plateau* initialiserPlateau() {
    return NULL;
}

DominoNode* supprimerDomino(DominoNode *tete, int v_gauche, int v_droite) {
    DominoNode *tmp = tete;
    DominoNode *precedent = NULL;

    while (tmp != NULL) {
        if (tmp->domino_current.v_gauche == v_gauche && tmp->domino_current.v_droite == v_droite) {
            if (precedent == NULL) {
                tete = tmp->domino_suivant;
            } else {
                precedent->domino_suivant = tmp->domino_suivant;
            }
            free(tmp);
            return tete;
        }
        precedent = tmp;
        tmp = tmp->domino_suivant;
    }
    return tete;
}

void afficherDomino(DominoNode *tete) {
    DominoNode *pointeurDomino = tete;
    printf("La pioche : ");
    while (pointeurDomino != NULL) {
        printf("(%d|%d) ", pointeurDomino->domino_current.v_gauche, pointeurDomino->domino_current.v_droite);
        pointeurDomino = pointeurDomino->domino_suivant;
    }
    printf("\n");
}

void afficherPlateau(Plateau *plateau) {
    Plateau *pointeurPlateau = plateau;
    printf("Le plateau : ");
    while (pointeurPlateau != NULL) {
        printf("(%d|%d) ", pointeurPlateau->domino.v_gauche, pointeurPlateau->domino.v_droite);
        pointeurPlateau = pointeurPlateau->suivant;
    }
    printf("\n");
}DominoNode* melange_pioche(DominoNode* tete) {
    int n = 0;
    DominoNode* tmp = tete;

    while (tmp != NULL) {
        n++;
        tmp = tmp->domino_suivant;
    }

    DominoNode** tableau = (DominoNode**)malloc(n * sizeof(DominoNode*));
    tmp = tete;
    for (int i = 0; i < n; i++) {
        tableau[i] = tmp;
        tmp = tmp->domino_suivant;
    }

    srand(time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        DominoNode* temp = tableau[i];
        tableau[i] = tableau[j];
        tableau[j] = temp;
    }

    for (int i = 0; i < n - 1; i++) {
        tableau[i]->domino_suivant = tableau[i + 1];
    }
    tableau[n - 1]->domino_suivant = NULL;

    tete = tableau[0];
    free(tableau);
    return tete;
}

void distribuer_piece(DominoNode** pioche, DominoNode** joueurs, int nb_joueurs) {
    int nb_pieces = (nb_joueurs >= 3) ? 6 : 7;

    for (int i = 0; i < nb_joueurs; i++) {
        for (int j = 0; j < nb_pieces; j++) {
            if (*pioche == NULL) {
                printf("Pioche vide!\n");
                return;
            }

            DominoNode* nouveau = *pioche;
            *pioche = (*pioche)->domino_suivant;
            nouveau->domino_suivant = NULL;

            if (joueurs[i] == NULL) {
                joueurs[i] = nouveau;
            } else {
                DominoNode* temp = joueurs[i];
                while (temp->domino_suivant != NULL) {
                    temp = temp->domino_suivant;
                }
                temp->domino_suivant = nouveau;
            }
        }
        printf("Pioche apres distribution au joueur %d:\n", i + 1);
        afficherDomino(*pioche);
    }
}
void afficherchaineDomino(DominoNode* chaine) {
    while (chaine != NULL) {
        printf("(%d|%d) ", chaine->domino_current.v_gauche, chaine->domino_current.v_droite);
        chaine = chaine->domino_suivant;
    }
    printf("\n");
}
void lib_chaine(DominoNode* chaine) {
    while (chaine != NULL) {
        DominoNode* tmp = chaine;
        chaine = chaine->domino_suivant;
        free(tmp);
    }
}
