#ifndef DOMINO_H
#define DOMINO_H

// Définition de la structure Domino
typedef struct {
    int v_gauche; // Valeur gauche
    int v_droite; // Valeur droite
} Domino;

// Structure pour les nœuds de la liste chaînée de dominos (pioche ou main de joueur)
typedef struct DominoNode {
    Domino domino_current;         // Domino actuel
    struct DominoNode* domino_suivant; // Pointeur vers le domino suivant
} DominoNode;

// Structure pour représenter une chaîne de dominos sur le plateau
typedef struct Plateau {
    Domino domino;         // Domino actuel
    struct Plateau* suivant; // Pointeur vers le domino suivant sur le plateau
} Plateau;

// Prototypes des fonctions
DominoNode* initialiserDomino();
Plateau* initialiserPlateau();
DominoNode* supprimerDomino(DominoNode* tete, int v_gauche, int v_droite);
void afficherDomino(DominoNode* tete);
void afficherPlateau(Plateau* plateau);
DominoNode* melange_pioche(DominoNode* tete);
void distribuer_piece(DominoNode** pioche, DominoNode** joueurs, int nb_joueurs);
void afficherchaineDomino(DominoNode* chaine);
void lib_chaine(DominoNode* chaine);

#endif // DOMINO_H
