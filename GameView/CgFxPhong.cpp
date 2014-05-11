/**
 * CgFxPhong.cpp
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

#include "CgFxPhong.h"
#include "GameViewConstants.h"

const char* CgFxPhong::BASIC_FG_TECHNIQUE_NAME                  = "BasicFG";
const char* CgFxPhong::BASIC_FG_NO_OUTLINE_TECHNIQUE_NAME       = "BasicFGNoOutline";
const char* CgFxPhong::TEXTURED_FG_TECHNIQUE_NAME               = "TexturedFG";
const char* CgFxPhong::TEXTURED_FG_NO_OUTLINE_TECHNIQUE_NAME	= "TexturedFGNoOutline";
const char* CgFxPhong::BASIC_BG_TECHNIQUE_NAME                  = "BasicBG";
const char* CgFxPhong::TEXTURED_BG_TECHNIQUE_NAME               = "TexturedBG";

// Default constructor, builds default, white material
CgFxPhong::CgFxPhong(MaterialProperties* properties) : 
CgFxMaterialEffect(GameViewConstants::GetInstance()->CGFX_PHONG_SHADER, properties) {
	assert(properties->materialType == MaterialProperties::MATERIAL_PHONG_TYPE);

	// Set the correct technique based on whether there's a texture and whether the
	// geometry is in the foreground or background and whether there is an outline or not
	
    if (this->properties->diffuseTexture != NULL) {
		if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {

			if (this->properties->outlineSize < EPSILON) {
				this->currTechnique = this->techniques[TEXTURED_FG_NO_OUTLINE_TECHNIQUE_NAME];
			}
			else {
				this->currTechnique = this->techniques[TEXTURED_FG_TECHNIQUE_NAME];
			}
		}
		else {
			this->currTechnique = this->techniques[TEXTURED_BG_TECHNIQUE_NAME];
		}
	}
	else {
		if (this->properties->geomType == MaterialProperties::MATERIAL_GEOM_FG_TYPE) {
			if (this->properties->outlineSize < EPSILON) {
				this->currTechnique = this->techniques[BASIC_FG_NO_OUTLINE_TECHNIQUE_NAME];
			}
			else {
				this->currTechnique = this->techniques[BASIC_FG_TECHNIQUE_NAME];
			}
		}
		else {
			this->currTechnique = this->techniques[BASIC_BG_TECHNIQUE_NAME];
		}
	}

	debug_cg_state();
}

CgFxPhong::~CgFxPhong() {
}
