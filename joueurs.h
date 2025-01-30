#ifndef JOUEURS_H
#define JOUEURS_H

#include <stdbool.h>
#include "do.h"
#include "game_state.h"

bool verificationPlacement(Domino domino_place, Plateau *plateau, bool a_gauche);
void placerDomino_Gauche(Domino domino, Plateau **plateau);
void placerDomino_Droite(Domino domino, Plateau **plateau);

#endif
