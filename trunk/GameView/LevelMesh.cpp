#include "LevelMesh.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "GameWorldAssets.h"
#include "BallSafetyNetMesh.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/ResourceManager.h"

#include "../GameModel/GameBall.h"

LevelMesh::LevelMesh(const GameWorldAssets* gameWorldAssets, const GameLevel* level) : currLevel(NULL),
styleBlock(NULL), basicBlock(NULL), bombBlock(NULL), triangleBlockUR(NULL), ballSafetyNet(NULL) {
	
	// Load the basic block and all other block types that stay consistent between worlds
	this->basicBlock			= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BASIC_BLOCK_MESH_PATH);
	this->bombBlock				= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BOMB_BLOCK_MESH);
	this->triangleBlockUR = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TRIANGLE_BLOCK_MESH_PATH);

	this->ballSafetyNet = new BallSafetyNetMesh();

	// Add the typical level meshes to the list of materials...
	std::map<std::string, MaterialGroup*> basicBlockMatGrps			= this->basicBlock->GetMaterialGroups();
	std::map<std::string, MaterialGroup*> triangleBlockMatGrps	= this->triangleBlockUR->GetMaterialGroups();
	std::map<std::string, MaterialGroup*> bombBlockMatGrps			= this->bombBlock->GetMaterialGroups();
	
	for (std::map<std::string, MaterialGroup*>::iterator iter = basicBlockMatGrps.begin(); iter != basicBlockMatGrps.end(); iter++) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::iterator iter = triangleBlockMatGrps.begin(); iter != triangleBlockMatGrps.end(); iter++) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::iterator iter = bombBlockMatGrps.begin(); iter != bombBlockMatGrps.end(); iter++) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}

	this->LoadNewLevel(gameWorldAssets, level);
}

LevelMesh::~LevelMesh() {
	// Delete all meshes
	delete this->ballSafetyNet;
	this->ballSafetyNet = NULL;

	// Clean up all assets pertaining to the currently loaded
	// level, if applicable.
	this->Flush();
}

/** 
 * Clean all the assets of the currently loaded level (if there is one loaded).
 */
void LevelMesh::Flush() {
	// Clear the style block
	if (this->styleBlock != NULL) {
		
		// Remove the materials associated with the style block...
		std::map<std::string, MaterialGroup*> styleBlockMatGrps = this->styleBlock->GetMaterialGroups();
		for (std::map<std::string, MaterialGroup*>::iterator iter = styleBlockMatGrps.begin(); iter != styleBlockMatGrps.end(); iter++) {
			
			// Make sure the material group can be found
			std::map<std::string, CgFxMaterialEffect*>::iterator foundMaterial = this->levelMaterials.find(iter->first);
			assert(foundMaterial != this->levelMaterials.end());

			this->levelMaterials.erase(foundMaterial);
		}
	}
	
	// Delete each of the display lists loaded for the previous level and clear up the
	// relevant mappings to those display lists.
	for (std::map<CgFxMaterialEffect*, std::list<GLuint>>::iterator iter = this->displayListsPerMaterial.begin(); 
		iter != this->displayListsPerMaterial.end(); iter++) {
		
		for (std::list<GLuint>::iterator dispListIter = iter->second.begin(); dispListIter != iter->second.end(); dispListIter++) {
			glDeleteLists((*dispListIter), 1);
			(*dispListIter) = 0;
		}
	}
	this->displayListsPerMaterial.clear();
	this->pieceDisplayLists.clear();

	// Clear the current level pointer
	this->currLevel = NULL;
}

/**
 * Load a new level mesh into this object. This will clear out any old loaded level.
 */
void LevelMesh::LoadNewLevel(const GameWorldAssets* gameWorldAssets, const GameLevel* level) {
	assert(gameWorldAssets != NULL);
	assert(level != NULL);

	// Make sure any previous levels are cleared...
	this->Flush();
	
	// Set the current level pointer
	this->currLevel = level;

	// Based on the world style read-in the appropriate block
	this->styleBlock = gameWorldAssets->GetWorldStyleBlock();

	// Load the materials for the style block...
	std::map<std::string, MaterialGroup*> styleBlockMatGrps  = this->styleBlock->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::iterator iter = styleBlockMatGrps.begin(); iter != styleBlockMatGrps.end(); iter++) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}

	// Load the actual level meshes as display lists...
	const std::vector<std::vector<LevelPiece*>>& levelPieces = level->GetCurrentLevelLayout();

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Get the proper vector to center the level
	Vector2D levelDimensions = Vector2D(level->GetLevelUnitWidth(), level->GetLevelUnitHeight());
	
	// Create the ball safety net for the level
	this->ballSafetyNet->Regenerate(levelDimensions);

	glTranslatef(-levelDimensions[0]/2.0f, -levelDimensions[1]/2.0f, 0.0f);

	// Get the appropriate transform to be applied to the piece
	float worldTransformVals[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, worldTransformVals);

	// Go through each piece and create an appropriate display list for it
	for (size_t h = 0; h < levelPieces.size(); h++) {
		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			
			// Obtain the current level piece and create a display list for it if necessary
			LevelPiece* currPiece	= levelPieces[h][w];

			// Create the appropriate display lists for the piece...
			Vector3D translation(worldTransformVals[12], worldTransformVals[13], worldTransformVals[14]);
			this->CreateDisplayListsForPiece(currPiece, translation);
		}
	}
	glPopMatrix();
}

/**
 * Adjust the level mesh to change the given piece.
 * Please note that this can be an EXPENSIVE operation!!
 * (Do not call > 1 per frame!!)
 */
void LevelMesh::ChangePiece(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {
	// Find the changed piece and change its display list...
	std::map<const LevelPiece*, std::map<CgFxMaterialEffect*, GLuint>>::iterator pieceInfoIter = this->pieceDisplayLists.find(&pieceBefore);
	assert(pieceInfoIter != this->pieceDisplayLists.end());

	// Go through each of the materials and clear up previous display lists and materials...
	for (std::map<CgFxMaterialEffect*, GLuint>::iterator iter = pieceInfoIter->second.begin();
		iter != pieceInfoIter->second.end(); iter++) {
		
		// Delete any previous display list...
		glDeleteLists(iter->second, 1);
		// ... and remove it from other relevant maps/arrays/etc.
		this->displayListsPerMaterial[iter->first].remove(iter->second);
		
		// Clean up
		iter->second = 0;
	}
	this->pieceDisplayLists[&pieceBefore].clear();

	Point2D changedPieceLoc = pieceAfter.GetCenter();
	Vector2D levelDimensions = Vector2D(this->currLevel->GetLevelUnitWidth(), this->currLevel->GetLevelUnitHeight());

	// Obtain the world transform matrix for the piece
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-levelDimensions[0]/2.0f, -levelDimensions[1]/2.0f, 0.0f);
	//glTranslatef(changedPieceLoc[0], changedPieceLoc[1], 0);
	float worldTransformVals[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, worldTransformVals);
	glPopMatrix();

	// Based on the new piece type we re-create a display list
	Vector3D translation(worldTransformVals[12], worldTransformVals[13], worldTransformVals[14]);
	this->CreateDisplayListsForPiece(&pieceAfter, translation);
}

/**
 * Draw the current level mesh.
 */
void LevelMesh::Draw(double dT, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) const {
	// Go through each material and draw all the display lists corresponding to it
	for (std::map<CgFxMaterialEffect*, std::list<GLuint>>::const_iterator iter = this->displayListsPerMaterial.begin(); 
		iter != this->displayListsPerMaterial.end(); iter++) {
		
		CgFxMaterialEffect* currEffect = iter->first;
		currEffect->SetKeyLight(keyLight);
		currEffect->SetFillLight(fillLight);
		currEffect->SetBallLight(ballLight);
		currEffect->Draw(camera, iter->second);
	}

	// If the ball safety net is active then we draw it
	assert(this->currLevel != NULL);
	if (this->currLevel->IsBallSafetyNetActive() || this->ballSafetyNet->IsPlayingAnimation()) {
		Vector2D levelDimensions = Vector2D(this->currLevel->GetLevelUnitWidth(), this->currLevel->GetLevelUnitHeight());

		glPushMatrix();
		glTranslatef(-levelDimensions[0] / 2.0f, -(levelDimensions[1] / 2.0f + LevelPiece::HALF_PIECE_HEIGHT), 0.0f);
		this->ballSafetyNet->Draw(dT, camera, keyLight, fillLight, ballLight);
		glPopMatrix();
	}
}

/**
 * Private helper function that will create the appropriate display lists for drawing the given piece
 * at the given translation in the world.
 */
void LevelMesh::CreateDisplayListsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation) {
	
	std::map<std::string, MaterialGroup*> pieceMatGrps = this->GetMaterialGrpsForPieceType(piece->GetType());

	// Go through each of the material groups ensuring that the material is associated with an appropriate display list
	for (std::map<std::string, MaterialGroup*>::iterator iter = pieceMatGrps.begin(); iter != pieceMatGrps.end(); iter++) {
		
		// Make sure that the material exists in our set of available level materials
		std::map<std::string, CgFxMaterialEffect*>::iterator currMaterialIter = this->levelMaterials.find(iter->first);
		assert(currMaterialIter != this->levelMaterials.end());

		CgFxMaterialEffect* currMaterial	= currMaterialIter->second;
		PolygonGroup* currPolyGrp					= iter->second->GetPolygonGroup();
		Colour currColour									= piece->GetColour();

		assert(currMaterial != NULL);
		assert(currPolyGrp  != NULL);

		// Transform the polygon group to its position in the level, draw it and then transform it back
		Matrix4x4 worldTransform = Matrix4x4::translationMatrix(worldTranslation);
		Matrix4x4 localTransform = piece->GetPieceToLevelTransform();
		Matrix4x4 fullTransform =  worldTransform * localTransform;
		
		currPolyGrp->Transform(fullTransform);
		
		// TODO: other local transform operations to the mesh e.g., reflect, rotate, etc.
		GLuint newDisplayList = glGenLists(1);
		glNewList(newDisplayList, GL_COMPILE);
		glColor3f(currColour.R(), currColour.G(), currColour.B());
		currPolyGrp->Draw();
		glEndList();
	
		Matrix4x4 localInvTransform = piece->GetPieceToLevelInvTransform();
		Matrix4x4 worldInvTransform = Matrix4x4::translationMatrix(-worldTranslation);
		Matrix4x4 fullInvTransform = localInvTransform * worldInvTransform;
		
		currPolyGrp->Transform(fullInvTransform);

		// Insert the new display list into the list of display lists...
		this->pieceDisplayLists[piece].insert(std::make_pair<CgFxMaterialEffect*, GLuint>(currMaterial, newDisplayList));
		this->displayListsPerMaterial[currMaterial].push_back(newDisplayList);
	}
}

/**
 * Private helper function for obtaining the material groups associated with a particular
 * Level Piece type.
 * Returns: A map of the identifiers to corresponding material groups for a particular mesh representation
 * of the given level piece type.
 */
std::map<std::string, MaterialGroup*> LevelMesh::GetMaterialGrpsForPieceType(LevelPiece::LevelPieceType type) const {
	
	std::map<std::string, MaterialGroup*> returnValue;
	switch (type) {
		case LevelPiece::Solid :
			returnValue = this->styleBlock->GetMaterialGroups();
			break;
		case LevelPiece::SolidTriangle:
			returnValue = this->triangleBlockUR->GetMaterialGroups();
			break;
		case LevelPiece::Breakable :
			returnValue = this->basicBlock->GetMaterialGroups();
			break;
		case LevelPiece::BreakableTriangle:
			returnValue = this->triangleBlockUR->GetMaterialGroups();
			break;
		case LevelPiece::Bomb :
			returnValue = this->bombBlock->GetMaterialGroups();
			break;
		case LevelPiece::Empty :
			break;
		default:
			assert(false);
			break;
	}
	return returnValue;
}

/**
 * Call when the ball safety net has been created. This will prompt any animations /
 * effects associated with the mesh representing the safety net.
 */
void LevelMesh::BallSafetyNetCreated() {
	this->ballSafetyNet->CreateBallSafetyNet();
}

/**
 * Call when the ball safety net has been destroyed this will prompt any
 * animations / effects associated with the mesh representing the safety net.
 */
void LevelMesh::BallSafetyNetDestroyed(const GameBall& ball) {
	Vector2D levelDimensions = Vector2D(this->currLevel->GetLevelUnitWidth(), this->currLevel->GetLevelUnitHeight());
	this->ballSafetyNet->DestroyBallSafetyNet(levelDimensions, ball.GetBounds().Center()[0]);
}