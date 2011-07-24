/**
 * LaserPaddleGun.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __LASERPADDLEGUN_H__
#define __LASERPADDLEGUN_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"

#include "../GameModel/PlayerPaddle.h"

/**
 * Helper class (similar to StickyPaddleGoo) for holding the mesh,
 * materials and effects used for the laser paddle gun attachment -
 * visible when the user has a laserpaddle power-up active.
 */
class LaserPaddleGun {
private:
	Mesh* laserGunMesh;
	AnimationMultiLerp<float> laserGunRecoilAnim;

public:
	LaserPaddleGun();
	~LaserPaddleGun();

	/**
	 * Draws the paddle laser gun attachment - also is responsible for
	 * animating the attachment and setting the appropriate transformations.
	 */
	inline void Draw(double dT, const PlayerPaddle& p, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight) {
		float paddleScaleFactor = p.GetPaddleScaleFactor();
		this->laserGunRecoilAnim.Tick(dT);

		glPushMatrix();
		glTranslatef(0, 0, this->laserGunRecoilAnim.GetInterpolantValue());	// Animate recoil on fire
		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
		this->laserGunMesh->Draw(camera, keyLight, fillLight);
		glPopMatrix();
	}

	void FirePaddleLaserGun(const PlayerPaddle& paddle);

};
#endif