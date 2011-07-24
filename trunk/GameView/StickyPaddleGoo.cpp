/**
 * StickyPaddleGoo.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "StickyPaddleGoo.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/CgFxEffect.h"

#include "../ResourceManager.h"

StickyPaddleGoo::StickyPaddleGoo() : paddleGooMesh(NULL), stickyGooMatEffect(NULL) {
	this->LoadMesh();
	assert(this->paddleGooMesh != NULL);
	assert(this->stickyGooMatEffect != NULL);
}

StickyPaddleGoo::~StickyPaddleGoo() {
	// Clean-up the mesh (this will also take care of the materials attached to it)
	//delete this->paddleGooMesh;
	//this->paddleGooMesh = NULL;
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
	gooMatProps->geomType			= MaterialProperties::MATERIAL_GEOM_FG_TYPE;
	gooMatProps->diffuse			= GameViewConstants::GetInstance()->STICKYPADDLE_GOO_COLOUR;
	gooMatProps->specular			= Colour(0.7f, 0.7f, 0.7f);
	gooMatProps->shininess		= 100.0f;
	this->stickyGooMatEffect = new CgFxStickyPaddle(gooMatProps);

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