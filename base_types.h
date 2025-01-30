#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include "raylib.h"

struct Triomino; // Forward declaration

// Structures de base
typedef struct {
    int v_gauche;
    int v_droite;
    Vector2 position;
    float rotation;
} Domino;

typedef struct DominoNode {
    Domino domino_current;
    struct DominoNode* domino_suivant;
} DominoNode;

typedef struct Plateau {
    Domino domino;
    struct Plateau* suivant;
    struct Plateau* precedent;
} Plateau;

typedef struct {
    Rectangle bounds;
    Color color;
} Chevalet;

typedef struct {
    char name[50];
    int score;
    bool isAI;
    Domino playerDominos[7];
    int dominoCount;
    struct Triomino* playerTriominos;
    int triominoCount;
    int triominoCapacity;
    Chevalet chevalet;
} Player;

#endif // BASE_TYPES_H
