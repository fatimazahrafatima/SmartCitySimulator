/**
 * VÉHICULE (LOGIQUE ET INTELLIGENCE ARTIFICIELLE)
 * Ce fichier gère tout : le mouvement, les collisions, les missions de secours,
 * les feux rouges et le dessin des voitures.
 */

#include "../include/vehicle.h"
#include "../include/world.h"
#include "../include/traffic_system.h"

// --- CONSTRUCTEUR ---
// C'est ici qu'une voiture naît.
// Si c'est une voiture de SECOURS, elle apparaît dans son garage.
// Si c'est une voiture CIVILE, elle apparaît au hasard au bord de l'écran.
Car::Car(Type t, const std::vector<Car*>& existingCars) {
    type = t;
    active = true;
    stuckTimer = 0;
    turnCooldown = 0;
    actionTimer = 0;
    isYielding = false; // Par défaut, on ne se gare pas sur le côté
    
    // Vitesse : Les secours vont beaucoup plus vite que les civils
    maxSpeed = (type == CIVIL) ? 1.4f : 4.0f; 
    
    speed = maxSpeed;
    hasTarget = false;
    emState = IDLE; // État "Au repos"
    
    // --- LOGIQUE D'APPARITION DES SECOURS ---
    if (type != CIVIL) {
        bool found = false;
        // On cherche le bâtiment qui correspond au véhicule (ex: Camion Pompier -> Caserne)
        for (const auto& b : buildings) {
            if (b.type == type) { 
                homeCenter = b.center;      // Centre du bâtiment
                homeEntry = b.entryPoint;   // Sortie du garage
                pos = homeCenter;           // On place la voiture DANS le bâtiment
                
                // Si une urgence est déjà en cours, on lui donne l'ordre d'y aller direct
                if (type == FIRE && fireActive) target = firePos;
                else if (type == AMBULANCE && accidentActive) target = accidentPos; 
                else target = homeEntry;    // Sinon, elle sort juste devant
                
                hasTarget = true; 
                emState = DEPLOYING; // État "Sortie du garage"
                found = true;
                break; 
            }
        }
        // Si on n'a pas trouvé de bâtiment pour ce véhicule, on annule la création
        if(!found) { active = false; return; }
        dir = DOWN; // Par défaut vers le bas pour sortir
    } 
    // --- LOGIQUE D'APPARITION DES CIVILS ---
    else {
        bool spawned = false;
        // On essaie 15 fois de trouver une place libre pour ne pas apparaître SUR une autre voiture
        for(int attempt=0; attempt<15; attempt++){ 
            // 50% de chance d'apparaître sur une route Verticale (Haut/Bas)
            if (GetRandomValue(0, 1) == 0 && !vRoads.empty()) { 
                int r = GetRandomValue(0, vRoads.size() - 1); // Choix de la route au hasard
                dir = (GetRandomValue(0, 1) == 0) ? DOWN : UP; // Sens de circulation
                // Calcul de la position X et Y (hors de l'écran)
                pos = { vRoads[r] + ((dir==DOWN)?LANE_NORMAL:-LANE_NORMAL), (dir==DOWN)? -90.0f : (float)GetScreenHeight() + 90 }; 
            } 
            // 50% de chance d'apparaître sur une route Horizontale (Gauche/Droite)
            else if (!hRoads.empty()) { 
                int r = GetRandomValue(0, hRoads.size() - 1);
                dir = (GetRandomValue(0, 1) == 0) ? RIGHT : LEFT;
                pos = { (dir==RIGHT)? -90.0f : (float)GetScreenWidth() + 90, hRoads[r] + ((dir==RIGHT)?LANE_NORMAL:-LANE_NORMAL) }; 
            }

            // On vérifie si la place est libre
            Rectangle myRect = GetRectInternal(pos, dir);
            // On élargit un peu la zone de vérification pour laisser de l'espace
            myRect.x -= 70; myRect.y -= 70; myRect.width += 140; myRect.height += 140;

            bool collision = false;
            for(auto c : existingCars) {
                if(CheckCollisionRecs(myRect, c->GetRect())) { collision = true; break; }
            }
            // Si pas de collision, c'est bon, on valide !
            if(!collision) { spawned = true; break; }
        }
        // Si après 15 essais on n'a pas trouvé de place, la voiture n'est pas créée
        if(!spawned) active = false;
    }
}

// Renvoie le rectangle physique de la voiture
Rectangle Car::GetRect() const { return GetRectInternal(pos, dir); }

// Calcul interne de la taille de la voiture (plus longue que large) selon la direction
Rectangle Car::GetRectInternal(Vector2 p, Dir d) {
    if (d == UP || d == DOWN) return { p.x - 8, p.y - 13, 16, 26 }; 
    return { p.x - 13, p.y - 8, 26, 16 };
}

// --- CAPTEUR (LES YEUX) ---
// Crée une zone invisible devant la voiture pour détecter les obstacles
Rectangle Car::GetSensor() const {
    // Plus on va vite, plus on regarde loin devant (distance de freinage)
    float lookAhead = 70.0f + (speed * 25.0f); 
    float width = (type != CIVIL) ? 8.0f : 14.0f; // Largeur du capteur
    
    // On place le rectangle devant selon la direction
    if (dir == UP)    return { pos.x - width/2, pos.y - 13 - lookAhead, width, lookAhead };
    if (dir == DOWN)  return { pos.x - width/2, pos.y + 13, width, lookAhead };
    if (dir == LEFT)  return { pos.x - 13 - lookAhead, pos.y - width/2, lookAhead, width };
    if (dir == RIGHT) return { pos.x + 13, pos.y - width/2, lookAhead, width };
    return {0,0,0,0};
}

// --- CERVEAU PRINCIPAL (UPDATE) ---
// Exécuté à chaque image (60 fois par seconde)
void Car::Update(LightCycle cycle, const std::vector<Car*>& cars, bool isNight) {
    if (!active) return; // Si la voiture est désactivée, on ne fait rien
    float dt = GetFrameTime(); // Temps écoulé depuis la dernière image
    if (turnCooldown > 0) turnCooldown -= dt; // On réduit le chrono de virage
    
    // --- GESTION DES MISSIONS (POMPIERS) ---
    if (type == FIRE) {
            // Si on est en route vers le feu
            if (emState == ON_MISSION) {
                if (!fireActive) { emState = RETURNING; target = homeEntry; } // Fausse alerte, on rentre
                else if (Vector2Distance(pos, firePos) < 70.0f) emState = EXTINGUISHING; // Arrivé ! On éteint.
            }
            // Si on est en train d'éteindre
            if (emState == EXTINGUISHING) {
                speed = Lerp(speed, 0.0f, 0.1f); // On s'arrête
                if (fireActive) {
                    actionTimer += dt; // On arrose pendant 3 secondes
                    if (actionTimer > 3.0f) { fireActive = false; emState = RETURNING; target = homeEntry; actionTimer = 0; }
                } else { emState = RETURNING; target = homeEntry; }
                return; // On ne bouge plus pendant qu'on éteint
            }
    }
    // --- GESTION DES MISSIONS (AMBULANCES) ---
    if (type == AMBULANCE) {
        if (emState == ON_MISSION) {
            if (!accidentActive) { emState = RETURNING; target = homeEntry; } 
            else if (Vector2Distance(pos, accidentPos) < 30.0f) emState = TREATING; // Arrivé ! On soigne.
        }
        if (emState == TREATING) {
            speed = Lerp(speed, 0.0f, 0.2f); // On s'arrête
            if (accidentActive) {
                actionTimer += dt; // On soigne pendant 3 secondes
                if (actionTimer > 3.0f) { accidentActive = false; emState = RETURNING; target = homeEntry; actionTimer = 0; }
            } else { emState = RETURNING; target = homeEntry; }
            return; 
        }
    }
    
    // --- SORTIE ET ENTRÉE DU GARAGE ---
    // Logique pour sortir proprement du bâtiment (DEPLOYING)
    if (emState == DEPLOYING) {
        if (Vector2Distance(pos, homeEntry) < 8.0f) {
            pos = homeEntry; emState = ON_MISSION;
            // Une fois sorti, on se place sur la bonne voie de la route la plus proche
            float cx = GetSnapAxis(pos.x, vRoads); float cy = GetSnapAxis(pos.y, hRoads);
            if (fabs(pos.x-cx) < fabs(pos.y-cy)) { dir=(pos.y<GetScreenHeight()/2)?DOWN:UP; pos.x=cx+((dir==DOWN)?LANE_NORMAL:-LANE_NORMAL); }
            else { dir=(pos.x<GetScreenWidth()/2)?RIGHT:LEFT; pos.y=cy+((dir==RIGHT)?LANE_NORMAL:-LANE_NORMAL); }
        } else { Vector2 diff = Vector2Subtract(homeEntry, pos); pos = Vector2Add(pos, Vector2Scale(Vector2Normalize(diff), 2.5f)); }
        return;
    }
    // Logique pour rentrer se garer (DOCKING)
    if (emState == DOCKING) {
        Vector2 diff = Vector2Subtract(homeCenter, pos);
        if (Vector2Length(diff) < 5.0f) active = false; // Garé ! On disparaît (active = false)
        else pos = Vector2Add(pos, Vector2Scale(Vector2Normalize(diff), 2.5f));
        return; 
    }
    // Si on est proche de l'entrée au retour, on passe en mode DOCKING
    if (emState == RETURNING && Vector2Distance(pos, homeEntry) < 20) emState = DOCKING;
    
    // Mise à jour de la cible (Target) si l'urgence se déplace ou change
    if (type != CIVIL && emState == ON_MISSION) {
            if (type == FIRE && fireActive) target = firePos;
            else if (type == AMBULANCE && accidentActive) target = accidentPos;
            else if (Vector2Distance(pos, target) < 30) { emState = RETURNING; target = homeEntry; }
    }

    // --- NAVIGATION GPS ---
    // On repère sur quelle route on est
    float currentRoadX = GetSnapAxis(pos.x, vRoads);
    float currentRoadY = GetSnapAxis(pos.y, hRoads);
    // On vérifie si on est au milieu d'un carrefour
    bool atIntersection = (fabs(pos.x - currentRoadX) < 20.0f && fabs(pos.y - currentRoadY) < 20.0f);

    if (atIntersection && turnCooldown <= 0.0f) {
        Dir newDir = dir;
        bool turnNeeded = false;
        
        // --- INTELLIGENCE DE DIRECTION ---
        // Si on a une destination précise (Secours)
        if (hasTarget || (type != CIVIL && emState != IDLE)) {
            float tDx = target.x - currentRoadX; float tDy = target.y - currentRoadY;
            // On décide de tourner si la cible n'est pas en face
            if (dir == LEFT || dir == RIGHT) { if (fabs(tDx) < 10.0f) { newDir = (target.y > pos.y) ? DOWN : UP; turnNeeded = true; } } 
            else { if (fabs(tDy) < 10.0f) { newDir = (target.x > pos.x) ? RIGHT : LEFT; turnNeeded = true; } }
            // Correction de trajectoire simple
            if (!turnNeeded) {
                if ((dir==LEFT||dir==RIGHT) && fabs(tDy) > fabs(tDx)) newDir = (tDy > 0)?DOWN:UP;
                else if ((dir==UP||dir==DOWN) && fabs(tDx) > fabs(tDy)) newDir = (tDx > 0)?RIGHT:LEFT;
            }
        } 
        // Si on est un Civil (Balade au hasard)
        else if (type == CIVIL) {
            // 25% de chance de tourner à chaque intersection
            if (GetRandomValue(0, 100) < 25) { 
                if (dir == UP || dir == DOWN) newDir = (GetRandomValue(0,1)) ? LEFT : RIGHT;
                else newDir = (GetRandomValue(0,1)) ? UP : DOWN;
            }
        }

        // Si on change de direction, on ajuste la position pour bien prendre le virage
        if (newDir != dir) {
            dir = newDir;
            float offset = (dir == DOWN || dir == RIGHT) ? LANE_NORMAL : -LANE_NORMAL;
            if (dir == UP || dir == DOWN) { pos.x = currentRoadX + offset; pos.y = currentRoadY; }
            else { pos.x = currentRoadX; pos.y = currentRoadY + offset; }
            turnCooldown = 0.8f; // On attend un peu avant de pouvoir re-tourner
        }
    }

    float desiredSpeed = maxSpeed;
    bool hardStop = false; 

    // --- RÈGLE : LAISSER PASSER LES SECOURS ---
    isYielding = false; 
    if (type == CIVIL) {
        // On regarde s'il y a un véhicule d'urgence en mission pas loin
        for(auto c : cars) {
            if (c->type != CIVIL && c->emState == ON_MISSION && c->active) {
                if (Vector2Distance(pos, c->pos) < 250.0f) { 
                    isYielding = true; // On active le mode "Se garer"
                }
            }
        }
    }
    if (isYielding) {
        desiredSpeed = 1.2f; // On ralentit pour se garer
    }

    // --- RÈGLE : FEUX TRICOLORES (Seulement pour Civils qui ne cèdent pas le passage) ---
    if (type == CIVIL && !isYielding) { 
        bool redLight = false;
        // On vérifie si le feu est rouge pour nous
        if ((dir == UP || dir == DOWN) && cycle != V_GREEN) redLight = true;
        if ((dir == LEFT || dir == RIGHT) && cycle != H_GREEN) redLight = true;
        
        if (redLight) {
            // Calcul de la distance jusqu'au feu
            float distToCenterX = fabs(pos.x - currentRoadX);
            float distToCenterY = fabs(pos.y - currentRoadY);
            float dist = (dir == UP || dir == DOWN) ? distToCenterY : distToCenterX;
            
            // On vérifie qu'on arrive bien VERS le feu (pas qu'on vient de le passer)
            bool approaching = false;
            if(dir==DOWN && pos.y < currentRoadY) approaching = true;
            if(dir==UP && pos.y > currentRoadY) approaching = true;
            if(dir==RIGHT && pos.x < currentRoadX) approaching = true;
            if(dir==LEFT && pos.x > currentRoadX) approaching = true;

            // Si on approche du feu rouge, on s'arrête
            if (approaching && dist > 40.0f && dist < 95.0f) {
                desiredSpeed = 0.0f;
            }
        }
    }

    // --- SYSTÈME ANTI-COLLISION ---
    Rectangle mySensor = GetSensor(); // On récupère la zone devant nous
    for(auto c : cars) {
        if (c == this || !c->active) continue; // On ne se teste pas soi-même
        if (type == CIVIL && isYielding && c->dir != dir) continue; // Si on se gare, on ignore ceux d'en face

        // Si notre capteur touche une autre voiture
        if (CheckCollisionRecs(mySensor, c->GetRect())) {
            if (!isYielding) {
                desiredSpeed = 0.0f; // On veut s'arrêter
                // Si on est très près, FREINAGE D'URGENCE (Hard Stop)
                if (Vector2Distance(pos, c->pos) < 55.0f) {
                    hardStop = true;
                    speed = 0.0f; 
                } else if (Vector2Distance(pos, c->pos) < 80.0f) {
                    hardStop = true; 
                }
            } else {
                // Si on est en train de se garer, on freine aussi si on touche quelqu'un
                if (Vector2Distance(pos, c->pos) < 35.0f) desiredSpeed = 0.0f;
            }
        }
    }

    // --- APPLICATION DE LA VITESSE ---
    if (hardStop || desiredSpeed == 0.0f) {
        speed = Lerp(speed, 0.0f, 0.3f); // Freinage rapide
        if (speed < 0.05f) speed = 0.0f; // Arrêt complet
    } else {
        speed = Lerp(speed, desiredSpeed, 0.05f); // Accélération douce
    }

    // --- MAINTIEN DE LA VOIE (LANE KEEPING) ---
    float offsetMagnitude = LANE_NORMAL;
    bool invertSide = false;

    // Si on doit laisser passer, on se décale plus loin (LANE_CIVIL_YIELD = 32.0f)
    if (type == CIVIL && isYielding) {
        offsetMagnitude = 22.0f; 
        invertSide = true;       // On se range vers l'extérieur (trottoir)
    }
    // Les secours roulent au milieu (voie prioritaire)
    if (type != CIVIL && emState == ON_MISSION) offsetMagnitude = LANE_EMERGENCY;

    // Calcul du décalage exact (gauche ou droite de la ligne jaune)
    float targetOffset = (dir == DOWN || dir == RIGHT) ? offsetMagnitude : -offsetMagnitude;
    if (invertSide) targetOffset = -targetOffset;

    // --- MOUVEMENT PHYSIQUE ---
    if (dir == UP)    pos.y -= speed;
    if (dir == DOWN)  pos.y += speed;
    if (dir == LEFT)  pos.x -= speed;
    if (dir == RIGHT) pos.x += speed;

    // Aimentation douce vers le centre de la voie (pour corriger les écarts)
    float lockStrength = 0.15f; 
    if (dir == UP || dir == DOWN) pos.x = Lerp(pos.x, currentRoadX + targetOffset, lockStrength);
    else pos.y = Lerp(pos.y, currentRoadY + targetOffset, lockStrength);

    // --- SUPPRESSION HORS ÉCRAN ---
    if (!hasTarget) {
        // Si on sort de l'écran très loin, on supprime la voiture pour libérer la mémoire
        if (pos.x < SIDEBAR_WIDTH - 100 || pos.x > GetScreenWidth() + 100 || 
            pos.y < -100 || pos.y > GetScreenHeight() + 100) active = false;
    } else {
        // "Teleport" pour effet pac-man (si nécessaire) ou bloquer aux murs pour les secours
            if(pos.x < SIDEBAR_WIDTH) { pos.x = SIDEBAR_WIDTH+2; dir=RIGHT; }
            if(pos.x > GetScreenWidth()) { pos.x = GetScreenWidth()-2; dir=LEFT; }
            if(pos.y < 0) { pos.y = 2; dir=DOWN; }
            if(pos.y > GetScreenHeight()) { pos.y = GetScreenHeight()-2; dir=UP; }
    }
}

// --- AFFICHAGE (DESSIN) ---
void Car::Draw(bool isNight) {
    // Choix de la couleur
    Color c = BLUE;
    if (type == POLICE) c = SKYBLUE;
    else if (type == AMBULANCE) c = WHITE;
    else if (type == FIRE) c = RED;
    
    Rectangle r = GetRect();
    
    // --- PHARES (HEADLIGHTS) SI NUIT ---
    if (isNight && active) {
        Color lightColor = Fade(YELLOW, 0.3f);
        if (type == POLICE) lightColor = Fade(BLUE, 0.4f); // Phares bleutés pour la police
        
        float beamL = 150.0f; // Longueur du faisceau
        float beamW = 40.0f;  // Largeur du faisceau
        Vector2 p1, p2, p3;
        
        // Calcul du triangle de lumière selon la direction
        if(dir == UP) {
            p1 = {pos.x, pos.y - 15};
            p2 = {pos.x - beamW, pos.y - beamL};
            p3 = {pos.x + beamW, pos.y - beamL};
        } else if(dir == DOWN) {
            p1 = {pos.x, pos.y + 15};
            p2 = {pos.x - beamW, pos.y + beamL};
            p3 = {pos.x + beamW, pos.y + beamL};
        } else if(dir == LEFT) {
            p1 = {pos.x - 15, pos.y};
            p2 = {pos.x - beamL, pos.y - beamW};
            p3 = {pos.x - beamL, pos.y + beamW};
        } else {
            p1 = {pos.x + 15, pos.y};
            p2 = {pos.x + beamL, pos.y - beamW};
            p3 = {pos.x + beamL, pos.y + beamW};
        }
        DrawTriangle(p1, p2, p3, lightColor);
    }
    // -----------------------------------

    // Si on se range sur le côté, on dessine un cadre orange autour
    if (isYielding) DrawRectangleLinesEx({r.x-2, r.y-2, r.width+4, r.height+4}, 1, ORANGE);

    // Dessin de l'ombre portée (pour effet 3D)
    DrawRectangle(r.x+3, r.y+3, r.width, r.height, Fade(BLACK, 0.3f)); 
    // Dessin de la carrosserie
    DrawRectangleRec(r, c);
    DrawRectangleLinesEx(r, 1, BLACK); // Contour noir

    // --- FEUX DE FREINAGE (STOP) ---
    // Si on ralentit, on allume les feux rouges arrière
    if (speed < maxSpeed * 0.8f) { 
        if(dir==UP) DrawRectangle(r.x, r.y+r.height, r.width, 2, RED);
        if(dir==DOWN) DrawRectangle(r.x, r.y-2, r.width, 2, RED);
        if(dir==LEFT) DrawRectangle(r.x+r.width, r.y, 2, r.height, RED);
        if(dir==RIGHT) DrawRectangle(r.x-2, r.y, 2, r.height, RED);
    }

    // --- GYROPHARES ET SIRÈNES (SECOURS) ---
    if(type != CIVIL && (int)(GetTime()*15)%2==0 && emState != DOCKING) {
        // Clignotement orange sur le toit
        DrawCircleV(pos, 4, ORANGE);
        // Onde de choc visuelle (cercle rouge qui s'agrandit)
        DrawCircleLines(pos.x, pos.y, 20 + sin(GetTime()*10)*5, Fade(RED, 0.5f));
    }

    // --- ANIMATIONS SPÉCIALES ---
    // Jet d'eau pour les pompiers
    if (type == FIRE && emState == EXTINGUISHING) DrawLineEx(pos, firePos, 4, Fade(SKYBLUE, 0.7f));
    // Croix verte clignotante pour l'ambulance qui soigne
    if (type == AMBULANCE && emState == TREATING) {
            if((int)(GetTime()*10)%2==0) {
                DrawLine(pos.x-5, pos.y, pos.x+5, pos.y, GREEN);
                DrawLine(pos.x, pos.y-5, pos.x, pos.y+5, GREEN);
            }
    }
}