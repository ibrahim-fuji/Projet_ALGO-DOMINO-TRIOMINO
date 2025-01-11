#ifndef JOUEURS_H
#define JOUEURS_H

#include "domino.h"

bool verificationPlacement(Domino domino_place, Plateau *plateau, bool a_gauche);
void placerDomino_Gauche(Domino domino, Plateau **plateau);
void placerDomino_Droite(Domino domino, Plateau **plateau);

#endif
