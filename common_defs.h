#ifndef COMMON_DEFS_H
#define COMMON_DEFS_H

#include "raylib.h"
#include <stdbool.h>

// Constantes de dimensions
#define DOMINO_WIDTH 50
#define DOMINO_HEIGHT 40
#define TRIOMINO_WIDTH 60
#define TRIOMINO_HEIGHT 52
#define DOTS_RADIUS 3
#define TOTAL_TRIOMINOS 56
#define MAX_INPUT_LENGTH 20
#define MAX_PLAYERS 4

// Constantes pour le gameplay
#define SNAP_DISTANCE 20.0f
#define ANIMATION_SPEED 5.0f
#define MIN_DISTANCE_BETWEEN_PIECES 5.0f

// Ajouter les constantes de gestion d'erreurs
#define MAX_ERRORS 10
#define ERROR_BUFFER_SIZE 256

// Constants pour la logique de jeu
#define INVALID_MOVE -1
#define VALID_MOVE 1
#define GAME_WIN 2

// Couleurs du jeu
#define CUSTOM_GREEN (Color){ 76, 187, 23, 255 }
#define CUSTOM_BLUE (Color){ 0, 121, 241, 255 }
#define CUSTOM_RED (Color){ 230, 41, 55, 255 }
#define CUSTOM_BEIGE (Color){ 253, 249, 238, 255 }
#define CUSTOM_DARK_BLUE (Color){ 0, 82, 172, 255 }
#define CUSTOM_LIGHT_BLUE (Color){ 95, 205, 228, 255 }
#define WHITE_COLOR (Color){ 255, 255, 255, 255 }
#define BLACK_TEXT (Color){ 0, 0, 0, 255 }
#define DARKRED (Color){ 190, 33, 55, 255 }

// Types de bonus
typedef enum {
    BONUS_NONE,
    BONUS_BRIDGE,
    BONUS_HEXAGON,
    BONUS_DOUBLE_HEXAGON,
    BONUS_TRIPLE_HEXAGON
} BonusType;

// Structure pour gérer les animations
typedef struct {
    bool isAnimating;
    Vector2 startPos;
    Vector2 targetPos;
    float progress;
} PieceAnimation;

// Structure pour les hexagones
typedef struct {
    Vector2 center;
    int triominoCount;
    Vector2 positions[6];
} Hexagon;

// Forward declaration
struct GameState;

// Structures de base
typedef struct Triomino {
    int values[3];
    Vector2 position;
    float rotation;
    bool isRotating;
    float rotationSpeed;
    PieceAnimation animation;
} Triomino;

// Structure pour la gestion des erreurs
typedef struct {
    char messages[MAX_ERRORS][ERROR_BUFFER_SIZE];
    int count;
} ErrorLog;

typedef struct {
    Triomino pieces[TOTAL_TRIOMINOS];
    int count;
} TriominoPioche;

typedef struct {
    Triomino *pieces;
    int count;
    Vector2 *positions;
} TriominoBoard;

// Structure pour gérer le statut du jeu
typedef struct {
    bool isValid;
    int score;
    char message[256];
} MoveResult;

#endif // COMMON_DEFS_H
