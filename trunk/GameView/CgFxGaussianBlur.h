/**
 * CgFxGaussianBlur.h
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

#ifndef __CGFXGAUSSIANBLUR_H__
#define __CGFXGAUSSIANBLUR_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/CgFxEffect.h"

class FBObj;

class CgFxGaussianBlur : public CgFxPostProcessingEffect {
public:
	enum BlurType { Kernel3x3, Kernel5x5, Kernel7x7 };

	CgFxGaussianBlur(BlurType blurType, FBObj* sceneFBO);
	~CgFxGaussianBlur();

	void SetPoisonBlurAnimation(bool on);
    void Tick(double dT);

	void SetBlurType(BlurType type);
	void Draw(int screenWidth, int screenHeight);

    void SetSigma(float sigma) { this->sigma = sigma; }

private:
	static const char* GAUSSIANBLUR_3x3_TECHNIQUE_NAME;
	static const char* GAUSSIANBLUR_5x5_TECHNIQUE_NAME;
	static const char* GAUSSIANBLUR_7x7_TECHNIQUE_NAME;
	
	FBObj* tempFBO;
	BlurType blurType;
    double sigma;

	// CG parameters
    CGparameter sceneSamplerParam;
	CGparameter blurSizeHorizontalParam;
    CGparameter blurSizeVerticalParam;
	CGparameter sigmaParam;

    bool isPoisonBlurActive;
	AnimationMultiLerp<float> poisonBlurAnim;

    void Draw(int, int, double) { assert(false); }

    DISALLOW_COPY_AND_ASSIGN(CgFxGaussianBlur);
};

inline void CgFxGaussianBlur::Tick(double dT) {
    // Check for any animations
    if (this->isPoisonBlurActive) {
        if (this->poisonBlurAnim.Tick(dT)) {
            this->isPoisonBlurActive = false;
        }
    }
}

#endif