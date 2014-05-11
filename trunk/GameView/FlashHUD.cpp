/**
 * FlashHUD.cpp
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