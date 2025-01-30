// triomino.c
#include "triomino.h"
#include "common_defs.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>     // Pour rand() et realloc()
#include <time.h>       // Pour srand()

// Fonction pour obtenir une couleur basée sur la valeur
Color GetColorByValue(int value) {
    switch(value) {
        case 0: return (Color){255, 255, 255, 255}; // Blanc
        case 1: return (Color){255, 0, 0, 255};     // Rouge
        case 2: return (Color){0, 255, 0, 255};     // Vert
        case 3: return (Color){0, 0, 255, 255};     // Bleu
        default: return BLACK_TEXT;
    }
}

// Fonction pour initialiser un triomino
void InitTriomino(Triomino *triomino) {
    // Initialiser les valeurs aléatoires
    triomino->values[0] = rand() % 4; // Valeur entre 0 et 3
    triomino->values[1] = rand() % 4;
    triomino->values[2] = rand() % 4;

    // Initialiser la position et la rotation
    triomino->position = (Vector2){0, 0};
    triomino->rotation = 0.0f;
    triomino->isRotating = false;
    triomino->rotationSpeed = 90.0f; // 90 degrés par seconde
}

// Fonction pour initialiser la pioche de triominos
void InitTriominoPioche(TriominoPioche *pioche) {
    pioche->count = TOTAL_TRIOMINOS;
    for(int i = 0; i < TOTAL_TRIOMINOS; i++) {
        InitTriomino(&pioche->pieces[i]);
    }
}

// Fonction pour mélanger la pioche de triominos
void MelangerPioche(TriominoPioche *pioche) {
    for(int i = pioche->count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Triomino temp = pioche->pieces[i];
        pioche->pieces[i] = pioche->pieces[j];
        pioche->pieces[j] = temp;
    }
}

// Fonction pour piocher un triomino
Triomino PiocherTriomino(TriominoPioche *pioche) {
    if(pioche->count <= 0) {
        // Retourner un triomino invalide ou gérer l'erreur
        Triomino invalid = {{-1, -1, -1}, {0, 0}, 0, false, 0};
        return invalid;
    }
    
    pioche->count--;
    return pioche->pieces[pioche->count];
}

// Fonction pour calculer les points d'un triomino avec un bonus
int CalculerPoints(Triomino *triomino, BonusType bonus) {
    int points = triomino->values[0] + triomino->values[1] + triomino->values[2];
    
    switch(bonus) {
        case BONUS_BRIDGE:
            points += 40;
            break;
        case BONUS_HEXAGON:
            points += 50;
            break;
        case BONUS_DOUBLE_HEXAGON:
            points += 60;
            break;
        case BONUS_TRIPLE_HEXAGON:
            points += 70;
            break;
        default:
            break;
    }
    
    return points;
}

// Fonction pour jouer un tour de l'IA
bool JouerIA(GameState *state, int playerIndex) {
    Player *ai = &state->players[playerIndex];
    
    // Stratégie simple : jouer le premier triomino valide
    for(int i = 0; i < ai->triominoCount; i++) {
        Triomino *triomino = &ai->playerTriominos[i];
        
        // Chercher une position valide sur le plateau
        // Pour simplifier, on essaie juste quelques positions prédéfinies
        Vector2 positions[4] = {
            {state->screenWidth/2, state->screenHeight/2},
            {state->screenWidth/2 - TRIOMINO_WIDTH, state->screenHeight/2},
            {state->screenWidth/2 + TRIOMINO_WIDTH, state->screenHeight/2},
            {state->screenWidth/2, state->screenHeight/2 + TRIOMINO_HEIGHT}
        };
        
        for(int p = 0; p < 4; p++) {
            if(VerifierPlacementTriomino(state, triomino, positions[p])) {
                PlacerTriomino(state, triomino, positions[p]);
                
                // Retirer le triomino de la main de l'IA
                for(int j = i; j < ai->triominoCount - 1; j++) {
                    ai->playerTriominos[j] = ai->playerTriominos[j + 1];
                }
                ai->triominoCount--;
                
                return true;
            }
        }
    }
    
    return false;
}

// Fonction pour mettre à jour la rotation d'un triomino
void UpdateTriominoRotation(Triomino *triomino, float deltaTime) {
    if (triomino->isRotating) {
        triomino->rotation += triomino->rotationSpeed * deltaTime;
        if (triomino->rotation >= 360.0f) triomino->rotation -= 360.0f;
    }
}

// Fonction pour dessiner un nombre sur une face du triomino
void DrawTriominoNumber(float x, float y, int value, int faceIndex) {
    char number[2];
    sprintf(number, "%d", value);
    float centerX = x + TRIOMINO_WIDTH / 2;

    switch(faceIndex) {
        case 0: // Face supérieure (haut)
            DrawText(number, 
                     centerX - MeasureText(number, 20) / 2, 
                     y + TRIOMINO_HEIGHT / 6, 
                     20, 
                     BLACK_TEXT);
            break;
        case 1: // Face droite
            DrawText(number, 
                     x + 3 * TRIOMINO_WIDTH / 4 - MeasureText(number, 20) / 2, 
                     y + 2 * TRIOMINO_HEIGHT / 3, 
                     20, 
                     BLACK_TEXT);
            break;
        case 2: // Face gauche
            DrawText(number, 
                     x + TRIOMINO_WIDTH / 4 - MeasureText(number, 20) / 2, 
                     y + 2 * TRIOMINO_HEIGHT / 3, 
                     20, 
                     BLACK_TEXT);
            break;
    }
}

// Fonction pour dessiner un triomino
void DrawTriomino(Triomino triomino) {
    // Calcul des points du triangle
    Vector2 points[3];
    float halfWidth = TRIOMINO_WIDTH / 2.0f;
    float halfHeight = TRIOMINO_HEIGHT / 2.0f;

    // Points du triangle de base (avant rotation)
    points[0] = (Vector2){triomino.position.x + halfWidth, triomino.position.y}; // Sommet
    points[1] = (Vector2){triomino.position.x + TRIOMINO_WIDTH, triomino.position.y + TRIOMINO_HEIGHT}; // Bas droite
    points[2] = (Vector2){triomino.position.x, triomino.position.y + TRIOMINO_HEIGHT}; // Bas gauche

    // Application de la rotation si nécessaire
    if (triomino.rotation != 0) {
        Vector2 center = {triomino.position.x + halfWidth, triomino.position.y + halfHeight};
        for (int i = 0; i < 3; i++) {
            // Translation au centre, rotation, puis retour
            points[i] = Vector2Subtract(points[i], center);
            points[i] = Vector2Rotate(points[i], triomino.rotation * DEG2RAD);
            points[i] = Vector2Add(points[i], center);
        }
    }

    // Dessin du fond coloré du triomino
    Color faceColors[3];
    for (int i = 0; i < 3; i++) {
        faceColors[i] = GetColorByValue(triomino.values[i]);
    }

    // Dessin du triomino avec des couleurs différentes pour chaque face
    for (int i = 0; i < 3; i++) {
        int next = (i + 1) % 3;
        DrawTriangle(
            points[i],
            points[next],
            (Vector2){(points[i].x + points[next].x) / 2, (points[i].y + points[next].y) / 2},
            faceColors[i]
        );
    }

    // Dessin du contour
    DrawTriangleLines(points[0], points[1], points[2], BLACK_TEXT);

    // Dessin des nombres sur chaque face
    for (int i = 0; i < 3; i++) {
        DrawTriominoNumber(triomino.position.x, triomino.position.y, triomino.values[i], i);
    }
}

// Fonction pour vérifier si deux triominos correspondent sur des côtés spécifiques
bool DoTriominosMatch(Triomino *tri1, Triomino *tri2, int side1, int side2) {
    // side1 et side2 représentent les côtés à comparer (0 = haut, 1 = droite, 2 = gauche)
    return tri1->values[side1] == tri2->values[side2];
}

// Fonction pour vérifier si le placement d'un triomino est valide
bool IsPlacementValid(GameState *state, Triomino *triomino) {
    // Parcourir tous les triominos déjà placés sur le plateau
    for (int p = 0; p < state->playerCount; p++) {
        for (int t = 0; t < state->players[p].triominoCount; t++) {
            Triomino *other = &state->players[p].playerTriominos[t];
            if (other == triomino) continue; // Ignorer le triomino actuel

            // Vérifier les collisions ou la proximité
            // Par exemple, si les positions sont proches, vérifier les correspondances
            if (Vector2Distance(triomino->position, other->position) < TRIOMINO_WIDTH) {
                // Vérifier les correspondances des valeurs
                // Déterminer quels côtés sont adjacents
                // Ceci est un exemple simplifié
                if (DoTriominosMatch(triomino, other, 1, 2)) { // Exemple : droite de tri1 avec gauche de tri2
                    return true;
                }
            }
        }
    }
    return false;
}

// Fonction pour vérifier si un point est à l'intérieur d'un triomino (utilisée pour les interactions)
bool IsPointInTriomino(Vector2 point, Triomino triomino) {
    Vector2 p1 = {triomino.position.x + TRIOMINO_WIDTH / 2, triomino.position.y};
    Vector2 p2 = {triomino.position.x + TRIOMINO_WIDTH, triomino.position.y + TRIOMINO_HEIGHT};
    Vector2 p3 = {triomino.position.x, triomino.position.y + TRIOMINO_HEIGHT};
    
    // Formule de l'aire d'un triangle utilisant les coordonnées
    float totalArea = fabs((p2.x - p1.x)*(p3.y - p1.y) - (p3.x - p1.x)*(p2.y - p1.y)) / 2.0f;
    float area1 = fabs((p1.x - point.x)*(p2.y - point.y) - (p2.x - point.x)*(p1.y - point.y)) / 2.0f;
    float area2 = fabs((p2.x - point.x)*(p3.y - point.y) - (p3.x - point.x)*(p2.y - point.y)) / 2.0f;
    float area3 = fabs((p3.x - point.x)*(p1.y - point.y) - (p1.x - point.x)*(p3.y - point.y)) / 2.0f;

    return fabs(totalArea - (area1 + area2 + area3)) < 0.1f;
}

bool VerifierPlacementTriomino(GameState *state, Triomino *triomino, Vector2 position) {
    if (state->board.count == 0) {
        return true; // Premier triomino peut être placé n'importe où
    }

    // Vérifier la distance avec les autres triominos
    for (int i = 0; i < state->board.count; i++) {
        float distance = Vector2Distance(position, state->board.positions[i]);
        if (distance < TRIOMINO_WIDTH) {
            // Vérifier que les valeurs correspondent sur les côtés adjacents
            Triomino *placedTriomino = &state->board.pieces[i];
            for (int side1 = 0; side1 < 3; side1++) {
                for (int side2 = 0; side2 < 3; side2++) {
                    if (DoTriominosMatch(triomino, placedTriomino, side1, side2)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

BonusType VerifierBonus(GameState *state, Vector2 position) {
    int adjacentCount = 0;
    bool formsBridge = false;
    bool formsHexagon = false;
    
    // Compter les triominos adjacents
    for (int i = 0; i < state->board.count - 1; i++) {
        float distance = Vector2Distance(position, state->board.positions[i]);
        if (distance < TRIOMINO_WIDTH * 1.5f) {
            adjacentCount++;
            
            // Vérifier les configurations spéciales
            if (adjacentCount == 2) formsBridge = true;
            if (adjacentCount == 6) formsHexagon = true;
        }
    }
    
    // Déterminer le type de bonus
    if (formsHexagon) {
        int hexagonCount = 0;
        // Compter le nombre d'hexagones déjà formés
        // Cette partie nécessite une logique plus complexe pour détecter les hexagones multiples
        switch(hexagonCount) {
            case 2: return BONUS_DOUBLE_HEXAGON;
            case 3: return BONUS_TRIPLE_HEXAGON;
            case 1: return BONUS_HEXAGON;
            default: break;
        }
    }
    
    if (formsBridge) return BONUS_BRIDGE;
    
    return BONUS_NONE;
}

void PlacerTriomino(GameState *state, Triomino *triomino, Vector2 position) {
    // Augmenter la taille des tableaux
    state->board.count++;
    state->board.pieces = realloc(state->board.pieces, state->board.count * sizeof(Triomino));
    state->board.positions = realloc(state->board.positions, state->board.count * sizeof(Vector2));
    
    // Ajouter le nouveau triomino
    state->board.pieces[state->board.count - 1] = *triomino;
    state->board.positions[state->board.count - 1] = position;
    
    // Mettre à jour le score du joueur
    BonusType bonus = VerifierBonus(state, position);
    state->players[state->currentPlayer].score += CalculerPoints(triomino, bonus);
}

void CalculateFinalScores(GameState *state) {
    // Points bonus pour le joueur qui finit
    if (state->players[state->currentPlayer].triominoCount == 0) {
        state->players[state->currentPlayer].score += 25;
    }
    
    // Pénalités pour les triominos restants
    for (int i = 0; i < state->playerCount; i++) {
        Player *player = &state->players[i];
        for (int t = 0; t < player->triominoCount; t++) {
            int valuesSum = player->playerTriominos[t].values[0] + 
                          player->playerTriominos[t].values[1] + 
                          player->playerTriominos[t].values[2];
            player->score -= valuesSum;
        }
    }
}

void DetectHexagons(GameState *state, Hexagon *hexagons, int *hexagonCount) {
    *hexagonCount = 0;
    
    // Pour chaque triomino sur le plateau
    for (int i = 0; i < state->board.count; i++) {
        Vector2 center = state->board.positions[i];
        int adjacentCount = 0;
        Vector2 adjacentPositions[6];
        
        // Vérifier les triominos adjacents
        for (int j = 0; j < state->board.count; j++) {
            if (i == j) continue;
            
            float distance = Vector2Distance(center, state->board.positions[j]);
            if (distance < TRIOMINO_WIDTH * 1.5f) {
                adjacentPositions[adjacentCount] = state->board.positions[j];
                adjacentCount++;
                
                if (adjacentCount == 6) {
                    // Hexagone trouvé
                    hexagons[*hexagonCount].center = center;
                    hexagons[*hexagonCount].triominoCount = 6;
                    for (int k = 0; k < 6; k++) {
                        hexagons[*hexagonCount].positions[k] = adjacentPositions[k];
                    }
                    (*hexagonCount)++;
                }
            }
        }
    }
}

void HandleTriominoRotation(GameState *state) {
    if (state->selectedTriomino && IsKeyPressed(KEY_R)) {
        state->selectedTriomino->rotation += 120.0f; // Rotation de 120° (un tiers de tour)
        if (state->selectedTriomino->rotation >= 360.0f) {
            state->selectedTriomino->rotation = 0.0f;
        }
        
        // Faire tourner les valeurs
        int temp = state->selectedTriomino->values[0];
        state->selectedTriomino->values[0] = state->selectedTriomino->values[2];
        state->selectedTriomino->values[2] = state->selectedTriomino->values[1];
        state->selectedTriomino->values[1] = temp;
    }
}

bool TryPiocherTriomino(GameState *state, int playerIndex) {
    if (state->pioche.count > 0 && state->consecutivePasses < 3) {
        Triomino newTriomino = PiocherTriomino(&state->pioche);
        state->players[playerIndex].triominoCount++;
        state->players[playerIndex].playerTriominos = realloc(
            state->players[playerIndex].playerTriominos,
            state->players[playerIndex].triominoCount * sizeof(Triomino)
        );
        state->players[playerIndex].playerTriominos[state->players[playerIndex].triominoCount - 1] = newTriomino;
        
        // Pénalité de points pour la pioche
        state->players[playerIndex].score -= 5;
        return true;
    }
    return false;
}

void UpdateTriominoPositions(GameState *state) {
    for (int i = 0; i < state->board.count; i++) {
        // Ajuster la position en fonction de la grille hexagonale
        float xOffset = (i % 2) * (TRIOMINO_WIDTH * 0.75f);
        float yOffset = (i / 2) * (TRIOMINO_HEIGHT * 1.5f);
        
        state->board.pieces[i].position.x = state->screenWidth/2 + xOffset;
        state->board.pieces[i].position.y = state->screenHeight/2 + yOffset;
    }
}

bool IsValidMove(GameState *state, Vector2 position) {
    if (state->board.count == 0) return true;
    
    // Vérifier la distance avec les autres triominos
    for (int i = 0; i < state->board.count; i++) {
        float distance = Vector2Distance(position, state->board.positions[i]);
        if (distance < TRIOMINO_WIDTH/2) return false; // Trop proche
        if (distance > TRIOMINO_WIDTH * 2) continue;   // Trop loin
        
        // Vérifier l'alignement
        float angle = atan2(position.y - state->board.positions[i].y,
                          position.x - state->board.positions[i].x);
        angle = angle * 180.0f / PI; // Convertir en degrés
        
        // Les angles valides sont 0°, 60°, 120°, 180°, 240°, 300°
        float validAngles[] = {0, 60, 120, 180, 240, 300};
        for (int j = 0; j < 6; j++) {
            if (fabs(angle - validAngles[j]) < 10.0f) return true;
        }
    }
    return false;
}

void UpdateBoardPositions(GameState *state) {
    if (state->board.count == 0) return;
    
    // Calculer le centre du plateau
    Rectangle playArea = {50, 100, state->screenWidth - 100, state->screenHeight - 200};
    Vector2 center = {playArea.x + playArea.width/2, playArea.y + playArea.height/2};
    
    // Mise à jour des positions
    for (int i = 0; i < state->board.count; i++) {
        state->board.pieces[i].position = Vector2Add(state->board.positions[i], center);
    }
}

bool CheckTriominoCollisions(GameState *state, Vector2 position) {
    for (int i = 0; i < state->board.count; i++) {
        float distance = Vector2Distance(position, state->board.positions[i]);
        if (distance < TRIOMINO_WIDTH) {
            return true;  // Collision détectée
        }
    }
    return false;  // Retour par défaut ajouté
}

Vector2 GetValidTriominoPosition(Vector2 mousePos, Vector2 gridPos) {
    // Arrondir à la grille la plus proche
    float gridSize = TRIOMINO_WIDTH;
    Vector2 snappedPos = {
        floorf(mousePos.x / gridSize) * gridSize,
        floorf(mousePos.y / gridSize) * gridSize
    };
    return snappedPos;
}

void UpdatePieceAnimation(PieceAnimation *anim, float deltaTime) {
    if (!anim->isAnimating) return;
    
    anim->progress += deltaTime * ANIMATION_SPEED;
    if (anim->progress >= 1.0f) {
        anim->progress = 1.0f;
        anim->isAnimating = false;
    }
}

Vector2 GetSnapPosition(Vector2 mousePos, GameState *state) {
    Vector2 bestPos = mousePos;
    float minDist = INFINITY;
    
    Rectangle playArea = {50, 100, state->screenWidth - 100, state->screenHeight - 200};
    
    // Grille virtuelle
    float gridSize = TRIOMINO_WIDTH;
    float x = playArea.x;
    float y = playArea.y;
    
    while (y < playArea.y + playArea.height) {
        while (x < playArea.x + playArea.width) {
            Vector2 gridPos = {x, y};
            float dist = Vector2Distance(mousePos, gridPos);
            
            if (dist < minDist && dist < SNAP_DISTANCE && !CheckTriominoCollisions(state, gridPos)) {
                minDist = dist;
                bestPos = gridPos;
            }
            x += gridSize * 0.75f;
        }
        y += gridSize * 0.866f; // √3/2 pour la grille hexagonale
        x = playArea.x + ((int)(y/gridSize) % 2) * gridSize * 0.5f;
    }
    
    return bestPos;
}

void ShowPlacementHints(GameState *state) {
    if (!state->showHints || !state->selectedTriomino) return;
    
    Vector2 mousePos = GetMousePosition();
    Vector2 snapPos = GetSnapPosition(mousePos, state);
    
    // Afficher la position possible en transparence
    Color hintColor = IsValidMove(state, snapPos) ? 
                     (Color){0, 255, 0, 128} : (Color){255, 0, 0, 128};
    
    DrawCircle(snapPos.x, snapPos.y, 5, hintColor);
}

void HandleAITurn(GameState *state) {
    if (!state->players[state->currentPlayer].isAI) return;
    
    if (JouerIA(state, state->currentPlayer)) {
        state->currentTurn++;
        state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
        state->consecutivePasses = 0;
    } else {
        // Tenter de piocher si possible
        if (TryPiocherTriomino(state, state->currentPlayer)) {
            state->consecutivePasses++;
            state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
        } else {
            // Si on ne peut pas piocher, passer son tour
            state->consecutivePasses++;
            state->currentPlayer = (state->currentPlayer + 1) % state->playerCount;
        }
    }
}

void AnimateTriominoPlacement(Triomino *triomino, Vector2 target, float progress) {
    Vector2 start = triomino->position;
    triomino->position.x = start.x + (target.x - start.x) * progress;
    triomino->position.y = start.y + (target.y - start.y) * progress;
}

void ShowTriominoPlacementHints(GameState *state) {
    if (!state->selectedTriomino || !state->showHints) return;

    Rectangle playArea = {50, 100, state->screenWidth - 100, state->screenHeight - 200};
    Vector2 mousePos = GetMousePosition();

    if (CheckCollisionPointRec(mousePos, playArea)) {
        Vector2 snapPos = GetSnapPosition(mousePos, state);
        Color hintColor = IsValidMove(state, snapPos) ? 
                         (Color){0, 255, 0, 128} : 
                         (Color){255, 0, 0, 128};

        DrawTriangleLines(
            (Vector2){snapPos.x, snapPos.y},
            (Vector2){snapPos.x + TRIOMINO_WIDTH, snapPos.y},
            (Vector2){snapPos.x + TRIOMINO_WIDTH/2, snapPos.y + TRIOMINO_HEIGHT},
            hintColor
        );
    }
}

// Fonction pour effectuer une rotation d'un triomino
Triomino rotateTriomino(Triomino t, int rotations) {
    Triomino result = t;
    rotations = rotations % 3; // Normaliser le nombre de rotations (0, 1, ou 2)
    
    for (int i = 0; i < rotations; i++) {
        int temp = result.values[0];
        result.values[0] = result.values[2];
        result.values[2] = result.values[1];
        result.values[1] = temp;
    }
    
    return result;
}

// Fonction pour vérifier si deux triominos sont équivalents (même après rotation)
bool areTriominosEquivalent(Triomino t1, Triomino t2) {
    for (int i = 0; i < 3; i++) {
        if (t1.values[0] == t2.values[0] && 
            t1.values[1] == t2.values[1] && 
            t1.values[2] == t2.values[2]) {
            return true;
        }
        t2 = rotateTriomino(t2, 1);
    }
    return false;
}
