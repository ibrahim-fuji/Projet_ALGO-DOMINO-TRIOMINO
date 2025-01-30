#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "common_defs.h"
#include "common_structs.h"

// Forward declaration
typedef struct GameState GameState;

// Enumération des écrans
typedef enum {
    MENU,
    GAME_SELECTION,
    PLAYER_SETUP,
    GAME_DOMINO,
    GAME_TRIOMINO,
    SCORES
} GameScreen;

// Ajout des états de jeu
typedef enum {
    STATE_SELECTING,
    STATE_PIECE_SELECTED,
    STATE_ANIMATING,
    STATE_PLACING
} GamePlayState;

// Structure de l'état du jeu
struct GameState {
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
    TriominoPioche pioche;
    TriominoBoard board;
    int currentTurn;
    int consecutivePasses;
    bool gameOver;
    GamePlayState playState;
    float deltaTime;
    bool showHints;
    Vector2 lastValidPosition;
    ErrorLog errors;
    bool isInitialized;
    DominoNode* piocheDominos;  // Ajouter ce champ
};

// Prototypes des fonctions
void CalculateFinalScores(GameState *state);
void HandleTriominoTurn(GameState *state);
bool VerifierPlacementTriomino(GameState *state, Triomino *triomino, Vector2 position);
void PlacerTriomino(GameState *state, Triomino *triomino, Vector2 position);

#endif // GAME_STATE_H
