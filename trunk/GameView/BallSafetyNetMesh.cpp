/**
 * BallSafetyNetMesh.cpp
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

#include "BallSafetyNetMesh.h"
#include "GameViewConstants.h"
#include "CgFxCelShading.h"

#include "../GameModel/LevelPiece.h"
#include "../GameModel/SafetyNet.h"
#include "../GameModel/GameLevel.h"

#include "../ResourceManager.h"
#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/Light.h"

const float BallSafetyNetMesh::SAFETY_NET_HEIGHT	= SafetyNet::SAFETY_NET_HEIGHT;
const float BallSafetyNetMesh::SAFETY_NET_DEPTH		= 1.25f;

BallSafetyNetMesh::BallSafetyNetMesh(bool isBottomNet) : shadingMaterial(NULL), displayListID(0), leftPieceDispListID(0),
rightPieceDispListID(0), currAnimation(BallSafetyNetMesh::Dead), idleAlpha(1.0f), centerPos(0,0), isBottomNet(isBottomNet),
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

float BallSafetyNetMesh::GetMidYCoord(const GameLevel& level) const {
    return this->isBottomNet ? -BallSafetyNetMesh::SAFETY_NET_HEIGHT / 2.0f : level.GetLevelUnitHeight() + BallSafetyNetMesh::SAFETY_NET_HEIGHT / 2.0f;
}

/**
 * Private helper function for initializing the material(s) of this mesh.
 */
void BallSafetyNetMesh::InitializeMaterials() {
	assert(this->shadingMaterial == NULL);

	// Read in the texture for the mesh and create the material properties
	// and shader effect
	Texture* ballSafetyNetTex = ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_BALL_SAFETY_NET, Texture::Trilinear, GL_TEXTURE_2D);
	assert(ballSafetyNetTex != NULL);

	// Create the material properties for the mesh
	MaterialProperties* matProps  = new MaterialProperties();
	matProps->diffuseTexture = ballSafetyNetTex;
	matProps->diffuse        = Colour(1,1,1);
	matProps->specular       = Colour(0.3, 0.3, 0.3);
	matProps->shininess      = 50.0f;
	matProps->geomType       = MaterialProperties::MATERIAL_GEOM_FG_TYPE;
    matProps->materialType 	 = MaterialProperties::MATERIAL_CELBASIC_TYPE;  	// Not really necessary, but just for brevity

	// Associate material properties with appropriate shading technique
	this->shadingMaterial = new CgFxCelShading(matProps);
}

/**
 * Static function for drawing the safety net mesh with the given parameters.
 */
void BallSafetyNetMesh::DrawSafetyNetMesh(float minXCoord, float maxXCoord, float midYCoord, float minTexCoordU, float maxTexCoordU) {
	const float HalfDepth  = BallSafetyNetMesh::SAFETY_NET_DEPTH  / 2.0f;
	const float HalfHeight = BallSafetyNetMesh::SAFETY_NET_HEIGHT / 2.0f;
    float maxYCoord = midYCoord + HalfHeight;
    float minYCoord = midYCoord - HalfHeight;

    // The safety net is just a long rectangular prism that spans the current level
    glBegin(GL_QUADS);
    glNormal3i(0, 1, 0);
    glTexCoord2f(maxTexCoordU, 1.0f);
    glVertex3f(maxXCoord, maxYCoord,-HalfDepth);	// Top Right Of The Quad (Top)
    glTexCoord2f(minTexCoordU, 1.0f);
    glVertex3f(minXCoord, maxYCoord,-HalfDepth);	// Top Left Of The Quad (Top)
    glTexCoord2f(minTexCoordU, 0.0f);
    glVertex3f(minXCoord, maxYCoord, HalfDepth);	// Bottom Left Of The Quad (Top)
    glTexCoord2f(maxTexCoordU, 0.0f);
    glVertex3f(maxXCoord, maxYCoord, HalfDepth);	// Bottom Right Of The Quad (Top)

    glNormal3i(0, -1, 0);
    glTexCoord2f(maxTexCoordU, 1.0f);
    glVertex3f(maxXCoord, minYCoord, HalfDepth);	// Top Right Of The Quad (Bottom)
    glTexCoord2f(minTexCoordU, 1.0f);
    glVertex3f(minXCoord, minYCoord, HalfDepth);	// Top Left Of The Quad (Bottom)
    glTexCoord2f(minTexCoordU, 0.0f);
    glVertex3f(minXCoord, minYCoord,-HalfDepth);	// Bottom Left Of The Quad (Bottom)
    glTexCoord2f(maxTexCoordU, 0.0f);
    glVertex3f(maxXCoord, minYCoord,-HalfDepth);	// Bottom Right Of The Quad (Bottom)

    glNormal3i(0, 0, 1);
    glTexCoord2f(maxTexCoordU, 1.0f);
    glVertex3f(maxXCoord, maxYCoord, HalfDepth);	// Top Right Of The Quad (Front)
    glTexCoord2f(minTexCoordU, 1.0f);
    glVertex3f(minXCoord, maxYCoord, HalfDepth);	// Top Left Of The Quad (Front)
    glTexCoord2f(minTexCoordU, 0.0f);
    glVertex3f(minXCoord, minYCoord, HalfDepth);	// Bottom Left Of The Quad (Front)
    glTexCoord2f(maxTexCoordU, 0.0f);
    glVertex3f(maxXCoord, minYCoord, HalfDepth);	// Bottom Right Of The Quad (Front)

    glNormal3i(0, 0, -1);
    glTexCoord2f(maxTexCoordU, 1.0f);
    glVertex3f(maxXCoord, minYCoord,-HalfDepth);	// Top Right Of The Quad (Back)
    glTexCoord2f(minTexCoordU, 1.0f);
    glVertex3f(minXCoord, minYCoord,-HalfDepth);	// Top Left Of The Quad (Back)
    glTexCoord2f(minTexCoordU, 0.0f);
    glVertex3f(minXCoord, maxYCoord,-HalfDepth);	// Bottom Left Of The Quad (Back)
    glTexCoord2f(maxTexCoordU, 0.0f);
    glVertex3f(maxXCoord, maxYCoord,-HalfDepth);	// Bottom Right Of The Quad (Back)

    glNormal3i(-1, 0, 0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(minXCoord, maxYCoord, HalfDepth);	// Top Right Of The Quad (Left)
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(minXCoord, maxYCoord,-HalfDepth);	// Top Left Of The Quad (Left)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(minXCoord, minYCoord,-HalfDepth);	// Bottom Left Of The Quad (Left)
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(minXCoord, minYCoord, HalfDepth);	// Bottom Right Of The Quad (Left)

    glNormal3i(1, 0, 0);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(maxXCoord, maxYCoord,-HalfDepth);	// Top Right Of The Quad (Right)
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(maxXCoord, maxYCoord, HalfDepth);	// Top Left Of The Quad (Right)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(maxXCoord, minYCoord, HalfDepth);	// Bottom Left Of The Quad (Right)
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(maxXCoord, minYCoord,-HalfDepth);	// Bottom Right Of The Quad (Right)
    glEnd();
}

/**
 * Public function for regenerating the display list of this mesh for
 * a given level dimension - this will reshape and initialize the 
 * display list for the mesh so that the Draw function can be used properly.
 */
void BallSafetyNetMesh::Regenerate(const GameLevel& currLevel) {
	assert(this->displayListID != 0);

    std::pair<const LevelPiece*, const LevelPiece*> minMaxPieces = SafetyNet::GetSafetyNetMinMaxPiece(currLevel, this->isBottomNet);

    const float minX = minMaxPieces.first->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH;
    const float maxX = minMaxPieces.second->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH;
    const float midY = 0;//this->GetMidYCoord(currLevel);
	const float maxTexCoordU = (maxX - minX) / 2.0f;

	glNewList(this->displayListID, GL_COMPILE);
    BallSafetyNetMesh::DrawSafetyNetMesh(minX, maxX, midY, 0.0f, maxTexCoordU);
	glEndList();

	debug_opengl_state();

    this->centerPos[0] = maxTexCoordU;
    this->centerPos[1] = midY;
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
void BallSafetyNetMesh::DestroyBallSafetyNet(const GameLevel& currLevel, float destructionXPos) {
	assert(this->displayListID != 0);
	
	// Make sure that both display lists for the broken pieces get initialized
	assert(this->leftPieceDispListID != 0 && this->rightPieceDispListID != 0);

    std::pair<const LevelPiece*, const LevelPiece*> minMaxPieces = 
        SafetyNet::GetSafetyNetMinMaxPiece(currLevel, this->isBottomNet);

	// (Re)compile the 2 new display lists of the broken pieces of the safety net
	const float minX = minMaxPieces.first->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH;
    const float maxX = minMaxPieces.second->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH;
    const float midY = 0;//this->GetMidYCoord(currLevel);

    if (minX > destructionXPos) {
        assert(false);
        destructionXPos = minX;
    }
    if (maxX < destructionXPos) {
        assert(false);
        destructionXPos = maxX;
    }

	const float maxTexCoordLeft  = (destructionXPos - minX) / 2.0f;
	const float maxTexCoordRight = (maxX - minX) / 2.0f;
	
	// Left broken piece
	glNewList(this->leftPieceDispListID, GL_COMPILE);
    BallSafetyNetMesh::DrawSafetyNetMesh(minX, destructionXPos, midY, 0.0f, maxTexCoordLeft);
	glEndList();

	// Right broken piece
	glNewList(this->rightPieceDispListID, GL_COMPILE);
	BallSafetyNetMesh::DrawSafetyNetMesh(destructionXPos, maxX, midY, maxTexCoordLeft, maxTexCoordRight);
	glEndList();

	// Setup the linear interpolation values for the animations of the two pieces
	this->leftPieceOrient  = Orientation3D(Vector3D(0,0,0), Vector3D(0,0,0));
	this->rightPieceOrient = Orientation3D(Vector3D(0,0,0), Vector3D(0,0,0));
	this->pieceAlpha = 1.0f;

    float ySign = -1;
    if (!this->isBottomNet) {
        ySign = 1.0f;
    }

	this->leftPieceAnim.SetLerp(2.0, Orientation3D(Vector3D(-3.0f, ySign*5.0f, 0.0f), Vector3D(60.0f, 0.0f, 0.0f)));
	this->rightPieceAnim.SetLerp(2.0, Orientation3D(Vector3D(3.0f, ySign*5.0f, 0.0f), Vector3D(50.0f, 0.0f, 0.0f)));
	this->pieceFadeAnim.SetLerp(2.0, 0.0f);

	//this->playDestructionAnimation = true;
	this->currAnimation = BallSafetyNetMesh::DestructionAnimation;
}

void BallSafetyNetMesh::Tick(double dT) {
  
    // Check to see whether we're playing an animation or just drawing the safety net normally
    switch (this->currAnimation) {

        case BallSafetyNetMesh::CreationAnimation:{
            bool creationAnimComplete = this->pieceFadeAnim.Tick(dT);
            if (creationAnimComplete) {
                this->currAnimation = BallSafetyNetMesh::Idle;
            }
            break;
        }
            
        case BallSafetyNetMesh::DestructionAnimation: {
            bool leftFinished  = this->leftPieceAnim.Tick(dT);
            bool rightFinished = this->rightPieceAnim.Tick(dT);
            this->pieceFadeAnim.Tick(dT);

            if (leftFinished && rightFinished) {
                this->currAnimation = BallSafetyNetMesh::Dead;
            }
            break;
        }

        case BallSafetyNetMesh::Idle:
        case BallSafetyNetMesh::Dead:
            break;

        default:
            assert(false);
            break;
    }
}

/**
 * Draw the active display lists for this mesh.
 */
void BallSafetyNetMesh::Draw(const Camera& camera, const BasicPointLight& keyLight, 
                             const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

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
