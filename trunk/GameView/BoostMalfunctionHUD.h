/**
 * BoostMalfunctionHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BOOSTMALFUNCTIONHUD_H__
#define __BOOSTMALFUNCTIONHUD_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/TextLabel.h"
#include "../BlammoEngine/Colour.h"

#include "../ESPEngine/ESP.h"

class Camera;
class Texture2D;
class GameSound;

class BoostMalfunctionHUD {
public:
    BoostMalfunctionHUD(GameSound* sound);
    ~BoostMalfunctionHUD();

    void Draw(double dT, const Camera& camera);

    void Activate();
    void Deactivate();
    void Reset();

    bool GetIsActive() const;

private:
    static const float ICON_TEXT_X_GAP;
    static const char* WARNING_TEXT;
    static const double TIME_UNTIL_BOOST_MALFUNCTON_DEACTIVATION;
    static const float STATIC_SCALE;

    GameSound* sound;

    bool isActive;
    double timeSinceLastBoostMalfunction;
    float fullTextWidth;
    float fullTextHeight;

    std::string malfunctionText;
    AnimationLerp<float> textAnim;

    Texture2D* malfunctionIconTex;
    TextLabel2DFixedWidth malfunctionLabel;

    AnimationMultiLerp<ColourRGBA> flashAnim;
    AnimationLerp<float> iconScaleAnim;

    AnimationLerp<float> fadeAnim;

    DISALLOW_COPY_AND_ASSIGN(BoostMalfunctionHUD);
};

inline bool BoostMalfunctionHUD::GetIsActive() const {
    return this->isActive;
}

#endif // __BOOSTMALFUNCTIONHUD_H__