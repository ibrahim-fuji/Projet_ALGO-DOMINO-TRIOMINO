#ifndef COMMON_STRUCTS_H
#define COMMON_STRUCTS_H

#include "raylib.h"
#include "common_defs.h"

// Forward declarations
struct Triomino;
struct GameState;
struct Player;

// Structures de base
typedef struct {
    Rectangle bounds;
    Color color;
} Chevalet;

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
    char name[MAX_INPUT_LENGTH];
    int score;
    bool isAI;
    Domino playerDominos[7];
    int dominoCount;
    struct Triomino* playerTriominos;
    int triominoCount;
    int triominoCapacity;
    Chevalet chevalet;
} Player;

#endif // COMMON_STRUCTS_H
