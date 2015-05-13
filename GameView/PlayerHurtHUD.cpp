/**
 * PlayerHurtHUD.cpp
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

#include "PlayerHurtHUD.h"
#include "../BlammoEngine/GeometryMaker.h"
#include "../GameModel/Projectile.h"
#include "../GameModel/FireGlobProjectile.h"

PlayerHurtHUD::PlayerHurtHUD() : isActive(false) {
}

PlayerHurtHUD::~PlayerHurtHUD() {
}

PlayerHurtHUD::PainIntensity PlayerHurtHUD::GetIntensityForProjectile(const Projectile& projectile) {

    PlayerHurtHUD::PainIntensity intensity = PlayerHurtHUD::MinorPain;
    switch (projectile.GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::BossLightningBoltBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
            intensity = PlayerHurtHUD::MinorPain;
            break;

        case Projectile::RocketTurretBulletProjectile:
        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::BossShockOrbBulletProjectile:
            intensity = PlayerHurtHUD::ModeratePain;
            break;

        case Projectile::CollateralBlockProjectile:
            intensity = PlayerHurtHUD::MoreThanModeratePain;
            break;

        case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
            intensity = PlayerHurtHUD::MajorPain;
            break;

        case Projectile::FireGlobProjectile: {

            const FireGlobProjectile* fireGlobProjectile = static_cast<const FireGlobProjectile*>(&projectile);

            switch (fireGlobProjectile->GetRelativeSize()) {
                case FireGlobProjectile::Small:
                    intensity = PlayerHurtHUD::MinorPain;
                    break;
                case FireGlobProjectile::Medium:
                    intensity = PlayerHurtHUD::ModeratePain;
                    break;
                case FireGlobProjectile::Large:
                    intensity = PlayerHurtHUD::MajorPain;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        }

        case Projectile::PaddleFlameBlastProjectile: {
            intensity = PlayerHurtHUD::MajorPain;
            break;
                                                     }

        case Projectile::PaddleIceBlastProjectile:
            intensity = PlayerHurtHUD::MinorPain;
            break;

        case Projectile::PortalBlobProjectile:
            intensity = PlayerHurtHUD::NoPain;
            break;

        default:
            assert(false);
            intensity = PlayerHurtHUD::NoPain;
            break;
    }

    return intensity;
}

void PlayerHurtHUD::Activate(PlayerHurtHUD::PainIntensity intensity) {
	double totalFadeOutTime = 0.0;				// Total time the overlay will fade out over
	float initialOverlayIntensity = 0.0;	// Initial alpha of the pain overlay

	// Set up the animation for the fade to immediately show the pain overlay
	// and then fade it out fairly fast - this is all based on the given intensity
	switch (intensity) {
        case PlayerHurtHUD::NoPain:
            return;
		case PlayerHurtHUD::MinorPain:
			totalFadeOutTime = 0.8;
			initialOverlayIntensity = 0.25f;
			break;
		case PlayerHurtHUD::ModeratePain:
			totalFadeOutTime = 1.0;
			initialOverlayIntensity = 0.5f;
			break;
        case PlayerHurtHUD::MoreThanModeratePain:
            totalFadeOutTime = 1.15;
            initialOverlayIntensity = 0.7f;
            break;
		case PlayerHurtHUD::MajorPain:
			totalFadeOutTime = 1.33;
			initialOverlayIntensity = 0.75f;
			break;
		default:
			assert(false);
			return;
	}

	this->Activate(totalFadeOutTime, initialOverlayIntensity);
}

void PlayerHurtHUD::Activate(double totalFadeOutTime, float initialIntensity) {
    std::vector<double> times;
    times.reserve(2);
    times.push_back(0.0);
    times.push_back(totalFadeOutTime);

    std::vector<float> fadeValues;
    fadeValues.reserve(2);
    fadeValues.push_back(this->fadeAnimation.GetInterpolantValue() + initialIntensity);
    fadeValues.push_back(0.0f);

    this->fadeAnimation.SetLerp(times, fadeValues);
    this->fadeAnimation.SetRepeat(false);

    this->isActive = true;
}

void PlayerHurtHUD::Deactivate() {
	this->isActive = false;
	this->fadeAnimation.ClearLerp();
	this->fadeAnimation.SetInterpolantValue(0.0f);
}

void PlayerHurtHUD::Draw(double dT, int displayWidth, int displayHeight) {
	// Do nothing if the effect is not active
	if (!this->isActive) {
		return;
	}
	
	float fadeAmt = this->fadeAnimation.GetInterpolantValue();

	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(displayWidth, displayHeight, 1.0, ColourRGBA(1, 0, 0, fadeAmt));
	glPopAttrib();

	bool isFinished = this->fadeAnimation.Tick(dT);
	if (isFinished) {
		this->Deactivate();
	}
}