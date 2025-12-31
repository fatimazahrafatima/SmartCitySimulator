#ifndef TRAFFIC_SYSTEM_H
#define TRAFFIC_SYSTEM_H

#include "config.h"

// --- VARIABLES GLOBALES (EXTERNES) ---
// Le mot "extern" signifie que ces boîtes de mémoire existent réellement ailleurs (dans world.cpp),
// mais ce fichier a besoin de connaître leur existence pour vérifier s'il y a une urgence.

extern bool fireActive;      // Est-ce qu'il y a un incendie en cours ?
extern Vector2 firePos;      // Si oui, à quel endroit précis (X, Y) ?

extern bool accidentActive;  // Est-ce qu'il y a un accident de voiture ?
extern Vector2 accidentPos;  // Si oui, où ça ?

// --- FONCTIONS ---

// Cette fonction dessine les feux tricolores (rouge/vert) à un croisement précis.
// Elle a besoin de savoir où c'est (x, y), quel feu est vert (cycle), et s'il fait nuit.
void DrawIntersectionLights(float x, float y, LightCycle cycle, bool isNight);

#endif