/**
 * CgFxFullscreenGoo.h
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

#ifndef __CGFXFULLSCREENGOO_H__
#define __CGFXFULLSCREENGOO_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

/**
 * Fullscreen goo - occurs when the player has activated paddle camera mode and
 * also has the sticky paddle powerup active.
 */
class CgFxFullscreenGoo : public CgFxPostProcessingEffect {

public:
	CgFxFullscreenGoo(FBObj* outputFBO);
	virtual ~CgFxFullscreenGoo();

	virtual void Draw(int screenWidth, int screenHeight, double dT);

	void SetColour(const Colour& c);
	void SetMask(const std::string& texMaskFilepath);
	void SetOutputFBO(FBObj* renderOut);
	void SetFadeAlpha(float alpha);
	void SetDisplacement(float d);
	void SetScale(float s);

protected:
	static const char* MASK_SPLATTER_TECHNIQUE_NAME;
	static const char* NOMASK_SPLATTER_TECHNIQUE_NAME;

	// Cg parameters
	CGparameter timerParam;
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter displacementParam;
	CGparameter fadeParam;
	CGparameter colourParam;
	
	CGparameter noiseSamplerParam;
	CGparameter maskSamplerParam;
	CGparameter sceneSamplerParam;

	float timer;
	float scale;
	float frequency;
	float displacement;
	float fade;

	Colour colour;
	Texture* maskTex;

	FBObj* resultFBO;

private:
    DISALLOW_COPY_AND_ASSIGN(CgFxFullscreenGoo);
};

inline void CgFxFullscreenGoo::SetColour(const Colour& c) {
	this->colour = c;
}
inline void CgFxFullscreenGoo::SetOutputFBO(FBObj* renderOut) {
	this->resultFBO = renderOut;
}
inline void CgFxFullscreenGoo::SetFadeAlpha(float alpha) {
	this->fade = alpha;
}
inline void CgFxFullscreenGoo::SetDisplacement(float d) {
	this->displacement = d;
}
inline void CgFxFullscreenGoo::SetScale(float s) {
	this->scale = s;
}

#endif