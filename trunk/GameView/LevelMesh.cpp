#include "LevelMesh.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "GameWorldAssets.h"
#include "BallSafetyNetMesh.h"
#include "PrismBlockMesh.h"
#include "PortalBlockMesh.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Point.h"

#include "../ResourceManager.h"

#include "../GameModel/GameBall.h"

LevelMesh::LevelMesh(const GameWorldAssets* gameWorldAssets, const GameLevel* level) : currLevel(NULL),
styleBlock(NULL), basicBlock(NULL), bombBlock(NULL), triangleBlockUR(NULL), inkBlock(NULL), portalBlock(NULL),
prismBlockDiamond(NULL), prismBlockTriangleUR(NULL), ballSafetyNet(NULL) {
	
	// Load the basic block and all other block types that stay consistent between worlds
	this->basicBlock						= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BASIC_BLOCK_MESH_PATH);
	this->bombBlock							= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BOMB_BLOCK_MESH);
	this->triangleBlockUR				= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TRIANGLE_BLOCK_MESH_PATH);
	this->prismBlockDiamond			= new PrismBlockMesh(PrismBlockMesh::DiamondPrism);
	this->prismBlockTriangleUR	= new PrismBlockMesh(PrismBlockMesh::TrianglePrism);
	this->inkBlock							= ResourceManager::GetInstance()->GetInkBlockMeshResource();
	this->portalBlock						= new PortalBlockMesh();

	this->ballSafetyNet = new BallSafetyNetMesh();

	// Add the typical level meshes to the list of materials...
	const std::map<std::string, MaterialGroup*>& basicBlockMatGrps		= this->basicBlock->GetMaterialGroups();
	const std::map<std::string, MaterialGroup*>& triangleBlockMatGrps	= this->triangleBlockUR->GetMaterialGroups();
	const std::map<std::string, MaterialGroup*>& bombBlockMatGrps			= this->bombBlock->GetMaterialGroups();
	const std::map<std::string, MaterialGroup*>& inkBlockMatGrps			= this->inkBlock->GetMaterialGroups();
	const std::map<std::string, MaterialGroup*>& prismBlockMatGrps		= this->prismBlockDiamond->GetMaterialGroups();
	const std::map<std::string, MaterialGroup*>& prismTriBlockMatGrps	= this->prismBlockTriangleUR->GetMaterialGroups();
	const std::map<std::string, MaterialGroup*>& portalBlockMatGrps   = this->portalBlock->GetMaterialGroups();
	
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = basicBlockMatGrps.begin(); iter != basicBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = triangleBlockMatGrps.begin(); iter != triangleBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = bombBlockMatGrps.begin(); iter != bombBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = inkBlockMatGrps.begin(); iter != inkBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = prismBlockMatGrps.begin(); iter != prismBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = prismTriBlockMatGrps.begin(); iter != prismTriBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = portalBlockMatGrps.begin(); iter != portalBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}	

	this->LoadNewLevel(gameWorldAssets, level);
}

LevelMesh::~LevelMesh() {
	// Delete all meshes
	delete this->ballSafetyNet;
	this->ballSafetyNet = NULL;
	delete this->prismBlockDiamond;
	this->prismBlockDiamond = NULL;
	delete this->prismBlockTriangleUR;
	this->prismBlockTriangleUR = NULL;

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
	for (std::map<CgFxMaterialEffect*, std::vector<GLuint>>::iterator iter = this->displayListsPerMaterial.begin(); 
		iter != this->displayListsPerMaterial.end(); ++iter) {
		
		for (std::vector<GLuint>::iterator dispListIter = iter->second.begin(); dispListIter != iter->second.end(); ++dispListIter) {
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
	for (std::map<std::string, MaterialGroup*>::iterator iter = styleBlockMatGrps.begin(); iter != styleBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}

	// Load the actual level meshes as precomputed batches for speed...
	const std::vector<std::vector<LevelPiece*>>& levelPieces = level->GetCurrentLevelLayout();

	// Get the proper vector to center the level
	Vector2D levelDimensions = Vector2D(level->GetLevelUnitWidth(), level->GetLevelUnitHeight());
	
	// Create the ball safety net for the level
	this->ballSafetyNet->Regenerate(levelDimensions);

	Vector3D worldTransform(-levelDimensions[0]/2.0f, -levelDimensions[1]/2.0f, 0.0f);

	// Go through each piece and create an appropriate display list for it
	for (size_t h = 0; h < levelPieces.size(); h++) {
		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			
			// Obtain the current level piece and create a display list for it if necessary
			LevelPiece* currPiece	= levelPieces[h][w];

			// Create the appropriate display lists for the piece...
			this->CreateDisplayListsForPiece(currPiece, worldTransform);
		}
	}
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
		iter != pieceInfoIter->second.end(); ++iter) {
		
		// Delete any previous display list...
		glDeleteLists(iter->second, 1);

		// ... and remove it from other relevant maps/arrays/etc.
		std::vector<GLuint>& displayLists = this->displayListsPerMaterial[iter->first];
		for(std::vector<GLuint>::iterator iterDL = displayLists.begin(); iterDL != displayLists.end(); ++iterDL) {
			if (*iterDL == iter->second)	{
				displayLists.erase(iterDL);
				break;
			}
		}
		if (displayLists.size() == 0) {
			this->displayListsPerMaterial.erase(iter->first);
		}

		// Clean up
		iter->second = 0;
	}
	pieceInfoIter->second.clear();
	this->pieceDisplayLists.erase(pieceInfoIter);

	// Based on the new piece type we re-create a display list
	Point2D changedPieceLoc = pieceAfter.GetCenter();
	Vector2D levelDimensions = Vector2D(this->currLevel->GetLevelUnitWidth(), this->currLevel->GetLevelUnitHeight());
	Vector3D translation(-levelDimensions[0]/2.0f, -levelDimensions[1]/2.0f, 0.0f);
	this->CreateDisplayListsForPiece(&pieceAfter, translation);
}

/**
 * Draw the current level mesh pieces (i.e., blocks that make up the level).
 */
void LevelMesh::DrawPieces(double dT, const Camera& camera, const PointLight& keyLight, 
													 const PointLight& fillLight, const PointLight& ballLight, const Texture2D* sceneTexture) {

	// Set any appropriate parameters on the various meshes materials, etc.
	this->prismBlockDiamond->SetSceneTexture(sceneTexture);
	this->prismBlockTriangleUR->SetSceneTexture(sceneTexture);
	this->portalBlock->SetSceneTexture(sceneTexture);
	this->portalBlock->Tick(dT);

	// Go through each material and draw all the display lists corresponding to it
	for (std::map<CgFxMaterialEffect*, std::vector<GLuint>>::const_iterator iter = this->displayListsPerMaterial.begin(); 
		iter != this->displayListsPerMaterial.end(); ++iter) {
		
		CgFxMaterialEffect* currEffect = iter->first;
		currEffect->SetKeyLight(keyLight);
		currEffect->SetFillLight(fillLight);
		currEffect->SetBallLight(ballLight);
		currEffect->Draw(camera, iter->second);
	}
}

/**
 * Draw the ball safety net if it is currently active in the game (this
 * appears at the bottom of the level).
 */
void LevelMesh::DrawSafetyNet(double dT, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) const {
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
	for (std::map<std::string, MaterialGroup*>::iterator iter = pieceMatGrps.begin(); iter != pieceMatGrps.end(); ++iter) {
		
		// Make sure that the material exists in our set of available level materials
		std::map<std::string, CgFxMaterialEffect*>::iterator currMaterialIter = this->levelMaterials.find(iter->first);
		assert(currMaterialIter != this->levelMaterials.end());

		CgFxMaterialEffect* currMaterial	= currMaterialIter->second;
		PolygonGroup* currPolyGrp					= iter->second->GetPolygonGroup();
		Colour currColour									= piece->GetColour();

		assert(currMaterial != NULL);
		assert(currPolyGrp != NULL);

		// Transform the polygon group to its position in the level, draw it and then transform it back
		Matrix4x4 worldTransform = Matrix4x4::translationMatrix(worldTranslation);
		Matrix4x4 localTransform = piece->GetPieceToLevelTransform();
		Matrix4x4 fullTransform =  worldTransform * localTransform;
		
		GLuint newDisplayList = glGenLists(1);
		assert(newDisplayList != 0);

		currPolyGrp->Transform(fullTransform);
		
		// TODO: other local transform operations to the mesh e.g., reflect, rotate, etc.
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
		case LevelPiece::Breakable:
			returnValue = this->basicBlock->GetMaterialGroups();
			break;
		case LevelPiece::BreakableTriangle:
			returnValue = this->triangleBlockUR->GetMaterialGroups();
			break;
		case LevelPiece::Bomb:
			returnValue = this->bombBlock->GetMaterialGroups();
			break;
		case LevelPiece::Ink:
			returnValue = this->inkBlock->GetMaterialGroups();
			break;
		case LevelPiece::Prism:
			returnValue = this->prismBlockDiamond->GetMaterialGroups();
			break;
		case LevelPiece::PrismTriangle:
			returnValue = this->prismBlockTriangleUR->GetMaterialGroups();
			break;
		case LevelPiece::Portal:
			returnValue = this->portalBlock->GetMaterialGroups();
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

/**

 */
void LevelMesh::PaddleCameraActiveToggle(bool isActive) {
	// Call to make the safety net visible or not when its activated. This is
  // useful to avoid obstruction of the player's viewpoint e.g., when in paddle camera mode.
	float safetyNetTransparency = isActive ? 0.5f : 1.0f;
	this->ballSafetyNet->SetTransparency(safetyNetTransparency);
}