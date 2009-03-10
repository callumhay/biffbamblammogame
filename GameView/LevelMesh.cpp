#include "LevelMesh.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "GameWorldAssets.h"

#include "../BlammoEngine/BlammoEngine.h"

LevelMesh::LevelMesh(const GameWorldAssets* gameWorldAssets, const GameLevel* level) : 
levelDimensions(0, 0), styleBlock(NULL), basicBlock(NULL), bombBlock(NULL) {
	
	// Load the basic block and all other block types that stay consistent between worlds
	this->basicBlock = ObjReader::ReadMesh(GameViewConstants::GetInstance()->BASIC_BLOCK_MESH_PATH);
	this->bombBlock  = ObjReader::ReadMesh(GameViewConstants::GetInstance()->BOMB_BLOCK_MESH);

	// Add the typical level meshes to the list of materials...
	std::map<std::string, MaterialGroup*> basicBlockMatGrps = this->basicBlock->GetMaterialGroups();
	std::map<std::string, MaterialGroup*> bombBlockMatGrps  = this->bombBlock->GetMaterialGroups();
	
	for (std::map<std::string, MaterialGroup*>::iterator iter = basicBlockMatGrps.begin(); iter != basicBlockMatGrps.end(); iter++) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}
	for (std::map<std::string, MaterialGroup*>::iterator iter = bombBlockMatGrps.begin(); iter != bombBlockMatGrps.end(); iter++) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}

	this->LoadNewLevel(gameWorldAssets, level);

}

LevelMesh::~LevelMesh() {
	// Delete all meshes
	delete this->basicBlock;
	this->basicBlock = NULL;
	delete this->bombBlock;
	this->bombBlock = NULL;

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
		}
	}
	this->displayListsPerMaterial.clear();
	this->pieceDisplayLists.clear();

	// Clear the level dimensions...
	this->levelDimensions = Vector2D(0,0);

}

/**
 * Load a new level mesh into this object. This will clear out any old loaded level.
 */
void LevelMesh::LoadNewLevel(const GameWorldAssets* gameWorldAssets, const GameLevel* level) {
	assert(gameWorldAssets != NULL);
	assert(level != NULL);

	// Make sure any previous levels are cleared...
	this->Flush();
	
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
	this->levelDimensions = Vector2D(level->GetLevelUnitWidth(), level->GetLevelUnitHeight());
	glTranslatef(-this->levelDimensions[0]/2.0f, -this->levelDimensions[1]/2.0f, 0.0f);

	// Go through each piece and create an appropriate display list for it
	for (size_t h = 0; h < levelPieces.size(); h++) {
		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			
			// Obtain the current level piece and create a display list for it if necessary
			LevelPiece* currPiece	= levelPieces[h][w];
			Point2D pieceLoc			= currPiece->GetCenter();

			// Set the appropriate transform to be applied to the piece
			glPushMatrix();
			glTranslatef(pieceLoc[0], pieceLoc[1], 0);
			float worldTransformVals[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, worldTransformVals);
			glPopMatrix();

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
	}
	this->pieceDisplayLists[&pieceBefore].clear();

	Point2D changedPieceLoc = pieceAfter.GetCenter();

	// Obtain the world transform matrix for the piece
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-this->levelDimensions[0]/2.0f, -this->levelDimensions[1]/2.0f, 0.0f);
	glTranslatef(changedPieceLoc[0], changedPieceLoc[1], 0);
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
void LevelMesh::Draw(const Camera& camera) const {
	// Go through each material and draw all the display lists corresponding to it
	for (std::map<CgFxMaterialEffect*, std::list<GLuint>>::const_iterator iter = this->displayListsPerMaterial.begin(); 
		iter != this->displayListsPerMaterial.end(); iter++) {

		iter->first->Draw(camera, iter->second);
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
		currPolyGrp->Translate(worldTranslation);

		GLuint newDisplayList = glGenLists(1);
		glNewList(newDisplayList, GL_COMPILE);
		glColor3f(currColour.R(), currColour.G(), currColour.B());
		currPolyGrp->Draw();
		glEndList();
	
		currPolyGrp->Translate(-1*worldTranslation);

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
		case LevelPiece::Breakable :
			returnValue = this->basicBlock->GetMaterialGroups();
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