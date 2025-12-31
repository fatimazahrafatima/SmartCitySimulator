#ifndef CONFIG_H
#define CONFIG_H

// --- BIBLIOTHÈQUES ---
// On inclut les outils nécessaires (Raylib pour l'affichage, Maths pour les calculs)
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cmath>
#include <string>

// --- DIMENSIONS DE L'ÉCRAN ---
const int INITIAL_SCREEN_WIDTH = 1300; // Largeur de la fenêtre
const int INITIAL_SCREEN_HEIGHT = 700; // Hauteur de la fenêtre
const int SIDEBAR_WIDTH = 250;         // Largeur du menu gris à gauche

// --- CONFIGURATION DES ROUTES ---
const float ROAD_WIDTH = 70.0f; // Largeur visuelle de la route

// --- POSITION DES VOIES (LANES) ---
// Distance par rapport au centre de la route pour placer les voitures
const float LANE_NORMAL = 18.0f;        // Voie de circulation normale
const float LANE_CIVIL_YIELD = 32.0f;   // Voie sur le côté (quand on laisse passer)
const float LANE_EMERGENCY = 6.0f;      // Voie centrale (prioritaire)

const float TARGET_BLOCK_SIZE = 220.0f; // Taille idéale d'un pâté de maisons

// --- COULEURS ---
// Définition de nos propres couleurs pour rendre le code plus lisible
#define COLOR_GRASS       (Color){ 34, 139, 34, 255 }   // Vert gazon
#define COLOR_ROAD        (Color){ 30, 30, 30, 255 }    // Gris bitume
#define COLOR_LINE        (Color){ 255, 215, 0, 200 }   // Jaune (lignes)
#define COLOR_SIDEBAR     (Color){ 45, 45, 45, 255 }    // Gris foncé (menu)
#define COLOR_BLOOD       (Color){ 180, 0, 0, 230 }     // Rouge (accident)

// --- ÉNUMÉRATIONS (CHOIX POSSIBLES) ---

// État général du programme : Soit on est dans le Menu, soit on Joue
enum GameState { MENU, GAME };

// Directions possibles pour les voitures
enum Dir { UP, DOWN, LEFT, RIGHT, NONE }; 

// Types d'unités (Voitures ou Bâtiments)
enum Type { CIVIL, POLICE, AMBULANCE, FIRE };

// Cycles des feux tricolores (Vert Vertical, Jaune Vertical, etc.)
enum LightCycle { V_GREEN, V_YELLOW, H_GREEN, H_YELLOW };

// Cerveau des urgences : Que fait le véhicule de secours ?
// (Repos, Sort du garage, En mission, Éteint le feu, Soigne, Retourne, Rentre au garage)
enum EmergencyState { IDLE, DEPLOYING, ON_MISSION, EXTINGUISHING, TREATING, RETURNING, DOCKING };

// --- STRUCTURES (OBJETS) ---

// Définition de ce qu'est un Bâtiment dans notre monde
struct Building {
    Rectangle rect;      // La forme physique (position et taille)
    Type type;           // Le type (Police, Pompier...)
    Color color;         // La couleur d'affichage
    std::string label;   // Le texte écrit dessus (ex: "POLICE")
    Vector2 entryPoint;  // Le point précis où les voitures sortent sur la route
    Vector2 center;      // Le centre du bâtiment (pour viser)
};

#endif