/**
 * LaserPaddleGun.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LaserPaddleGun.h"
#include "GameViewConstants.h"

#include "../ResourceManager.h"

LaserPaddleGun::LaserPaddleGun() : laserGunMesh(NULL) {
	this->laserGunRecoilAnim.SetInterpolantValue(0.0f);
	this->laserGunMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_LASER_ATTACHMENT_MESH);
}

LaserPaddleGun::~LaserPaddleGun() {
}

/**
 * Cause the laser paddle attachment to animate - like it's reacting to shooting
 * a bullet ... or a laser?!
 */
void LaserPaddleGun::FirePaddleLaserGun(const PlayerPaddle& paddle) {
		// Setup any animations for the laser attachment
		std::vector<float> translateVals;
		translateVals.reserve(3);
		translateVals.push_back(0);
		translateVals.push_back(paddle.GetHalfHeight());
		translateVals.push_back(0);
		std::vector<double> timeVals;
		timeVals.reserve(3);
		timeVals.push_back(0);
		timeVals.push_back(0.1f);
		timeVals.push_back(0.2f);
		this->laserGunRecoilAnim.SetLerp(timeVals, translateVals);
}