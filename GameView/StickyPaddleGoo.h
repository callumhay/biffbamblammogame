/**
 * StickyPaddleGoo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
		glPushMatrix();
		glRotatef(p.GetZRotation(), 0.0f, 0.0f, 1.0f);
		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);

        if ((p.GetPaddleType() & PlayerPaddle::InvisiPaddle) == PlayerPaddle::InvisiPaddle) {
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