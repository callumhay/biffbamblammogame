/**
 * MagnetPaddleEffect.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __MAGNETPADDLEEFFECT_H__
#define __MAGNETPADDLEEFFECT_H__

#include "../BlammoEngine/BasicIncludes.h"

class Texture2D;
class PlayerPaddle;

/**
 * Draws the effect for the magnet paddle in paddle space.
 */
class MagnetPaddleEffect {
public:
    MagnetPaddleEffect();
    ~MagnetPaddleEffect();

    void Reset();
    void Draw(double dT, const PlayerPaddle& p);

private:
    static const double MAX_MAGNET_LINE_TIME;
    static const int NUM_MAGNET_LINES;
    static const double TIME_DIFF_BETWEEN_MAGNET_LINES;
    static const float CIRCLE_WIDTH_TO_HEIGHT_RATIO;
    
    static const float MIN_BASE_CIRCLE_HEIGHT;
    static const float MIN_BASE_CIRCLE_WIDTH;
    static const float MAX_BASE_CIRCLE_HEIGHT;
    static const float MAX_BASE_CIRCLE_WIDTH;

    enum State { StartingUp, Running };

    State currState;

    std::vector<double> magnetLineTimeTrackers;
    Texture2D* magnetCircleTex;

    void DrawLines(float scaleFactor, float alphaMultiplier);

    float GetCircleWidthFromTime(double t, float scaleFactor);
    float GetCircleHeightFromTime(double t, float scaleFactor);
    float GetAlphaFromTime(double t);

    DISALLOW_COPY_AND_ASSIGN(MagnetPaddleEffect);
};

inline float MagnetPaddleEffect::GetCircleWidthFromTime(double t, float scaleFactor) {
    return scaleFactor * (MIN_BASE_CIRCLE_WIDTH + t * (MAX_BASE_CIRCLE_WIDTH - MIN_BASE_CIRCLE_WIDTH) / MAX_MAGNET_LINE_TIME);
}

inline float MagnetPaddleEffect::GetCircleHeightFromTime(double t, float scaleFactor) {
    return scaleFactor * (MIN_BASE_CIRCLE_HEIGHT + t * (MAX_BASE_CIRCLE_HEIGHT - MIN_BASE_CIRCLE_HEIGHT) / MAX_MAGNET_LINE_TIME);
}

inline float MagnetPaddleEffect::GetAlphaFromTime(double t) {
    return 1.0f + (t - MAX_MAGNET_LINE_TIME/2.0f) * (-1.0f) / (MAX_MAGNET_LINE_TIME - MAX_MAGNET_LINE_TIME/2.0f);
}

#endif // __MAGNETPADDLEEFFECT_H__