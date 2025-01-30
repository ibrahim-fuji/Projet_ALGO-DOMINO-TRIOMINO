// domino.h
#ifndef DOMINO_H
#define DOMINO_H

#include "raylib.h"
#include "common_defs.h"
#include "game_state.h"
#include "triomino.h"      // Inclusion du module Triomino

// Button structure
typedef struct {
    Rectangle rect;
    const char *text;
    Color baseColor;
    Color hoverColor;
    Color textColor;
    bool isHovered;
} Button;

// Function declarations
bool DrawButtonCustom(Button *button);
void DrawDominoDot(float x, float y);
void DrawDominoFace(float x, float y, int value, float width, float height);
void DrawDomino(Domino domino);
void DrawDominoHighlight(Domino *domino, Color color);
void UpdateDominoPlacement(GameState *state, Vector2 mousePos);

#endif // DOMINO_H
