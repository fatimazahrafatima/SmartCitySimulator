#ifndef WORLD_H
#define WORLD_H

#include "config.h"

// --- VARIABLES PARTAGÉES (GLOBALES) ---
// Le mot "extern" est une étiquette qui dit au programme :
// "Ces listes existent réellement dans world.cpp, mais on les déclare ici
// pour que tout le monde sache qu'elles existent."

extern std::vector<float> vRoads;       // La liste des positions (X) de toutes les routes verticales
extern std::vector<float> hRoads;       // La liste des positions (Y) de toutes les routes horizontales
extern std::vector<Building> buildings; // La liste de tous les bâtiments posés sur la carte

// --- FONCTIONS (OUTILS) ---

// Outil mathématique : Trouve la route la plus proche d'une position donnée.
// Ça sert à "aimanter" les voitures pour qu'elles restent bien au milieu de leur voie.
float GetSnapAxis(float val, const std::vector<float>& axes);

// Outil graphique : Dessine une ligne pointillée (le marquage au sol jaune/blanc)
void DrawDashedLine(Vector2 start, Vector2 end, float thick, Color color);

// LA FONCTION MAJEURE : C'est l'architecte.
// Elle efface tout et reconstruit la ville, les routes et les bâtiments.
// On l'utilise au lancement du jeu ou quand on change la taille de la fenêtre.
void RecalculateGrid();

// Outil de hasard : Trouve un point aléatoire sur une route.
// C'est utilisé pour décider où va se déclencher le prochain incendie ou accident.
Vector2 GetRandomRoadTarget();

#endif