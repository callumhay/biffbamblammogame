/**
 * OmniLaserBallEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __OMNILASERBALLEFFECT_H__
#define __OMNILASERBALLEFFECT_H__

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Animation.h"

class Camera;
class GameBall;

class OmniLaserBallEffect {
public:
    OmniLaserBallEffect();
    ~OmniLaserBallEffect();

    void Draw(double dT, const Camera& camera, const GameBall& ball);

private:
    AnimationMultiLerp<float> rotAngleAnim;
    Texture2D* sentryTexture;
    //Texture2D* flareTexture;

    void DrawSentries(const GameBall& ball, float size);

    DISALLOW_COPY_AND_ASSIGN(OmniLaserBallEffect);
};

#endif // __OMNILASERBALLEFFECT_H__