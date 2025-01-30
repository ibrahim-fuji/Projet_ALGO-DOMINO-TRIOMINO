#ifndef DOMINO_LOGIC_H
#define DOMINO_LOGIC_H

#include "game_state.h"

// Évaluation d'un coup
typedef struct {
    bool isValid;
    int score;
    bool isDouble;
    Vector2 position;
    float rotation;
} DominoMove;

// Fonctions de logique de jeu
bool isDominoDouble(Domino domino);
int findFirstPlayer(Player *players, int playerCount);
DominoMove evaluateMove(Domino domino, Plateau *plateau, bool isLeft);
bool canDrawFromPile(GameState *state);
void rotateDomino(Domino *domino);
int calculateDominoScore(Plateau *plateau);

// IA
typedef struct {
    Domino domino;
    bool placementGauche;
    int score;
} AIMove;

AIMove findBestMove(Player *player, Plateau *plateau);

#endif
