/**
 * FlashHUD.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "FlashHUD.h"
#include "../BlammoEngine/GeometryMaker.h"

FlashHUD::FlashHUD() : isActive(false) {
}

FlashHUD::~FlashHUD() {
	this->Deactivate();
}

void FlashHUD::Activate(double length, float percentIntensity) {
	std::vector<double> times;
	times.reserve(2);
	times.push_back(0.0);
	times.push_back(length);

	std::vector<float> fadeValues;
	fadeValues.reserve(2);
	fadeValues.push_back(this->fadeAnimation.GetInterpolantValue() + percentIntensity);
	fadeValues.push_back(0.0f);

	this->fadeAnimation.SetLerp(times, fadeValues);
	this->fadeAnimation.SetRepeat(false);

	this->isActive = true;
}

void FlashHUD::Deactivate() {
	this->isActive = false;
	this->fadeAnimation.ClearLerp();
	this->fadeAnimation.SetInterpolantValue(0.0f);
}

void FlashHUD::Draw(double dT, int displayWidth, int displayHeight) {
	// Do nothing if the effect is not active
	if (!this->isActive) {
		return;
	}

	float fadeAmt = this->fadeAnimation.GetInterpolantValue();

	glPushAttrib(GL_ENABLE_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(displayWidth, displayHeight, 1.0, ColourRGBA(1, 1, 1, fadeAmt));
	glPopAttrib();

	bool isFinished = this->fadeAnimation.Tick(dT);
	if (isFinished) {
		this->Deactivate();
	}
}