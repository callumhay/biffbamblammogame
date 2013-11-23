/**
 * ESPAnimatedSpriteParticle.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ESPANIMATEDSPRITEPARTICLE_H__
#define __ESPANIMATEDSPRITEPARTICLE_H__

#include "ESPParticle.h"

class ESPAnimatedSpriteParticle : public ESPParticle {
public:
	ESPAnimatedSpriteParticle(const Texture2D* spriteTex, int perSpriteXSize, int perSpriteYSize, double fps);
	~ESPAnimatedSpriteParticle();

	void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	void Tick(const double dT);
	void Draw(const Camera& camera, const ESP::ESPAlignment& alignment);

private:
    const Texture2D* spriteTex;
    int spriteXSize, spriteYSize;
    double timePerFrame;
    double currFrameTimeCounter;
    int numSprites;
    int currSpriteIdx;

    int numSpriteRows;
    int numSpriteCols;
    
    void GetCurrAnimationTextureCoords(float& leftTexCoord, float& rightTexCoord, float& topTexCoord, float& bottomTexCoord) const;
    
    DISALLOW_COPY_AND_ASSIGN(ESPAnimatedSpriteParticle);
};

#endif // __ESPANIMATEDSPRITEPARTICLE_H__