// domino.h
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "game_state.h"    // Inclure game_state.h pour les définitions des structures et constantes
#include "raymath.h"       // Inclusion nécessaire pour les fonctions Vector2
#include "triomino.h"      // Inclusion du module Triomino
#include "do.h"

// Définition des couleurs personnalisées
#define CUSTOM_BLUE        (Color){0, 70, 140, 255}
#define CUSTOM_GREEN       (Color){34, 139, 34, 255}
#define CUSTOM_LIGHT_BLUE  (Color){135, 206, 250, 255}
#define CUSTOM_RED         (Color){220, 20, 60, 255}
#define CUSTOM_DARK_BLUE   (Color){0, 0, 139, 255}
#define CUSTOM_BEIGE       (Color){245, 245, 220, 255}
#define BLACK_TEXT         (Color){0, 0, 0, 255}

// Définition de DARKRED si elle n'est pas déjà définie
#ifndef DARKRED
    #define DARKRED   (Color){139, 0, 0, 255}
#endif

// **Définition de WHITE_COLOR**
#define WHITE_COLOR        (Color){255, 255, 255, 255}

// Structure pour représenter un bouton
typedef struct {
    Rectangle rect;
    const char *text;
    Color baseColor;
    Color hoverColor;
    Color textColor;
    bool isHovered;
} Button;

// Prototypes des fonctions de dessin graphiques générales
bool DrawButtonCustom(Button *button);
void DrawDominoDot(float x, float y);
void DrawDominoFace(float x, float y, int value, float width, float height);
void DrawDomino(Domino domino);

#endif // GRAPHICS_H
