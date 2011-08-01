/**
 * PlayerHurtHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "PlayerHurtHUD.h"
#include "../BlammoEngine/GeometryMaker.h"

PlayerHurtHUD::PlayerHurtHUD() : isActive(false) {
}

PlayerHurtHUD::~PlayerHurtHUD() {
}

void PlayerHurtHUD::Activate(PlayerHurtHUD::PainIntensity intensity) {
	double totalFadeOutTime = 0.0;				// Total time the overlay will fade out over
	float initialOverlayIntensity = 0.0;	// Initial alpha of the pain overlay

	// Set up the animation for the fade to immediately show the pain overlay
	// and then fade it out fairly fast - this is all based on the given intensity
	switch (intensity) {
		case PlayerHurtHUD::MinorPain:
			totalFadeOutTime = 0.8;
			initialOverlayIntensity = 0.25f;
			break;
		case PlayerHurtHUD::ModeratePain:
			totalFadeOutTime = 1.0;
			initialOverlayIntensity = 0.5f;
			break;
		case PlayerHurtHUD::MajorPain:
			totalFadeOutTime = 1.33;
			initialOverlayIntensity = 0.75f;
			break;
		default:
			assert(false);
			return;
	}
	
	std::vector<double> times;
	times.reserve(2);
	times.push_back(0.0);
	times.push_back(totalFadeOutTime);

	std::vector<float> fadeValues;
	fadeValues.reserve(2);
	fadeValues.push_back(this->fadeAnimation.GetInterpolantValue() + initialOverlayIntensity);
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