/**
 * CgFxPostFirey.h
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

#ifndef __CGFXPOSTFIREY_H__
#define __CGFXPOSTFIREY_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/CgFxEffect.h"

class CgFxPostFirey : public CgFxPostProcessingEffect {
public:
	CgFxPostFirey(FBObj* outputFBO);
	~CgFxPostFirey();

	void Draw(int screenWidth, int screenHeight, double dT);

	void SetColour(const Colour& c);
	void SetOutputFBO(FBObj* renderOut);
	void SetFadeAlpha(float alpha);

private:
	static const char* POSTFIREY_TECHNIQUE_NAME;

	// Texture sampler params
	CGparameter noiseSamplerParam;
	CGparameter sceneSamplerParam;

	// Timer paramter
	CGparameter timerParam;

	// Tweakable params
	CGparameter scaleParam;
	CGparameter frequencyParam;
	CGparameter fadeParam;

	// Actual values to be used for parameters in the shader
	float scale, freq, fade, timer;
	GLuint noiseTexID;
	FBObj* resultFBO;

	DISALLOW_COPY_AND_ASSIGN(CgFxPostFirey);
};

inline void CgFxPostFirey::SetOutputFBO(FBObj* renderOut) {
	this->resultFBO = renderOut;
}
inline void CgFxPostFirey::SetFadeAlpha(float alpha) {
	this->fade = alpha;
}

#endif // __CGFXPOSTFIREY_H__