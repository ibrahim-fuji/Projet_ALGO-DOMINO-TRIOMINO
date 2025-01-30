#ifndef DO_H
#define DO_H
#include "raylib.h"

// Définition de la structure Domino
typedef struct {
    int v_gauche;
    int v_droite;
    Vector2 position;
    float rotation;
} Domino;

// Structure pour les nœuds de la liste chaînée de dominos (pioche ou main de joueur)
typedef struct DominoNode {
    Domino domino_current;         // Domino actuel
    struct DominoNode* domino_suivant; // Pointeur vers le domino suivant
} DominoNode;

// Structure pour représenter une chaîne de dominos sur le plateau
typedef struct Plateau {
    Domino domino;         // Domino actuel
    struct Plateau* suivant;	 // Pointeur vers le domino suivant sur le plateau
	struct Plateau* precedent;
} Plateau;

// Prototypes des fonctions
DominoNode* initialiserDomino();
Plateau* initialiserPlateau();
void supprimerDomino(Domino main[], int index, int* dominoCount);
void afficherDomino(DominoNode* tete);
void afficherPlateau(Plateau* plateau);
void melange_pioche(DominoNode** tete);
void distribuer_piece(DominoNode** pioche, DominoNode** joueurs, int nb_joueurs);
void afficherchaineDomino(DominoNode* chaine);
void lib_chaine(DominoNode* chaine);

#endif // DOMINO_H
