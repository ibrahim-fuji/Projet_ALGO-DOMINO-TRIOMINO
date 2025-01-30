#ifndef DO_H
#define DO_H

#include "common_structs.h"

// Forward declaration des structures
struct GameState;

// Prototypes des fonctions uniquement
DominoNode* initialiserDomino(void);
Plateau* initialiserPlateau(void);
DominoNode* supprimerDomino(DominoNode* tete, int v_gauche, int v_droite);
void afficherDomino(DominoNode* tete);
void afficherPlateau(Plateau* plateau);
void melange_pioche(DominoNode** tete);
void distribuer_piece(DominoNode** pioche, DominoNode* joueurs[], int nb_joueurs);
void lib_chaine(DominoNode* chaine);
Domino tournerDomino(Domino domino);
void supprimerDominoJoueur(Player* player, Domino domino);
bool dominosEgaux(Domino d1, Domino d2);
DominoNode* piocherDomino(DominoNode** pioche);
bool essayerPiocherDomino(struct GameState* state, int playerIndex);

// Fonctions pour l'IA et le score
bool jouerIA_Domino(struct GameState* state);
int calculerScore(Plateau* plateau);
void verifierFinPartie(struct GameState* state);
bool peutJouerDomino(Player* player, Plateau* plateau);

// Nouvelles fonctions pour l'IA et le score
int evaluerCoupDomino(Domino domino, Plateau* plateau, bool aGauche);
Domino* trouverMeilleurCoup(Player* ai, Plateau* plateau, bool* aGauche);
int calculerScoreMain(Player* player);
void ajouterPointsPlateau(struct GameState* state, int playerIndex);

#endif // DO_H
