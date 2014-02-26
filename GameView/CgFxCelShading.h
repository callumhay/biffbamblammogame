/**
 * CgFxCelShading.h
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

#ifndef __CGFXCELSHADING_H__
#define __CGFXCELSHADING_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Texture1D.h"

/**
 * Represents CgFx CelShader material.
 */
class CgFxCelShading : public CgFxMaterialEffect {

private:
	// Constants used for loading the CelShading effect
	static const char* BASIC_FG_TECHNIQUE_NAME;
	static const char* TEXTURED_FG_TECHNIQUE_NAME;
	static const char* BASIC_BG_TECHNIQUE_NAME;
	static const char* TEXTURED_BG_TECHNIQUE_NAME;

	static Texture* CelDiffuseTexture;

	// Cel shading texture: parameter and texture objects
	CGparameter celSamplerParam;
    CGparameter keyPointLightAttenParam;

protected:
	virtual void SetupBeforePasses(const Camera& camera);

public:
	CgFxCelShading(MaterialProperties* properties);
	virtual ~CgFxCelShading();

};

#endif