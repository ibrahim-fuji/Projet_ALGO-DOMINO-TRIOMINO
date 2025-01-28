// interface_graphique.c
#include "raylib.h"
#include "raymath.h"
#include "triomino.h"      // Include Triomino module
#include "domino.h"        // Include the Domino module after triomino.h
#include "game_state.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// Define constants
#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()

// Function prototypes
void InitGameState(GameState *state);
void DrawMainMenu(GameState *state);
void DrawGameSelection(GameState *state, Texture2D dominoImage, Texture2D triominoImage);
void DrawPlayerSetup(GameState *state);
void DrawGameScreen(GameState *state);
void DrawScores(GameState *state);
void HandleInput(GameState *state);
void HandlePlayerInput(GameState *state);
void InitGamePieces(GameState *state);

// Main function
int main(void) {
    // Initialize random seed
    srand(time(NULL));

    // Initialize window
    InitWindow(GetScreenWidth(), GetScreenHeight(), "Dominos & Triominos");
    ToggleFullscreen();
    SetTargetFPS(60);


    // Initialize game state
    GameState gameState;
    InitGameState(&gameState);

    // Load textures for the selection screen
    Texture2D dominoImage = LoadTexture("resources/domino.png");
    Texture2D triominoImage = LoadTexture("resources/triomino.png");

    // Check if textures loaded successfully
    if (dominoImage.id == 0 || triominoImage.id == 0) {
        printf("Error loading textures\n");
        CloseWindow();
        return -1;
    }

    // Main game loop
    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        HandleInput(&gameState);
        HandlePlayerInput(&gameState);

        // Update triomino rotations
        for (int p = 0; p < gameState.playerCount; p++) {
            for (int t = 0; t < gameState.players[p].triominoCount; t++) {
                UpdateTriominoRotation(&gameState.players[p].playerTriominos[t], deltaTime);
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            // Draw based on the current screen
            switch (gameState.currentScreen) {
                case MENU:
                    DrawMainMenu(&gameState);
                    break;
                case GAME_SELECTION:
                    DrawGameSelection(&gameState, dominoImage, triominoImage);
                    break;
                case PLAYER_SETUP:
                    DrawPlayerSetup(&gameState);
                    break;
                case GAME_DOMINO:
                case GAME_TRIOMINO:
                    DrawGameScreen(&gameState);
                    break;
                case SCORES:
                    DrawScores(&gameState);
                    break;
            }
        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < MAX_PLAYERS; i++) {
        free(gameState.players[i].playerTriominos);
    }
    UnloadTexture(dominoImage);
    UnloadTexture(triominoImage);
    CloseWindow();
    return 0;
}

// ------------------------
// Initialization function for game state
// ------------------------
void InitGameState(GameState *state) {
    state->currentScreen = MENU;
    state->playerCount = 0;
    state->isGameStarted = false;
    state->selectedGame = -1;
    state->editingPlayer = -1;
    state->currentPlayer = 0;
    state->inputText[0] = '\0';
    state->selectedTriomino = NULL;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        state->players[i].name[0] = '\0';
        state->players[i].score = 0;
        state->players[i].isAI = false;
        state->players[i].dominoCount = 0;
        state->players[i].triominoCount = 0;
        state->players[i].triominoCapacity = 7; // Initial capacity
        state->players[i].playerTriominos = malloc(state->players[i].triominoCapacity * sizeof(Triomino));
        if (!state->players[i].playerTriominos) {
            printf("Memory allocation error for triominos\n");
            exit(-1);
        }
    }
}

// ------------------------
// Function to draw the main menu
// ------------------------
void DrawMainMenu(GameState *state) {
    // Menu background
    ClearBackground(RAYWHITE);

    // Game title
    DrawText("DOMINOS & TRIOMINOS", SCREEN_WIDTH/2 - MeasureText("DOMINOS & TRIOMINOS", 40)/2, 100, 40, CUSTOM_BLUE);

    // Define buttons
    Button playButton = {
        .rect = {SCREEN_WIDTH/2 - 100, 250, 200, 50},
        .text = "JOUER",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    Button scoresButton = {
        .rect = {SCREEN_WIDTH/2 - 100, 350, 200, 50},
        .text = "SCORES",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    Button exitButton = {
        .rect = {SCREEN_WIDTH/2 - 100, 450, 200, 50},
        .text = "QUITTER",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    // Draw and handle button clicks
    if (DrawButtonCustom(&playButton)) state->currentScreen = GAME_SELECTION;
    if (DrawButtonCustom(&scoresButton)) state->currentScreen = SCORES;
    if (DrawButtonCustom(&exitButton)) CloseWindow();
}

// ------------------------
// Function to draw the game selection screen
// ------------------------
void DrawGameSelection(GameState *state, Texture2D dominoImage, Texture2D triominoImage) {
    // Screen background
    ClearBackground(RAYWHITE);

    // Selection title
    DrawText("CHOISISSEZ VOTRE JEU", SCREEN_WIDTH/2 - MeasureText("CHOISISSEZ VOTRE JEU", 30)/2, 50, 30, CUSTOM_BLUE);

    // Define rectangles for games
    Rectangle dominoRect = {SCREEN_WIDTH/4 - 125, 150, 250, 250};
    Rectangle triominoRect = {3 * SCREEN_WIDTH/4 - 125, 150, 250, 250};

    // Draw game backgrounds
    DrawRectangleRec(dominoRect, CUSTOM_BLUE);
    DrawRectangleRec(triominoRect, CUSTOM_BLUE);

    // Draw game images
    DrawTexturePro(dominoImage, 
                   (Rectangle){0, 0, (float)dominoImage.width, (float)dominoImage.height},
                   (Rectangle){dominoRect.x + 25, dominoRect.y + 25, 200, 200},
                   (Vector2){0, 0}, 0, WHITE);
    
    DrawTexturePro(triominoImage,
                   (Rectangle){0, 0, (float)triominoImage.width, (float)triominoImage.height},
                   (Rectangle){triominoRect.x + 25, triominoRect.y + 25, 200, 200},
                   (Vector2){0, 0}, 0, WHITE);

    // Handle game selection clicks
    Vector2 mousePoint = GetMousePosition();
    if (CheckCollisionPointRec(mousePoint, dominoRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state->selectedGame = 0;
        state->currentScreen = PLAYER_SETUP;
        InitGamePieces(state); // Initialize dominos
    }
    if (CheckCollisionPointRec(mousePoint, triominoRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state->selectedGame = 1;
        state->currentScreen = PLAYER_SETUP;
        InitGamePieces(state); // Initialize triominos
    }

    // Back button
    Button backButton = {
        .rect = {50, SCREEN_HEIGHT - 70, 150, 50},
        .text = "RETOUR",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&backButton)) state->currentScreen = MENU;
}

// ------------------------
// Function to draw the player setup screen
// ------------------------
void DrawPlayerSetup(GameState *state) {
    // Screen background
    ClearBackground(RAYWHITE);

    // Screen title
    DrawText("CONFIGURATION DES JOUEURS", 
             SCREEN_WIDTH/2 - MeasureText("CONFIGURATION DES JOUEURS", 30)/2, 50, 30, CUSTOM_BLUE);

    // Player count selection
    DrawText("Nombre de joueurs:", SCREEN_WIDTH/2 - 150, 150, 20, BLACK_TEXT);

    for (int i = 2; i <= 4; i++) {
        Button countButton = {
            .rect = {SCREEN_WIDTH/2 - 150 + (i-2) * 100, 180, 80, 50},
            .text = TextFormat("%d", i),
            .baseColor = CUSTOM_BLUE,
            .hoverColor = CUSTOM_LIGHT_BLUE,
            .textColor = CUSTOM_BEIGE,
            .isHovered = false
        };

        if (DrawButtonCustom(&countButton)) {
            state->playerCount = i;
            // Initialize default names if not AI
            for (int j = 0; j < state->playerCount; j++) {
                if (state->players[j].name[0] == '\0') {
                    strcpy(state->players[j].name, TextFormat("Joueur %d", j+1));
                }
            }
        }
    }

    // Player configuration
    for (int i = 0; i < state->playerCount; i++) {
        // Player text
        DrawText(TextFormat("Joueur %d:", i+1), SCREEN_WIDTH/2 - 200, 250 + i * 60, 20, CUSTOM_BLUE);
        DrawText(state->players[i].isAI ? "IA" : "Humain", SCREEN_WIDTH/2 - 50, 250 + i * 60, 20, CUSTOM_BEIGE);

        // AI/Human toggle button
        Button typeButton = {
            .rect = {SCREEN_WIDTH/2 - 20, 250 + i * 60, 80, 30},
            .text = state->players[i].isAI ? "IA" : "Humain",
            .baseColor = state->players[i].isAI ? CUSTOM_GREEN : CUSTOM_RED,
            .hoverColor = state->players[i].isAI ? (Color){0, 150, 0, 255} : (Color){150, 0, 0, 255},
            .textColor = CUSTOM_BEIGE,
            .isHovered = false
        };

        if (DrawButtonCustom(&typeButton)) {
            state->players[i].isAI = !state->players[i].isAI;
            if (state->players[i].isAI) {
                strcpy(state->players[i].name, "IA");
            } else {
                strcpy(state->players[i].name, TextFormat("Joueur %d", i+1));
            }
        }

        // Edit button for human players
        if (!state->players[i].isAI) {
            Button editButton = {
                .rect = {SCREEN_WIDTH/2 + 80, 250 + i * 60, 50, 30},
                .text = "EDIT",
                .baseColor = CUSTOM_BLUE,
                .hoverColor = CUSTOM_LIGHT_BLUE,
                .textColor = CUSTOM_BEIGE,
                .isHovered = false
            };

            if (DrawButtonCustom(&editButton)) {
                state->editingPlayer = i;
                strcpy(state->inputText, state->players[i].name);
            }
        }

        // Display player name
        DrawText(state->players[i].name, SCREEN_WIDTH/2 - 200, 280 + i * 60, 20, BLACK_TEXT);
    }

    // Start game button
    Button startButton = {
        .rect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT - 100, 200, 50},
        .text = "DÉMARRER",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&startButton) && state->playerCount > 0) {
        if (state->selectedGame == 0) {
            state->currentScreen = GAME_DOMINO;
        } else if (state->selectedGame == 1) {
            state->currentScreen = GAME_TRIOMINO;
        }
        state->isGameStarted = true;
    }

    // Back button
    Button backButton = {
        .rect = {50, SCREEN_HEIGHT - 70, 150, 50},
        .text = "RETOUR",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&backButton)) {
        state->currentScreen = GAME_SELECTION;
    }

    // Player name input field
    if (state->editingPlayer >= 0) {
        DrawText("Entrez le pseudo:", SCREEN_WIDTH/2 + 100, 250 + state->editingPlayer * 60, 20, BLACK_TEXT);
        DrawText(state->inputText, SCREEN_WIDTH/2 + 100, 280 + state->editingPlayer * 60, 20, BLACK_TEXT);

        // Blinking cursor
        static float cursorTimer = 0.0f;
        cursorTimer += GetFrameTime();
        if (cursorTimer >= 0.5f) cursorTimer = 0.0f;
        if ((int)(cursorTimer * 2) % 2) {
            int textWidth = MeasureText(state->inputText, 20);
            DrawLine(SCREEN_WIDTH/2 + 100 + textWidth, 280 + state->editingPlayer * 60,
                     SCREEN_WIDTH/2 + 100 + textWidth, 300 + state->editingPlayer * 60, BLACK_TEXT);
        }
    }
}

// ------------------------
// Function to draw the game screen (Domino and Triomino)
// ------------------------
void DrawGameScreen(GameState *state) {
    // Screen background
    ClearBackground(state->selectedGame == 0 ? CUSTOM_GREEN : CUSTOM_LIGHT_BLUE);
    DrawRectangle(0, 0, SCREEN_WIDTH, 50, CUSTOM_DARK_BLUE); // Top bar
    DrawRectangle(0, SCREEN_HEIGHT - 50, SCREEN_WIDTH, 50, CUSTOM_DARK_BLUE); // Bottom bar

    // Game title
    const char* gameTitle = (state->selectedGame == 0) ? "DOMINOS" : "TRIOMINOS";
    DrawText(gameTitle, SCREEN_WIDTH / 2 - MeasureText(gameTitle, 30)/2, 10, 30, CUSTOM_BEIGE);

    // Main play area
    Rectangle playArea = {50, 100, SCREEN_WIDTH - 100, SCREEN_HEIGHT - 200};
    DrawRectangleRec(playArea, state->selectedGame == 0 ? CUSTOM_GREEN : CUSTOM_LIGHT_BLUE);
    DrawRectangleLinesEx(playArea, 2, CUSTOM_BEIGE);

    // Player zones
    // Player 1 (top)
    Rectangle topPlayerArea = {SCREEN_WIDTH/2 - 100, 60, 200, 80};
    DrawRectangleRec(topPlayerArea, CUSTOM_BLUE);
    DrawRectangleLinesEx(topPlayerArea, 2, CUSTOM_BEIGE);
    DrawText(TextFormat("J1: %s", state->players[0].name), topPlayerArea.x + 10, topPlayerArea.y + 10, 20, CUSTOM_BEIGE);
    DrawText(TextFormat("Score: %d", state->players[0].score), topPlayerArea.x + 10, topPlayerArea.y + 40, 20, CUSTOM_BEIGE);

    // Player 2 (bottom)
    if (state->playerCount >= 2) {
        Rectangle bottomPlayerArea = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT - 140, 200, 80};
        DrawRectangleRec(bottomPlayerArea, CUSTOM_BLUE);
        DrawRectangleLinesEx(bottomPlayerArea, 2, CUSTOM_BEIGE);
        DrawText(TextFormat("J2: %s", state->players[1].name), bottomPlayerArea.x + 10, bottomPlayerArea.y + 10, 20, CUSTOM_BEIGE);
        DrawText(TextFormat("Score: %d", state->players[1].score), bottomPlayerArea.x + 10, bottomPlayerArea.y + 40, 20, CUSTOM_BEIGE);
    }

    // Additional players (left and right)
    if (state->playerCount >= 3) {
        Rectangle leftPlayerArea = {10, SCREEN_HEIGHT/2 - 100, 80, 200};
        DrawRectangleRec(leftPlayerArea, CUSTOM_BLUE);
        DrawRectangleLinesEx(leftPlayerArea, 2, CUSTOM_BEIGE);
        DrawText(TextFormat("J3: %s", state->players[2].name), leftPlayerArea.x + 10, leftPlayerArea.y + 10, 20, CUSTOM_BEIGE);
        DrawText(TextFormat("Score: %d", state->players[2].score), leftPlayerArea.x + 10, leftPlayerArea.y + 40, 20, CUSTOM_BEIGE);
    }

    if (state->playerCount == 4) {
        Rectangle rightPlayerArea = {SCREEN_WIDTH - 90, SCREEN_HEIGHT/2 - 100, 80, 200};
        DrawRectangleRec(rightPlayerArea, CUSTOM_BLUE);
        DrawRectangleLinesEx(rightPlayerArea, 2, CUSTOM_BEIGE);
        DrawText(TextFormat("J4: %s", state->players[3].name), rightPlayerArea.x + 10, rightPlayerArea.y + 10, 20, CUSTOM_BEIGE);
        DrawText(TextFormat("Score: %d", state->players[3].score), rightPlayerArea.x + 10, rightPlayerArea.y + 40, 20, CUSTOM_BEIGE);
    }

    // Control buttons
    Button playButton = {
        .rect = {SCREEN_WIDTH/2 - 160, SCREEN_HEIGHT - 45, 150, 40},
        .text = "JOUER",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    Button passButton = {
        .rect = {SCREEN_WIDTH/2 + 10, SCREEN_HEIGHT - 45, 150, 40},
        .text = "PASSER",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    // Menu button (top-left corner)
    Button menuButton = {
        .rect = {10, 5, 100, 40},
        .text = "MENU",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    // Draw and handle button clicks
    if (DrawButtonCustom(&playButton)) {
        // TODO: Logic to play a piece
        // For example, select a piece and place it on the board
    }

    if (DrawButtonCustom(&passButton)) {
        // TODO: Logic to pass the turn
        // For example, move to the next player
    }

    if (DrawButtonCustom(&menuButton)) {
        state->currentScreen = MENU;
    }

    // Display current turn
    char currentPlayerText[50];
    sprintf(currentPlayerText, "Tour de %s", state->players[state->currentPlayer].name);
    DrawText(currentPlayerText, SCREEN_WIDTH - 300, 15, 20, CUSTOM_BEIGE);

    // **Display dominos or triominos for each player**
    for (int p = 0; p < state->playerCount; p++) {
        if (state->selectedGame == 0) { // Dominos
            for (int d = 0; d < state->players[p].dominoCount; d++) {
                Domino currentDomino = state->players[p].playerDominos[d];
                // Position dominos based on player
                switch (p) {
                    case 0: // Player 1 (top)
                        currentDomino.position = (Vector2){
                            SCREEN_WIDTH / 2 - (state->players[p].dominoCount * (DOMINO_WIDTH + PANEL_PADDING)) / 2 + d * (DOMINO_WIDTH + PANEL_PADDING),
                            150
                        };
                        currentDomino.rotation = 0;
                        break;
                    case 1: // Player 2 (bottom)
                        currentDomino.position = (Vector2){
                            SCREEN_WIDTH / 2 - (state->players[p].dominoCount * (DOMINO_WIDTH + PANEL_PADDING)) / 2 + d * (DOMINO_WIDTH + PANEL_PADDING),
                            SCREEN_HEIGHT - 220
                        };
                        currentDomino.rotation = 0;
                        break;
                    case 2: // Player 3 (left)
                        currentDomino.position = (Vector2){
                            100,
                            SCREEN_HEIGHT / 2 - (state->players[p].dominoCount * (DOMINO_HEIGHT + PANEL_PADDING)) / 2 + d * (DOMINO_HEIGHT + PANEL_PADDING)
                        };
                        currentDomino.rotation = 0;
                        break;
                    case 3: // Player 4 (right)
                        currentDomino.position = (Vector2){
                            SCREEN_WIDTH - 160,
                            SCREEN_HEIGHT / 2 - (state->players[p].dominoCount * (DOMINO_HEIGHT + PANEL_PADDING)) / 2 + d * (DOMINO_HEIGHT + PANEL_PADDING)
                        };
                        currentDomino.rotation = 0;
                        break;
                }

                // Update position and rotation in game state
                state->players[p].playerDominos[d] = currentDomino;

                // Draw the domino
                DrawDomino(currentDomino);
            }
        }
        else if (state->selectedGame == 1) { // Triominos
            for (int t = 0; t < state->players[p].triominoCount; t++) {
                Triomino currentTriomino = state->players[p].playerTriominos[t];
                
                // Position triominos based on player
                switch (p) {
                    case 0: // Player 1 (top)
                        currentTriomino.position = (Vector2){
                            SCREEN_WIDTH / 2 - (state->players[p].triominoCount * (TRIOMINO_WIDTH + PANEL_PADDING)) / 2 + t * (TRIOMINO_WIDTH + PANEL_PADDING),
                            150
                        };
                        break;
                    case 1: // Player 2 (bottom)
                        currentTriomino.position = (Vector2){
                            SCREEN_WIDTH / 2 - (state->players[p].triominoCount * (TRIOMINO_WIDTH + PANEL_PADDING)) / 2 + t * (TRIOMINO_WIDTH + PANEL_PADDING),
                            SCREEN_HEIGHT - 220
                        };
                        break;
                    case 2: // Player 3 (left)
                        currentTriomino.position = (Vector2){
                            100,
                            SCREEN_HEIGHT / 2 - (state->players[p].triominoCount * (TRIOMINO_HEIGHT + PANEL_PADDING)) / 2 + t * (TRIOMINO_HEIGHT + PANEL_PADDING)
                        };
                        break;
                    case 3: // Player 4 (right)
                        currentTriomino.position = (Vector2){
                            SCREEN_WIDTH - 160,
                            SCREEN_HEIGHT / 2 - (state->players[p].triominoCount * (TRIOMINO_HEIGHT + PANEL_PADDING)) / 2 + t * (TRIOMINO_HEIGHT + PANEL_PADDING)
                        };
                        break;
                }

                // Update position and rotation in game state
                state->players[p].playerTriominos[t] = currentTriomino;

                // Update rotation if necessary
                UpdateTriominoRotation(&state->players[p].playerTriominos[t], GetFrameTime());

                // Draw the triomino
                DrawTriomino(state->players[p].playerTriominos[t]);
            }
        }
    }
}

// ------------------------
// Function to draw the scores screen
// ------------------------
void DrawScores(GameState *state) {
    // Screen background
    ClearBackground(RAYWHITE);

    // Scores title
    DrawText("SCORES", SCREEN_WIDTH/2 - MeasureText("SCORES", 30)/2, 50, 30, CUSTOM_BLUE);

    // Display player scores
    for (int i = 0; i < state->playerCount; i++) {
        DrawText(TextFormat("%s : %d points", state->players[i].name, state->players[i].score),
                 SCREEN_WIDTH/2 - 100, 150 + i * 40, 20, CUSTOM_BEIGE);
    }

    // Back button
    Button backButton = {
        .rect = {50, SCREEN_HEIGHT - 70, 150, 50},
        .text = "RETOUR",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&backButton)) {
        state->currentScreen = MENU;
    }
}

// ------------------------
// Function to handle general user inputs
// ------------------------
void HandleInput(GameState *state) {
    // Currently, inputs are handled within the draw functions
    // You can add additional interactions here if necessary
}

// ------------------------
// Function to handle player inputs and triomino interactions
// ------------------------
void HandlePlayerInput(GameState *state) {
    if (state->currentScreen == PLAYER_SETUP) {
        if (state->editingPlayer >= 0) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (strlen(state->inputText) < MAX_INPUT_LENGTH - 1)) {
                    strcat(state->inputText, (char[]){(char)key, '\0'});
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                int len = strlen(state->inputText);
                if (len > 0) state->inputText[len - 1] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER)) {
                if (strlen(state->inputText) > 0) {
                    strcpy(state->players[state->editingPlayer].name, state->inputText);
                    state->inputText[0] = '\0';
                    state->editingPlayer = -1;
                }
            }
        }
    }
    else if (state->currentScreen == GAME_DOMINO || state->currentScreen == GAME_TRIOMINO) {
        // Handle triomino interactions
        // Select a triomino
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePoint = GetMousePosition();
            state->selectedTriomino = NULL; // Reset selection
            for (int p = 0; p < state->playerCount; p++) {
                for (int t = 0; t < state->players[p].triominoCount; t++) {
                    Triomino *tri = &state->players[p].playerTriominos[t];
                    if (IsPointInTriomino(mousePoint, *tri)) {
                        state->selectedTriomino = tri;
                        break;
                    }
                }
                if (state->selectedTriomino != NULL) break;
            }
        }

        // Move the selected triomino
        if (state->selectedTriomino != NULL && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            state->selectedTriomino->position = GetMousePosition();
        }

        // Release the selected triomino
        if (state->selectedTriomino != NULL && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            // Check if placement is valid
            if (IsPlacementValid(state, state->selectedTriomino)) {
                // Valid placement, finalize position
                state->selectedTriomino->isRotating = false;
            }
            else {
                // Invalid placement, reset position
                // You can store the previous position before moving
                // For simplicity, we'll reset to (0,0)
                state->selectedTriomino->position = (Vector2){0, 0};
            }
            state->selectedTriomino = NULL;
        }

        // Rotate the selected triomino
        if (IsKeyPressed(KEY_R) && state->selectedTriomino != NULL) {
            state->selectedTriomino->isRotating = !state->selectedTriomino->isRotating;
        }
    }
}

// ------------------------
// Function to initialize game pieces (dominos or triominos)
// ------------------------
void InitGamePieces(GameState *state) {
    // This function initializes dominos or triominos for each player
    // For simplicity, we'll assign random values
    if (state->selectedGame == 0) { // Dominos
        for (int p = 0; p < state->playerCount; p++) {
            state->players[p].dominoCount = 7; // Example: 7 dominos per player
            for (int d = 0; d < state->players[p].dominoCount; d++) {
                state->players[p].playerDominos[d].top = rand() % 7;     // Value between 0 and 6
                state->players[p].playerDominos[d].bottom = rand() % 7;
                state->players[p].playerDominos[d].position = (Vector2){0, 0}; // Initial position
                state->players[p].playerDominos[d].rotation = 0.0f;          // Initial rotation
            }
        }
    }
    else if (state->selectedGame == 1) { // Triominos
        for (int p = 0; p < state->playerCount; p++) {
            state->players[p].triominoCount = 7; // Example: 7 triominos per player
            for (int t = 0; t < state->players[p].triominoCount; t++) {
                if (t >= state->players[p].triominoCapacity) {
                    state->players[p].triominoCapacity *= 2;
                    state->players[p].playerTriominos = realloc(state->players[p].playerTriominos, state->players[p].triominoCapacity * sizeof(Triomino));
                    if (!state->players[p].playerTriominos) {
                        printf("Memory reallocation error for triominos\n");
                        exit(-1);
                    }
                }
                InitTriomino(&state->players[p].playerTriominos[t]);
                state->players[p].playerTriominos[t].position = (Vector2){0, 0}; // Initial position
            }
        }
    }
}
