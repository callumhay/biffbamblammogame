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

class GameAssets;
class Camera;

class RemoteControlRocketHUD {
public:
    RemoteControlRocketHUD(GameAssets& assets);
    ~RemoteControlRocketHUD();

    void Draw(double dT, const Camera& camera);

    void Activate();
    void Deactivate();
    void Reinitialize();

private:
    static const float MOVE_HINT_BOTTOM_FROM_SCREEN_BOTTOM;
    bool isActive;

    ButtonTutorialHint moveHint;
    //ButtonTutorialHint* detonateHint;

    //RocketFuelGaugeHUD fuelGauge;

    DISALLOW_COPY_AND_ASSIGN(RemoteControlRocketHUD);
};

#endif // __REMOTECONTROLROCKETHUD_H__