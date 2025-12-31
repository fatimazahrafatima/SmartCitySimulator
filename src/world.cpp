/**
 * MONDE (WORLD)
 * Ce fichier gère la construction de la ville :
 * - Où sont les routes ?
 * - Où sont les bâtiments ?
 * - Outils mathématiques pour se repérer dans la grille.
 */

#include "../include/world.h"

// --- VARIABLES GLOBALES ---
// Ce sont les conteneurs qui stockent la structure de notre ville.
std::vector<float> vRoads;       // Liste des positions X des routes verticales
std::vector<float> hRoads;       // Liste des positions Y des routes horizontales
std::vector<Building> buildings; // Liste des bâtiments

// --- FONCTION "AIMANT" (SNAP) ---
// Cette fonction prend une position (val) et cherche dans une liste (axes)
// quelle est la valeur la plus proche.
// Utile pour dire à une voiture : "La route la plus proche est à X = 500".
float GetSnapAxis(float val, const std::vector<float>& axes) {
    float minD = 99999; // On commence avec une distance immense
    float best = val;
    
    // On parcourt toutes les lignes possibles
    for (float axis : axes) {
        float d = fabs(val - axis); // Distance absolue
        if (d < minD) { minD = d; best = axis; } // On a trouvé plus près !
    }
    return best;
}

// --- DESSIN DE LIGNE POINTILLÉE ---
// Raylib ne fait pas ça par défaut, donc on le fait à la main.
// On dessine de petits segments les uns à la suite des autres avec des trous.
void DrawDashedLine(Vector2 start, Vector2 end, float thick, Color color) {
    Vector2 diff = Vector2Subtract(end, start);
    float length = Vector2Length(diff);
    Vector2 dir = Vector2Normalize(diff);
    
    // On avance de 30 pixels à chaque fois
    for (float i = 0; i < length; i += 30) {
        // Point de début du tiret
        Vector2 p1 = Vector2Add(start, Vector2Scale(dir, i));
        // Point de fin du tiret (longueur 15)
        Vector2 p2 = Vector2Add(start, Vector2Scale(dir, fminf(i + 15, length)));
        
        DrawLineEx(p1, p2, thick, color);
    }
}

// --- GÉNÉRATEUR D'URGENCE ---
// Choisit une intersection au hasard sur la carte.
// C'est utilisé pour dire "Le feu a démarré ICI".
Vector2 GetRandomRoadTarget() {
    if(vRoads.empty() || hRoads.empty()) return {0,0};
    
    int rV = GetRandomValue(0, vRoads.size()-1); // Une route verticale au hasard
    int rH = GetRandomValue(0, hRoads.size()-1); // Une route horizontale au hasard
    
    return { vRoads[rV], hRoads[rH] }; // Le croisement des deux
}

// --- L'ARCHITECTE (CONSTRUCTION DE LA VILLE) ---
// Cette fonction vide la carte et recalcule tout selon la taille de la fenêtre.
void RecalculateGrid() {
    // 1. On efface tout
    vRoads.clear(); hRoads.clear();
    buildings.clear();

    int w = GetScreenWidth();
    int h = GetScreenHeight();
    
    // 2. On calcule combien de routes on peut mettre
    // On enlève la largeur du menu de gauche (SIDEBAR_WIDTH)
    int cols = (w - SIDEBAR_WIDTH) / TARGET_BLOCK_SIZE;
    if (cols < 2) cols = 2; // Minimum 2 routes
    int rows = h / TARGET_BLOCK_SIZE;
    if (rows < 2) rows = 2;

    // Espace entre les routes
    float spaceX = (float)(w - SIDEBAR_WIDTH) / cols;
    float spaceY = (float)h / rows;

    // 3. On remplit les listes de routes
    for(int i=0; i<cols; i++) vRoads.push_back(SIDEBAR_WIDTH + spaceX * i + spaceX/2);
    for(int i=0; i<rows; i++) hRoads.push_back(spaceY * i + spaceY/2);

    if (vRoads.empty() || hRoads.empty()) return;

    // 4. On place les bâtiments (Hôpital, Police, Pompiers)
    float bSize = 50; // Taille d'un bâtiment
    int lastV = vRoads.size() - 1;
    int lastH = hRoads.size() - 1;

    // Petite fonction locale pour trouver où est la sortie du garage (Entry Point)
    // Elle cherche le point de la route le plus proche du centre du bâtiment.
    auto GetEntry = [](Vector2 center) -> Vector2 {
        float cx = GetSnapAxis(center.x, vRoads);
        float cy = GetSnapAxis(center.y, hRoads);
        // On choisit l'axe le plus proche (X ou Y)
        if (fabs(center.x - cx) < fabs(center.y - cy)) return {cx, center.y};
        else return {center.x, cy};
    };

    // -- HÔPITAL (Coin Haut-Gauche) --
    // On le place entre le bord gauche et la première route
    Vector2 posH = { (SIDEBAR_WIDTH + vRoads[0]) / 2.0f, (0 + hRoads[0]) / 2.0f };
    buildings.push_back({ 
        {posH.x - bSize/2, posH.y - bSize/2, bSize, bSize}, // Le rectangle physique
        AMBULANCE, WHITE, "HOPITAL", 
        GetEntry(posH), // Point de sortie sur la route
        posH // Centre
    });

    // -- CASERNE POMPIERS (Coin Haut-Droit) --
    Vector2 posF = { (vRoads[lastV] + w) / 2.0f, (0 + hRoads[0]) / 2.0f };
    buildings.push_back({ 
        {posF.x - bSize/2, posF.y - bSize/2, bSize, bSize}, 
        FIRE, RED, "CASERNE", 
        GetEntry(posF), posF 
    });

    // -- POLICE (Coin Bas-Droit) --
    Vector2 posP = { (vRoads[lastV] + w) / 2.0f, (hRoads[lastH] + h) / 2.0f };
    buildings.push_back({ 
        {posP.x - bSize/2, posP.y - bSize/2, bSize, bSize}, 
        POLICE, BLUE, "POLICE", 
        GetEntry(posP), posP 
    });
}