#ifndef JOUEURS_H
#define JOUEURS_H

#include <stdbool.h>
#include "do.h"
#include "game_state.h"

bool verificationPlacement(Domino domino_place, Plateau *plateau, bool a_gauche);
void placerDomino_Gauche(Domino domino, Plateau **plateau);
void placerDomino_Droite(Domino domino, Plateau **plateau);

// Nouvelles fonctions pour la gestion du jeu
MoveResult verifierPlacementDomino(Domino domino, Plateau *plateau, bool a_gauche);
// void supprimerDominoJoueur(Player *player, int dominoIndex);
bool peutJouer(Player *player, Plateau *plateau);
int calculerScoreDomino(Plateau *plateau);

// Add these function declarations
bool peutRotate(Domino domino);
int calculerScorePotentiel(Domino domino, Plateau *plateau, bool placementGauche);

#endif
