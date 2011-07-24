/**
 * BallSafetyNetMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BallSafetyNetMesh.h"
#include "GameViewConstants.h"
#include "CgFxCelShading.h"
#include "CgFxPhong.h"

#include "../GameModel/LevelPiece.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Light.h"

const float BallSafetyNetMesh::SAFETY_NET_HEIGHT	= 1.0f;
const float BallSafetyNetMesh::SAFETY_NET_DEPTH		= 1.25f;

BallSafetyNetMesh::BallSafetyNetMesh() : shadingMaterial(NULL), displayListID(0), leftPieceDispListID(0),
rightPieceDispListID(0), currAnimation(BallSafetyNetMesh::Dead), idleAlpha(1.0f),
leftPieceAnim(&leftPieceOrient), rightPieceAnim(&rightPieceOrient), pieceFadeAnim(&pieceAlpha) {

	this->displayListID = glGenLists(1);
	assert(this->displayListID != 0);
	this->leftPieceDispListID = glGenLists(1);
	assert(this->leftPieceDispListID != 0);
	this->rightPieceDispListID = glGenLists(1);
	assert(this->rightPieceDispListID != 0);

	this->InitializeMaterials();
}

BallSafetyNetMesh::~BallSafetyNetMesh() {
	// Delete the shading material for this mesh
	if (this->shadingMaterial != NULL) {
		delete this->shadingMaterial;
		this->shadingMaterial = NULL;
	}

	// Destroy any undeleted display lists for this mesh
	if (this->displayListID != 0) {
		glDeleteLists(this->displayListID, 1);
		this->displayListID = 0;
	}
	if (this->leftPieceDispListID != 0) {
		glDeleteLists(this->leftPieceDispListID, 1);
		this->leftPieceDispListID = 0;
	}
	if (this->rightPieceDispListID != 0) {
		glDeleteLists(this->rightPieceDispListID, 1);
		this->rightPieceDispListID = 0;
	}
}

/**
 * Private helper function for initializing the material(s) of this mesh.
 */
void BallSafetyNetMesh::InitializeMaterials() {
	assert(this->shadingMaterial == NULL);

	// Read in the texture for the mesh and create the material properties
	// and shader effect
	Texture* ballSafetyNetTex = ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BALL_SAFETY_NET, Texture::Trilinear, GL_TEXTURE_2D);
	assert(ballSafetyNetTex != NULL);

	// Create the material properties for the mesh
	MaterialProperties* matProps  = new MaterialProperties();
	matProps->diffuseTexture = ballSafetyNetTex;
	matProps->diffuse				= Colour(1,1,1);
	matProps->specular			= Colour(0.3, 0.3, 0.3);
	matProps->shininess			= 50.0f;
	matProps->geomType			= MaterialProperties::MATERIAL_GEOM_FG_TYPE;
	matProps->materialType	= MaterialProperties::MATERIAL_PHONG_TYPE;	// Not really necessary, but just for brevity

	// Associate material properties with appropriate shading technique
	this->shadingMaterial = new CgFxPhong(matProps);
}

/**
 * Static function for drawing the safety net mesh with the given parameters.
 */
void BallSafetyNetMesh::DrawSafetyNetMesh(float minXCoord, float maxXCoord, float minTexCoordU, float maxTexCoordU) {
	const float HalfDepth		= BallSafetyNetMesh::SAFETY_NET_DEPTH	 / 2.0f;
	const float HalfHeight	= BallSafetyNetMesh::SAFETY_NET_HEIGHT / 2.0f;	
	
	// The safety net is just a long rectangular prism that spans the bottom portion
	// of the current level
	glBegin(GL_QUADS);
		glNormal3i(0, 1, 0);
		glTexCoord2f(maxTexCoordU, 1.0f);
    glVertex3f(maxXCoord, HalfHeight,-HalfDepth);	// Top Right Of The Quad (Top)
		glTexCoord2f(minTexCoordU, 1.0f);
    glVertex3f(minXCoord, HalfHeight,-HalfDepth);	// Top Left Of The Quad (Top)
    glTexCoord2f(minTexCoordU, 0.0f);
		glVertex3f(minXCoord, HalfHeight, HalfDepth);	// Bottom Left Of The Quad (Top)
    glTexCoord2f(maxTexCoordU, 0.0f);
		glVertex3f(maxXCoord, HalfHeight, HalfDepth);	// Bottom Right Of The Quad (Top)

		glNormal3i(0, -1, 0);
		glTexCoord2f(maxTexCoordU, 1.0f);
    glVertex3f(maxXCoord,-HalfHeight, HalfDepth);	// Top Right Of The Quad (Bottom)
		glTexCoord2f(minTexCoordU, 1.0f);
    glVertex3f(minXCoord,-HalfHeight, HalfDepth);	// Top Left Of The Quad (Bottom)
		glTexCoord2f(minTexCoordU, 0.0f);
    glVertex3f(minXCoord,-HalfHeight,-HalfDepth);	// Bottom Left Of The Quad (Bottom)
		glTexCoord2f(maxTexCoordU, 0.0f);
    glVertex3f(maxXCoord,-HalfHeight,-HalfDepth);	// Bottom Right Of The Quad (Bottom)

		glNormal3i(0, 0, 1);
		glTexCoord2f(maxTexCoordU, 1.0f);
    glVertex3f(maxXCoord, HalfHeight, HalfDepth);	// Top Right Of The Quad (Front)
		glTexCoord2f(minTexCoordU, 1.0f);
    glVertex3f(minXCoord, HalfHeight, HalfDepth);	// Top Left Of The Quad (Front)
		glTexCoord2f(minTexCoordU, 0.0f);
    glVertex3f(minXCoord,-HalfHeight, HalfDepth);	// Bottom Left Of The Quad (Front)
		glTexCoord2f(maxTexCoordU, 0.0f);
    glVertex3f(maxXCoord,-HalfHeight, HalfDepth);	// Bottom Right Of The Quad (Front)

		glNormal3i(0, 0, -1);
		glTexCoord2f(maxTexCoordU, 1.0f);
    glVertex3f(maxXCoord,-HalfHeight,-HalfDepth);	// Top Right Of The Quad (Back)
		glTexCoord2f(minTexCoordU, 1.0f);
    glVertex3f(minXCoord,-HalfHeight,-HalfDepth);	// Top Left Of The Quad (Back)
		glTexCoord2f(minTexCoordU, 0.0f);
    glVertex3f(minXCoord, HalfHeight,-HalfDepth);	// Bottom Left Of The Quad (Back)
		glTexCoord2f(maxTexCoordU, 0.0f);
    glVertex3f(maxXCoord, HalfHeight,-HalfDepth);	// Bottom Right Of The Quad (Back)

		glNormal3i(-1, 0, 0);
		glTexCoord2f(1.0f, 1.0f);
    glVertex3f(minXCoord, HalfHeight, HalfDepth);	// Top Right Of The Quad (Left)
		glTexCoord2f(0.0f, 1.0f);
    glVertex3f(minXCoord, HalfHeight,-HalfDepth);	// Top Left Of The Quad (Left)
		glTexCoord2f(0.0f, 0.0f);
    glVertex3f(minXCoord,-HalfHeight,-HalfDepth);	// Bottom Left Of The Quad (Left)
		glTexCoord2f(1.0f, 0.0f);
    glVertex3f(minXCoord,-HalfHeight, HalfDepth);	// Bottom Right Of The Quad (Left)

		glNormal3i(1, 0, 0);
		glTexCoord2f(1.0f, 1.0f);
    glVertex3f(maxXCoord, HalfHeight,-HalfDepth);	// Top Right Of The Quad (Right)
		glTexCoord2f(0.0f, 1.0f);
    glVertex3f(maxXCoord, HalfHeight, HalfDepth);	// Top Left Of The Quad (Right)
		glTexCoord2f(0.0f, 0.0f);
    glVertex3f(maxXCoord,-HalfHeight, HalfDepth);	// Bottom Left Of The Quad (Right)
		glTexCoord2f(1.0f, 0.0f);
    glVertex3f(maxXCoord,-HalfHeight,-HalfDepth);	// Bottom Right Of The Quad (Right)
	glEnd();
}

/**
 * Public function for regenerating the display list of this mesh for
 * a given level dimension - this will reshape and initialize the 
 * display list for the mesh so that the Draw function can be used properly.
 */
void BallSafetyNetMesh::Regenerate(const Vector2D& levelDimensions) {
	assert(this->displayListID != 0);

	const float width = levelDimensions[0];
	const float maxTexCoordU = width / 2.0f;

	glNewList(this->displayListID, GL_COMPILE);
	BallSafetyNetMesh::DrawSafetyNetMesh(0.0f, width, 0.0f, maxTexCoordU);
	glEndList();

	debug_opengl_state();
}

/**
 * Prompt the creation of the ball safety net - this will play any necessary
 * creation animations for the mesh.
 */
void BallSafetyNetMesh::CreateBallSafetyNet() {
	this->pieceAlpha = 0.0f;
	this->pieceFadeAnim.SetLerp(0.5, 1.0f);

	this->currAnimation = BallSafetyNetMesh::CreationAnimation;
}

/** 
 * Prompt the start of the destruction animation for the ball safety net mesh -
 * this animation is the breaking apart of the mesh at the location where the ball 
 * hit it.
 */
void BallSafetyNetMesh::DestroyBallSafetyNet(const Vector2D& levelDimensions, float destructionXPos) {
	assert(this->displayListID != 0);
	
	// Make sure that both display lists for the broken pieces get initialized
	assert(this->leftPieceDispListID != 0 && this->rightPieceDispListID != 0);

	// (Re)compile the 2 new display lists of the broken pieces of the safety net
	const float maxLeft = destructionXPos;
	const float maxRight = levelDimensions[0];

	const float maxTexCoordLeft  = maxLeft / 2.0f;
	const float maxTexCoordRight = maxRight / 2.0f;
	
	// Left broken piece
	glNewList(this->leftPieceDispListID, GL_COMPILE);
	BallSafetyNetMesh::DrawSafetyNetMesh(0.0f, maxLeft, 0.0f, maxTexCoordLeft);
	glEndList();

	// Right broken piece
	glNewList(this->rightPieceDispListID, GL_COMPILE);
	BallSafetyNetMesh::DrawSafetyNetMesh(maxLeft, maxRight, maxTexCoordLeft, maxTexCoordRight);
	glEndList();

	// Setup the linear interpolation values for the animations of the two pieces
	this->leftPieceOrient  = Orientation3D(Vector3D(0,0,0), Vector3D(0,0,0));
	this->rightPieceOrient = Orientation3D(Vector3D(0,0,0), Vector3D(0,0,0));
	this->pieceAlpha = 1.0f;

	this->leftPieceAnim.SetLerp(2.0, Orientation3D(Vector3D(-3.0f, -5.0f, 0.0f), Vector3D(60.0f, 0.0f, 0.0f)));
	this->rightPieceAnim.SetLerp(2.0, Orientation3D(Vector3D(3.0f, -5.0f, 0.0f), Vector3D(50.0f, 0.0f, 0.0f)));
	this->pieceFadeAnim.SetLerp(2.0, 0.0f);

	//this->playDestructionAnimation = true;
	this->currAnimation = BallSafetyNetMesh::DestructionAnimation;
}

/**
 * Draw the active display lists for this mesh.
 */
void BallSafetyNetMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
														 const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
	// TODO: Creation animation?

	// Set the appropriate light values for the shading material
	this->shadingMaterial->SetKeyLight(keyLight);
	this->shadingMaterial->SetFillLight(fillLight);
	this->shadingMaterial->SetBallLight(ballLight);

	// Check to see whether we're playing an animation or just drawing the safety net normally
	switch (this->currAnimation) {
		
		case BallSafetyNetMesh::Idle:
			assert(this->displayListID != 0);
			// Draw the typical ball safety net
			glPushAttrib(GL_CURRENT_BIT);
			glColor4f(1.0f, 1.0f, 1.0f, this->idleAlpha);
			this->shadingMaterial->Draw(camera, this->displayListID);
			glPopAttrib();
			break;

		case BallSafetyNetMesh::CreationAnimation:
			assert(this->displayListID != 0);
			glPushAttrib(GL_CURRENT_BIT);
			glColor4f(1.0f, 1.0f, 1.0f, this->pieceAlpha);
			this->shadingMaterial->Draw(camera, this->displayListID);	
			{
				bool creationAnimComplete = this->pieceFadeAnim.Tick(dT);
				if (creationAnimComplete) {
					this->currAnimation = BallSafetyNetMesh::Idle;
				}
			}
			glPopAttrib();
			break;

		case BallSafetyNetMesh::DestructionAnimation:
			glPushAttrib(GL_CURRENT_BIT);
			// Play the destruction animation for the safety net
			glColor4f(1, 1, 1, this->pieceAlpha);

			glPushMatrix();
			glTranslatef(this->leftPieceOrient.GetTX(), this->leftPieceOrient.GetTY(), this->leftPieceOrient.GetTZ());
			glRotatef(this->leftPieceOrient.GetRX(), 1, 0, 0);
			glRotatef(this->leftPieceOrient.GetRY(), 0, 1, 0);
			glRotatef(this->leftPieceOrient.GetRZ(), 0, 0, 1);
			this->shadingMaterial->Draw(camera, this->leftPieceDispListID);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(this->rightPieceOrient.GetTX(), this->rightPieceOrient.GetTY(), this->rightPieceOrient.GetTZ());
			glRotatef(this->rightPieceOrient.GetRX(), 1, 0, 0);
			glRotatef(this->rightPieceOrient.GetRY(), 0, 1, 0);
			glRotatef(this->rightPieceOrient.GetRZ(), 0, 0, 1);
			this->shadingMaterial->Draw(camera, this->rightPieceDispListID);
			glPopMatrix();
			
			{
				bool leftFinished  = this->leftPieceAnim.Tick(dT);
				bool rightFinished = this->rightPieceAnim.Tick(dT);
				this->pieceFadeAnim.Tick(dT);

				if (leftFinished && rightFinished) {
					this->currAnimation = BallSafetyNetMesh::Dead;
				}
			}

			glPopAttrib();
			break;

		case BallSafetyNetMesh::Dead:
			// Don't do anything - there's nothing to draw.
			break;

		default:
			assert(false);
			break;
	}
}