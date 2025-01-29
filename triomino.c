// triomino.c
#include "triomino.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>

// Définition des couleurs personnalisées (évitez de redéfinir celles de raylib)
#define BLACK_TEXT (Color){0, 0, 0, 255}
#define WHITE_COLOR (Color){255, 255, 255, 255} // Renommez si nécessaire

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
