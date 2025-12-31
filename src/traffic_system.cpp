/**
 * SYSTÈME DE TRAFIC
 * Ce fichier gère les variables d'urgence et l'affichage des feux aux carrefours.
 */

#include "../include/traffic_system.h"

// --- VARIABLES D'URGENCE ---
// Ces variables servent à dire au jeu si une catastrophe est en cours.

bool fireActive = false;       // Est-ce qu'il y a un incendie ? (Faux au début)
Vector2 firePos = { 0, 0 };    // Position exacte du feu sur la carte

bool accidentActive = false;   // Est-ce qu'il y a un accident ? (Faux au début)
Vector2 accidentPos = { 0, 0 };// Position exacte de l'accident

// --- AFFICHAGE DES FEUX ---
// Cette fonction dessine les 4 feux à un croisement donné (x, y)
void DrawIntersectionLights(float x, float y, LightCycle cycle, bool isNight) {
    // Par défaut, on met tout le monde au ROUGE (sécurité)
    Color vColor = RED; // Couleur des feux Verticaux (Haut/Bas)
    Color hColor = RED; // Couleur des feux Horizontaux (Gauche/Droite)
    
    // On change les couleurs selon le tour (Cycle)
    if (cycle == V_GREEN) { 
        vColor = GREEN; hColor = RED; // Vertical passe
    }
    else if (cycle == V_YELLOW) { 
        vColor = ORANGE; hColor = RED; // Vertical ralentit
    }
    else if (cycle == H_GREEN) { 
        vColor = RED; hColor = GREEN; // Horizontal passe
    }
    else if (cycle == H_YELLOW) { 
        vColor = RED; hColor = ORANGE; // Horizontal ralentit
    }

    // Calcul de la position : On décale les feux pour qu'ils soient au coin de la route
    float off = ROAD_WIDTH / 2.0f + 5.0f; 

    // On dessine les 4 ampoules physiques (cercles pleins)
    DrawCircle(x + off, y - off, 6, vColor);
    DrawCircle(x - off, y + off, 6, vColor);
    DrawCircle(x - off, y - off, 6, hColor);
    DrawCircle(x + off, y + off, 6, hColor);
    
    // Si c'est la Nuit, on ajoute un effet visuel (Halo lumineux)
    if (isNight) {
        // On dessine un cercle flou et transparent autour de la lampe pour faire "briller"
        DrawCircleGradient(x + off, y - off, 15, Fade(vColor, 0.5f), Fade(vColor, 0.0f));
        DrawCircleGradient(x - off, y + off, 15, Fade(vColor, 0.5f), Fade(vColor, 0.0f));
        DrawCircleGradient(x - off, y - off, 15, Fade(hColor, 0.5f), Fade(hColor, 0.0f));
        DrawCircleGradient(x + off, y + off, 15, Fade(hColor, 0.5f), Fade(hColor, 0.0f));
    }
}