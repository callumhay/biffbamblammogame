/**
 * PaddleGunAttachment.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2014
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLEGUNATTACHMENT_H__
#define __PADDLEGUNATTACHMENT_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"

#include "../GameModel/PlayerPaddle.h"

/**
 * Helper class (similar to StickyPaddleGoo) for holding the mesh,
 * materials and effects used for the various paddle gun-types -
 * visible when the user has a paddle gun-type item active.
 */
class PaddleGunAttachment {
public:
	PaddleGunAttachment();
	~PaddleGunAttachment();

	/**
	 * Draws the paddle laser gun attachment - also is responsible for
	 * animating the attachment and setting the appropriate transformations.
	 */
	inline void Draw(double dT, const PlayerPaddle& p, const Camera& camera, CgFxEffectBase* replacementMat,
                     const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
		float paddleScaleFactor = p.GetPaddleScaleFactor();
      
		this->gunRecoilAnim.Tick(dT);

		glPushMatrix();
		glTranslatef(0, 0, this->gunRecoilAnim.GetInterpolantValue());	// Animate recoil on fire
		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
        glRotatef(p.GetZRotation(), 0, 0, 1);
		this->gunMesh->Draw(camera, replacementMat, keyLight, fillLight, ballLight);
		glPopMatrix();
	}

	void FirePaddleGun(const PlayerPaddle& paddle);

private:
	Mesh* gunMesh;
	AnimationMultiLerp<float> gunRecoilAnim;

    DISALLOW_COPY_AND_ASSIGN(PaddleGunAttachment);
};
#endif // __PADDLEGUNATTACHMENT_H__