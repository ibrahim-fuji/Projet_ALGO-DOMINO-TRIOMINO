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
void DrawDominoInChevalet(GameState *state, int playerIndex, int dominoIndex);
void DrawTriominoInChevalet(GameState *state, int playerIndex, int triominoIndex);
void InitGamePieces(GameState *state);

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1024, 768, "Dominos & Triominos");
    SetTargetFPS(60);

    srand(time(NULL));

    GameState gameState;
    InitGameState(&gameState);

    Texture2D dominoImage = LoadTexture("resources/domino.png");
    Texture2D triominoImage = LoadTexture("resources/triomino.png");

    if (dominoImage.id == 0 || triominoImage.id == 0) {
        printf("Error loading textures\n");
        CloseWindow();
        return -1;
    }

    while (!WindowShouldClose()) {
        gameState.screenWidth = GetScreenWidth();
        gameState.screenHeight = GetScreenHeight();

        if (IsKeyPressed(KEY_F11)) {
            if (!gameState.isFullscreen) {
                SetWindowState(FLAG_FULLSCREEN_MODE);
                gameState.isFullscreen = true;
            } else {
                ClearWindowState(FLAG_FULLSCREEN_MODE);
                gameState.isFullscreen = false;
            }
        }

        HandleInput(&gameState);
        HandlePlayerInput(&gameState);

        BeginDrawing();
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
        EndDrawing();
    }

    for (int i = 0; i < MAX_PLAYERS; i++) {
        free(gameState.players[i].playerTriominos);
    }
    UnloadTexture(dominoImage);
    UnloadTexture(triominoImage);
    CloseWindow();
    return 0;
}

void InitGameState(GameState *state) {
    state->currentScreen = MENU;
    state->pioche = NULL;
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

//		Gestion des pièces du joueur actuel
        if (state->currentPlayer >= 0 && state->currentPlayer <= state->playerCount) {
            Player *currentPlayer = &state->players[state->currentPlayer - 1];

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
                            // Logique de sélection du domino
                            // À implémenter selon les besoins
                            //supprimerDomino(&state->players[state->currentPlayer].playerDominos, d, &state->players->dominoCount);
                            printf("le domino %d est supprime\n", domino->v_droite);
                    	}
                    }
                } else {  // Triominos
                    for (int t = 0; t < currentPlayer->triominoCount; t++) {
                        Triomino *triomino = &currentPlayer->playerTriominos[t];
                        if (IsPointInTriomino(mousePoint, *triomino)) {
                            // Logique de sélection du triomino
                            // À implémenter selon les besoins
                            break;
                        }
                    }
                }
            }
        }

        // Gestion des boutons de jeu
        Rectangle playButtonRect = {state->screenWidth/2 - 160, state->screenHeight - 45, 150, 40};
        Rectangle passButtonRect = {state->screenWidth/2 + 10, state->screenHeight - 45, 150, 40};

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            if (CheckCollisionPointRec(mousePoint, playButtonRect)) {
                // Logique pour jouer une pièce et passer au joueur suivant
                // À implémenter selon les besoins
            }
            else if (CheckCollisionPointRec(mousePoint, passButtonRect)) {
                // Passer au joueur suivant
            }
        }
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

    Button playButton = {
        .rect = {state->screenWidth/2 - 160, state->screenHeight - 45, 150, 40},
        .text = "JOUER",
        .baseColor = CUSTOM_GREEN,
        .hoverColor = CUSTOM_LIGHT_BLUE,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    Button piocherButton = {
        .rect = {state->screenWidth/2 + 10, state->screenHeight - 45, 150, 40},
        .text = "PIOCHER",
        .baseColor = CUSTOM_RED,
        .hoverColor = DARKRED,
        .textColor = CUSTOM_BEIGE,
        .isHovered = false
    };

    DrawButtonCustom(&playButton);
    if (DrawButtonCustom(&piocherButton)){
    	if (state->pioche != NULL){
	    	switch (state->currentPlayer) {
	    		case 1:    			
					state->players[0].playerDominos[state->players[0].dominoCount].v_gauche = state->pioche->domino_current.v_gauche;
					state->players[0].playerDominos[state->players[0].dominoCount].v_droite = state->pioche->domino_current.v_droite;
					state->players[0].playerDominos[state->players[0].dominoCount].position = (Vector2){0, 0};
	                state->players[0].playerDominos[state->players[0].dominoCount].rotation = 90.0f;
	                state->pioche = state->pioche->domino_suivant;
	                DrawDominoInChevalet(state, 0, state->players[0].dominoCount-1);
	                state->players[0].dominoCount++;
	                state->currentPlayer = 2;
	                break;
	            case 2:
					state->players[1].playerDominos[state->players[1].dominoCount].v_gauche = state->pioche->domino_current.v_gauche;
					state->players[1].playerDominos[state->players[1].dominoCount].v_droite = state->pioche->domino_current.v_droite;
					state->players[1].playerDominos[state->players[1].dominoCount].position = (Vector2){0, 0};
	                state->players[1].playerDominos[state->players[1].dominoCount].rotation = 90.0f;
	                state->pioche = state->pioche->domino_suivant;
	                DrawDominoInChevalet(state, 1, state->players[1].dominoCount-1);
	                state->players[1].dominoCount++;
		           	state->currentPlayer = (state->playerCount == 2) ?  1 : 3;
	            	break;
				case 3:
					state->players[2].playerDominos[state->players[2].dominoCount].v_gauche = state->pioche->domino_current.v_gauche;
					state->players[2].playerDominos[state->players[2].dominoCount].v_droite = state->pioche->domino_current.v_droite;
					state->players[2].playerDominos[state->players[2].dominoCount].position = (Vector2){0, 0};
	                state->players[2].playerDominos[state->players[2].dominoCount].rotation = 90.0f;
	                state->pioche = state->pioche->domino_suivant;
	                DrawDominoInChevalet(state, 2, state->players[2].dominoCount-1);
	                state->players[2].dominoCount++;
					state->currentPlayer = (state->playerCount == 3) ?  1 : 4;
					break;	
				case 4:
					state->players[3].playerDominos[state->players[3].dominoCount].v_gauche = state->pioche->domino_current.v_gauche;
					state->players[3].playerDominos[state->players[3].dominoCount].v_droite = state->pioche->domino_current.v_droite;
					state->players[3].playerDominos[state->players[3].dominoCount].position = (Vector2){0, 0};
	                state->players[3].playerDominos[state->players[3].dominoCount].rotation = 90.0f;
	                state->pioche = state->pioche->domino_suivant;
	                DrawDominoInChevalet(state, 3, state->players[3].dominoCount-1);
	                state->players[3].dominoCount++;
					state->currentPlayer = 1;
					break;
	    	}
		}
		else {
			printf("Erreur : Pioche vide.\n");
		}
	}

    DrawText(TextFormat("Tour de %s", state->players[state->currentPlayer - 1].name),
    state->screenWidth - 200, 15, 20, CUSTOM_BEIGE);
}

void DrawDominoInChevalet(GameState *state, int playerIndex, int dominoIndex) {
    Player *player = &state->players[playerIndex];
    Domino *domino = &player->playerDominos[dominoIndex];
    float spacing = 10.0f;
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
            startX + (dominoIndex * (dominoWidth + spacing)),
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



void InitGamePieces(GameState *state) {
	
	state->currentPlayer = 1;
	state->pioche = initialiserDomino();
    melange_pioche(&state->pioche);
    printf("la pioche au debut :\n");
    afficherDomino(state->pioche);

    for (int p = 0; p < state->playerCount; p++) {
    	state->players[p].dominoCount = (state->playerCount == 2) ? 7 : 6;
        if (state->selectedGame == 0) {
            for (int d = 0; d < state->players[p].dominoCount; d++) {
            	if (state->pioche == NULL) {
                    printf("Erreur : Pioche vide avant distribution compl�te.\n");
                    return;
                }
                state->players[p].playerDominos[d].v_gauche = state->pioche->domino_current.v_gauche;
                state->players[p].playerDominos[d].v_droite = state->pioche->domino_current.v_droite;
                state->players[p].playerDominos[d].position = (Vector2){0, 0};
                state->players[p].playerDominos[d].rotation = 90.0f;
                state->pioche = state->pioche->domino_suivant;
                printf("(%d|", state->players[p].playerDominos[d].v_gauche);
                printf("%d)\n", state->players[p].playerDominos[d].v_droite);
			}
        } else {
            state->players[p].triominoCount = 7;
            for (int t = 0; t < state->players[p].triominoCount; t++) {
                if (t >= state->players[p].triominoCapacity) {
                    state->players[p].triominoCapacity *= 2;
                    state->players[p].playerTriominos = realloc(
                        state->players[p].playerTriominos, 
                        state->players[p].triominoCapacity * sizeof(Triomino)
                    );
                    if (!state->players[p].playerTriominos) {
                        printf("Memory reallocation error for triominos\n");
                        exit(-1);
                    }
                }
                InitTriomino(&state->players[p].playerTriominos[t]);
            }
        }
    }
	printf("la pioche en fin :\n");
	afficherDomino(state->pioche);
}
