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