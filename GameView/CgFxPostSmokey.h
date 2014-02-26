/**
 * CgFxPostSmokey.h
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

#ifndef __CGFXPOSTSMOKEY_H__
#define __CGFXPOSTSMOKEY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

/**
 * Fullscreen smokeyness - Makes the whole screen foggy/smokey; happens
 * when the player is in ball camera mode and also has the ghost ball active.
 */
class CgFxPostSmokey : public CgFxPostProcessingEffect {
public:
	static const char* POSTSMOKEY_TECHNIQUE_NAME;
	static const char* POST_ICY_TECHNIQUE_NAME;

	CgFxPostSmokey(FBObj* outputFBO);
	~CgFxPostSmokey();

	void Draw(int screenWidth, int screenHeight, double dT);

	inline void SetColour(const Colour& c) {
		this->colour = c;
	}
	inline void SetOutputFBO(FBObj* renderOut) {
		this->resultFBO = renderOut;
	}
	inline void SetFadeAlpha(float alpha) {
		this->fade = alpha;
	}
	inline void SetScale(float scale) {
		this->scale = scale;
	}
	inline void SetFrequency(float freq) {
		this->frequency = freq;
	}

private:
	// Cg parameters
	CGparameter timerParam;
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter fadeParam;
	CGparameter colourParam;
	
	CGparameter noiseSamplerParam;
	CGparameter sceneSamplerParam;
	CGparameter overlaySamplerParam;

	float timer;
	float scale;
	float frequency;
	float fade;
	Colour colour;
	FBObj* resultFBO;
	Texture2D* overlayTex; 

};
#endif