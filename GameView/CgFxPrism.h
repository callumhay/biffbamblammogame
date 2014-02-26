/**
 * CgFxPrism.h
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

#ifndef __CGFXPRISM_H__
#define __CGFXPRISM_H__

#include "../BlammoEngine/CgFxEffect.h"

class Texture2D;

/**
 * Material shader class for prism blocks and other prism-like
 * objects in the game.
 */
class CgFxPrism : public CgFxMaterialEffect {
public:
	CgFxPrism(MaterialProperties* properties);
	~CgFxPrism();

	/**
	 * Set the FBO texture (the texture where the scene thus far
	 * has been rendered to).
	 */
	void SetSceneTexture(const Texture2D* tex) {
		this->sceneTex = tex;
	}

protected:
	void SetupBeforePasses(const Camera& camera);

private:
	static const char* DEFAULT_PRISM_TECHNIQUE_NAME;

	CGparameter indexOfRefractionParam;
	CGparameter warpAmountParam;
	CGparameter sceneWidthParam;
	CGparameter sceneHeightParam;
	CGparameter sceneSamplerParam;

	float indexOfRefraction, warpAmount;
	const Texture2D* sceneTex;

};
#endif // __CGFXPRISM_H__