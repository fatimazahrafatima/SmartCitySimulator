#ifndef VEHICLE_H
#define VEHICLE_H

#include "config.h"

// La classe "Car" est le plan de construction pour toutes nos voitures.
// Elle contient tout ce qu'une voiture sait (variables) et tout ce qu'elle sait faire (fonctions).
class Car {
public:
    // --- POSITION ET MOUVEMENT ---
    Vector2 pos;        // Position actuelle (X, Y) sur l'écran
    Dir dir;            // Direction vers laquelle elle regarde (Haut, Bas, Gauche, Droite)
    Type type;          // Son métier : CIVIL, POLICE, AMBULANCE ou POMPIER
    float speed;        // Vitesse actuelle
    float maxSpeed;     // Vitesse maximum autorisée pour ce véhicule
    bool active;        // Si "false", la voiture est sortie de l'écran et doit être supprimée

    // --- NAVIGATION (GPS) ---
    Vector2 target;     // La destination précise où elle essaie d'aller
    bool hasTarget;     // Est-ce qu'elle a une destination définie ? (Vrai/Faux)
    float stuckTimer;   // Chrono pour détecter si la voiture est coincée dans un mur ou un bouchon
    float turnCooldown; // Petit délai pour l'empêcher de changer de direction trop vite (éviter qu'elle tremble)
    float actionTimer;  // Compte le temps d'une intervention (ex: temps pour éteindre un feu)

    // --- GESTION DES URGENCES ---
    EmergencyState emState; // État du cerveau (Au repos, En route, Sur place, Rentre à la base...)
    Vector2 homeCenter;     // Le centre de son bâtiment de base (Commissariat, Hôpital...)
    Vector2 homeEntry;      // Le point précis où elle doit entrer pour se garer
    
    bool isYielding;        // Vrai si c'est une voiture civile qui se range sur le côté pour laisser passer les secours

    // --- FONCTIONS (ACTIONS) ---

    // Constructeur : C'est la fonction appelée quand on crée une nouvelle voiture ("new Car")
    Car(Type t, const std::vector<Car*>& existingCars);

    // Renvoie le rectangle physique de la voiture (utile pour savoir si on touche quelque chose)
    Rectangle GetRect() const;

    // Outil de calcul (statique) pour obtenir un rectangle sans avoir besoin de l'objet voiture complet
    static Rectangle GetRectInternal(Vector2 p, Dir d);

    // Renvoie la zone devant la voiture (ses "yeux") pour détecter les obstacles ou feux rouges
    Rectangle GetSensor() const;

    // LE CERVEAU : C'est ici que tout se décide (avancer, freiner, tourner...)
    // Appelée 60 fois par seconde.
    void Update(LightCycle cycle, const std::vector<Car*>& cars, bool isNight);

    // L'AFFICHAGE : C'est ici qu'on dessine le rectangle coloré et les phares
    void Draw(bool isNight);
};

#endif