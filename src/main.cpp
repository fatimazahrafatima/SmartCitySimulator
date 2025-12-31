/**
 * MAIN (PROGRAMME PRINCIPAL)
 * C'est le point d'entrée. Il contient la boucle principale (Game Loop).
 * Il gère le Menu, les entrées clavier, et l'ordre d'affichage.
 */

// Inclusions des fichiers d'en-tête situés dans le dossier parent "../include"
#include "../include/config.h"
#include "../include/world.h"
#include "../include/traffic_system.h"
#include "../include/vehicle.h"
#include "../include/engine.h"
#include <math.h> 

int main() {
    // 1. INITIALISATION DE LA FENÊTRE
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "Sim Ville - Complet + Menu + Nuit");
    SetTargetFPS(60);

    // Construction initiale de la ville (routes et bâtiments)
    RecalculateGrid();

    // Liste de toutes les voitures en jeu
    std::vector<Car*> cars;
    
    // Variables de temps
    LightCycle cycle = V_GREEN; // Les feux commencent au vert vertical
    float timer = 0;

    // --- BOUCLE PRINCIPALE (Tant qu'on ne ferme pas la fenêtre) ---
    while (!WindowShouldClose()) {
        
        // --- A. LOGIQUE DU MENU ---
        // Si le jeu est en mode MENU, on affiche l'écran titre et on attend
        if (currentState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) {
                currentState = GAME; // Le joueur appuie sur Entrée -> Le jeu commence
            }
            
            BeginDrawing();
            ClearBackground(BLACK);
            
            int sw = GetScreenWidth();
            int sh = GetScreenHeight();
            
            // Animation "pulsation" du titre (Zoom in/out avec sin)
            float scale = 1.0f + sin(GetTime() * 2.0f) * 0.05f;
            
            const char* title = "SIMULATION VILLE";
            int titleW = MeasureText(title, 60);
            Vector2 origin = { (float)titleW/2, 30 }; // Centre du texte
            DrawTextPro(GetFontDefault(), title, (Vector2){(float)sw/2, (float)sh/2 - 100}, origin, 0, 60 * scale, 5, SKYBLUE);
            
            const char* sub = "Appuyez sur [ENTREE] pour commencer";
            int subW = MeasureText(sub, 20);
            DrawText(sub, sw/2 - subW/2, sh/2 + 20, 20, WHITE);
            
            const char* info = "Controles: Souris pour boutons, 'N' pour Mode Nuit";
            int infoW = MeasureText(info, 15);
            DrawText(info, sw/2 - infoW/2, sh/2 + 60, 15, DARKGRAY);
            
            EndDrawing();
            continue; // IMPORTANT : On saute le reste de la boucle tant qu'on est dans le menu
        }
        
        // --- B. LOGIQUE DU JEU (JOUABLE) ---
        
        // Gestion du redimensionnement de la fenêtre (Reconstruire la ville si on change la taille)
        if (IsWindowResized()) RecalculateGrid();

        // Touche 'N' pour changer Jour / Nuit
        if (IsKeyPressed(KEY_N)) isNight = !isNight;

        // Gestion des feux tricolores (Timer de 3 secondes)
        timer += GetFrameTime();
        if (timer > 3.0f) { 
            if(cycle == V_GREEN) cycle = V_YELLOW;
            else if(cycle == V_YELLOW) cycle = H_GREEN;
            else if(cycle == H_GREEN) cycle = H_YELLOW;
            else if(cycle == H_YELLOW) cycle = V_GREEN;
            timer = 0;
        }

        // --- GÉNÉRATION D'ÉVÉNEMENTS ALÉATOIRES ---
        
        // 1. Incendies (Probabilité très faible par image : 3 sur 1000)
        if (!fireActive && GetRandomValue(0, 1000) < 3) {
            // Algorithme pour trouver un endroit libre (pas sur une route, pas sur un bâtiment)
            for(int attempt=0; attempt<10; attempt++) {
                int col = GetRandomValue(0, vRoads.size()); 
                int row = GetRandomValue(0, hRoads.size());
                
                // Calcul des limites d'un bloc de maisons entre les routes
                float minX = (col == 0) ? SIDEBAR_WIDTH : vRoads[col-1] + ROAD_WIDTH/2;
                float maxX = (col == (int)vRoads.size()) ? GetScreenWidth() : vRoads[col] - ROAD_WIDTH/2;
                float minY = (row == 0) ? 0 : hRoads[row-1] + ROAD_WIDTH/2;
                float maxY = (row == (int)hRoads.size()) ? GetScreenHeight() : hRoads[row] - ROAD_WIDTH/2;

                Rectangle zone = { minX, minY, maxX - minX, maxY - minY };

                // Si la zone est assez grande
                if (zone.width > 20 && zone.height > 20) {
                    // On choisit un coin du bloc au hasard
                    int corner = GetRandomValue(0, 3);
                    float pad = 20.0f;
                    Vector2 candidate;
                    
                    if(corner == 0) candidate = (Vector2){ zone.x + pad, zone.y + pad }; 
                    else if(corner == 1) candidate = (Vector2){ zone.x + zone.width - pad, zone.y + pad };
                    else if(corner == 2) candidate = (Vector2){ zone.x + pad, zone.y + zone.height - pad }; 
                    else candidate = (Vector2){ zone.x + zone.width - pad, zone.y + zone.height - pad }; 

                    // Vérification finale : pas sur un bâtiment existant (Police/Hopital/Caserne)
                    bool onBuilding = false;
                    for(const auto& b : buildings) {
                        if(CheckCollisionPointRec(candidate, b.rect)) { onBuilding = true; break; }
                    }
                    // Si c'est libre, on déclenche le feu !
                    if(!onBuilding) { firePos = candidate; fireActive = true; break; }
                }
            }
        }

        // 2. Accidents de la route
        if (!accidentActive && GetRandomValue(0, 1000) < 3) {
            accidentActive = true;
            accidentPos = GetRandomRoadTarget(); // Sur une intersection
        }

        // 3. Apparition automatique des voitures civiles
        if (GetRandomValue(0, 80) == 0 && cars.size() < 35) {
            Car* newCar = new Car(CIVIL, cars);
            if(newCar->active) cars.push_back(newCar);
            else delete newCar; 
        }

        // Mise à jour de toutes les voitures (Mouvement, IA, Collisions)
        for (int i=0; i<cars.size(); i++) {
            cars[i]->Update(cycle, cars, isNight);
            // Suppression des voitures sorties de l'écran ou garées (ménage mémoire)
            if (!cars[i]->active) { delete cars[i]; cars.erase(cars.begin()+i); i--; }
        }

        // --- C. DESSIN (Rendu Graphique) ---
        BeginDrawing();
        ClearBackground(COLOR_GRASS);

        int sw = GetScreenWidth();
        int sh = GetScreenHeight();

        // 1. Routes (Asphalte)
        for(float vx : vRoads) DrawRectangle(vx-ROAD_WIDTH/2, 0, ROAD_WIDTH, sh, COLOR_ROAD);
        for(float hy : hRoads) DrawRectangle(SIDEBAR_WIDTH, hy-ROAD_WIDTH/2, sw-SIDEBAR_WIDTH, ROAD_WIDTH, COLOR_ROAD);
        
        // 2. EFFET NUIT (Filtre sombre sur le sol)
        // On le dessine APRES le sol mais AVANT les lumières
        if (isNight) {
            DrawRectangle(SIDEBAR_WIDTH, 0, sw-SIDEBAR_WIDTH, sh, Fade(BLACK, 0.7f));
        }

        // 3. Feux tricolores (Dessinés par dessus la nuit pour briller)
        for(float vx : vRoads) {
            for(float hy : hRoads) {
                DrawIntersectionLights(vx, hy, cycle, isNight);
            }
        }

        // 4. Lignes pointillées au centre des routes
        for(float vx : vRoads) DrawDashedLine((Vector2){vx, 0}, (Vector2){vx, (float)sh}, 2, COLOR_LINE);
        for(float hy : hRoads) DrawDashedLine((Vector2){(float)SIDEBAR_WIDTH, hy}, (Vector2){(float)sw, hy}, 2, COLOR_LINE);

        // 5. Bâtiments
        for(auto& b : buildings) {
            DrawLineEx(b.center, b.entryPoint, 15, DARKGRAY); // Allée de garage
            DrawRectangleRec(b.rect, b.color);
            DrawRectangleLinesEx(b.rect, 3, BLACK);
            DrawText(b.label.c_str(), b.rect.x, b.rect.y - 15, 10, isNight ? WHITE : BLACK);
            
            // Fenêtres allumées la nuit
            if (isNight) {
                DrawRectangle(b.rect.x + 10, b.rect.y + 10, 10, 10, YELLOW);
                DrawRectangle(b.rect.x + 30, b.rect.y + 10, 10, 10, YELLOW);
                DrawRectangle(b.rect.x + 10, b.rect.y + 30, 10, 10, YELLOW);
            }
        }

        // 6. Événements visuels (Feu et Sang)
        if (fireActive) {
            DrawCircleV(firePos, 30 + sin(GetTime()*10)*10, Fade(ORANGE, 0.6f)); // Halo qui bouge
            DrawCircleV(firePos, 20 + sin(GetTime()*15)*8, Fade(RED, 0.7f));    // Cœur du feu
            DrawText("FEU", firePos.x - 10, firePos.y - 10, 20, YELLOW);
        }

        if (accidentActive) {
            DrawCircle(accidentPos.x, accidentPos.y, 15, COLOR_BLOOD);
            DrawCircle(accidentPos.x+5, accidentPos.y+5, 10, COLOR_BLOOD);
            DrawCircle(accidentPos.x-5, accidentPos.y-4, 8, COLOR_BLOOD);
            DrawText("ACCIDENT", accidentPos.x - 20, accidentPos.y - 30, 15, RED);
        }

        // 7. Voitures
        for(auto c : cars) c->Draw(isNight);

        // 8. BARRE LATÉRALE (Interface utilisateur à gauche)
        DrawRectangle(0, 0, SIDEBAR_WIDTH, sh, COLOR_SIDEBAR);
        DrawRectangle(SIDEBAR_WIDTH, 0, 4, sh, BLACK); // Séparation verticale

        DrawText("URGENCES", 20, 20, 30, WHITE);
        
        // Boutons pour lancer des véhicules manuellement
        if(DrawButton((Rectangle){20, 80, 200, 40}, BLUE, WHITE, "POLICE")) {
            Car* c = new Car(POLICE, cars); if(c->active) cars.push_back(c); else delete c;
        }
        if(DrawButton((Rectangle){20, 140, 200, 40}, RED, WHITE, "POMPIERS")) {
            Car* c = new Car(FIRE, cars); if(c->active) cars.push_back(c); else delete c;
        }
        if(DrawButton((Rectangle){20, 200, 200, 40}, WHITE, BLACK, "AMBULANCE")) {
            Car* c = new Car(AMBULANCE, cars); if(c->active) cars.push_back(c); else delete c;
        }

        // Mini-carte et infos
        DrawMiniMap(cars);
        DrawText(TextFormat("Voitures: %d", (int)cars.size()), 20, sh-40, 20, GRAY);
        DrawText("MODE NUIT: [N]", 20, sh-80, 20, isNight ? YELLOW : GRAY);

        EndDrawing();
    }
    
    // Nettoyage de la mémoire avant de quitter (très important en C++)
    for(auto c : cars) delete c;
    CloseWindow();
    return 0;
}