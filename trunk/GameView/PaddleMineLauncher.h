/**
 * PaddleMineLauncher.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __PADDLEMINELAUNCHER_H__
#define __PADDLEMINELAUNCHER_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"

#include "../GameModel/PlayerPaddle.h"

class PaddleMineLauncher {

    PaddleMineLauncher();
    ~PaddleMineLauncher();

	/**
	 * Draws the paddle mine launcher gun attachment - also is responsible for
	 * animating the attachment and setting the appropriate transformations.
	 */
	inline void Draw(double dT, const PlayerPaddle& p, const Camera& camera, CgFxEffectBase* replacementMat,
                     const BasicPointLight& keyLight, const BasicPointLight& fillLight) {

		float paddleScaleFactor = p.GetPaddleScaleFactor();
		this->loadingMineAnim.Tick(dT);

        glPushMatrix();
        
        glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
		this->paddleMineAttachmentMesh->Draw(camera, replacementMat, keyLight, fillLight);

        glTranslatef(0, 0, this->loadingMineAnim.GetInterpolantValue());	// Animate the mine, moving up into a firing position
		this->paddleMineAttachmentMesh->Draw(camera, replacementMat, keyLight, fillLight);

		glPopMatrix();
	}

	void FireMine();

private:
	Mesh* paddleMineAttachmentMesh;
    Mesh* mineMesh;
	AnimationMultiLerp<float> loadingMineAnim;

    DISALLOW_COPY_AND_ASSIGN(PaddleMineLauncher);
};

#endif // __PADDLEMINELAUNCHER_H__