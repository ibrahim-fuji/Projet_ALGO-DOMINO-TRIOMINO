// triomino.h
#ifndef TRIOMINO_H
#define TRIOMINO_H

#include "game_state.h"
#include "raylib.h"
#include <stdbool.h>

// Définition de la structure Triomino
struct Triomino {
    int values[3];          // Les trois valeurs du triomino
    Vector2 position;       // Position à l'écran
    float rotation;         // Rotation en degrés
    bool isRotating;        // Indique si le triomino est en rotation
    float rotationSpeed;    // Vitesse de rotation en degrés par seconde
};

// Prototypes des fonctions
void InitTriomino(Triomino *triomino);
void DrawTriomino(Triomino triomino);
void UpdateTriominoRotation(Triomino *triomino, float deltaTime);
bool DoTriominosMatch(Triomino *tri1, Triomino *tri2, int side1, int side2);
bool IsPlacementValid(GameState *state, Triomino *triomino);
bool IsPointInTriomino(Vector2 point, Triomino triomino);
Color GetColorByValue(int value);
void DrawTriominoNumber(float x, float y, int value, int faceIndex);

#endif // TRIOMINO_H
