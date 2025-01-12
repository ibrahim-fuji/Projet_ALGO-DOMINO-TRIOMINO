// game_state.h
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>

// Définition des constantes pour les dimensions des dominos et triominos
#define DOMINO_WIDTH 100
#define DOMINO_HEIGHT 50

#define TRIOMINO_WIDTH 60
#define TRIOMINO_HEIGHT 52
#define TRIOMINO_DOT_RADIUS 3

#define DOTS_RADIUS 5

#define MAX_INPUT_LENGTH 20
#define MAX_PLAYERS 4
#define PANEL_PADDING 20

// Structure pour représenter un domino
typedef struct {
    int top;            // Valeur de la face supérieure
    int bottom;         // Valeur de la face inférieure
    Vector2 position;   // Position sur l'écran
    float rotation;     // Rotation en degrés
} Domino;

// Forward declaration de Triomino
typedef struct Triomino Triomino;

// Structure pour représenter un joueur
typedef struct {
    char name[MAX_INPUT_LENGTH];
    int score;
    bool isAI;
    Domino playerDominos[7];      // Maximum 7 dominos par joueur
    int dominoCount;
    Triomino *playerTriominos;    // Tableau dynamique de triominos
    int triominoCount;
    int triominoCapacity;
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
    int selectedGame;      // 0 = Dominos, 1 = Triominos
    int editingPlayer;     // Index du joueur en cours d'édition (-1 si aucun)
    int currentPlayer;     // Joueur actuel
    Triomino* selectedTriomino; // Triomino actuellement sélectionné
} GameState;

#endif // GAME_STATE_H
