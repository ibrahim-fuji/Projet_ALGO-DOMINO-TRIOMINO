#include "domino.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Initialise une pioche de dominos avec toutes les combinaisons possibles (0|0 à 6|6).
DominoNode* initialiserDomino() {
    DominoNode *tete = NULL;
    int i, j;

    for (i = 0; i <= 6; i++) {
        for (j = i; j <= 6; j++) {
            // Alloue de la mémoire pour un nouveau domino.
            DominoNode* nouveau = (DominoNode*)malloc(sizeof(DominoNode));
            nouveau->domino_current.v_gauche = i;
            nouveau->domino_current.v_droite = j;
            // Ajoute le nouveau domino au début de la liste.
            nouveau->domino_suivant = tete;
            tete = nouveau;
        }
    }
    return tete;
}

// Initialise un plateau vide.
Plateau* initialiserPlateau() {
    return NULL;
}

// Supprime un domino spécifique (v_gauche, v_droite) de la liste de dominos.
DominoNode* supprimerDomino(DominoNode *tete, int v_gauche, int v_droite) {
    DominoNode *tmp = tete;
    DominoNode *precedent = NULL;

    while (tmp != NULL) {
        // Si le domino correspondant est trouvé.
        if (tmp->domino_current.v_gauche == v_gauche && tmp->domino_current.v_droite == v_droite) {
            // Si le domino à supprimer est le premier de la liste.
            if (precedent == NULL) {
                tete = tmp->domino_suivant;
            } else {
                precedent->domino_suivant = tmp->domino_suivant;
            }
            free(tmp); // Libère la mémoire du domino supprimé.
            return tete; // Retourne la nouvelle tête de liste.
        }
        precedent = tmp;
        tmp = tmp->domino_suivant;
    }
    return tete;
}

// Affiche tous les dominos d'une liste.
void afficherDomino(DominoNode *tete) {
    DominoNode *pointeurDomino = tete;
    printf("La pioche : ");
    while (pointeurDomino != NULL) {
        printf("(%d|%d) ", pointeurDomino->domino_current.v_gauche, pointeurDomino->domino_current.v_droite);
        pointeurDomino = pointeurDomino->domino_suivant;
    }
    printf("\n");
}

// Affiche tous les dominos présents sur le plateau.
void afficherPlateau(Plateau *plateau) {
    Plateau *pointeurPlateau = plateau;
    printf("Le plateau : ");
    while (pointeurPlateau != NULL) {
        printf("(%d|%d) ", pointeurPlateau->domino.v_gauche, pointeurPlateau->domino.v_droite);
        pointeurPlateau = pointeurPlateau->suivant;
    }
    printf("\n");
}

// Mélange la pioche de dominos en utilisant l'algorithme de Fisher-Yates.
DominoNode* melange_pioche(DominoNode* tete) {
    int n = 0;
    DominoNode* tmp = tete;

    // Compte le nombre de dominos dans la liste.
    while (tmp != NULL) {
        n++;
        tmp = tmp->domino_suivant;
    }

    // Stocke les dominos dans un tableau pour faciliter le mélange.
    DominoNode** tableau = (DominoNode**)malloc(n * sizeof(DominoNode*));
    tmp = tete;
    for (int i = 0; i < n; i++) {
        tableau[i] = tmp;
        tmp = tmp->domino_suivant;
    }

    // Mélange les dominos dans le tableau.
    srand(time(NULL)); // Initialise le générateur aléatoire.
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        DominoNode* temp = tableau[i];
        tableau[i] = tableau[j];
        tableau[j] = temp;
    }

    // Reconstruit la liste chaînée à partir du tableau mélangé.
    for (int i = 0; i < n - 1; i++) {
        tableau[i]->domino_suivant = tableau[i + 1];
    }
    tableau[n - 1]->domino_suivant = NULL;

    tete = tableau[0]; // Met à jour la tête de la liste.
    free(tableau); // Libère la mémoire du tableau.
    return tete;
}

// Distribue des dominos aux joueurs depuis la pioche.
void distribuer_piece(DominoNode** pioche, DominoNode** joueurs, int nb_joueurs) {
    int nb_pieces = (nb_joueurs >= 3) ? 6 : 7; // Nombre de pièces à distribuer par joueur.

    for (int i = 0; i < nb_joueurs; i++) {
        for (int j = 0; j < nb_pieces; j++) {
            if (*pioche == NULL) { // Si la pioche est vide, affiche un message et arrête.
                printf("Pioche vide!\n");
                return;
            }

            DominoNode* nouveau = *pioche; // Prend le premier domino de la pioche.
            *pioche = (*pioche)->domino_suivant; // Met à jour la tête de la pioche.
            nouveau->domino_suivant = NULL;

            if (joueurs[i] == NULL) { // Si le joueur n'a pas encore de dominos, initialise sa liste.
                joueurs[i] = nouveau;
            } else { // Ajoute le domino à la fin de la liste du joueur.
                DominoNode* temp = joueurs[i];
                while (temp->domino_suivant != NULL) {
                    temp = temp->domino_suivant;
                }
                temp->domino_suivant = nouveau;
            }
        }
        // Affiche la pioche restante après distribution au joueur.
        printf("Pioche apres distribution au joueur %d:\n", i + 1);
        afficherDomino(*pioche);
    }
}

// Affiche une chaîne de dominos.
void afficherchaineDomino(DominoNode* chaine) {
    while (chaine != NULL) {
        printf("(%d|%d) ", chaine->domino_current.v_gauche, chaine->domino_current.v_droite);
        chaine = chaine->domino_suivant;
    }
    printf("\n");
}

// Libère la mémoire allouée pour une chaîne de dominos.
void lib_chaine(DominoNode* chaine) {
    while (chaine != NULL) {
        DominoNode* tmp = chaine;
        chaine = chaine->domino_suivant;
        free(tmp); // Libère chaque domino individuellement.
    }
}

