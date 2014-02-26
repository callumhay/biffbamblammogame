/**
 * StickyPaddleGoo.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __STICKYPADDLEGOO_H__
#define __STICKYPADDLEGOO_H__

#include "../BlammoEngine/Mesh.h"

#include "../GameModel/PlayerPaddle.h"

#include "CgFxStickyPaddle.h"
#include "CgFxPostRefract.h"

class Camera;
class PointLight;
class Texture2D;

/**
 * Helper class for holding the mesh, materials and effects responsible for
 * the visualization of the sticky paddle goo (stuff that sits ontop of the
 * sticky paddle when the user has a sticky paddle power-up active).
 */
class StickyPaddleGoo {
private:
	Mesh* paddleGooMesh;
	CgFxStickyPaddle* stickyGooMatEffect;
    CgFxPostRefract* invisibleEffect;

	void LoadMesh();

public:
	StickyPaddleGoo();
	~StickyPaddleGoo();
 
    /**
	 * Draw the sticky goo - basically just wraps the draw method for the goo mesh.
	 */
	inline void StickyPaddleGoo::Draw(const PlayerPaddle& p, const Camera& camera, const BasicPointLight& keyLight, 
									  const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
		
        float paddleScaleFactor = p.GetPaddleScaleFactor();
        this->stickyGooMatEffect->SetAlpha(p.GetAlpha());	

        glPushMatrix();
		glRotatef(p.GetZRotation(), 0.0f, 0.0f, 1.0f);
		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
        glColor4f(1.0f, 1.0f, 1.0f, p.GetAlpha());

        if (p.HasPaddleType(PlayerPaddle::InvisiPaddle)) {
            this->paddleGooMesh->Draw(camera, this->invisibleEffect, keyLight, fillLight, ballLight);
        }
        else {
		    this->paddleGooMesh->Draw(camera, keyLight, fillLight, ballLight);
        }
        glPopMatrix();
	}

	/**
	 * Set the scene texture for the material used to draw the sticky goo -
	 * this texture is used to draw the refractive portion of the material.
	 */
	inline void SetSceneTexture(const Texture2D* sceneTex) {
		assert(sceneTex != NULL);
		this->stickyGooMatEffect->SetSceneTexture(sceneTex);
	}

	void SetPaddleLaserBeamIsActive(bool isActive);

};
#endif