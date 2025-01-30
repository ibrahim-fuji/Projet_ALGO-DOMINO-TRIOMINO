// domino.c
#include "domino.h"
#include <math.h>

// Function to draw a custom button and handle interactions
bool DrawButtonCustom(Button *button) {
    Vector2 mousePoint = GetMousePosition();
    button->isHovered = CheckCollisionPointRec(mousePoint, button->rect);

    Color currentColor = button->isHovered ? button->hoverColor : button->baseColor;
    DrawRectangleRec(button->rect, currentColor);
    DrawRectangleLinesEx(button->rect, 2, BLACK_TEXT);

    int fontSize = 20;
    int textWidth = MeasureText(button->text, fontSize);
    Vector2 textPosition = {
        button->rect.x + (button->rect.width - textWidth) / 2,
        button->rect.y + (button->rect.height - fontSize) / 2
    };

    DrawText(button->text, textPosition.x, textPosition.y, fontSize, button->textColor);

    // Return true if the button is clicked
    return button->isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

// Function to draw a domino dot
void DrawDominoDot(float x, float y) {
    DrawCircle(x, y, DOTS_RADIUS, BLACK_TEXT);
}

// Function to draw the dots on a domino face
void DrawDominoFace(float x, float y, int value, float width, float height) {
    switch(value) {
        case 1:
            DrawDominoDot(x + width/2, y + height/2);
            break;
        case 2:
            DrawDominoDot(x + width/4, y + height/4);
            DrawDominoDot(x + 3*width/4, y + 3*height/4);
            break;
        case 3:
            DrawDominoDot(x + width/4, y + height/4);
            DrawDominoDot(x + width/2, y + height/2);
            DrawDominoDot(x + 3*width/4, y + 3*height/4);
            break;
        case 4:
            DrawDominoDot(x + width/4, y + height/4);
            DrawDominoDot(x + 3*width/4, y + height/4);
            DrawDominoDot(x + width/4, y + 3*height/4);
            DrawDominoDot(x + 3*width/4, y + 3*height/4);
            break;
        case 5:
            DrawDominoDot(x + width/4, y + height/4);
            DrawDominoDot(x + 3*width/4, y + height/4);
            DrawDominoDot(x + width/2, y + height/2);
            DrawDominoDot(x + width/4, y + 3*height/4);
            DrawDominoDot(x + 3*width/4, y + 3*height/4);
            break;
        case 6:
            DrawDominoDot(x + width/4, y + height/4);
            DrawDominoDot(x + width/4, y + height/2);
            DrawDominoDot(x + width/4, y + 3*height/4);
            DrawDominoDot(x + 3*width/4, y + height/4);
            DrawDominoDot(x + 3*width/4, y + height/2);
            DrawDominoDot(x + 3*width/4, y + 3*height/4);
            break;
    }
}

// Function to draw a domino
void DrawDomino(Domino domino) {
    float halfWidth = DOMINO_WIDTH / 2.0f;
    float halfHeight = DOMINO_HEIGHT / 2.0f;
    
    // Draw the white background of the domino
    DrawRectangle(domino.position.x, domino.position.y, DOMINO_WIDTH, DOMINO_HEIGHT, WHITE_COLOR);
    
    // Draw the separating line
    DrawLine(domino.position.x + halfWidth, domino.position.y, 
             domino.position.x + halfWidth, domino.position.y + DOMINO_HEIGHT, BLACK_TEXT);
    
    // Draw the outline
    DrawRectangleLines(domino.position.x, domino.position.y, DOMINO_WIDTH, DOMINO_HEIGHT, BLACK_TEXT);
    
    // Draw the dots
    DrawDominoFace(domino.position.x, domino.position.y, domino.v_gauche, halfWidth, DOMINO_HEIGHT);
    DrawDominoFace(domino.position.x + halfWidth, domino.position.y, domino.v_droite, halfWidth, DOMINO_HEIGHT);
}

