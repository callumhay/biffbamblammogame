/**
 * RemoteControlRocketHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __REMOTECONTROLROCKETHUD_H__
#define __REMOTECONTROLROCKETHUD_H__

#include "ButtonTutorialHint.h"
#include "CountdownHUD.h"

class GameAssets;
class Camera;
class PaddleRemoteControlRocketProjectile;

class RemoteControlRocketHUD {
public:
    RemoteControlRocketHUD(GameAssets& assets);
    ~RemoteControlRocketHUD();

    void Draw(double dT, const Camera& camera);

    void Activate(const PaddleRemoteControlRocketProjectile* rocket);
    void Deactivate();
    void Reinitialize();

private:
    static const float MOVE_HINT_BOTTOM_FROM_SCREEN_BOTTOM;
    const PaddleRemoteControlRocketProjectile* rocket;
    bool isActive;

    CountdownHUD rocketExplodeCountdown;
    ButtonTutorialHint moveHint;
    ButtonTutorialHint thrustHint;

    //RocketFuelGaugeHUD fuelGauge;

    DISALLOW_COPY_AND_ASSIGN(RemoteControlRocketHUD);
};

#endif // __REMOTECONTROLROCKETHUD_H__