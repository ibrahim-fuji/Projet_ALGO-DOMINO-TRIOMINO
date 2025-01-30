// interface_graphique.c
#include "raylib.h"
#include "raymath.h"
#include "triomino.h"
#include "domino.h"
#include "do.h" 
#include "joueurs.h"
#include "game_state.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define CHEVALET_H_WIDTH 600
#define CHEVALET_H_HEIGHT 80
#define CHEVALET_V_WIDTH 100
#define CHEVALET_V_HEIGHT 500

static inline int max(int a, int b) {
    return (a > b) ? a : b;
}

void InitGameState(GameState *state);
void DrawMainMenu(GameState *state);
void DrawGameSelection(GameState *state, Texture2D dominoImage, Texture2D triominoImage);
void DrawPlayerSetup(GameState *state);
void DrawGameScreen(GameState *state);
void DrawScores(GameState *state);
void HandleInput(GameState *state);
void HandlePlayerInput(GameState *state);
void DrawDominoPlateau(GameState *state);
void DrawDominoInChevalet(GameState *state, int playerIndex, int dominoIndex);
void DrawTriominoInChevalet(GameState *state, int playerIndex, int triominoIndex);
void InitGamePieces(GameState *state);
void HandleTriominoTurn(GameState *state);
void UpdateGame(GameState *state);

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1024, 768, "Dominos & Triominos");
    SetTargetFPS(60);

    printf("Fenêtre initialisée\n");

    GameState gameState;
    InitGameState(&gameState);

    // Load textures
    Texture2D dominoImage = LoadTexture("resources/domino.png");
    Texture2D triominoImage = LoadTexture("resources/triomino.png");

    if (dominoImage.id == 0 || triominoImage.id == 0) {
        printf("Error loading textures\n");
        CloseWindow();
        return -1;
    }

    printf("Début de la boucle de jeu\n");

    while (!WindowShouldClose()) {
        gameState.screenWidth = GetScreenWidth();
        gameState.screenHeight = GetScreenHeight();
        gameState.deltaTime = GetFrameTime();

        HandleInput(&gameState);
        HandlePlayerInput(&gameState);
        UpdateGame(&gameState);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            
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
        }
        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (gameState.players[i].playerTriominos != NULL) {
            free(gameState.players[i].playerTriominos);
        }
    }
    UnloadTexture(dominoImage);
    UnloadTexture(triominoImage);
    CloseWindow();
    return 0;
}

void InitGameState(GameState *state) {
    state->currentScreen = MENU;
    state->playerCount = 0;
    state->isGameStarted = false;
    state->selectedGame = -1;
    state->editingPlayer = -1;
    state->currentPlayer = 0;
    state->inputText[0] = '\0';
    state->selectedTriomino = NULL;
    state->screenWidth = GetScreenWidth();
    state->screenHeight = GetScreenHeight();
    state->isFullscreen = false;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        state->players[i].name[0] = '\0';
        state->players[i].score = 0;
        state->players[i].isAI = false;
        state->players[i].dominoCount = 0;
        state->players[i].triominoCount = 0;
        state->players[i].triominoCapacity = 7;
        state->players[i].playerTriominos = malloc(state->players[i].triominoCapacity * sizeof(Triomino));
        if (!state->players[i].playerTriominos) {
            printf("Memory allocation error for triominos\n");
            exit(-1);
        }
    }
}

void DrawMainMenu(GameState *state) {
    ClearBackground(RAYWHITE);
    DrawText("DOMINOS & TRIOMINOS", 
             state->screenWidth/2 - MeasureText("DOMINOS & TRIOMINOS", 40)/2, 
             100, 40, CUSTOM_BLUE);

    Button playButton = {
        .rect = {state->screenWidth/2 - 100, 250, 200, 50},
        .text = "JOUER",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    Button scoresButton = {
        .rect = {state->screenWidth/2 - 100, 350, 200, 50},
        .text = "SCORES",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    Button exitButton = {
        .rect = {state->screenWidth/2 - 100, 450, 200, 50},
        .text = "QUITTER",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&playButton)) state->currentScreen = GAME_SELECTION;
    if (DrawButtonCustom(&scoresButton)) state->currentScreen = SCORES;
    if (DrawButtonCustom(&exitButton)) CloseWindow();
}

void DrawGameScreen(GameState *state) {
    ClearBackground(state->selectedGame == 0 ? CUSTOM_GREEN : CUSTOM_LIGHT_BLUE);
    DrawRectangle(0, 0, state->screenWidth, 50, CUSTOM_DARK_BLUE);
    DrawRectangle(0, state->screenHeight - 50, state->screenWidth, 50, CUSTOM_DARK_BLUE);

    Button menuButton = {
        .rect = {10, 10, 100, 30},
        .text = "MENU",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&menuButton)) {
        state->currentScreen = MENU;
    }

    const char* gameTitle = (state->selectedGame == 0) ? "DOMINOS" : "TRIOMINOS";
    DrawText(gameTitle, state->screenWidth/2 - MeasureText(gameTitle, 30)/2, 10, 30, CUSTOM_BEIGE);

    Rectangle playArea = {50, 100, state->screenWidth - 100, state->screenHeight - 200};
    DrawRectangleRec(playArea, state->selectedGame == 0 ? CUSTOM_GREEN : CUSTOM_LIGHT_BLUE);
    DrawRectangleLinesEx(playArea, 2, CUSTOM_BEIGE);

    for (int i = 0; i < state->playerCount; i++) {
        Color chevaletColor = (Color){50, 50, 50, 200};
        Rectangle chevaletBounds;
        Vector2 textPos;

        switch(i) {
            case 0:
                chevaletBounds = (Rectangle){
                    state->screenWidth/2 - CHEVALET_H_WIDTH/2,
                    60,
                    CHEVALET_H_WIDTH,
                    CHEVALET_H_HEIGHT
                };
                textPos = (Vector2){
                    chevaletBounds.x,
                    chevaletBounds.y - 25
                };
                DrawRectangle(textPos.x - 2, textPos.y - 2, 300, 24, (Color){0, 0, 0, 150});
                break;

            case 1:
                chevaletBounds = (Rectangle){
                    state->screenWidth/2 - CHEVALET_H_WIDTH/2,
                    state->screenHeight - CHEVALET_H_HEIGHT - 60,
                    CHEVALET_H_WIDTH,
                    CHEVALET_H_HEIGHT
                };
                textPos = (Vector2){
                    chevaletBounds.x,
                    chevaletBounds.y - 25
                };
                DrawRectangle(textPos.x - 2, textPos.y - 2, 300, 24, (Color){0, 0, 0, 150});
                break;

            case 2:
                chevaletBounds = (Rectangle){
                    20,
                    state->screenHeight/2 - CHEVALET_V_HEIGHT/2,
                    CHEVALET_V_WIDTH,
                    CHEVALET_V_HEIGHT
                };
                textPos = (Vector2){
                    chevaletBounds.x,
                    chevaletBounds.y - 30
                };
                DrawRectangle(textPos.x - 2, textPos.y - 2, 300, 24, (Color){0, 0, 0, 150});
                break;

            case 3:
                chevaletBounds = (Rectangle){
                    state->screenWidth - CHEVALET_V_WIDTH - 20,
                    state->screenHeight/2 - CHEVALET_V_HEIGHT/2,
                    CHEVALET_V_WIDTH,
                    CHEVALET_V_HEIGHT
                };
                textPos = (Vector2){
                    state->screenWidth - 320,
                    chevaletBounds.y - 30
                };
                DrawRectangle(textPos.x - 2, textPos.y - 2, 300, 24, (Color){0, 0, 0, 150});
                break;
        }

        state->players[i].chevalet.bounds = chevaletBounds;
        state->players[i].chevalet.color = chevaletColor;

        DrawRectangleRec(chevaletBounds, chevaletColor);
        DrawRectangleLinesEx(chevaletBounds, 2, CUSTOM_BEIGE);

        char playerInfo[100];
        sprintf(playerInfo, "J%d: %s - Score: %d", i+1, state->players[i].name, state->players[i].score);
        DrawText(playerInfo, textPos.x, textPos.y, 20, CUSTOM_BEIGE);

        if (state->selectedGame == 0) {
            for (int d = 0; d < state->players[i].dominoCount; d++) {
                DrawDominoInChevalet(state, i, d);
            }
        } else {
            for (int t = 0; t < state->players[i].triominoCount; t++) {
                DrawTriominoInChevalet(state, i, t);
            }
        }
    }
    
    if (state->selectedGame == 0) {
        DrawDominoPlateau(state);
    } else {
        DrawTriominoPlateau(state);
    }

    Button playButtonGauche = {
        .rect = {state->screenWidth/2 - 350, state->screenHeight - 45, 150, 40},
        .text = "GAUCHE",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };
    
    Button playButtonDroite = {
        .rect = {state->screenWidth/2 - 180, state->screenHeight - 45, 150, 40},
        .text = "DROITE",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    Button tournerButton = {
        .rect = {state->screenWidth/2 + 200, state->screenHeight - 45, 150, 40},
        .text = "TOURNER",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    Button passButton = {
        .rect = {state->screenWidth/2 + 10, state->screenHeight - 45, 150, 40},
        .text = "PASSER",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    DrawButtonCustom(&playButtonGauche);
    DrawButtonCustom(&playButtonDroite);
    DrawButtonCustom(&tournerButton);
    DrawButtonCustom(&passButton);

    DrawText(TextFormat("Tour de %s", state->players[state->currentPlayer].name),
             state->screenWidth - 200, 15, 20, CUSTOM_BEIGE);

    if (state->selectedGame == 1) {  // Pour le jeu de Triominos
        // Dessiner le plateau de triominos
        for (int i = 0; i < state->board.count; i++) {
            DrawTriomino(state->board.pieces[i]);
        }

        // Dessiner les boutons spécifiques aux triominos
        Button rotateButton = {
            .rect = {state->screenWidth/2 + 170, state->screenHeight - 45, 150, 40},
            .text = "TOURNER",
            .baseColor = CUSTOM_BLUE,
            .hoverColor = CUSTOM_LIGHT_BLUE,
            .textColor = CUSTOM_BEIGE,
            .isHovered = false
        };

        Button piocherButton = {
            .rect = {state->screenWidth/2 + 330, state->screenHeight - 45, 150, 40},
            .text = "PIOCHER",
            .baseColor = CUSTOM_GREEN,
            .hoverColor = CUSTOM_LIGHT_BLUE,
            .textColor = CUSTOM_BEIGE,
            .isHovered = false
        };

        // Désactiver le bouton de pioche si plus de 3 pioches consécutives
        if (state->consecutivePasses >= 3) {
            piocherButton.baseColor = LIGHTGRAY;
            piocherButton.hoverColor = LIGHTGRAY;
        }

        DrawButtonCustom(&rotateButton);
        DrawButtonCustom(&piocherButton);
    }

    // Ajouter le bouton de pioche pour les dominos
    if (state->selectedGame == 0) { // Pour le jeu de dominos
        Button piocherButton = {
            .rect = {state->screenWidth/2 + 330, state->screenHeight - 45, 150, 40},
            .text = "PIOCHER",
            .baseColor = CUSTOM_GREEN,
            .hoverColor = CUSTOM_LIGHT_BLUE,
            .textColor = CUSTOM_BEIGE,
            .isHovered = false
        };
        DrawButtonCustom(&piocherButton);
    }
}

void DrawDominoPlateau(GameState *state) {
    Rectangle playArea = {50, 100, state->screenWidth - 100, state->screenHeight - 200};
    Vector2 center = {playArea.x + playArea.width/2, playArea.y + playArea.height/2};
    
    if (!state->plateau) return;

    Plateau* current = state->plateau;
    float currentX = center.x;
    float currentY = center.y;
    float spacing = DOMINO_WIDTH + 5;

    while (current) {
        current->domino.position = (Vector2){currentX, currentY};
        DrawDomino(current->domino);
        currentX += spacing;
        current = current->suivant;
    }
}

void DrawTriominoPlateau(GameState *state) {
    if (state->board.count == 0) return;

    // Mettre à jour les positions du plateau
    UpdateTriominoPositions(state);
    
    // Dessiner tous les triominos placés
    for (int i = 0; i < state->board.count; i++) {
        DrawTriomino(state->board.pieces[i]);
    }
}

void DrawDominoInChevalet(GameState *state, int playerIndex, int dominoIndex) {
    Player *player = &state->players[playerIndex];
    Domino *domino = &player->playerDominos[dominoIndex];
    float spacing = 5.0f;
    float dominoWidth = 40.0f;    // Largeur d'un domino
    float dominoHeight = 80.0f;   // Hauteur d'un domino
    
    if (playerIndex == 2 || playerIndex == 3) {
        // Joueurs sur les côtés (dominos horizontaux)
        float startY = player->chevalet.bounds.y + 10;
        domino->position = (Vector2){
            player->chevalet.bounds.x + (player->chevalet.bounds.width - dominoWidth)/2,
            startY + (dominoIndex * (dominoHeight + spacing))
        };
        domino->rotation = 0.0f;
    } else {
        // Joueurs en haut et en bas (dominos verticaux)
        float startX = player->chevalet.bounds.x + 10;
        domino->position = (Vector2){
            startX + (dominoIndex * (dominoWidth + spacing + 20)),
            player->chevalet.bounds.y + (player->chevalet.bounds.height - dominoHeight)/2
        };
        domino->rotation = 90.0f;
    }
    
    DrawDomino(*domino);
}

void DrawTriominoInChevalet(GameState *state, int playerIndex, int triominoIndex) {
    Player *player = &state->players[playerIndex];
    Triomino *triomino = &player->playerTriominos[triominoIndex];
    float spacing = 5.0f;
    float triominoWidth = 60.0f;  // Largeur d'un triomino
    float triominoHeight = 52.0f; // Hauteur d'un triomino
    
    if (playerIndex == 2 || playerIndex == 3) {
        // Joueurs sur les côtés (triominos verticaux)
        float startY = player->chevalet.bounds.y + 10;
        triomino->position = (Vector2){
            player->chevalet.bounds.x + (player->chevalet.bounds.width - triominoWidth)/2,
            startY + (triominoIndex * (triominoHeight + spacing))
        };
        triomino->rotation = 0.0f; // Rotation pour orientation verticale
    } else {
        // Joueurs en haut et en bas (triominos verticaux)
        float startX = player->chevalet.bounds.x + 10;
        triomino->position = (Vector2){
            startX + (triominoIndex * (triominoWidth + spacing)),
            player->chevalet.bounds.y + (player->chevalet.bounds.height - triominoHeight)/2
        };
        triomino->rotation = 0.0f; // Rotation pour orientation verticale
    }
    
    DrawTriomino(*triomino);
}

void DrawGameSelection(GameState *state, Texture2D dominoImage, Texture2D triominoImage) {
    ClearBackground(RAYWHITE);
    DrawText("CHOISISSEZ VOTRE JEU", 
             state->screenWidth/2 - MeasureText("CHOISISSEZ VOTRE JEU", 30)/2, 
             50, 30, CUSTOM_BLUE);

    Button backButton = {
        .rect = {10, 10, 100, 30},
        .text = "RETOUR",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&backButton)) state->currentScreen = MENU;

    Rectangle dominoRect = {state->screenWidth/4 - 125, 150, 250, 250};
    Rectangle triominoRect = {3 * state->screenWidth/4 - 125, 150, 250, 250};

    DrawRectangleRec(dominoRect, CUSTOM_BLUE);
    DrawRectangleRec(triominoRect, CUSTOM_BLUE);

    DrawTexturePro(dominoImage, 
                   (Rectangle){0, 0, (float)dominoImage.width, (float)dominoImage.height},
                   (Rectangle){dominoRect.x + 25, dominoRect.y + 25, 200, 200},
                   (Vector2){0, 0}, 0, WHITE);
    
    DrawTexturePro(triominoImage,
                   (Rectangle){0, 0, (float)triominoImage.width, (float)triominoImage.height},
                   (Rectangle){triominoRect.x + 25, triominoRect.y + 25, 200, 200},
                   (Vector2){0, 0}, 0, WHITE);

    DrawText("DOMINOS", 
             dominoRect.x + 125 - MeasureText("DOMINOS", 20)/2,
             dominoRect.y + 270, 20, CUSTOM_BEIGE);
    DrawText("TRIOMINOS",
             triominoRect.x + 125 - MeasureText("TRIOMINOS", 20)/2,
             triominoRect.y + 270, 20, CUSTOM_BEIGE);

    Vector2 mousePoint = GetMousePosition();
    if (CheckCollisionPointRec(mousePoint, dominoRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state->selectedGame = 0;
        state->currentScreen = PLAYER_SETUP;
        
    }
    if (CheckCollisionPointRec(mousePoint, triominoRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        state->selectedGame = 1;
        state->currentScreen = PLAYER_SETUP;
        InitGamePieces(state);
    }
}
void HandlePlayerInput(GameState *state) {
    // Ignorer les entrées pendant le tour de l'IA
    if (state->currentScreen == GAME_DOMINO && state->players[state->currentPlayer].isAI) {
        return;
    }
    
    if (state->currentScreen == PLAYER_SETUP && state->editingPlayer >= 0) {
        // Gestion de l'édition des noms des joueurs
        int key = GetCharPressed();
        
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (strlen(state->inputText) < MAX_INPUT_LENGTH - 1)) {
                int len = strlen(state->inputText);
                state->inputText[len] = (char)key;
                state->inputText[len + 1] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            int len = strlen(state->inputText);
            if (len > 0) {
                state->inputText[len - 1] = '\0';
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            if (strlen(state->inputText) > 0) {
                strcpy(state->players[state->editingPlayer].name, state->inputText);
                state->inputText[0] = '\0';
                state->editingPlayer = -1;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            state->inputText[0] = '\0';
            state->editingPlayer = -1;
        }
    }
    else if (state->currentScreen == GAME_DOMINO || state->currentScreen == GAME_TRIOMINO) {
        Vector2 mousePoint = GetMousePosition();

        // Gestion des pièces du joueur actuel
        if (state->currentPlayer >= 0 && state->currentPlayer < state->playerCount) {
            Player *currentPlayer = &state->players[state->currentPlayer];

            // Vérifier si le clic est dans le chevalet du joueur actuel
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
                CheckCollisionPointRec(mousePoint, currentPlayer->chevalet.bounds)) {
                
                if (state->selectedGame == 0) {  // Dominos
                    for (int d = 0; d < currentPlayer->dominoCount; d++) {
                        Domino *domino = &currentPlayer->playerDominos[d];
                        Rectangle dominoRect = {
                            domino->position.x,
                            domino->position.y,
                            40.0f,   // Hauteur du domino (inversée car vertical)
                            80.0f    // Largeur du domino (inversée car vertical)
                        };
                        
                        if (CheckCollisionPointRec(mousePoint, dominoRect)) {
                            // Sélection du domino si aucun domino n'est déjà sélectionné
                            if (state->selectedDomino == NULL) {
                                state->selectedDomino = domino;
                                printf("Domino sélectionné : [%d | %d]\n", domino->v_gauche, domino->v_droite);
                            } else {
                                // Désélectionner le domino si on clique à nouveau dessus
                                if (state->selectedDomino == domino) {
                                    state->selectedDomino = NULL;
                                    printf("Domino désélectionné\n");
                                }
                            }
                            break;  // Sortir de la boucle après avoir sélectionné un domino
                        }
                    }
                } else {  // Triominos
                    for (int t = 0; t < currentPlayer->triominoCount; t++) {
                        Triomino *triomino = &currentPlayer->playerTriominos[t];
                        if (IsPointInTriomino(mousePoint, *triomino)) {
                            // Sélection du triomino
                            state->selectedTriomino = triomino;
                            printf("Triomino sélectionné !\n");
                            break;
                        }
                    }
                }
            }
        }

        // Gestion des boutons de jeu
        Rectangle playButtonGaucheRect = {state->screenWidth / 2 - 350, state->screenHeight - 45, 150, 40};
        Rectangle playButtonDroiteRect = {state->screenWidth / 2 - 180, state->screenHeight - 45, 150, 40};
        Rectangle tournerButtonRect = {state->screenWidth/2 + 200, state->screenHeight - 45, 150, 40};

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && state->selectedDomino != NULL) {
            if (CheckCollisionPointRec(mousePoint, tournerButtonRect)) {
                printf("Rotation domino: %d|%d -> ", state->selectedDomino->v_droite, state->selectedDomino->v_gauche);
                *state->selectedDomino = tournerDomino(*state->selectedDomino);
                printf("%d|%d\n", state->selectedDomino->v_droite, state->selectedDomino->v_gauche);
            }
            else if (CheckCollisionPointRec(mousePoint, playButtonGaucheRect)) {
                if (verificationPlacement(*state->selectedDomino, state->plateau, true)) {
                    placerDomino_Gauche(*state->selectedDomino, &state->plateau);
                    supprimerDominoJoueur(&state->players[state->currentPlayer], *state->selectedDomino);
                    state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
                } else {
                    printf("Placement invalide à gauche !\n");
                }
                state->selectedDomino = NULL;
            }
            else if (CheckCollisionPointRec(mousePoint, playButtonDroiteRect)) {
                if (verificationPlacement(*state->selectedDomino, state->plateau, false)) {
                    placerDomino_Droite(*state->selectedDomino, &state->plateau);
                    supprimerDominoJoueur(&state->players[state->currentPlayer], *state->selectedDomino);
                    state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
                } else {
                    printf("Placement invalide à droite !\n");
                }
                state->selectedDomino = NULL;
            }
        }

        if (state->currentScreen == GAME_TRIOMINO) {
            Vector2 mousePoint = GetMousePosition();
            
            // Gestion de la rotation avec la touche R
            HandleTriominoRotation(state);

            // Gestion du clic sur le plateau pour placer un triomino
            if (state->selectedTriomino && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle playArea = {50, 100, state->screenWidth - 100, state->screenHeight - 200};
                if (CheckCollisionPointRec(mousePoint, playArea)) {
                    if (VerifierPlacementTriomino(state, state->selectedTriomino, mousePoint)) {
                        PlacerTriomino(state, state->selectedTriomino, mousePoint);
                        
                        // Retirer le triomino de la main du joueur
                        Player *currentPlayer = &state->players[state->currentPlayer];
                        for (int i = 0; i < currentPlayer->triominoCount; i++) {
                            if (&currentPlayer->playerTriominos[i] == state->selectedTriomino) {
                                // Décaler les triominos restants
                                for (int j = i; j < currentPlayer->triominoCount - 1; j++) {
                                    currentPlayer->playerTriominos[j] = currentPlayer->playerTriominos[j + 1];
                                }
                                currentPlayer->triominoCount--;
                                break;
                            }
                        }
                        
                        state->selectedTriomino = NULL;
                        state->consecutivePasses = 0;
                        state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
                    }
                }
            }

            // Gestion du bouton de pioche
            Rectangle piocherButtonRect = {state->screenWidth/2 + 330, state->screenHeight - 45, 150, 40};
            if (CheckCollisionPointRec(mousePoint, piocherButtonRect) && 
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && 
                state->consecutivePasses < 3) {
                if (TryPiocherTriomino(state, state->currentPlayer)) {
                    state->consecutivePasses++;
                    state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
                }
            }

            // Vérification de fin de partie
            bool gameOver = false;
            for (int i = 0; i < state->playerCount; i++) {
                if (state->players[i].triominoCount == 0) {
                    gameOver = true;
                    break;
                }
            }
            if (gameOver || state->consecutivePasses >= state->playerCount * 3) {
                CalculateFinalScores(state);
                state->currentScreen = SCORES;
            }
        }

        if (state->currentScreen == GAME_DOMINO) {
            // Ajouter la gestion du bouton piocher
            Rectangle piocherButtonRect = {state->screenWidth/2 + 330, state->screenHeight - 45, 150, 40};
            if (CheckCollisionPointRec(mousePoint, piocherButtonRect) && 
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                
                // Vérifier si le joueur peut jouer
                bool peutJouer = false;
                Player* currentPlayer = &state->players[state->currentPlayer];
                
                for (int i = 0; i < currentPlayer->dominoCount; i++) {
                    if (verificationPlacement(currentPlayer->playerDominos[i], state->plateau, true) ||
                        verificationPlacement(currentPlayer->playerDominos[i], state->plateau, false)) {
                        peutJouer = true;
                        break;
                    }
                }
                
                // Si le joueur ne peut pas jouer, il peut piocher
                if (!peutJouer) {
                    if (essayerPiocherDomino(state, state->currentPlayer)) {
                        printf("Domino pioché avec succès\n");
                    } else {
                        printf("Plus de dominos dans la pioche\n");
                        state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
                    }
                }
            }
        }
    }
    else if (state->currentScreen == GAME_DOMINO) {
        // Gérer le tour de l'IA
        if (state->players[state->currentPlayer].isAI) {
            if (jouerIA_Domino(state)) {
                // L'IA a joué avec succès
                state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
            } else {
                // L'IA n'a pas pu jouer ou a pioché
                state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
            }
            verifierFinPartie(state);
            return;
        }

        // ...rest of the existing game logic...
        
        // Ajouter la vérification de fin de partie après chaque coup
        verifierFinPartie(state);
    }
}

void DrawPlayerSetup(GameState *state) {
    ClearBackground(RAYWHITE);
    DrawText("CONFIGURATION DES JOUEURS", 
             state->screenWidth/2 - MeasureText("CONFIGURATION DES JOUEURS", 30)/2, 
             50, 30, CUSTOM_BLUE);

    Button backButton = {
        .rect = {10, 10, 100, 30},
        .text = "RETOUR",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&backButton)) {
        state->currentScreen = GAME_SELECTION;
        state->editingPlayer = -1;
        state->inputText[0] = '\0';
    }

    DrawText("Nombre de joueurs:", state->screenWidth/2 - 150, 150, 20, BLACK_TEXT);

    for (int i = 2; i <= 4; i++) {
        Button countButton = {
            .rect = {state->screenWidth/2 - 150 + (i-2) * 100, 180, 80, 50},
            .text = TextFormat("%d", i),
            .baseColor = (state->playerCount == i) ? CUSTOM_LIGHT_BLUE : CUSTOM_BLUE,
            .hoverColor = CUSTOM_LIGHT_BLUE,
            .textColor = CUSTOM_BEIGE,
            .isHovered = false
        };

        if (DrawButtonCustom(&countButton)) {
            state->playerCount = i;
            for (int j = 0; j < state->playerCount; j++) {
                if (state->players[j].name[0] == '\0') {
                    sprintf(state->players[j].name, "Joueur %d", j+1);
                }
            }
        }
    }

    for (int i = 0; i < state->playerCount; i++) {
        DrawText(TextFormat("Joueur %d:", i+1),
                state->screenWidth/2 - 200,
                250 + i * 60,
                20,
                CUSTOM_BLUE);

        Rectangle nameRect = {
            state->screenWidth/2 - 190,
            270 + i * 60,
            300,
            30
        };

        if (state->editingPlayer == i) {
            DrawRectangleRec(nameRect, LIGHTGRAY);
            DrawText(state->inputText,
                    nameRect.x + 5,
                    nameRect.y + 5,
                    20,
                    BLACK_TEXT);

            if (((int)(GetTime() * 2) % 2) == 0) {
                DrawText("_",
                        nameRect.x + 5 + MeasureText(state->inputText, 20),
                        nameRect.y + 5,
                        20,
                        BLACK_TEXT);
            }
        } else {
            DrawText(state->players[i].name,
                    nameRect.x + 5,
                    nameRect.y + 5,
                    20,
                    BLACK_TEXT);
        }

        Button editButton = {
            .rect = {state->screenWidth/2 + 120, 270 + i * 60, 50, 30},
            .text = "EDIT",
            .baseColor = CUSTOM_BLUE,
            .hoverColor = CUSTOM_LIGHT_BLUE,
            .textColor = CUSTOM_BEIGE,
            .isHovered = false
        };

        Button typeButton = {
            .rect = {state->screenWidth/2 + 180, 270 + i * 60, 80, 30},
            .text = state->players[i].isAI ? "IA" : "Humain",
            .baseColor = state->players[i].isAI ? CUSTOM_GREEN : CUSTOM_RED,
            .hoverColor = state->players[i].isAI ? (Color){0, 150, 0, 255} : (Color){150, 0, 0, 255},
            .textColor = CUSTOM_BEIGE,
            .isHovered = false
        };

        if (!state->players[i].isAI && state->editingPlayer != i && DrawButtonCustom(&editButton)) {
            state->editingPlayer = i;
            strcpy(state->inputText, state->players[i].name);
        }

        if (DrawButtonCustom(&typeButton)) {
            state->players[i].isAI = !state->players[i].isAI;
            if (state->players[i].isAI) {
                sprintf(state->players[i].name, "IA %d", i + 1);
                if (state->editingPlayer == i) {
                    state->editingPlayer = -1;
                    state->inputText[0] = '\0';
                }
            } else {
                sprintf(state->players[i].name, "Joueur %d", i+1);
            }
        }
    }

    Button startButton = {
        .rect = {state->screenWidth/2 - 100, state->screenHeight - 100, 200, 50},
        .text = "DÉMARRER",
        .baseColor = state->playerCount >= 2 ? CUSTOM_GREEN : LIGHTGRAY,
        .hoverColor = state->playerCount >= 2 ? CUSTOM_LIGHT_BLUE : LIGHTGRAY,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (state->playerCount >= 2 && DrawButtonCustom(&startButton)) {
        state->currentScreen = state->selectedGame == 0 ? GAME_DOMINO : GAME_TRIOMINO;
        state->isGameStarted = true;
        state->editingPlayer = -1;
        state->inputText[0] = '\0';
        InitGamePieces(state);
    }
}

void DrawScores(GameState *state) {
    ClearBackground(RAYWHITE);
    DrawText("SCORES", 
             state->screenWidth/2 - MeasureText("SCORES", 30)/2, 
             50, 30, CUSTOM_BLUE);

    for (int i = 0; i < state->playerCount; i++) {
        Rectangle scoreZone = {
            state->screenWidth/2 - 200,
            150 + i * 80,
            400,
            60
        };
        DrawRectangleRec(scoreZone, CUSTOM_GREEN);
        DrawRectangleLinesEx(scoreZone, 2, CUSTOM_BEIGE);

        char scoreText[50];
        sprintf(scoreText, "%s: %d points", state->players[i].name, state->players[i].score);
        DrawText(scoreText,
                scoreZone.x + 20,
                scoreZone.y + 20,
                20, CUSTOM_BEIGE);
    }

    Button backButton = {
        .rect = {state->screenWidth/2 - 100, state->screenHeight - 100, 200, 50},
        .text = "RETOUR",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    if (DrawButtonCustom(&backButton)) state->currentScreen = MENU;
}

void HandleInput(GameState *state) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        if (state->editingPlayer >= 0) {
            state->editingPlayer = -1;
            state->inputText[0] = '\0';
        }
        else if (state->currentScreen == GAME_DOMINO || state->currentScreen == GAME_TRIOMINO) {
            state->currentScreen = MENU;
        }
    }
}

void InitGamePieces(GameState *state) {
    if (state->selectedGame == 0) {
        // Initialisation des dominos
        state->piocheDominos = initialiserDomino();
        melange_pioche(&state->piocheDominos);
        
        // Distribution des dominos aux joueurs
        DominoNode* joueurs[MAX_PLAYERS] = {NULL};
        distribuer_piece(&state->piocheDominos, joueurs, state->playerCount);
        
        // Conversion des dominos distribués en format de jeu
        for (int p = 0; p < state->playerCount; p++) {
            DominoNode* current = joueurs[p];
            int d = 0;
            while (current && d < 7) {
                state->players[p].playerDominos[d] = current->domino_current;
                state->players[p].playerDominos[d].position = (Vector2){0, 0};
                state->players[p].playerDominos[d].rotation = 90.0f;
                current = current->domino_suivant;
                d++;
            }
            state->players[p].dominoCount = d;
            lib_chaine(joueurs[p]);
        }
        // Ne pas libérer la pioche car elle sera utilisée plus tard
        state->plateau = NULL;
    } else {
        // Initialisation des triominos
        InitTriominoPioche(&state->pioche);
        MelangerPioche(&state->pioche);
        
        // Distribution des triominos
        int piecesPerPlayer = (state->playerCount == 2) ? 9 : 7;
        for (int p = 0; p < state->playerCount; p++) {
            state->players[p].triominoCount = piecesPerPlayer;
            for (int t = 0; t < piecesPerPlayer; t++) {
                state->players[p].playerTriominos[t] = PiocherTriomino(&state->pioche);
            }
        }
        
        // Initialisation du plateau
        state->board.pieces = NULL;
        state->board.count = 0;
        state->board.positions = NULL;
        state->currentTurn = 0;
        state->consecutivePasses = 0;
        state->gameOver = false;
    }
}

void HandleTriominoTurn(GameState *state) {
    if (state->players[state->currentPlayer].isAI) {
        if (JouerIA(state, state->currentPlayer)) {
            state->currentTurn++;
            state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
            state->consecutivePasses = 0;
        } else {
            state->consecutivePasses++;
        }
    } else {
        // Gestion du tour du joueur humain
        // ...existing code...
    }

    // Vérification de fin de partie
    if (state->consecutivePasses >= state->playerCount) {
        state->gameOver = true;
        CalculateFinalScores(state);
    }
}

void UpdateGame(GameState *state) {
    if (state->currentScreen == GAME_DOMINO && state->players[state->currentPlayer].isAI) {
        static float aiTimer = 0;
        aiTimer += GetFrameTime();
        
        if (aiTimer >= 1.0f) { // Délai d'une seconde
            printf("Tour de l'IA: %s\n", state->players[state->currentPlayer].name);
            bool aJoue = jouerIA_Domino(state);
            
            if (aJoue) {
                printf("L'IA a joué avec succès\n");
                state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
            } else {
                // Si l'IA n'a pas pu jouer, elle essaie de piocher
                if (essayerPiocherDomino(state, state->currentPlayer)) {
                    printf("L'IA a pioché un domino\n");
                } else {
                    printf("L'IA passe son tour\n");
                    state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
                }
            }
            
            verifierFinPartie(state);
            aiTimer = 0;
        }
    }
    
    // ...rest of UpdateGame code...
}
