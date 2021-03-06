/**
 * CgFxPrism.cpp
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

#include "CgFxPrism.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Texture2D.h"

const char* CgFxPrism::DEFAULT_PRISM_TECHNIQUE_NAME = "Prism";
const char* CgFxPrism::SHINE_PRISM_TECHNIQUE_NAME   = "PrismShine";
const char* CgFxPrism::ICE_TECHNIQUE_NAME           = "Ice";

CgFxPrism::CgFxPrism(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PRISM_SHADER, properties),
indexOfRefractionParam(NULL), warpAmountParam(NULL), sceneWidthParam(NULL), sceneHeightParam(NULL), 
sceneSamplerParam(NULL), indexOfRefraction(1.6), warpAmount(200), timer(0), timerParam(NULL),
shineSamplerParam(NULL), shineTex(NULL), shineAlphaParam(NULL), shineAlpha(0.0f) {

	assert(properties->materialType == MaterialProperties::MATERIAL_PRISM_TYPE ||
        properties->materialType == MaterialProperties::MATERIAL_ICE_TYPE);

	this->currTechnique = this->techniques[DEFAULT_PRISM_TECHNIQUE_NAME];

	this->indexOfRefractionParam = cgGetNamedEffectParameter(this->cgEffect, "IndexOfRefraction");
	this->warpAmountParam        = cgGetNamedEffectParameter(this->cgEffect, "WarpAmount");
	this->sceneWidthParam        = cgGetNamedEffectParameter(this->cgEffect, "SceneWidth");
	this->sceneHeightParam       = cgGetNamedEffectParameter(this->cgEffect, "SceneHeight");
	this->sceneSamplerParam      = cgGetNamedEffectParameter(this->cgEffect, "SceneSampler");

    this->timerParam        = cgGetNamedEffectParameter(this->cgEffect, "Timer");
    this->shineSamplerParam = cgGetNamedEffectParameter(this->cgEffect, "ShineSampler");
    this->shineAlphaParam   = cgGetNamedEffectParameter(this->cgEffect, "ShineAlpha");
    //this->shineDirParam     = cgGetNamedEffectParameter(this->cgEffect, "ShineDir");

	// Set the appropriate diffuse colour
	this->properties->diffuse   = GameViewConstants::GetInstance()->PRISM_BLOCK_COLOUR;
	this->properties->specular  = Colour(0.7f, 0.7f, 0.7f);
    this->properties->shininess = 90.0f;
    this->properties->geomType  = MaterialProperties::MATERIAL_GEOM_FG_TYPE;

	debug_cg_state();
}

CgFxPrism::~CgFxPrism() {
}

void CgFxPrism::SetupBeforePasses(const Camera& camera) {

	cgGLSetParameter1f(this->sceneWidthParam, camera.GetWindowWidth());
	cgGLSetParameter1f(this->sceneHeightParam, camera.GetWindowHeight());
	cgGLSetParameter1f(this->warpAmountParam, this->warpAmount);
	cgGLSetParameter1f(this->indexOfRefractionParam, this->indexOfRefraction);
	
	assert(this->sceneTex != NULL);
	cgGLSetTextureParameter(this->sceneSamplerParam, this->sceneTex->GetTextureID());

    if (this->currTechnique == this->techniques[SHINE_PRISM_TECHNIQUE_NAME]) {
        cgGLSetParameter1f(this->timerParam, this->timer);
        cgGLSetParameter1f(this->shineAlphaParam, this->shineAlpha);
        //cgGLSetParameter2f(this->shineDirParam, this->shineDir[0], this->shineDir[1]);
        assert(this->shineTex != NULL);
        cgGLSetTextureParameter(this->shineSamplerParam, this->shineTex->GetTextureID());
    }

	CgFxMaterialEffect::SetupBeforePasses(camera);
}