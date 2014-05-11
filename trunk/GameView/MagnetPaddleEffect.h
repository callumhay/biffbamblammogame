/**
 * MagnetPaddleEffect.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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