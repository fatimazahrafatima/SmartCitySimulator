/**
 * MOTEUR (ENGINE)
 * Ce fichier contient les outils graphiques comme les boutons et la mini-carte (Radar).
 */

#include "../include/engine.h"
#include "../include/world.h"
#include "../include/traffic_system.h"

// --- VARIABLES GLOBALES ---
// On les définit ici pour qu'elles existent en mémoire.
GameState currentState = MENU; // Le jeu commence sur le Menu
bool isNight = false;          // Le jeu commence de jour

// --- FONCTION BOUTON ---
// Dessine un bouton et renvoie "Vrai" si le joueur clique dessus
bool DrawButton(Rectangle rect, Color bgColor, Color textColor, const char* text) {
    // 1. Le fond du bouton
    DrawRectangleRec(rect, bgColor);
    // 2. La bordure blanche
    DrawRectangleLinesEx(rect, 2, RAYWHITE);
    // 3. Le texte à l'intérieur
    DrawText(text, rect.x + 15, rect.y + 12, 18, textColor);

    // 4. La logique du clic :
    // Si on clique (Bouton Gauche) ET que la souris est SUR le rectangle -> C'est gagné
    return (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), rect));
}

// --- FONCTION MINI-CARTE (RADAR) ---
// Affiche une vue aérienne miniature de toute la ville
void DrawMiniMap(const std::vector<Car*>& cars) {
    // Définition de la zone de la mini-carte (en bas à gauche dans la barre latérale)
    Rectangle mapArea = { 25, 300, 200, 150 }; 
    
    // Fond noir et bordure blanche
    DrawRectangleRec(mapArea, BLACK);
    DrawRectangleLinesEx(mapArea, 2, RAYWHITE); 

    // Dimensions du "vrai" monde (sans la barre latérale)
    float worldW = (float)GetScreenWidth() - SIDEBAR_WIDTH;
    float worldH = (float)GetScreenHeight();

    // --- MATHÉMATIQUES DE CONVERSION ---
    // Cette petite fonction "lambda" transforme une position du Grand Monde (ex: x=500)
    // vers une position sur la Petite Carte (ex: x=35).
    auto ToMap = [&](Vector2 p) -> Vector2 {
        // On calcule le pourcentage de position (0.0 à 1.0)
        float nx = (p.x - SIDEBAR_WIDTH) / worldW;
        float ny = p.y / worldH;
        // On applique ce pourcentage à la taille de la mini-carte
        return { mapArea.x + nx * mapArea.width, mapArea.y + ny * mapArea.height };
    };

    // On dessine les routes en gris foncé sur la mini-carte
    for(float vx : vRoads) DrawLineV(ToMap({vx, 0}), ToMap({vx, worldH}), DARKGRAY);
    for(float hy : hRoads) DrawLineV(ToMap({(float)SIDEBAR_WIDTH, hy}), ToMap({(float)GetScreenWidth(), hy}), DARKGRAY);

    // On fait clignoter les points d'alerte (Feu = Orange, Accident = Rouge)
    // L'astuce "(int)(GetTime()*5)%2==0" permet de créer le clignotement
    if (fireActive && (int)(GetTime()*5)%2==0) DrawCircleV(ToMap(firePos), 5, ORANGE);
    if (accidentActive && (int)(GetTime()*5)%2==0) DrawCircleV(ToMap(accidentPos), 5, RED);

    // On dessine toutes les voitures sous forme de petits points
    for(auto c : cars) {
        Vector2 mPos = ToMap(c->pos); // On calcule sa position sur le radar
        
        // Choix de la couleur selon le type de véhicule
        Color col = (c->type == CIVIL) ? GREEN : 
                    ((c->type == POLICE) ? SKYBLUE : 
                    ((c->type == AMBULANCE) ? WHITE : RED)); // RED = Pompier
        
        // Si une voiture civile s'est garée pour laisser passer, elle devient Orange
        if (c->isYielding) col = ORANGE;
        
        DrawCircleV(mPos, 2, col); // On dessine le point
    }
    
    // Petit titre au dessus
    DrawText("MINI-MAP", mapArea.x, mapArea.y - 15, 10, GRAY);
}