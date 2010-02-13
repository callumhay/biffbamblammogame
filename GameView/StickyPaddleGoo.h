#ifndef __STICKYPADDLEGOO_H__
#define __STICKYPADDLEGOO_H__

#include "../BlammoEngine/Mesh.h"

#include "../GameModel/PlayerPaddle.h"

#include "CgFxStickyPaddle.h"

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

	void LoadMesh();

public:
	StickyPaddleGoo();
	~StickyPaddleGoo();
 
  /**
	 * Draw the sticky goo - basically just wraps the draw method for the goo mesh.
	 */
	inline void StickyPaddleGoo::Draw(const PlayerPaddle& p, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) {
		float paddleScaleFactor = p.GetPaddleScaleFactor();
		glPushMatrix();
		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
		this->paddleGooMesh->Draw(camera, keyLight, fillLight, ballLight);
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