// triomino.h
#ifndef TRIOMINO_H
#define TRIOMINO_H

#include "common_structs.h"
#include "game_state.h"

// Prototypes des fonctions de base
void InitTriomino(Triomino *triomino);
void DrawTriomino(Triomino triomino);
bool IsPointInTriomino(Vector2 point, Triomino triomino);
void HandleTriominoRotation(struct GameState *state);
int CalculerPoints(Triomino *triomino, BonusType bonus);
bool JouerIA(struct GameState *state, int playerIndex);
void UpdateTriominoRotation(Triomino *triomino, float deltaTime);
bool DoTriominosMatch(Triomino *tri1, Triomino *tri2, int side1, int side2);

// Prototypes des fonctions de gestion du jeu
void InitTriominoPioche(TriominoPioche *pioche);
void MelangerPioche(TriominoPioche *pioche);
Triomino PiocherTriomino(TriominoPioche *pioche);
bool TryPiocherTriomino(GameState *state, int playerIndex);
void UpdateTriominoPositions(GameState *state);
void DrawTriominoPlateau(GameState *state);
bool CheckTriominoCollisions(GameState *state, Vector2 position);

// Add IsValidMove declaration here
bool IsValidMove(GameState *state, Vector2 position);

// Ajout des prototypes manquants
void UpdatePieceAnimation(PieceAnimation *anim, float deltaTime);
Vector2 GetSnapPosition(Vector2 mousePos, GameState *state);
void ShowPlacementHints(GameState *state);
void HandleAITurn(GameState *state);

// Nouvelles fonctions pour la gestion des triominos
bool verifierCoupPossible(GameState *state);
void updateTriominoPlacement(GameState *state, Vector2 mousePos);
int calculerScoreTriomino(GameState *state);
void highlightPossibleMoves(GameState *state);
void animatePlacement(GameState *state, Triomino *triomino, Vector2 targetPos);

#endif // TRIOMINO_H
