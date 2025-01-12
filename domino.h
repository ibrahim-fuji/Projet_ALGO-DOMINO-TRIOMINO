#ifndef DOMINO_H
#define DOMINO_H

#include <stdbool.h>

#define total_piece 28 // Nombre total de pièces dans un jeu de dominos

typedef struct Domino {
    int v_gauche;
    int v_droite;
} Domino;

typedef struct DominoNode {
    Domino domino_current;
    struct DominoNode *domino_suivant;
} DominoNode;

typedef struct Plateau {
    Domino domino;
    struct Plateau *precedent;
    struct Plateau *suivant;
} Plateau;

DominoNode* initialiserDomino();
Plateau* initialiserPlateau();
DominoNode* supprimerDomino(DominoNode *tete, int v_gauche, int v_droite);
void afficherDomino(DominoNode *tete);
void afficherPlateau(Plateau *plateau);

void init_pioche(Domino pioche[]);
void melange_pioche(Domino pioche[]);
void distribuer_piece(Domino pioche[], int* cpt_pioche, DominoNode** joueurs, int nb_joueurs);
void afficherchaineDomino(DominoNode* chaine);
void lib_chaine(DominoNode* chaine);

#endif
