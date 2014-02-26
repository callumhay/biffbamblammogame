/**
 * CgFxPostTutorialAttention.h
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

#ifndef __CGFXPOSTTUTORIALATTENTION_H__
#define __CGFXPOSTTUTORIALATTENTION_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

class FBObj;

class CgFxPostTutorialAttention : public CgFxPostProcessingEffect {
public:
	CgFxPostTutorialAttention(FBObj* sceneFBO);
	~CgFxPostTutorialAttention();

    void SetAlpha(float alpha);
    void CrossFadeIn(double fadeInTime);
    void CrossFadeOut(double fadeOutTime);

	void Draw(int screenWidth, int screenHeight, double dT);

private:
	static const char* TECHNIQUE_NAME;
	
	FBObj* tempFBO;
    AnimationLerp<float> fadeAnim;

	// CG parameters
    CGparameter sceneSamplerParam;
	CGparameter blurSizeHorizontalParam;
    CGparameter blurSizeVerticalParam;
	CGparameter sigmaParam;
    CGparameter greyscaleFactorParam;

    DISALLOW_COPY_AND_ASSIGN(CgFxPostTutorialAttention);
};

#endif // __CGFXPOSTTUTORIALATTENTION_H__