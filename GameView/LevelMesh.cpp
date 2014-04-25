/**
 * LevelMesh.cpp
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

#include "LevelMesh.h"
#include "GameDisplay.h"
#include "GameViewConstants.h"
#include "GameWorldAssets.h"
#include "PrismBlockMesh.h"
#include "PortalBlockMesh.h"
#include "CannonBlockMesh.h"
#include "FragileCannonBlockMesh.h"
#include "CollateralBlockMesh.h"
#include "TeslaBlockMesh.h"
#include "SwitchBlockMesh.h"
#include "AlwaysDropBlockMesh.h"
#include "OneWayBlockMesh.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Point.h"

#include "../ResourceManager.h"

#include "../GameModel/GameBall.h"
#include "../GameModel/CannonBlock.h"
#include "../GameModel/FragileCannonBlock.h"
#include "../GameModel/CollateralBlock.h"
#include "../GameModel/TeslaBlock.h"
#include "../GameModel/SwitchBlock.h"
#include "../GameModel/OneWayBlock.h"
#include "../GameModel/LaserTurretBlock.h"
#include "../GameModel/RocketTurretBlock.h"
#include "../GameModel/AlwaysDropBlock.h"
#include "../GameModel/RegenBlock.h"
#include "../GameModel/OneWayBlock.h"

LevelMesh::LevelMesh(GameSound* sound, const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level) :
currLevel(NULL), styleBlock(NULL), basicBlock(NULL), bombBlock(NULL), triangleBlockUR(NULL), inkBlock(NULL), portalBlock(NULL),
prismBlockDiamond(NULL), prismBlockTriangleUR(NULL), cannonBlock(NULL), fragileCannonBlock(NULL), collateralBlock(NULL),
teslaBlock(NULL), switchBlock(NULL), noEntryBlock(NULL), oneWayBlock(NULL), 
laserTurretBlock(NULL), rocketTurretBlock(NULL), mineTurretBlock(NULL), alwaysDropBlock(NULL), regenBlock(NULL),
statusEffectRenderer(NULL), remainingPieceGlowTexture(NULL),
remainingPiecePulser(0,0), bossMesh(NULL), levelAlpha(1.0f) {
	
    this->remainingPieceGlowTexture = static_cast<Texture2D*>(ResourceManager::GetInstance()->GetImgTextureResource(
        GameViewConstants::GetInstance()->TEXTURE_CLEAN_CIRCLE_GRADIENT, Texture::Trilinear));
    assert(this->remainingPieceGlowTexture != NULL);
    ScaleEffect pulseSettings(0.75f, 1.8f);
    this->remainingPiecePulser = ESPParticleScaleEffector(pulseSettings);

	// Load the basic block and all other block types that stay consistent between worlds
	this->basicBlock					= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BASIC_BLOCK_MESH_PATH);
	this->bombBlock						= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->BOMB_BLOCK_MESH);
	this->triangleBlockUR				= ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TRIANGLE_BLOCK_MESH_PATH);
	this->prismBlockDiamond			    = new PrismBlockMesh(PrismBlockMesh::DiamondPrism);
	this->prismBlockTriangleUR	        = new PrismBlockMesh(PrismBlockMesh::TrianglePrism);
	this->inkBlock						= ResourceManager::GetInstance()->GetInkBlockMeshResource();
	this->portalBlock					= new PortalBlockMesh();
	this->cannonBlock					= new CannonBlockMesh();
    this->fragileCannonBlock            = new FragileCannonBlockMesh();
	this->collateralBlock				= new CollateralBlockMesh();
	this->teslaBlock                    = new TeslaBlockMesh();
	this->itemDropBlock					= new ItemDropBlockMesh();
    this->switchBlock                   = new SwitchBlockMesh();
    this->laserTurretBlock              = new LaserTurretBlockMesh();
    this->rocketTurretBlock             = new RocketTurretBlockMesh();
    this->mineTurretBlock               = new MineTurretBlockMesh();
    this->alwaysDropBlock               = new AlwaysDropBlockMesh();
    this->regenBlock                    = new RegenBlockMesh();
    this->oneWayBlock                   = new OneWayBlockMesh();
    this->noEntryBlock                  = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->NO_ENTRY_BLOCK_MESH);

	// Add the typical level meshes to the list of materials...
#define INSERT_MATERIAL_GRPS(block, allowDuplicateMaterials) { const std::map<std::string, MaterialGroup*>& matGrps = block->GetMaterialGroups(); \
    for (std::map<std::string, MaterialGroup*>::const_iterator iter = matGrps.begin(); iter != matGrps.end(); ++iter) { \
    if (this->levelMaterials.find(iter->first) != this->levelMaterials.end()) { if (allowDuplicateMaterials) { continue; } else { assert(false); } } \
    this->levelMaterials.insert(std::make_pair<std::string, CgFxAbstractMaterialEffect*>(iter->first, iter->second->GetMaterial())); } }

#pragma warning(push)
#pragma warning(disable: 4127)

    INSERT_MATERIAL_GRPS(this->basicBlock, false);
    INSERT_MATERIAL_GRPS(this->triangleBlockUR, false);
    INSERT_MATERIAL_GRPS(this->bombBlock, false);
    INSERT_MATERIAL_GRPS(this->inkBlock, false);
    INSERT_MATERIAL_GRPS(this->prismBlockDiamond, false);
    INSERT_MATERIAL_GRPS(this->prismBlockTriangleUR, false);
    INSERT_MATERIAL_GRPS(this->portalBlock, false);
    INSERT_MATERIAL_GRPS(this->teslaBlock, false);
    INSERT_MATERIAL_GRPS(this->itemDropBlock, false);
    INSERT_MATERIAL_GRPS(this->switchBlock, false);
    INSERT_MATERIAL_GRPS(this->laserTurretBlock, true);
    INSERT_MATERIAL_GRPS(this->rocketTurretBlock, true);
    INSERT_MATERIAL_GRPS(this->mineTurretBlock, true);
    INSERT_MATERIAL_GRPS(this->noEntryBlock, false);
    INSERT_MATERIAL_GRPS(this->regenBlock, false);

#pragma warning(pop)

#undef INSERT_MATERIAL_GRPS

	// Initialize the status renderer
	this->statusEffectRenderer = new BlockStatusEffectRenderer();

	this->LoadNewLevel(sound, gameWorldAssets, gameItemAssets, level);
}

LevelMesh::~LevelMesh() {
	// Clean up all assets pertaining to the currently loaded
	// level, if applicable.
	this->Flush();

	// Delete all meshes
	delete this->prismBlockDiamond;
	this->prismBlockDiamond = NULL;
	delete this->prismBlockTriangleUR;
	this->prismBlockTriangleUR = NULL;
	delete this->portalBlock;
	this->portalBlock = NULL;
	delete this->cannonBlock;
	this->cannonBlock = NULL;
    delete this->fragileCannonBlock;
    this->fragileCannonBlock = NULL;
	delete this->collateralBlock;
	this->collateralBlock = NULL;
	delete this->teslaBlock;
	this->teslaBlock = NULL;
	delete this->itemDropBlock;
	this->itemDropBlock = NULL;
    delete this->switchBlock;
    this->switchBlock = NULL;
    delete this->laserTurretBlock;
    this->laserTurretBlock = NULL;
    delete this->rocketTurretBlock;
    this->rocketTurretBlock = NULL;
    delete this->mineTurretBlock;
    this->mineTurretBlock = NULL;
    delete this->alwaysDropBlock;
    this->alwaysDropBlock = NULL;
    delete this->regenBlock;
    this->regenBlock = NULL;
    delete this->oneWayBlock;
    this->oneWayBlock = NULL;

    // Release all resources
    bool success = false;
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->noEntryBlock);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->basicBlock);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->bombBlock);
    assert(success);
    success = ResourceManager::GetInstance()->ReleaseMeshResource(this->triangleBlockUR);
    assert(success);

    success = ResourceManager::GetInstance()->ReleaseTextureResource(this->remainingPieceGlowTexture);
	assert(success);

    UNUSED_VARIABLE(success);

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
			std::map<std::string, CgFxAbstractMaterialEffect*>::iterator foundMaterial = this->levelMaterials.find(iter->first);
			assert(foundMaterial != this->levelMaterials.end());

			this->levelMaterials.erase(foundMaterial);
		}
	}
	
	// Delete each of the display lists loaded for the previous level and clear up the
	// relevant mappings to those display lists.
	for (std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> >::iterator iter = this->firstPassDisplayListsPerMaterial.begin();
		iter != this->firstPassDisplayListsPerMaterial.end(); ++iter) {
		
		for (std::vector<GLuint>::iterator dispListIter = iter->second.begin(); dispListIter != iter->second.end(); ++dispListIter) {
			glDeleteLists((*dispListIter), 1);
			(*dispListIter) = 0;
		}
	}
	this->firstPassDisplayListsPerMaterial.clear();
    this->firstPassPieceDisplayLists.clear();

    for (std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> >::iterator iter = this->secondPassDisplayListsPerMaterial.begin();
        iter != this->secondPassDisplayListsPerMaterial.end(); ++iter) {

            for (std::vector<GLuint>::iterator dispListIter = iter->second.begin(); dispListIter != iter->second.end(); ++dispListIter) {
                glDeleteLists((*dispListIter), 1);
                (*dispListIter) = 0;
            }
    }
    this->secondPassDisplayListsPerMaterial.clear();
    this->secondPassPieceDisplayLists.clear();

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

    // Delete any emitters for the final pieces in the level
    for (std::map<const LevelPiece*, ESPEmitter*>::iterator iter = this->lastPieceEffects.begin();
		iter != this->lastPieceEffects.end(); ++iter) {

        ESPEmitter* emitter = iter->second;
        delete emitter;
        emitter = NULL;
    }
    this->lastPieceEffects.clear();

	this->cannonBlock->Flush();
    this->fragileCannonBlock->Flush();
	this->collateralBlock->Flush();
	this->teslaBlock->Flush();
	this->itemDropBlock->Flush();
    this->switchBlock->Flush();
    this->laserTurretBlock->Flush();
    this->rocketTurretBlock->Flush();
    this->mineTurretBlock->Flush();
    this->alwaysDropBlock->Flush();
    this->regenBlock->Flush();
    this->oneWayBlock->Flush();

    // Clear any boss mesh
    if (this->bossMesh != NULL) {
        delete this->bossMesh;
        this->bossMesh = NULL;
    }

	// Clear the current level pointer
	this->currLevel = NULL;
}

/**
 * Load a new level mesh into this object. This will clear out any old loaded level.
 */
void LevelMesh::LoadNewLevel(GameSound* sound, const GameWorldAssets& gameWorldAssets, 
                             const GameItemAssets& gameItemAssets, const GameLevel& level) {

	// Make sure any previous levels are cleared...
	this->Flush();
	
	// Set the current level pointer
	this->currLevel = &level;

	// Based on the world style read-in the appropriate block
	this->styleBlock = gameWorldAssets.GetWorldStyleBlock();

	// Load the materials for the style block...
	std::map<std::string, MaterialGroup*> styleBlockMatGrps  = this->styleBlock->GetMaterialGroups();
	for (std::map<std::string, MaterialGroup*>::iterator iter = styleBlockMatGrps.begin(); iter != styleBlockMatGrps.end(); ++iter) {
		this->levelMaterials.insert(std::make_pair<std::string, CgFxAbstractMaterialEffect*>(iter->first, iter->second->GetMaterial()));
	}

	// Load the actual level meshes as precomputed batches for speed...
	const std::vector<std::vector<LevelPiece*> >& levelPieces = level.GetCurrentLevelLayout();

	// Get the proper vector to center the level
	Vector2D levelDimensions = Vector2D(level.GetLevelUnitWidth(), level.GetLevelUnitHeight());
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
            switch (currPiece->GetType()) {

			    // Case of a cannon block - we need to store all the cannon blocks we can
			    // properly draw their barrels oriented unique for each one
                case LevelPiece::Cannon: {
				    const CannonBlock* cannonLvlPiece = static_cast<const CannonBlock*>(currPiece);
				    this->cannonBlock->AddCannonBlock(cannonLvlPiece);
                    break;
			    }

                case LevelPiece::FragileCannon: {
                    const FragileCannonBlock* fragileCannonLvlPiece = static_cast<const FragileCannonBlock*>(currPiece);
                    this->fragileCannonBlock->AddFragileCannonBlock(fragileCannonLvlPiece);
                    break;
                }

			    // Collateral block - we need to store all of them so that they can be drawn dynamically
			    // when the go into collateral mode
                case LevelPiece::Collateral: {
				    const CollateralBlock* collateralLvlPiece = static_cast<const CollateralBlock*>(currPiece);
				    this->collateralBlock->AddCollateralBlock(collateralLvlPiece);
                    break;
			    }

			    // Tesla block - similar to the cannon block, we store all of them to draw certain
			    // parts of the block oriented differently / animating
                case LevelPiece::Tesla: {
				    TeslaBlock* telsaLvlPiece = static_cast<TeslaBlock*>(currPiece);
				    this->teslaBlock->AddTeslaBlock(telsaLvlPiece);
                    break;
			    }

			    // Item drop block - like the above - since we need to set the texture for the item
			    // each block will drop next, we need to store them in a separate container object
                case LevelPiece::ItemDrop: {
				    const ItemDropBlock* itemDrpPiece = static_cast<const ItemDropBlock*>(currPiece);

				    // Figure out what texture is associated with the next item to be dropped from the block
				    Texture2D* itemTexture = gameItemAssets.GetItemTexture(itemDrpPiece->GetNextItemDropType());
				    this->itemDropBlock->AddItemDropBlock(itemDrpPiece, itemTexture);
                    break;
			    }

                // Switch block
                case LevelPiece::Switch: {
                    const SwitchBlock* switchPiece = static_cast<const SwitchBlock*>(currPiece);
                    this->switchBlock->AddSwitchBlock(switchPiece);
                    break;
                }
                
                // Laser Turret Block
                case LevelPiece::LaserTurret: {
				    const LaserTurretBlock* laserTurretLvlPiece = static_cast<const LaserTurretBlock*>(currPiece);
				    this->laserTurretBlock->AddLaserTurretBlock(laserTurretLvlPiece);
                    break;
                }

                // Rocket Turret Block
                case LevelPiece::RocketTurret: {
                    const RocketTurretBlock* rocketTurretLvlPiece = static_cast<const RocketTurretBlock*>(currPiece);
                    this->rocketTurretBlock->AddRocketTurretBlock(rocketTurretLvlPiece);
                    break;
                }

                // Mine Turret Block
                case LevelPiece::MineTurret: {
                    const MineTurretBlock* mineTurretLvlPiece = static_cast<const MineTurretBlock*>(currPiece);
                    this->mineTurretBlock->AddMineTurretBlock(mineTurretLvlPiece);
                    break;
                }

                // Always Drop Block
                case LevelPiece::AlwaysDrop: {
                    const AlwaysDropBlock* alwaysDropLvlPiece = static_cast<const AlwaysDropBlock*>(currPiece);

                    Texture2D* itemTexture = gameItemAssets.GetItemTexture(alwaysDropLvlPiece->GetNextDropItemType());
                    this->alwaysDropBlock->AddAlwaysDropBlock(alwaysDropLvlPiece, itemTexture);
                    break;
                }

                // Regen Block
                case LevelPiece::Regen: {
                    RegenBlock* regenLvlPiece = static_cast<RegenBlock*>(currPiece);
                    this->regenBlock->AddRegenBlock(regenLvlPiece);
                    break;
                }

                case LevelPiece::OneWay: {
                    OneWayBlock* oneWayLvlPiece = static_cast<OneWayBlock*>(currPiece);
                    this->oneWayBlock->AddOneWayBlock(oneWayLvlPiece);
                    break;
                }

                default:
                    break;
            }
		}
	}

    // Load the boss, if there is one...
    assert(this->bossMesh == NULL);
    if (currLevel->GetHasBoss()) {
        this->bossMesh = BossMesh::Build(gameWorldAssets.GetStyle(), currLevel->GetBoss(), sound);
        assert(this->bossMesh != NULL);
    }

}

void LevelMesh::LevelPieceStatusAdded(const LevelPiece& piece, const LevelPiece::PieceStatus& status) {
    this->statusEffectRenderer->AddLevelPieceStatus(piece, status);
    if (piece.GetType() == LevelPiece::OneWay) {
        this->oneWayBlock->IceCubeStatusAdded(static_cast<const OneWayBlock*>(&piece));
    }
}

void LevelMesh::LevelPieceStatusRemoved(const LevelPiece& piece, const LevelPiece::PieceStatus& status) {
    this->statusEffectRenderer->RemoveLevelPieceStatus(piece, status);
    if (piece.GetType() == LevelPiece::OneWay) {
        this->oneWayBlock->IceCubeStatusRemoved(static_cast<const OneWayBlock*>(&piece));
    }
}

/**
 * Adjust the level mesh to change the given piece.
 * Please note that this can be an EXPENSIVE operation!!
 * (Do not call > 1 per frame!!)
 */
void LevelMesh::ChangePiece(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter) {

	// Find the changed piece and change its display list...
	std::map<const LevelPiece*, std::map<CgFxAbstractMaterialEffect*, GLuint> >::iterator pieceInfoIter =
        this->firstPassPieceDisplayLists.find(&pieceBefore);
    
    bool foundPieceInfo = false;
    std::map<const LevelPiece*, std::map<CgFxAbstractMaterialEffect*, GLuint> >* displayListMap = NULL;
    std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> >* displayListPerMaterialMap = NULL;

    if (pieceInfoIter == this->firstPassPieceDisplayLists.end()) {
        pieceInfoIter = this->secondPassPieceDisplayLists.find(&pieceBefore);
        if (pieceInfoIter != this->secondPassPieceDisplayLists.end()) {
            foundPieceInfo = true;
            displayListMap = &this->secondPassPieceDisplayLists;
            displayListPerMaterialMap = &this->secondPassDisplayListsPerMaterial;
        }
    }
    else {
        foundPieceInfo = true;
        displayListMap = &this->firstPassPieceDisplayLists;
        displayListPerMaterialMap = &this->firstPassDisplayListsPerMaterial;
    }

	if (foundPieceInfo) {
		
        // Go through each of the materials and clear up previous display lists and materials...
		for (std::map<CgFxAbstractMaterialEffect*, GLuint>::iterator iter = pieceInfoIter->second.begin();
			iter != pieceInfoIter->second.end(); ++iter) {
			
			// Delete any previous display list...
			glDeleteLists(iter->second, 1);

			// ... and remove it from other relevant maps/arrays/etc.
			std::vector<GLuint>& displayLists = (*displayListPerMaterialMap)[iter->first];
			for (std::vector<GLuint>::iterator iterDL = displayLists.begin(); iterDL != displayLists.end(); ++iterDL) {
				if (*iterDL == iter->second)	{
					displayLists.erase(iterDL);
					break;
				}
			}
			if (displayLists.size() == 0) {
				displayListPerMaterialMap->erase(iter->first);
			}

			// Clean up
			iter->second = 0;
		}
		pieceInfoIter->second.clear();
		displayListMap->erase(pieceInfoIter);
	}
	else {
        
		// The piece is a special type that must be handled in a very specific way...
		switch (pieceBefore.GetType()) {
			case LevelPiece::Cannon:
				break;
            case LevelPiece::FragileCannon:
                break;
			case LevelPiece::Collateral:
				break;
            case LevelPiece::AlwaysDrop:
                break;
            case LevelPiece::OneWay:
                break;
			default:
				assert(false);
				break;
		}
	}

	// Based on the new piece type we re-create a display list
	Vector3D translation(-this->currLevel->GetLevelUnitWidth()/2.0f, -this->currLevel->GetLevelUnitHeight()/2.0f, 0.0f);
	this->CreateDisplayListsForPiece(&pieceAfter, translation);

    // Remove any last-remaining-piece special effects as well (if the piece became empty)
    if (pieceAfter.GetType() == LevelPiece::Empty) {
        std::map<const LevelPiece*, ESPEmitter*>::iterator lastPieceIter = this->lastPieceEffects.find(&pieceBefore);
        if (lastPieceIter != this->lastPieceEffects.end()) {
            ESPEmitter* emitter = lastPieceIter->second;
            delete emitter;
            emitter = NULL;
            this->lastPieceEffects.erase(lastPieceIter);
        }
    }
}

void LevelMesh::RemovePiece(const LevelPiece& piece) {
	switch (piece.GetType()) {

		case LevelPiece::Cannon: {
			const CannonBlock* cannonLvlPiece = static_cast<const CannonBlock*>(&piece);
			this->cannonBlock->RemoveCannonBlock(cannonLvlPiece);
			break;
        }

        case LevelPiece::FragileCannon: {
            const FragileCannonBlock* fragileCannonLvlPiece = static_cast<const FragileCannonBlock*>(&piece);
            this->fragileCannonBlock->RemoveFragileCannonBlock(fragileCannonLvlPiece);
            break;
        }

		case LevelPiece::Collateral: {
			const CollateralBlock* collateralLvlPiece = static_cast<const CollateralBlock*>(&piece);
			this->collateralBlock->RemoveCollateralBlock(collateralLvlPiece);
			break;
        }

		case LevelPiece::Tesla: {
			TeslaBlock* teslaLvlPiece = static_cast<TeslaBlock*>(const_cast<LevelPiece*>(&piece));
			this->teslaBlock->RemoveTeslaBlock(teslaLvlPiece);
			break;
        }

		case LevelPiece::ItemDrop: {
			const ItemDropBlock* itemDrpPiece = static_cast<const ItemDropBlock*>(&piece);
			this->itemDropBlock->RemoveItemDropBlock(itemDrpPiece);
            break;
        }

        case LevelPiece::Switch: {
			const SwitchBlock* switchPiece = static_cast<const SwitchBlock*>(&piece);
			this->switchBlock->RemoveSwitchBlock(switchPiece);
            break;
        }

        case LevelPiece::LaserTurret: {
            const LaserTurretBlock* laserTurretPiece = static_cast<const LaserTurretBlock*>(&piece);
            this->laserTurretBlock->RemoveLaserTurretBlock(laserTurretPiece);
            break;
        }

        case LevelPiece::RocketTurret: {
            const RocketTurretBlock* rocketTurretPiece = static_cast<const RocketTurretBlock*>(&piece);
            this->rocketTurretBlock->RemoveRocketTurretBlock(rocketTurretPiece);
            break;
        }

        case LevelPiece::MineTurret: {
            const MineTurretBlock* mineTurretPiece = static_cast<const MineTurretBlock*>(&piece);
            this->mineTurretBlock->RemoveMineTurretBlock(mineTurretPiece);
            break;
        }

        case LevelPiece::AlwaysDrop: {
            const AlwaysDropBlock* alwaysDropPiece = static_cast<const AlwaysDropBlock*>(&piece);
            this->alwaysDropBlock->RemoveAlwaysDropBlock(alwaysDropPiece);
            break;
        }

        case LevelPiece::Regen: {
            const RegenBlock* regenPiece = static_cast<const RegenBlock*>(&piece);
            this->regenBlock->RemoveRegenBlock(regenPiece);
            break;
        }
        
        case LevelPiece::OneWay: {
            const OneWayBlock* oneWayPiece = static_cast<const OneWayBlock*>(&piece);
            this->oneWayBlock->RemoveOneWayBlock(oneWayPiece);
            break;
        }

		default:
			break;
	}
}

/**
 * Draw the current level mesh pieces (i.e., blocks that make up the level).
 */
void LevelMesh::DrawFirstPassPieces(const Vector3D& worldTranslation, double dT, const Camera& camera,
                                    const GameModel* gameModel, const BasicPointLight& keyLight, 
                                    const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    // Draw any 'remaining pieces' effects
    ESPEmitter* emitter = NULL;
    for (std::map<const LevelPiece*, ESPEmitter*>::iterator iter = this->lastPieceEffects.begin();
         iter != this->lastPieceEffects.end(); ++iter) {

        emitter = iter->second;
		emitter->Tick(dT);
		emitter->Draw(camera);
    }

    // Special case: item drop block has some sparkles that drop from it, draw these before any of the blocks
    // so that they get hidden by any blocks in front of them
    this->itemDropBlock->DrawEffects(worldTranslation, dT, camera);

	// Go through each material and draw all the display lists corresponding to it
	CgFxAbstractMaterialEffect* currEffect = NULL;
	for (std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> >::const_iterator iter = this->firstPassDisplayListsPerMaterial.begin();
		iter != this->firstPassDisplayListsPerMaterial.end(); ++iter) {
		
		currEffect = iter->first;
		currEffect->SetKeyLight(keyLight);
		currEffect->SetFillLight(fillLight);
		currEffect->SetBallLight(ballLight);
		currEffect->Draw(camera, iter->second);
	}

	glPushMatrix();
	glTranslatef(worldTranslation[0], worldTranslation[1], worldTranslation[2]);
	this->cannonBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
    this->fragileCannonBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
	this->collateralBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
    this->switchBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
    this->laserTurretBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
    this->rocketTurretBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
    this->mineTurretBlock->Draw(dT, camera, keyLight, fillLight, ballLight);
    this->regenBlock->Draw(dT, camera, gameModel, keyLight, fillLight, ballLight);
    this->itemDropBlock->DrawBloomPass(dT, camera, keyLight, fillLight, ballLight);
    this->alwaysDropBlock->DrawBloomPass(dT, camera, keyLight, fillLight, ballLight);
    this->oneWayBlock->DrawRegularPass(dT, camera, keyLight, fillLight, ballLight);
	glPopMatrix();
}

void LevelMesh::DrawSecondPassPieces(double dT, const Camera& camera, const GameModel* gameModel, 
                                     const BasicPointLight& keyLight, const BasicPointLight& fillLight, 
                                     const BasicPointLight& ballLight, const Texture2D* sceneTexture) {

    UNUSED_PARAMETER(gameModel);

    this->prismBlockDiamond->SetSceneTexture(sceneTexture);
    this->prismBlockTriangleUR->SetSceneTexture(sceneTexture);
    this->portalBlock->SetSceneTexture(sceneTexture);
    this->portalBlock->Tick(dT);

    // Draw all of the second pass display lists
    CgFxAbstractMaterialEffect* currEffect = NULL;
    for (std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> >::const_iterator iter = this->secondPassDisplayListsPerMaterial.begin();
         iter != this->secondPassDisplayListsPerMaterial.end(); ++iter) {

        currEffect = iter->first;
        currEffect->SetKeyLight(keyLight);
        currEffect->SetFillLight(fillLight);
        currEffect->SetBallLight(ballLight);
        currEffect->Draw(camera, iter->second);
    }

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

void LevelMesh::DrawNoBloomPieces(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                                  const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

    this->itemDropBlock->DrawNoBloomPass(dT, camera, keyLight, fillLight, ballLight);
    this->alwaysDropBlock->DrawNoBloomPass(dT, camera, keyLight, fillLight, ballLight);
}

void LevelMesh::DrawTransparentNoBloomPieces(double dT, const Camera& camera, const BasicPointLight& keyLight, 
                                             const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
    this->oneWayBlock->DrawTransparentNoBloomPass(dT, camera, keyLight, fillLight, ballLight);
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
		std::map<std::string, CgFxAbstractMaterialEffect*>::iterator currMaterialIter = this->levelMaterials.find(iter->first);
		assert(currMaterialIter != this->levelMaterials.end());

		CgFxAbstractMaterialEffect* currMaterial = currMaterialIter->second;
		PolygonGroup* currPolyGrp        = iter->second->GetPolygonGroup();
		const ColourRGBA& currColour     = piece->GetColour();

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
		glColor4f(currColour.R(), currColour.G(), currColour.B(), currColour.A());
		currPolyGrp->Draw();
		glEndList();
		
		Matrix4x4 localInvTransform = piece->GetPieceToLevelInvTransform();
		Matrix4x4 worldInvTransform = Matrix4x4::translationMatrix(-worldTranslation);
		Matrix4x4 fullInvTransform = localInvTransform * worldInvTransform;
		
		currPolyGrp->Transform(fullInvTransform);
	
		// Insert the new display list into the list of display lists...
        
        // Based on the type of block we decide whether this is a first or second pass piece
        std::map<const LevelPiece*, std::map<CgFxAbstractMaterialEffect*, GLuint> >* pieceDisplayListMap = &this->firstPassPieceDisplayLists;
        std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> >* displayListsPerMaterialMap = &this->firstPassDisplayListsPerMaterial;
        switch (piece->GetType()) {
            case LevelPiece::Portal:
            case LevelPiece::Prism:
            case LevelPiece::PrismTriangle:
                pieceDisplayListMap = &this->secondPassPieceDisplayLists;
                displayListsPerMaterialMap = &this->secondPassDisplayListsPerMaterial;
                break;
            default:
                break;
        }

		std::map<CgFxAbstractMaterialEffect*, GLuint>& currPieceMatMap = (*pieceDisplayListMap)[piece];
        currPieceMatMap.insert(std::make_pair<CgFxAbstractMaterialEffect*, GLuint>(currMaterial, newDisplayList));
		
        std::vector<GLuint>& currDisplayListVec = (*displayListsPerMaterialMap)[currMaterial];
        currDisplayListVec.push_back(newDisplayList);
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
			this->portalBlock->CreatePortalBlockEmitters(piece->GetColour(), fullTransform.getTranslationPt3D(), pieceEmitters);
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

		case LevelPiece::SolidTriangle:
			return &this->triangleBlockUR->GetMaterialGroups();

		case LevelPiece::Breakable:
			return &this->basicBlock->GetMaterialGroups();

		case LevelPiece::BreakableTriangle:
			return &this->triangleBlockUR->GetMaterialGroups();

		case LevelPiece::Bomb:
			return &this->bombBlock->GetMaterialGroups();

		case LevelPiece::Ink:
			return &this->inkBlock->GetMaterialGroups();

		case LevelPiece::Prism:
			return &this->prismBlockDiamond->GetMaterialGroups();

		case LevelPiece::PrismTriangle:
			return &this->prismBlockTriangleUR->GetMaterialGroups();

		case LevelPiece::Portal:
			return &this->portalBlock->GetMaterialGroups();

		case LevelPiece::Cannon:
			break;
        case LevelPiece::FragileCannon:
            break;

		case LevelPiece::Tesla:
			return &this->teslaBlock->GetMaterialGroups();

		case LevelPiece::ItemDrop:
			return &this->itemDropBlock->GetMaterialGroups();

        case LevelPiece::Switch:
            return &this->switchBlock->GetMaterialGroups();

        case LevelPiece::NoEntry:
            return &this->noEntryBlock->GetMaterialGroups();

        case LevelPiece::LaserTurret:
            return &this->laserTurretBlock->GetMaterialGroups();

        case LevelPiece::RocketTurret:
            return &this->rocketTurretBlock->GetMaterialGroups();

        case LevelPiece::MineTurret:
            return &this->mineTurretBlock->GetMaterialGroups();

		case LevelPiece::Collateral:
			break;

        case LevelPiece::OneWay:
            break;

		case LevelPiece::Empty:
			break;

        case LevelPiece::AlwaysDrop:
            break;

        case LevelPiece::Regen:
            return &this->regenBlock->GetMaterialGroups();

		default:
			assert(false);
			break;
	}

	return NULL;
}

void LevelMesh::LevelIsAlmostComplete() {
    assert(this->lastPieceEffects.empty());

    // Add effects to the last pieces that need to be broken to end the level:
    // This highlights them for the player
    
    // Get a list of the remaining pieces...
    std::list<const LevelPiece*> remainingVitalPieces;
    const std::vector<std::vector<LevelPiece*> >& levelPieces = this->currLevel->GetCurrentLevelLayout();
    for (std::vector<std::vector<LevelPiece*> >::const_iterator iter1 = levelPieces.begin(); iter1 != levelPieces.end(); ++iter1) {
        const std::vector<LevelPiece*>& row = *iter1;
        for (std::vector<LevelPiece*>::const_iterator iter2 = row.begin(); iter2 != row.end(); ++iter2) {
            const LevelPiece* currPiece = *iter2;

            if (currPiece->MustBeDestoryedToEndLevel()) {
                remainingVitalPieces.push_back(currPiece);
            }
        }

        if (remainingVitalPieces.size() == this->currLevel->GetNumPiecesLeft()) {
            break;
        }
    }

    Vector2D levelTransform = Vector2D(-this->currLevel->GetLevelUnitWidth() / 2.0f, -this->currLevel->GetLevelUnitHeight() / 2.0f);

    // Place the highlight effect on each of the remaining pieces
    for (std::list<const LevelPiece*>::iterator iter = remainingVitalPieces.begin();
        iter != remainingVitalPieces.end(); ++iter) {

        const LevelPiece* piece = *iter;

        ESPPointEmitter* glowPulseEffect = new ESPPointEmitter();
	    glowPulseEffect->SetSpawnDelta(ESPInterval(ESPEmitter::ONLY_SPAWN_ONCE));
	    glowPulseEffect->SetInitialSpd(ESPInterval(0));
	    glowPulseEffect->SetParticleLife(ESPInterval(ESPParticle::INFINITE_PARTICLE_LIFETIME));
        glowPulseEffect->SetParticleSize(ESPInterval(1.2f*LevelPiece::PIECE_WIDTH), ESPInterval(1.2f*LevelPiece::PIECE_HEIGHT));
	    glowPulseEffect->SetEmitAngleInDegrees(0);
	    glowPulseEffect->SetRadiusDeviationFromCenter(ESPInterval(0.0f));
	    glowPulseEffect->SetParticleAlignment(ESP::ScreenAlignedGlobalUpVec);
        glowPulseEffect->SetEmitPosition(Point3D(piece->GetCenter() + levelTransform));
	    glowPulseEffect->SetParticleColour(ESPInterval(1), ESPInterval(1), ESPInterval(1), ESPInterval(0.75f));
	    glowPulseEffect->AddEffector(&this->remainingPiecePulser);
	    glowPulseEffect->SetParticles(1, this->remainingPieceGlowTexture);

        this->lastPieceEffects.insert(std::make_pair(piece, glowPulseEffect));
    }

}

void LevelMesh::SwitchActivated(const SwitchBlock* block, const GameLevel* currLevel) {
    const std::vector<LevelPiece*>* activatedPieces = currLevel->GetTriggerableLevelPieces(block->GetIDTriggeredBySwitch());
    if (activatedPieces == NULL) {
        return;
    }

    for (std::vector<LevelPiece*>::const_iterator iter = activatedPieces->begin();
         iter != activatedPieces->end(); ++iter) {

        this->switchBlock->SwitchBlockActivated(block, *iter);
    }
}

void LevelMesh::LaserTurretAIStateChanged(const LaserTurretBlock* block,
                                          const LaserTurretBlock::TurretAIState& oldState,
                                          const LaserTurretBlock::TurretAIState& newState) {
    this->laserTurretBlock->AIStateChanged(block, oldState, newState);
}

void LevelMesh::LaserFired(const LaserTurretBlock* block) {
    this->laserTurretBlock->LaserShotByBlock(block);
}

void LevelMesh::RocketTurretAIStateChanged(const RocketTurretBlock* block,
                                           const RocketTurretBlock::TurretAIState& oldState,
                                           const RocketTurretBlock::TurretAIState& newState) {
    this->rocketTurretBlock->AIStateChanged(block, oldState, newState);
}

void LevelMesh::MineTurretAIStateChanged(const MineTurretBlock* block,
                                         const MineTurretBlock::TurretAIState& oldState,
                                         const MineTurretBlock::TurretAIState& newState) {
    this->mineTurretBlock->AIStateChanged(block, oldState, newState);
}

void LevelMesh::RocketFired(const RocketTurretBlock* block) {
    this->rocketTurretBlock->RocketShotByBlock(block);
}

void LevelMesh::MineFired(const MineTurretBlock* block) {
    this->mineTurretBlock->MineShotByBlock(block);
}

void LevelMesh::SetLevelAlpha(float alpha) {
	assert(alpha >= 0.0f && alpha <= 1.0f);
    this->levelAlpha = alpha;

	// First go through each stored material effect and change its alpha multiplier
	std::map<CgFxAbstractMaterialEffect*, std::vector<GLuint> >::iterator iter = this->firstPassDisplayListsPerMaterial.begin();
	for (; iter != this->firstPassDisplayListsPerMaterial.end(); ++iter) {
		CgFxAbstractMaterialEffect* currMatEffect = iter->first;
        currMatEffect->SetAlphaMultiplier(alpha);
	}
    iter = this->secondPassDisplayListsPerMaterial.begin();
    for (; iter != this->secondPassDisplayListsPerMaterial.end(); ++iter) {
        CgFxAbstractMaterialEffect* currMatEffect = iter->first;
        currMatEffect->SetAlphaMultiplier(alpha);
    }

    for (std::map<const LevelPiece*, ESPEmitter*>::iterator iter = this->lastPieceEffects.begin();
         iter != this->lastPieceEffects.end(); ++iter) {

        ESPEmitter* emitter = iter->second;
		emitter->SetParticleAlpha(ESPInterval(alpha));
    }

	// Make sure all other materials (inside certain special meshes) also get their alpha multiplier set
	this->portalBlock->SetAlphaMultiplier(alpha);
	this->cannonBlock->SetAlphaMultiplier(alpha);
    this->fragileCannonBlock->SetAlphaMultiplier(alpha);
	this->collateralBlock->SetAlphaMultiplier(alpha);
	this->teslaBlock->SetAlphaMultiplier(alpha);
	this->itemDropBlock->SetAlphaMultiplier(alpha);
    this->switchBlock->SetAlphaMultiplier(alpha);
    this->laserTurretBlock->SetAlphaMultiplier(alpha);
    this->rocketTurretBlock->SetAlphaMultiplier(alpha);
    this->mineTurretBlock->SetAlphaMultiplier(alpha);
    this->alwaysDropBlock->SetAlphaMultiplier(alpha);
    this->regenBlock->SetAlphaMultiplier(alpha);
    this->oneWayBlock->SetAlphaMultiplier(alpha);
}

void LevelMesh::UpdateNoEntryBlock(bool remoteControlRocketOn) {
    float alpha = 0.75f;
    if (remoteControlRocketOn) {
        alpha = 0.5f;
    }
    this->noEntryBlock->SetAlpha(alpha);
}

double LevelMesh::ActivateBossIntro() {
    assert(this->bossMesh != NULL);
    return this->bossMesh->ActivateIntroAnimation();
}

double LevelMesh::ActivateBossExplodingFlashEffects(double delayInSecs, const GameModel* model, const Camera& camera) {
    assert(this->bossMesh != NULL);
    return this->bossMesh->ActivateBossExplodingFlashEffects(delayInSecs, model, camera);
}

void LevelMesh::ClearActiveBossEffects() {
    assert(this->bossMesh != NULL);
    this->bossMesh->ClearActiveEffects();
}