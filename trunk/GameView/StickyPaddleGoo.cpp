/**
 * StickyPaddleGoo.cpp
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

#include "StickyPaddleGoo.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/CgFxEffect.h"

#include "../ResourceManager.h"

StickyPaddleGoo::StickyPaddleGoo() : paddleGooMesh(NULL), stickyGooMatEffect(NULL), invisibleEffect(NULL) {
	this->LoadMesh();

	assert(this->paddleGooMesh != NULL);
	assert(this->stickyGooMatEffect != NULL);
    assert(this->invisibleEffect != NULL);
}

StickyPaddleGoo::~StickyPaddleGoo() {
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->paddleGooMesh);
    assert(success);
    UNUSED_VARIABLE(success);

    delete this->invisibleEffect;
    this->invisibleEffect = NULL;
}

/**
 * Load the mesh and material for the sticky goo that will sit on the paddle
 * when the user gets a stickypaddle power-up.
 */
void StickyPaddleGoo::LoadMesh() {
	assert(this->paddleGooMesh == NULL);
	assert(this->stickyGooMatEffect == NULL);

	// Grab goo mesh from the resource manager
	this->paddleGooMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->PADDLE_STICKY_ATTACHMENT_MESH);
	
	// Create the material properties and effect (sticky paddle cgfx shader - makes the goo all wiggly, refractive and stuff)
	MaterialProperties* gooMatProps = new MaterialProperties();
	gooMatProps->materialType	= MaterialProperties::MATERIAL_STICKYGOO_TYPE;
	gooMatProps->geomType		= MaterialProperties::MATERIAL_GEOM_FG_TYPE;
	gooMatProps->diffuse		= GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR;
	gooMatProps->specular		= Colour(0.7f, 0.7f, 0.7f);
	gooMatProps->shininess		= 100.0f;
	
    this->stickyGooMatEffect = new CgFxStickyPaddle(gooMatProps);
	this->invisibleEffect    = new CgFxPostRefract();
	this->invisibleEffect->SetWarpAmountParam(50.0f);
	this->invisibleEffect->SetIndexOfRefraction(1.33f);

	// Replace the material of the goo mesh with the sticky goo effect material
	this->paddleGooMesh->ReplaceMaterial(this->stickyGooMatEffect);
}

/**
 * Tells the goo whether the laser beam on the paddle is currently active -
 * this will change the appearance of the goo.
 */
void StickyPaddleGoo::SetPaddleLaserBeamIsActive(bool isActive) {
	MaterialProperties* gooMatProps = this->stickyGooMatEffect->GetProperties();
	if (isActive) {
		gooMatProps->diffuse		= GameViewConstants::GetInstance()->STICKYPADDLE_PLUS_BEAM_GOO_COLOUR;
		gooMatProps->specular		= Colour(0.9f, 0.9f, 0.9f);
		gooMatProps->shininess	= 120.0f;
	}
	else {
		gooMatProps->diffuse		= GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR;
		gooMatProps->specular		= Colour(0.7f, 0.7f, 0.7f);
		gooMatProps->shininess	= 100.0f;
	}
}