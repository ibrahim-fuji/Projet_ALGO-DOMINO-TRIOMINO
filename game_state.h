#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include "do.h" 
#include "joueurs.h"

// Définition des constantes pour les dimensions
#define DOMINO_WIDTH 50
#define DOMINO_HEIGHT 40
#define TRIOMINO_WIDTH 60
#define TRIOMINO_HEIGHT 52
#define TRIOMINO_DOT_RADIUS 3
#define DOTS_RADIUS 3
#define MAX_INPUT_LENGTH 20
#define MAX_PLAYERS 4
#define PANEL_PADDING 20

// Structure pour le chevalet
typedef struct {
    Rectangle bounds;
    Color color;
} Chevalet;

// Structure pour représenter un domino


// Structure pour un triomino
typedef struct {
    int values[3];
    Vector2 position;
    float rotation;
    bool isRotating;
    float rotationSpeed;
} Triomino;

// Structure pour représenter un joueur
typedef struct {
    char name[MAX_INPUT_LENGTH];
    int score;
    bool isAI;
    Domino playerDominos[7];
    int dominoCount;
    Triomino *playerTriominos;
    int triominoCount;
    int triominoCapacity;
    Chevalet chevalet;
} Player;

// Enumération des différents écrans du jeu
typedef enum {
    MENU,
    GAME_SELECTION,
    PLAYER_SETUP,
    GAME_DOMINO,
    GAME_TRIOMINO,
    SCORES
} GameScreen;

// Structure de l'état du jeu
typedef struct {
    GameScreen currentScreen;
    char inputText[MAX_INPUT_LENGTH];
    int playerCount;
    Player players[MAX_PLAYERS];
    bool isGameStarted;
    int selectedGame;
    int editingPlayer;
    int currentPlayer;
    Triomino* selectedTriomino;
    int screenWidth;
    int screenHeight;
    bool isFullscreen;
    Plateau *plateau;
    Domino *selectedDomino;
} GameState;

// Fonctions de calcul des dimensions adaptatives
float GetDominoWidth(GameState *state);
float GetDominoHeight(GameState *state);
float GetTriominoWidth(GameState *state);
float GetTriominoHeight(GameState *state);

#endif // GAME_STATE_H
