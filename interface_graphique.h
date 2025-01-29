// interface_graphique.h
#ifndef INTERFACE_GRAPHIQUE_H
#define INTERFACE_GRAPHIQUE_H

#include "game_state.h"
#include "raylib.h"

// Déclarations des fonctions
void DrawMainMenu(GameState *state);
void DrawGameSelection(GameState *state, Texture2D dominoImage, Texture2D triominoImage);
void DrawPlayerSetup(GameState *state);
void DrawGameScreen(GameState *state);
void DrawScores(GameState *state);
void HandleInput(GameState *state);
void HandlePlayerInput(GameState *state);
void DrawDominoInChevalet(GameState *state, int playerIndex, int dominoIndex);
void DrawTriominoInChevalet(GameState *state, int playerIndex, int triominoIndex);
void InitGamePieces(GameState *state);

#endif // INTERFACE_GRAPHIQUE_H