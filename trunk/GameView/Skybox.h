/**
 * Skybox.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "CgFxSkybox.h"

class Skybox {
public:
    Skybox();
    ~Skybox();

    void SetFGStarMoveSpd(float spd);

    void Draw(const Camera& camera);

private:
	GLuint skyboxDispList;
	CgFxSkybox skyboxEffect;

    static void DrawSkyboxGeometry(int texTiling = 1, float width = 0.5f);

    DISALLOW_COPY_AND_ASSIGN(Skybox);
};

inline void Skybox::SetFGStarMoveSpd(float spd) {
    this->skyboxEffect.SetMoveFrequency(spd);
}

#endif // __SKYBOX_H__