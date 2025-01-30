#include "do.h"
#include "joueurs.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_state.h"  // Ajout de l'inclusion pour GameState

DominoNode* initialiserDomino() {
    DominoNode *tete = NULL;
    int i, j;

    for (i = 0; i <= 6; i++) {
        for (j = i; j <= 6; j++) {
            DominoNode *nouveau = (DominoNode*)malloc(sizeof(DominoNode));
            nouveau->domino_current.v_gauche = i;
            nouveau->domino_current.v_droite = j;
            nouveau->domino_suivant = tete;
            tete = nouveau;
        }
    }
    return tete;
}

Plateau* initialiserPlateau() {
    return NULL;
}

DominoNode* supprimerDomino(DominoNode *tete, int v_gauche, int v_droite) {
    DominoNode *tmp = tete;
    DominoNode *precedent = NULL;

    while (tmp != NULL) {
        if (tmp->domino_current.v_gauche == v_gauche && tmp->domino_current.v_droite == v_droite) {
            if (precedent == NULL) {
                tete = tmp->domino_suivant;
            } else {
                precedent->domino_suivant = tmp->domino_suivant;
            }
            free(tmp);
            return tete;
        }
        precedent = tmp;
        tmp = tmp->domino_suivant;
    }
    return tete;
}

void afficherDomino(DominoNode *tete) {
    DominoNode *pointeurDomino = tete;
    printf("La pioche : ");
    while (pointeurDomino != NULL) {
        printf("(%d|%d) ", pointeurDomino->domino_current.v_gauche, pointeurDomino->domino_current.v_droite);
        pointeurDomino = pointeurDomino->domino_suivant;
    }
    printf("\n");
}

void afficherPlateau(Plateau *plateau) {
    Plateau *pointeurPlateau = plateau;
    printf("Le plateau : ");
    while (pointeurPlateau != NULL) {
        printf("(%d|%d) ", pointeurPlateau->domino.v_gauche, pointeurPlateau->domino.v_droite);
        pointeurPlateau = pointeurPlateau->suivant;
    }
    printf("\n");
}
void melange_pioche(DominoNode** tete) {
    if (tete == NULL || *tete == NULL) {
        return; // Rien � m�langer si la pioche est vide
    }

    int n = 0;
    DominoNode* tmp = *tete;

    // Compter les dominos dans la pioche
    while (tmp != NULL) {
        n++;
        tmp = tmp->domino_suivant;
    }

    // Stocker les adresses des dominos dans un tableau
    DominoNode** tableau = (DominoNode**)malloc(n * sizeof(DominoNode*));
    if (tableau == NULL) {
        return; // Erreur d'allocation m�moire
    }

    tmp = *tete;
    for (int i = 0; i < n; i++) {
        tableau[i] = tmp;
        tmp = tmp->domino_suivant;
    }

    // M�langer le tableau avec l'algorithme de Fisher-Yates
    srand(time(NULL));
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        DominoNode* temp = tableau[i];
        tableau[i] = tableau[j];
        tableau[j] = temp;
    }

    // Recr�er la liste cha�n�e m�lang�e
    for (int i = 0; i < n - 1; i++) {
        tableau[i]->domino_suivant = tableau[i + 1];
    }
    tableau[n - 1]->domino_suivant = NULL;

    // Mettre � jour la t�te de la pioche
    *tete = tableau[0];

    free(tableau);
}

//DominoNode* melange_pioche(DominoNode* tete) {
//    int n = 0;
//    DominoNode* tmp = tete;
//
//    while (tmp != NULL) {
//        n++;
//        tmp = tmp->domino_suivant;
//    }
//
//    DominoNode** tableau = (DominoNode**)malloc(n * sizeof(DominoNode*));
//    tmp = tete;
//    for (int i = 0; i < n; i++) {
//        tableau[i] = tmp;
//        tmp = tmp->domino_suivant;
//    }
//
//    srand(time(NULL));
//    for (int i = n - 1; i > 0; i--) {
//        int j = rand() % (i + 1);
//        DominoNode* temp = tableau[i];
//        tableau[i] = tableau[j];
//        tableau[j] = temp;
//    }
//
//    for (int i = 0; i < n - 1; i++) {
//        tableau[i]->domino_suivant = tableau[i + 1];
//    }
//    tableau[n - 1]->domino_suivant = NULL;
//
//    tete = tableau[0];
//    free(tableau);
//    return tete;
//}
void distribuer_piece(DominoNode** pioche, DominoNode* joueurs[], int nb_joueurs) {
    if (pioche == NULL || *pioche == NULL || joueurs == NULL) {
        fprintf(stderr, "Erreur: Param�tres invalides pour la distribution des pi�ces\n");
        return;
    }

    int nb_pieces = (nb_joueurs >= 3) ? 6 : 7;

    for (int i = 0; i < nb_joueurs; i++) {
        for (int j = 0; j < nb_pieces; j++) {
            if (*pioche == NULL) {
                printf("Pioche vide!\n");
                return;
            }

            DominoNode* nouveau = *pioche;
            *pioche = (*pioche)->domino_suivant;
            nouveau->domino_suivant = NULL;

            // Debugging output
            printf("Distribuer (%d|%d) au joueur %d\n", nouveau->domino_current.v_gauche, nouveau->domino_current.v_droite, i);

            if (joueurs[i] == NULL) {
                joueurs[i] = nouveau;
            } else {
                DominoNode* temp = joueurs[i];
                while (temp->domino_suivant != NULL) {
                    temp = temp->domino_suivant;
                }
                temp->domino_suivant = nouveau;
            }
        }
    }
}



//void distribuer_piece(DominoNode** pioche, DominoNode** joueurs, int nb_joueurs) {
//    int nb_pieces = (nb_joueurs >= 3) ? 6 : 7;
//
//    for (int i = 0; i < nb_joueurs; i++) {
//        for (int j = 0; j < nb_pieces; j++) {
//            if (*pioche == NULL) {
//                printf("Pioche vide!\n");
//                return;
//            }
//
//            DominoNode* nouveau = *pioche;
//            *pioche = (*pioche)->domino_suivant;
//            nouveau->domino_suivant = NULL;
//
//            if (joueurs[i] == NULL) {
//                joueurs[i] = nouveau;
//            } else {
//                DominoNode* temp = joueurs[i];
//                while (temp->domino_suivant != NULL) {
//                    temp = temp->domino_suivant;
//                }
//                temp->domino_suivant = nouveau;
//            }
//        }
//        printf("Pioche apres distribution au joueur %d:\n", i + 1);
//        afficherDomino(*pioche);
//    }
//}
//void afficherchaineDomino(DominoNode* chaine) {
//    while (chaine != NULL) {
//        printf("(%d|%d) ", chaine->domino_current.v_gauche, chaine->domino_current.v_droite);
//        chaine = chaine->domino_suivant;
//    }
//    printf("\n");
//}
void lib_chaine(DominoNode* chaine) {
    while (chaine != NULL) {
        DominoNode* tmp = chaine;
        chaine = chaine->domino_suivant;
        free(tmp);
    }
}

Domino tournerDomino(Domino domino) {
    Domino resultat = domino;
    
    // Échanger les valeurs
    int temp = resultat.v_droite;
    resultat.v_droite = resultat.v_gauche;
    resultat.v_gauche = temp;
    
    // Mettre à jour la rotation physique
    resultat.rotation += 180.0f;
    if (resultat.rotation >= 360.0f) {
        resultat.rotation -= 360.0f;
    }
    
    return resultat;
}

bool dominosEgaux(Domino d1, Domino d2) {
    return (d1.v_gauche == d2.v_gauche && d1.v_droite == d2.v_droite) ||
           (d1.v_gauche == d2.v_droite && d1.v_droite == d2.v_gauche);
}

void supprimerDominoJoueur(Player* player, Domino domino) {
    int indexASupprimer = -1;
    
    // Chercher le domino à supprimer
    for (int i = 0; i < player->dominoCount; i++) {
        if (dominosEgaux(player->playerDominos[i], domino)) {
            indexASupprimer = i;
            break;
        }
    }
    
    // Si le domino a été trouvé, le supprimer
    if (indexASupprimer >= 0) {
        // Décaler tous les dominos suivants
        for (int i = indexASupprimer; i < player->dominoCount - 1; i++) {
            player->playerDominos[i] = player->playerDominos[i + 1];
        }
        player->dominoCount--;
    }
}

DominoNode* piocherDomino(DominoNode** pioche) {
    if (pioche == NULL || *pioche == NULL) {
        return NULL;
    }

    DominoNode* nouveau = *pioche;
    *pioche = (*pioche)->domino_suivant;
    nouveau->domino_suivant = NULL;
    return nouveau;
}

bool essayerPiocherDomino(GameState* state, int playerIndex) {
    if (state->piocheDominos == NULL) {
        return false;
    }

    DominoNode* nouveauDomino = piocherDomino(&state->piocheDominos);
    if (nouveauDomino == NULL) {
        return false;
    }

    Player* player = &state->players[playerIndex];
    if (player->dominoCount < 7) {
        player->playerDominos[player->dominoCount] = nouveauDomino->domino_current;
        player->playerDominos[player->dominoCount].position = (Vector2){0, 0};
        player->playerDominos[player->dominoCount].rotation = 90.0f;
        player->dominoCount++;
        free(nouveauDomino);
        return true;
    }

    free(nouveauDomino);
    return false;
}

int evaluerCoupDomino(Domino domino, Plateau* plateau, bool aGauche) {
    int score = 0;
    
    // Points de base pour les valeurs du domino
    score += domino.v_gauche + domino.v_droite;
    
    // Bonus pour les doubles
    if (domino.v_gauche == domino.v_droite) {
        score += 10;
    }
    
    // Bonus pour les connections parfaites
    if (plateau != NULL) {
        if (aGauche) {
            if (plateau->domino.v_gauche == domino.v_droite) {
                score += 15;
            }
        } else {
            Plateau* dernierDomino = plateau;
            while (dernierDomino->suivant != NULL) {
                dernierDomino = dernierDomino->suivant;
            }
            if (dernierDomino->domino.v_droite == domino.v_gauche) {
                score += 15;
            }
        }
    }
    
    return score;
}

bool jouerIA_Domino(GameState* state) {
    if (!state->players[state->currentPlayer].isAI) return false;
    
    Player* ai = &state->players[state->currentPlayer];
    
    // Premier domino au plateau
    if (state->plateau == NULL && ai->dominoCount > 0) {
        // Jouer le premier domino disponible
        placerDomino_Gauche(ai->playerDominos[0], &state->plateau);
        supprimerDominoJoueur(ai, ai->playerDominos[0]);
        return true;
    }
    
    // Trouver le meilleur coup
    int meilleurScore = -1;
    int meilleurIndex = -1;
    bool placerAGauche = false;
    
    // Évaluer chaque domino
    for (int i = 0; i < ai->dominoCount; i++) {
        // Essayer à gauche
        if (verificationPlacement(ai->playerDominos[i], state->plateau, true)) {
            int score = evaluerCoupDomino(ai->playerDominos[i], state->plateau, true);
            if (score > meilleurScore) {
                meilleurScore = score;
                meilleurIndex = i;
                placerAGauche = true;
            }
        }
        
        // Essayer à droite
        if (verificationPlacement(ai->playerDominos[i], state->plateau, false)) {
            int score = evaluerCoupDomino(ai->playerDominos[i], state->plateau, false);
            if (score > meilleurScore) {
                meilleurScore = score;
                meilleurIndex = i;
                placerAGauche = false;
            }
        }
    }
    
    // Jouer le meilleur coup trouvé
    if (meilleurIndex >= 0) {
        Domino dominoAJouer = ai->playerDominos[meilleurIndex];
        
        if (placerAGauche) {
            placerDomino_Gauche(dominoAJouer, &state->plateau);
        } else {
            placerDomino_Droite(dominoAJouer, &state->plateau);
        }
        
        supprimerDominoJoueur(ai, dominoAJouer);
        printf("L'IA a joué le domino [%d|%d] à %s\n", 
               dominoAJouer.v_gauche, 
               dominoAJouer.v_droite, 
               placerAGauche ? "gauche" : "droite");
        return true;
    }
    
    // Si aucun coup possible, essayer de piocher
    if (essayerPiocherDomino(state, state->currentPlayer)) {
        printf("L'IA a pioché un domino\n");
    }
    return false;
}

int calculerScoreMain(Player* player) {
    int score = 0;
    for (int i = 0; i < player->dominoCount; i++) {
        score += player->playerDominos[i].v_gauche + player->playerDominos[i].v_droite;
    }
    return score;
}

void ajouterPointsPlateau(GameState* state, int playerIndex) {
    // Ajouter les points du plateau
    int scoreTotal = calculerScore(state->plateau);
    printf("Points du plateau: %d\n", scoreTotal);
    
    // Ajouter bonus pour avoir fini
    if (state->players[playerIndex].dominoCount == 0) {
        scoreTotal += 25;
        printf("Bonus de fin: +25\n");
    }
    
    state->players[playerIndex].score += scoreTotal;
}

void verifierFinPartie(GameState* state) {
    bool partieTerminee = false;
    int gagnantIndex = -1;
    
    // Vérifier si un joueur n'a plus de dominos
    for (int i = 0; i < state->playerCount; i++) {
        if (state->players[i].dominoCount == 0) {
            partieTerminee = true;
            gagnantIndex = i;
            break;
        }
    }
    
    // Vérifier si la partie est bloquée
    if (!partieTerminee) {
        bool bloque = true;
        for (int i = 0; i < state->playerCount; i++) {
            if (peutJouerDomino(&state->players[i], state->plateau)) {
                bloque = false;
                break;
            }
        }
        if (bloque && state->piocheDominos == NULL) {
            partieTerminee = true;
            
            // Trouver le joueur avec le moins de points dans sa main
            int minScore = INT_MAX;
            for (int i = 0; i < state->playerCount; i++) {
                int scoreMain = calculerScoreMain(&state->players[i]);
                if (scoreMain < minScore) {
                    minScore = scoreMain;
                    gagnantIndex = i;
                }
            }
        }
    }
    
    if (partieTerminee) {
        // Appliquer les pénalités pour les dominos restants
        for (int i = 0; i < state->playerCount; i++) {
            int penalite = calculerScoreMain(&state->players[i]);
            state->players[i].score -= penalite;
            printf("Pénalité joueur %d: -%d\n", i + 1, penalite);
        }
        
        // Ajouter les points au gagnant
        if (gagnantIndex >= 0) {
            ajouterPointsPlateau(state, gagnantIndex);
        }
        
        state->currentScreen = SCORES;
    }
}

bool peutJouerDomino(Player* player, Plateau* plateau) {
    for (int i = 0; i < player->dominoCount; i++) {
        if (verificationPlacement(player->playerDominos[i], plateau, true) ||
            verificationPlacement(player->playerDominos[i], plateau, false)) {
            return true;
        }
    }
    return false;
}

int calculerScore(Plateau* plateau) {
    int score = 0;
    Plateau* current = plateau;
    
    while (current != NULL) {
        score += current->domino.v_gauche + current->domino.v_droite;
        current = current->suivant;
    }
    
    return score;
}
