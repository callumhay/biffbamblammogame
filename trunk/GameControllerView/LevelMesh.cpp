#include "LevelMesh.h"
#include "Camera.h"
#include "GameDisplay.h"
#include "ObjReader.h"
#include "GameAssets.h"
#include "CgShaderManager.h"

#include "../Utils/Vector.h"

// Filepaths for block assets
const std::string LevelMesh::BASIC_BLOCK_MESH_PATH			= GameAssets::RESOURCE_DIR + "/" + GameAssets::MESH_DIR + "/block.obj";
const std::string LevelMesh::DECO_BLOCK_MESH_PATH				= GameAssets::RESOURCE_DIR + "/" + GameAssets::MESH_DIR + "/deco_block.obj";
const std::string LevelMesh::CYBERPUNK_BLOCK_MESH_PATH	= GameAssets::RESOURCE_DIR + "/" + GameAssets::MESH_DIR + "/cyberpunk_block.obj";

LevelMesh::LevelMesh(GameWorld::WorldStyle worldStyle) : styleBlock(NULL), basicBlock(NULL) {
	
	// Based on the world style read-in the appropriate block
	switch(worldStyle) {
		default:
		case GameWorld::Deco:
			this->styleBlock = ObjReader::ReadMesh(DECO_BLOCK_MESH_PATH);
			break;
		case GameWorld::Cyberpunk:
			this->styleBlock = ObjReader::ReadMesh(CYBERPUNK_BLOCK_MESH_PATH);
			break;
	}
	// Load the basic block
	this->basicBlock = ObjReader::ReadMesh(BASIC_BLOCK_MESH_PATH);
	
	// Make sure the block was loaded and that it has only one material on it
	assert(this->styleBlock != NULL);
	assert(this->styleBlock->GetMaterialGroups().size() == 1);
}

LevelMesh::~LevelMesh() {
	// Delete all meshes
	delete this->styleBlock;
	delete this->basicBlock;

	// Delete all the pieces
	for (size_t h = 0; h < this->pieceMeshes.size(); h++) {
		for (size_t w = 0; w < this->pieceMeshes[h].size(); w++) {
			glDeleteLists(this->pieceMeshes[h][w], 1);
		}
		this->pieceMeshes[h].clear();
	}
	this->pieceMeshes.clear();
}

/**
 * Adjust the level mesh to change the given piece to the next type it will become after
 * being hit by the ball.
 */
void LevelMesh::ChangePiece(const LevelPiece& piece) {
	LevelPiece::LevelPieceType currPieceType = piece.GetType();
	
	// Leave in the case that it's empty
	if (currPieceType == LevelPiece::Empty) {
		return;
	}
	LevelPiece::LevelPieceType nextPieceType = LevelPiece::GetDecrementedPieceType(currPieceType);

	// Move the piece from one draw array to the next
	GLint dispListNum = this->pieceMeshes[piece.GetHeightIndex()][piece.GetWidthIndex()];
	
	// Remove the piece from its current array
	std::vector<GLint> &srcPieceSet  = this->piecesByMaterial[currPieceType];
	std::vector<GLint>::iterator pieceSetIter = srcPieceSet.begin();
	for (; pieceSetIter != srcPieceSet.end(); pieceSetIter++) {
		if ((*pieceSetIter) == dispListNum) {
			srcPieceSet.erase(pieceSetIter);
			break;
		}
	}

	// If the piece is being destoryed then do so
	if (nextPieceType == LevelPiece::Empty) {
		glDeleteLists(dispListNum, 1);
		this->pieceMeshes[piece.GetHeightIndex()][piece.GetWidthIndex()] = NO_PIECE;
		return;
	}
	
	// .. otherwise move it to the proper destination array
	this->piecesByMaterial[nextPieceType].push_back(dispListNum);
}

void LevelMesh::Draw(const Camera& camera) const {

	// Obtain block material stuffs
	CgFxEffect* styleBlockMaterial = this->styleBlock->GetMaterialGroups().begin()->second->GetMaterial();
	CgFxEffect* basicBlockMaterial = this->basicBlock->GetMaterialGroups().begin()->second->GetMaterial();

	MaterialProperties* materialBasicProps = basicBlockMaterial->GetProperties();

	// Set all of the required Cg parameters
	// Set model-view-proj transform parameter
	styleBlockMaterial->SetWorldViewProjMatrixParam();
	basicBlockMaterial->SetWorldViewProjMatrixParam();

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set the model-view transform parameters
	styleBlockMaterial->SetWorldInverseMatrixParam();
	styleBlockMaterial->SetWorldMatrixParam();
	basicBlockMaterial->SetWorldInverseMatrixParam();
	basicBlockMaterial->SetWorldMatrixParam();
	
	Matrix4x4 invViewXf = camera.GetInvViewTransform();
	glMultMatrixf(invViewXf.begin());
	
	// Set the inverse view transform parameter
	styleBlockMaterial->SetViewInverseMatrixParam();
	basicBlockMaterial->SetViewInverseMatrixParam();

	glPopMatrix();

	// Material properties
	styleBlockMaterial->SetSpecularColourParam();
	styleBlockMaterial->SetShininessParam();
	basicBlockMaterial->SetSpecularColourParam();
	basicBlockMaterial->SetShininessParam();

	// Textures
	styleBlockMaterial->SetDiffuseTextureSamplerParam();
	basicBlockMaterial->SetDiffuseTextureSamplerParam();

	// Lights... TODO...
	styleBlockMaterial->SetSceneLightColourParam(Colour(1, 1, 1));
	styleBlockMaterial->SetSceneLightPositionParam(Point3D(0, 0, -20));
	basicBlockMaterial->SetSceneLightColourParam(Colour(1, 1, 1));
	basicBlockMaterial->SetSceneLightPositionParam(Point3D(0, 0, -20));

	std::map<LevelPiece::LevelPieceType, std::vector<GLint>>::const_iterator pieceMatIter;
	for (pieceMatIter = this->piecesByMaterial.begin(); pieceMatIter != this->piecesByMaterial.end(); pieceMatIter++) {
		
		LevelPiece::LevelPieceType pieceType = pieceMatIter->first;
		const std::vector<GLint>& pieces = pieceMatIter->second;
		
		// Draw the current material
		CGpass pass = cgGetFirstPass(basicBlockMaterial->GetCurrentTechnique());
		switch(pieceType) {
			case LevelPiece::RedBreakable:
				materialBasicProps->diffuse = Colour(1.0f, 0.0f, 0.0f);
				break;			
			case LevelPiece::OrangeBreakable: 
				materialBasicProps->diffuse = Colour(1.0f, 0.5f,  0.0f);
				break;
			case LevelPiece::YellowBreakable: 
				materialBasicProps->diffuse = Colour(1.0f, 1.0f, 0.0f);
				break;
			case LevelPiece::GreenBreakable:
				materialBasicProps->diffuse = Colour(0.0f, 1.0f, 0.0f);
				break;			
			case LevelPiece::Solid:
				pass = cgGetFirstPass(styleBlockMaterial->GetCurrentTechnique());
				break;
			default:
				assert(false);
				break;
		}

		// Set the diffuse colour
		styleBlockMaterial->SetDiffuseColourParam();
		basicBlockMaterial->SetDiffuseColourParam();

		// Draw the pieces of the current material
		cgSetPassState(pass);
		for (size_t i = 0; i < pieces.size(); i++) {
			glCallList(pieces[i]);
		}
		cgResetPassState(pass);
	}

	CgShaderManager::Instance()->CheckForCgError("Drawing level piece effect");

	// Go through each piece and draw the outlines
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	GameDisplay::SetOutlineRenderAttribs(1.0f);

	for (size_t h = 0; h < this->pieceMeshes.size(); h++) {
		for (size_t w = 0; w < this->pieceMeshes[h].size(); w++) {
			glCallList(this->pieceMeshes[h][w]);
		}
	}
	glPopAttrib();
	
}

LevelMesh* LevelMesh::CreateLevelMesh(GameWorld::WorldStyle worldStyle, const GameLevel* level) {
	assert(level != NULL);

	LevelMesh* newLvlMesh = new LevelMesh(worldStyle);
	
	PolygonGroup* solidBlockPoly			= newLvlMesh->styleBlock->GetMaterialGroups().begin()->second->GetPolygonGroup();
	PolygonGroup* breakableBlockPoly	= newLvlMesh->basicBlock->GetMaterialGroups().begin()->second->GetPolygonGroup();

	const std::vector<std::vector<LevelPiece*>>& levelPieces = level->GetCurrentLevelLayout();
	
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Get the proper vector to center the level
	Vector2D levelDim(level->GetLevelUnitWidth(), level->GetLevelUnitHeight());
	glTranslatef(-levelDim[0]/2.0f, -levelDim[1]/2.0f, 0.0f);

	// Go through each piece and create an appropriate display list for it
	for (size_t h = 0; h < levelPieces.size(); h++) {
		std::vector<GLint> rowBlocks;

		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			LevelPiece* p = levelPieces[h][w];

			if (p->GetType() == LevelPiece::Empty || p->GetType() == LevelPiece::Bomb) {
				rowBlocks.push_back(NO_PIECE);
				continue;
			}
			
			// Assign the correct polygon group based on the type of block
			PolygonGroup* currBlockPoly = breakableBlockPoly;
			if (p->GetType() == LevelPiece::Solid) {
				currBlockPoly = solidBlockPoly;
			}

			Point2D loc = p->GetCenter();

			// Get this piece's appropriate transform
			glPushMatrix();
			glTranslatef(loc[0], loc[1], 0);
			float worldTransformVals[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, worldTransformVals);
			glPopMatrix();
			Vector3D translation(worldTransformVals[12], worldTransformVals[13], worldTransformVals[14]);

			// Get the current transform matrix and apply it to a duplicated
			// polygon group of a block
			currBlockPoly->Translate(translation);

			GLint newDisplayList = glGenLists(1);
			glNewList(newDisplayList, GL_COMPILE);
			currBlockPoly->Draw();
			glEndList();
			
			// Transform the polygon group back to its original position
			currBlockPoly->Translate(-1*translation);
		
			rowBlocks.push_back(newDisplayList);
			newLvlMesh->piecesByMaterial[p->GetType()].push_back(newDisplayList);
		}
		newLvlMesh->pieceMeshes.push_back(rowBlocks);
	}
	glPopMatrix();

	return newLvlMesh;
}