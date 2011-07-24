/**
 * LevelMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2009-2010
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "LevelMesh.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "GameWorldAssets.h"
#include "PrismBlockMesh.h"
#include "PortalBlockMesh.h"
#include "CannonBlockMesh.h"
#include "CollateralBlockMesh.h"
#include "TeslaBlockMesh.h"
#include "SwitchBlockMesh.h"

#include "../ESPEngine/ESPEmitter.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Point.h"

#include "../ResourceManager.h"

#include "../GameModel/GameBall.h"
#include "../GameModel/CannonBlock.h"
#include "../GameModel/CollateralBlock.h"
#include "../GameModel/TeslaBlock.h"
#include "../GameModel/SwitchBlock.h"
#include "../GameModel/OneWayBlock.h"

LevelMesh::LevelMesh(const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level) : currLevel(NULL),
styleBlock(NULL), basicBlock(NULL), bombBlock(NULL), triangleBlockUR(NULL), inkBlock(NULL), portalBlock(NULL),
prismBlockDiamond(NULL), prismBlockTriangleUR(NULL), ballSafetyNet(NULL), cannonBlock(NULL), collateralBlock(NULL),
teslaBlock(NULL), switchBlock(NULL), oneWayUpBlock(NULL), oneWayDownBlock(NULL), oneWayLeftBlock(NULL), 
oneWayRightBlock(NULL), statusEffectRenderer(NULL) {
	
	// Load the basic block and all other block types that stay consistent between worlds
	this->basicBlock					= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BASIC_BLOCK_MESH_PATH);
	this->bombBlock						= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BOMB_BLOCK_MESH);
	this->triangleBlockUR				= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TRIANGLE_BLOCK_MESH_PATH);
	this->prismBlockDiamond			    = new PrismBlockMesh(PrismBlockMesh::DiamondPrism);
	this->prismBlockTriangleUR	        = new PrismBlockMesh(PrismBlockMesh::TrianglePrism);
	this->inkBlock						= ResourceManager::GetInstance()->GetInkBlockMeshResource();
	this->portalBlock					= new PortalBlockMesh();
	this->cannonBlock					= new CannonBlockMesh();
	this->collateralBlock				= new CollateralBlockMesh();
	this->teslaBlock                    = new TeslaBlockMesh();
	this->itemDropBlock					= new ItemDropBlockMesh();
    this->switchBlock                   = new SwitchBlockMesh();
    this->oneWayUpBlock                 = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ONE_WAY_BLOCK_UP_MESH);
    this->oneWayDownBlock               = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ONE_WAY_BLOCK_DOWN_MESH);
    this->oneWayLeftBlock               = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ONE_WAY_BLOCK_LEFT_MESH);
    this->oneWayRightBlock              = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->ONE_WAY_BLOCK_RIGHT_MESH);

	this->ballSafetyNet = new BallSafetyNetMesh();

	// Add the typical level meshes to the list of materials...
#define INSERT_MATERIAL_GRPS(block) { const std::map<std::string, MaterialGroup*>& matGrps = block->GetMaterialGroups(); \
    for (std::map<std::string, MaterialGroup*>::const_iterator iter = matGrps.begin(); iter != matGrps.end(); ++iter) { \
    this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial())); } }
    
    INSERT_MATERIAL_GRPS(this->basicBlock);
    INSERT_MATERIAL_GRPS(this->triangleBlockUR);
    INSERT_MATERIAL_GRPS(this->bombBlock);
    INSERT_MATERIAL_GRPS(this->inkBlock);
    INSERT_MATERIAL_GRPS(this->prismBlockDiamond);
    INSERT_MATERIAL_GRPS(this->prismBlockTriangleUR);
    INSERT_MATERIAL_GRPS(this->portalBlock);
    INSERT_MATERIAL_GRPS(this->cannonBlock);
    INSERT_MATERIAL_GRPS(this->teslaBlock);
    INSERT_MATERIAL_GRPS(this->itemDropBlock);
    INSERT_MATERIAL_GRPS(this->switchBlock);
    INSERT_MATERIAL_GRPS(this->oneWayUpBlock);
    INSERT_MATERIAL_GRPS(this->oneWayDownBlock);
    INSERT_MATERIAL_GRPS(this->oneWayLeftBlock);
    INSERT_MATERIAL_GRPS(this->oneWayRightBlock);

#undef INSERT_MATERIAL_GRPS

	// Initialize the status renderer
	this->statusEffectRenderer = new BlockStatusEffectRenderer();

	this->LoadNewLevel(gameWorldAssets, gameItemAssets, level);
}

LevelMesh::~LevelMesh() {
	// Clean up all assets pertaining to the currently loaded
	// level, if applicable.
	this->Flush();

	// Delete all meshes
	delete this->ballSafetyNet;
	this->ballSafetyNet = NULL;
	delete this->prismBlockDiamond;
	this->prismBlockDiamond = NULL;
	delete this->prismBlockTriangleUR;
	this->prismBlockTriangleUR = NULL;
	delete this->portalBlock;
	this->portalBlock = NULL;
	delete this->cannonBlock;
	this->cannonBlock = NULL;
	delete this->collateralBlock;
	this->collateralBlock = NULL;
	delete this->teslaBlock;
	this->teslaBlock = NULL;
	delete this->itemDropBlock;
	this->itemDropBlock = NULL;
    delete this->switchBlock;
    this->switchBlock = NULL;

	// Delete the status effect renderer
	delete this->statusEffectRenderer;
	this->statusEffectRenderer = NULL;
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
	for (std::map<CgFxMaterialEffect*, std::vector<GLuint> >::iterator iter = this->displayListsPerMaterial.begin();
		iter != this->displayListsPerMaterial.end(); ++iter) {
		
		for (std::vector<GLuint>::iterator dispListIter = iter->second.begin(); dispListIter != iter->second.end(); ++dispListIter) {
			glDeleteLists((*dispListIter), 1);
			(*dispListIter) = 0;
		}
	}
	this->displayListsPerMaterial.clear();
	this->pieceDisplayLists.clear();

	// Delete all of the emitter effects for any of the level pieces
	for (std::map<const LevelPiece*, std::list<ESPEmitter*> >::iterator pieceIter = this->pieceEmitterEffects.begin();
		pieceIter != this->pieceEmitterEffects.end(); ++pieceIter) {

		std::list<ESPEmitter*>& emitterList = pieceIter->second;
		for (std::list<ESPEmitter*>::iterator emitterIter = emitterList.begin(); emitterIter != emitterList.end(); ++emitterIter) {
			ESPEmitter* emitter = *emitterIter;
			delete emitter;
			emitter = NULL;
		}
		emitterList.clear();
	}
	this->pieceEmitterEffects.clear();

	this->cannonBlock->Flush();
	this->collateralBlock->Flush();
	this->teslaBlock->Flush();
	this->itemDropBlock->Flush();
    this->switchBlock->Flush();

	// Clear the current level pointer
	this->currLevel = NULL;
}

/**
 * Load a new level mesh into this object. This will clear out any old loaded level.
 */
void LevelMesh::LoadNewLevel(const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level) {
	// Make sure any previous levels are cleared...
	this->Flush();
	
	// Set the current level pointer
	this->currLevel = &level;

	// Based on the world style read-in the appropriate block
	this->styleBlock = gameWorldAssets.GetWorldStyleBlock();

	// Load the materials for the style block...
	std::map<std::string, MaterialGroup*> styleBlockMatGrps  = this->styleBlock->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::iterator iter = styleBlockMatGrps.begin(); iter != styleBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}

	// Load the actual level meshes as precomputed batches for speed...
	const std::vector<std::vector<LevelPiece*> >& levelPieces = level.GetCurrentLevelLayout();

	// Get the proper vector to center the level
	Vector2D levelDimensions = Vector2D(level.GetLevelUnitWidth(), level.GetLevelUnitHeight());
	
	// Create the ball safety net for the level
	this->ballSafetyNet->Regenerate(levelDimensions);

	Vector3D worldTransform(-levelDimensions[0]/2.0f, -levelDimensions[1]/2.0f, 0.0f);

	// Go through each piece and create an appropriate display list for it
	for (size_t h = 0; h < levelPieces.size(); h++) {
		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			
			// Obtain the current level piece and create a display list for it if necessary
			LevelPiece* currPiece	= levelPieces[h][w];

            // Unlock the blammopedia entry for the piece
            ResourceManager::GetInstance()->GetBlammopedia()->UnlockBlock(currPiece->GetType());

			// Create the appropriate display lists for the piece...
			this->CreateDisplayListsForPiece(currPiece, worldTransform);
			// Create the emitters/effects for the piece...
			this->CreateEmitterEffectsForPiece(currPiece, worldTransform);

			// Special cases:
			// 1) Case of a cannon block - we need to store all the cannon blocks we can
			// properly draw their barrels oriented unique for each one
			if (currPiece->GetType() == LevelPiece::Cannon) {
				const CannonBlock* cannonLvlPiece = static_cast<const CannonBlock*>(currPiece);
				assert(cannonLvlPiece != NULL);
				this->cannonBlock->AddCannonBlock(cannonLvlPiece);
			}
			// 2) Collateral block - we need to store all of them so that they can be drawn dynamically
			// when the go into collateral mode
			else if (currPiece->GetType() == LevelPiece::Collateral) {
				const CollateralBlock* collateralLvlPiece = static_cast<const CollateralBlock*>(currPiece);
				assert(collateralLvlPiece != NULL);
				this->collateralBlock->AddCollateralBlock(collateralLvlPiece);
			}
			// 3) Tesla block - similar to the cannon block, we store all of them to draw certain
			// parts of the block oriented differently / animating
			else if (currPiece->GetType() == LevelPiece::Tesla) {
				const TeslaBlock* telsaLvlPiece = static_cast<const TeslaBlock*>(currPiece);
				assert(telsaLvlPiece != NULL);
				this->teslaBlock->AddTeslaBlock(telsaLvlPiece);
			}
			// 4) Item drop block - like the above - since we need to set the texture for the item
			// each block will drop next, we need to store them in a seperate container object
			else if (currPiece->GetType() == LevelPiece::ItemDrop) {
				const ItemDropBlock* itemDrpPiece = static_cast<const ItemDropBlock*>(currPiece);
				assert(itemDrpPiece != NULL);

				// Figure out what texture is associated with the next item to be dropped from the block
				Texture2D* itemTexture = gameItemAssets.GetItemTexture(itemDrpPiece->GetNextDropItemType());
				this->itemDropBlock->AddItemDropBlock(itemDrpPiece, itemTexture);
			}
            // 5) Switch block
            else if (currPiece->GetType() == LevelPiece::Switch) {
                const SwitchBlock* switchPiece = static_cast<const SwitchBlock*>(currPiece);
                assert(switchPiece != NULL);
                this->switchBlock->AddSwitchBlock(switchPiece);
            }
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
	std::map<const LevelPiece*, std::map<CgFxMaterialEffect*, GLuint> >::iterator pieceInfoIter = this->pieceDisplayLists.find(&pieceBefore);

	if (pieceInfoIter != this->pieceDisplayLists.end()) {
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
	}
	else {
		// The piece is a special type that must be handled in a very specific way...
		switch (pieceBefore.GetType()) {
			case LevelPiece::Cannon:
				break;
			case LevelPiece::Collateral:
				break;
			default:
				assert(false);
				break;
		}
	}

	// Based on the new piece type we re-create a display list
	Vector3D translation(-this->currLevel->GetLevelUnitWidth()/2.0f, -this->currLevel->GetLevelUnitHeight()/2.0f, 0.0f);
	this->CreateDisplayListsForPiece(&pieceAfter, translation);
}

void LevelMesh::RemovePiece(const LevelPiece& piece) {
	switch (piece.GetType()) {
		case LevelPiece::Cannon:
			{
				const CannonBlock* cannonLvlPiece = static_cast<const CannonBlock*>(&piece);
				assert(cannonLvlPiece != NULL);
				this->cannonBlock->RemoveCannonBlock(cannonLvlPiece);
			}
			break;

		case LevelPiece::Collateral:
			{
				const CollateralBlock* collateralLvlPiece = static_cast<const CollateralBlock*>(&piece);
				assert(collateralLvlPiece != NULL);
				this->collateralBlock->RemoveCollateralBlock(collateralLvlPiece);
			}
			break;

		case LevelPiece::Tesla:
			{
				const TeslaBlock* teslaLvlPiece = static_cast<const TeslaBlock*>(&piece);
				assert(teslaLvlPiece != NULL);
				this->teslaBlock->RemoveTeslaBlock(teslaLvlPiece);
			}
			break;

		case LevelPiece::ItemDrop:
			{
				const ItemDropBlock* itemDrpPiece = static_cast<const ItemDropBlock*>(&piece);
				assert(itemDrpPiece != NULL);
				this->itemDropBlock->RemoveItemDropBlock(itemDrpPiece);
			}
            break;

        case LevelPiece::Switch:
            {
				const SwitchBlock* switchPiece = static_cast<const SwitchBlock*>(&piece);
				assert(switchPiece != NULL);
				this->switchBlock->RemoveSwitchBlock(switchPiece);
            }
            break;

		default:
			break;
	}
}

/**
 * Draw the current level mesh pieces (i.e., blocks that make up the level).
 */
void LevelMesh::DrawPieces(const Vector3D& worldTranslation, double dT, const Camera& camera,
                           bool lightsAreOut, const BasicPointLight& keyLight, 
                           const BasicPointLight& fillLight, const BasicPointLight& ballLight,
                           const Texture2D* sceneTexture) {

	// Set any appropriate parameters on the various meshes materials, etc.
	this->prismBlockDiamond->SetSceneTexture(sceneTexture);
	this->prismBlockTriangleUR->SetSceneTexture(sceneTexture);
	this->portalBlock->SetSceneTexture(sceneTexture);
	this->portalBlock->Tick(dT);

	// Go through each material and draw all the display lists corresponding to it
	CgFxMaterialEffect* currEffect = NULL;
	for (std::map<CgFxMaterialEffect*, std::vector<GLuint> >::const_iterator iter = this->displayListsPerMaterial.begin();
		iter != this->displayListsPerMaterial.end(); ++iter) {
		
		currEffect = iter->first;
		currEffect->SetKeyLight(keyLight);
		currEffect->SetFillLight(fillLight);
		currEffect->SetBallLight(ballLight);
		currEffect->Draw(camera, iter->second);
	}

	glPushMatrix();
	glTranslatef(worldTranslation[0], worldTranslation[1], worldTranslation[2]);
	this->cannonBlock->Draw(dT, camera, keyLight, fillLight, ballLight, lightsAreOut);
	this->collateralBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
	this->itemDropBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
	this->teslaBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
    this->switchBlock->Draw(dT, camera, keyLight, fillLight, ballLight, lightsAreOut);
	glPopMatrix();

	ESPEmitter* emitter = NULL;
	for (std::map<const LevelPiece*, std::list<ESPEmitter*> >::iterator pieceIter = this->pieceEmitterEffects.begin();
		pieceIter != this->pieceEmitterEffects.end(); ++pieceIter) {

		std::list<ESPEmitter*>& emitterList = pieceIter->second;
		for (std::list<ESPEmitter*>::iterator emitterIter = emitterList.begin(); emitterIter != emitterList.end(); ++emitterIter) {
			emitter = *emitterIter;
			emitter->Tick(dT);
			emitter->Draw(camera);
		}
	}

}

/**
 * Private helper function that will create the appropriate display lists for drawing the given piece
 * at the given translation in the world.
 */
void LevelMesh::CreateDisplayListsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation) {
	assert(piece != NULL);
	const std::map<std::string, MaterialGroup*>* pieceMatGrps = this->GetMaterialGrpsForPieceType(piece);
	if (pieceMatGrps == NULL) {
		return;
	}

	glPushAttrib(GL_CURRENT_BIT);

	// Go through each of the material groups ensuring that the material is associated with an appropriate display list
	for (std::map<std::string, MaterialGroup*>::const_iterator iter = pieceMatGrps->begin(); iter != pieceMatGrps->end(); ++iter) {
		
		// Make sure that the material exists in our set of available level materials
		std::map<std::string, CgFxMaterialEffect*>::iterator currMaterialIter = this->levelMaterials.find(iter->first);
		assert(currMaterialIter != this->levelMaterials.end());

		CgFxMaterialEffect* currMaterial	= currMaterialIter->second;
		PolygonGroup* currPolyGrp					= iter->second->GetPolygonGroup();
		const Colour& currColour					= piece->GetColour();

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
		glColor3fv(currColour.begin());
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

	glPopAttrib();
}

/**
 * Private helper function for creating emitter effects for certain level pieces and
 * adding them to the piece emitter map.
 */
void LevelMesh::CreateEmitterEffectsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation) {
	assert(piece != NULL);

	Matrix4x4 worldTransform = Matrix4x4::translationMatrix(worldTranslation);
	Matrix4x4 localTransform = piece->GetPieceToLevelTransform();
	Matrix4x4 fullTransform =  worldTransform * localTransform;

	std::list<ESPEmitter*> pieceEmitters;
	switch (piece->GetType()) {
		case LevelPiece::Portal:
			// Portal block has special emitters that suck/spit particles and stuff
			pieceEmitters = this->portalBlock->CreatePortalBlockEmitters(piece->GetColour(), fullTransform.getTranslation());
			break;
		default:
			return;
	}

	assert(pieceEmitters.size() > 0);
	this->pieceEmitterEffects.insert(std::make_pair(piece, pieceEmitters));
}

/**
 * Private helper function for obtaining the material groups associated with a particular
 * Level Piece type.
 * Returns: A map of the identifiers to corresponding material groups for a particular mesh representation
 * of the given level piece type.
 */
const std::map<std::string, MaterialGroup*>* LevelMesh::GetMaterialGrpsForPieceType(const LevelPiece* piece) const {
    assert(piece != NULL);

    switch (piece->GetType()) {
		case LevelPiece::Solid :
			return &this->styleBlock->GetMaterialGroups();
			break;
		case LevelPiece::SolidTriangle:
			return &this->triangleBlockUR->GetMaterialGroups();
			break;
		case LevelPiece::Breakable:
			return &this->basicBlock->GetMaterialGroups();
			break;
		case LevelPiece::BreakableTriangle:
			return &this->triangleBlockUR->GetMaterialGroups();
			break;
		case LevelPiece::Bomb:
			return &this->bombBlock->GetMaterialGroups();
			break;
		case LevelPiece::Ink:
			return &this->inkBlock->GetMaterialGroups();
			break;
		case LevelPiece::Prism:
			return &this->prismBlockDiamond->GetMaterialGroups();
			break;
		case LevelPiece::PrismTriangle:
			return &this->prismBlockTriangleUR->GetMaterialGroups();
			break;
		case LevelPiece::Portal:
			return &this->portalBlock->GetMaterialGroups();
			break;
		case LevelPiece::Cannon:
			return &this->cannonBlock->GetMaterialGroups();
			break;
		case LevelPiece::Tesla:
			return &this->teslaBlock->GetMaterialGroups();
			break;
		case LevelPiece::ItemDrop:
			return &this->itemDropBlock->GetMaterialGroups();
			break;
        case LevelPiece::Switch:
            return &this->switchBlock->GetMaterialGroups();
            break;
		case LevelPiece::Collateral:
			break;
        case LevelPiece::OneWay:
            {
                const OneWayBlock* oneWayBlock = static_cast<const OneWayBlock*>(piece);
                assert(oneWayBlock != NULL);
                switch (oneWayBlock->GetDirType()) {
                    case OneWayBlock::OneWayUp:
                        return &this->oneWayUpBlock->GetMaterialGroups();
                    case OneWayBlock::OneWayDown:
                        return &this->oneWayDownBlock->GetMaterialGroups();
                    case OneWayBlock::OneWayLeft:
                        return &this->oneWayLeftBlock->GetMaterialGroups();
                    case OneWayBlock::OneWayRight:
                        return &this->oneWayRightBlock->GetMaterialGroups();
                    default:
                        assert(false);
                        break;
                }
            }
            break;
		case LevelPiece::Empty:
			break;
		default:
			assert(false);
			break;
	}
	return NULL;
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
void LevelMesh::BallSafetyNetDestroyed(const Point2D& pos) {
	Vector2D levelDimensions = Vector2D(this->currLevel->GetLevelUnitWidth(), this->currLevel->GetLevelUnitHeight());
	this->ballSafetyNet->DestroyBallSafetyNet(levelDimensions, pos[0]);
}

void LevelMesh::SwitchActivated(const SwitchBlock* block, const GameLevel* currLevel) {
    this->switchBlock->SwitchBlockActivated(block, currLevel->GetTriggerableLevelPiece(block->GetIDTriggeredBySwitch()));
}

/**
 * Call when the camera mode changes from/to paddle camera.
 */
void LevelMesh::PaddleCameraActiveToggle(bool isActive) {
	// Call to make the safety net visible or not when its activated. This is
  // useful to avoid obstruction of the player's viewpoint e.g., when in paddle camera mode.
	float safetyNetTransparency = isActive ? 0.5f : 1.0f;
	this->ballSafetyNet->SetTransparency(safetyNetTransparency);
}

void LevelMesh::SetLevelAlpha(float alpha) {
	assert(alpha >= 0.0f && alpha <= 1.0f);

	// First go through each stored material effect and change its alpha multiplier
	std::map<CgFxMaterialEffect*, std::vector<GLuint> >::iterator iter = this->displayListsPerMaterial.begin();
	for (; iter != this->displayListsPerMaterial.end(); ++iter) {
		CgFxMaterialEffect* currMatEffect = iter->first;
		MaterialProperties* matProperties = currMatEffect->GetProperties();
		matProperties->alphaMultiplier = alpha;
	}

	// Make sure all other materials (inside certain special meshes) also get their alpha multiplier set
	this->portalBlock->SetAlphaMultiplier(alpha);
	this->cannonBlock->SetAlphaMultiplier(alpha);
	this->collateralBlock->SetAlphaMultiplier(alpha);
	this->teslaBlock->SetAlphaMultiplier(alpha);
	this->itemDropBlock->SetAlphaMultiplier(alpha);
    this->switchBlock->SetAlphaMultiplier(alpha);
}