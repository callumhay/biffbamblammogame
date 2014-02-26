/**
 * CgFxInkSplatter.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#include "CgFxInkSplatter.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Noise.h"
#include "../BlammoEngine/Texture3D.h"
#include "../BlammoEngine/FBObj.h"

CgFxInkSplatter::CgFxInkSplatter(FBObj* outputFBO, const std::string& maskTexFilepath) : 
CgFxFullscreenGoo(outputFBO), isInkSplatActivated(false), isInkSplatVisible(false) {
	this->SetMask(maskTexFilepath);
	this->SetColour(GameViewConstants::GetInstance()->INK_BLOCK_COLOUR);
}

CgFxInkSplatter::~CgFxInkSplatter() {
}

/**
 * Activates the ink splatter effect.
 */
void CgFxInkSplatter::ActivateInkSplat() {
	if (this->isInkSplatActivated) {
		// Setup the animation for fading in and out the ink splatter
		// such that it is a secondary ink splatter to occur
		std::vector<double> timeVals;
		timeVals.reserve(3);
		timeVals.push_back(0.0);
        timeVals.push_back(0.79);
        timeVals.push_back(0.8);
		timeVals.push_back(4.0);
		timeVals.push_back(9.0);

		std::vector<float> fadeVals;
		fadeVals.reserve(3);
		fadeVals.push_back(this->inkSplatFadeAnim.GetInterpolantValue());
        fadeVals.push_back(this->inkSplatFadeAnim.GetInterpolantValue());
		fadeVals.push_back(1.0f);
        fadeVals.push_back(1.0f);
		fadeVals.push_back(0.0f);

		this->inkSplatFadeAnim.SetLerp(timeVals, fadeVals);		
	}
	else {
		// Setup the animation for fading in and out the ink splatter
		// such that it is the first ink splatter to occur
		std::vector<double> timeVals;
		timeVals.reserve(5);
		timeVals.push_back(0.0);
		timeVals.push_back(0.79);
		timeVals.push_back(0.8);
		timeVals.push_back(4.0);
		timeVals.push_back(9.0);

		std::vector<float> fadeVals;
		fadeVals.reserve(5);
		fadeVals.push_back(0.0f);	// Don't have any splatter to begin
		fadeVals.push_back(0.0f);	// Makes sure there's no interpolation
		fadeVals.push_back(1.0f);	// Start the full splatter immediately
		fadeVals.push_back(1.0f); // Keep splatter until here
		fadeVals.push_back(0.0f); // Fade splatter

		this->inkSplatFadeAnim.SetLerp(timeVals, fadeVals);
		this->isInkSplatActivated = true;
	}
}

void CgFxInkSplatter::Draw(int screenWidth, int screenHeight, double dT) {
	// If this effect is not activated then leave
	if (!this->isInkSplatActivated) {
		return;
	}

	this->timer += dT;

	// Setup all the relevant parameters
	cgGLSetParameter1f(this->timerParam, this->timer);
	cgGLSetParameter1f(this->scaleParam, this->scale);
	cgGLSetParameter1f(this->frequencyParam, this->frequency);
	cgGLSetParameter1f(this->displacementParam, this->displacement);
	cgGLSetParameter1f(this->fadeParam, this->inkSplatFadeAnim.GetInterpolantValue());
	cgGLSetParameter3fv(this->colourParam, this->colour.begin());

	if (this->currTechnique == this->techniques[CgFxFullscreenGoo::MASK_SPLATTER_TECHNIQUE_NAME]) {
		cgGLSetTextureParameter(this->maskSamplerParam, this->maskTex->GetTextureID());
	}
	cgGLSetTextureParameter(this->noiseSamplerParam, Noise::GetInstance()->GetNoise3DTexture()->GetTextureID());
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneFBO->GetFBOTexture()->GetTextureID());

	// Draw a fullscreen quad with the effect applied and draw it all into the result FBO
	this->resultFBO->BindFBObj();
	CGpass currPass = cgGetFirstPass(this->currTechnique);
	cgSetPassState(currPass);
	GeometryMaker::GetInstance()->DrawFullScreenQuad(screenWidth, screenHeight);
	cgResetPassState(currPass);
	this->resultFBO->UnbindFBObj();
	
	// The effect will be active as long as the timer is not complete
	this->isInkSplatActivated = !this->inkSplatFadeAnim.Tick(dT);
    this->isInkSplatVisible   = this->inkSplatFadeAnim.GetInterpolantValue() >= 0.05;
}