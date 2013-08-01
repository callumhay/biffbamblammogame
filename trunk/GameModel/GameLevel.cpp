/**
 * GameLevel.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "GameLevel.h"
#include "GameBall.h"
#include "PlayerPaddle.h"
#include "GameEventManager.h"
#include "GameItemFactory.h"
#include "LevelPiece.h"
#include "EmptySpaceBlock.h"
#include "SolidBlock.h"
#include "BreakableBlock.h"
#include "BombBlock.h"
#include "TriangleBlocks.h"
#include "InkBlock.h"
#include "PrismBlock.h"
#include "PortalBlock.h"
#include "CannonBlock.h"
#include "CollateralBlock.h"
#include "TeslaBlock.h"
#include "ItemDropBlock.h"
#include "SwitchBlock.h"
#include "OneWayBlock.h"
#include "NoEntryBlock.h"
#include "LaserTurretBlock.h"
#include "RocketTurretBlock.h"
#include "MineTurretBlock.h"
#include "AlwaysDropBlock.h"
#include "RegenBlock.h"
#include "GameModel.h"
#include "Projectile.h"
#include "PointAward.h"
#include "PaddleRocketProjectile.h"
#include "PaddleMineProjectile.h"
#include "Boss.h"

#include "../BlammoEngine/StringHelper.h"

#include "../ResourceManager.h"

const int GameLevel::MAX_STARS_PER_LEVEL = 5;

const char GameLevel::EMPTY_SPACE_CHAR          = 'E';
const char GameLevel::SOLID_BLOCK_CHAR          = 'S';
const char GameLevel::GREEN_BREAKABLE_CHAR		= 'G';
const char GameLevel::YELLOW_BREAKABLE_CHAR		= 'Y';
const char GameLevel::ORANGE_BREAKABLE_CHAR		= 'O';
const char GameLevel::RED_BREAKABLE_CHAR        = 'R';
const char GameLevel::BOMB_CHAR	                = 'B';
const char GameLevel::INKBLOCK_CHAR             = 'I';
const char GameLevel::PRISM_BLOCK_CHAR          = 'P';
const char GameLevel::PORTAL_BLOCK_CHAR         = 'X';
const char GameLevel::CANNON_BLOCK_CHAR         = 'C';
const char GameLevel::COLLATERAL_BLOCK_CHAR	    = 'L';
const char GameLevel::TESLA_BLOCK_CHAR          = 'A';
const char GameLevel::ITEM_DROP_BLOCK_CHAR      = 'D';
const char GameLevel::SWITCH_BLOCK_CHAR         = 'W';
const char GameLevel::ONE_WAY_BLOCK_CHAR        = 'F';
const char GameLevel::NO_ENTRY_BLOCK_CHAR       = 'N';
const char GameLevel::LASER_TURRET_BLOCK_CHAR   = 'H';
const char GameLevel::ROCKET_TURRET_BLOCK_CHAR  = 'J';
const char GameLevel::MINE_TURRET_BLOCK_CHAR    = 'M';
const char GameLevel::ALWAYS_DROP_BLOCK_CHAR    = 'K';
const char GameLevel::REGEN_BLOCK_CHAR          = 'Q';

const char GameLevel::TRIANGLE_BLOCK_CHAR	= 'T';
const char GameLevel::TRI_UPPER_CORNER		= 'u';
const char GameLevel::TRI_LOWER_CORNER		= 'l';
const char GameLevel::TRI_LEFT_CORNER       = 'l';
const char GameLevel::TRI_RIGHT_CORNER		= 'r';

const char GameLevel::FINITE_LIFE_CHAR   = 'f';
const char GameLevel::INFINITE_LIFE_CHAR = 'i';

const char* GameLevel::BOSS_LEVEL_KEYWORD = "boss";

const char* GameLevel::ALL_ITEM_TYPES_KEYWORD               = "all";
const char* GameLevel::POWERUP_ITEM_TYPES_KEYWORD           = "powerups";
const char* GameLevel::POWERNEUTRAL_ITEM_TYPES_KEYWORD      = "powerneutrals";
const char* GameLevel::POWERDOWN_ITEM_TYPES_KEYWORD         = "powerdowns";

const char* GameLevel::STAR_POINT_MILESTONE_KEYWORD = "STARS:";

const char* GameLevel::PADDLE_STARTING_X_POS = "PADDLESTARTXPOS:";

// Private constructor, requires all the pieces that make up the level
GameLevel::GameLevel(size_t levelIdx, const std::string& filepath, const std::string& levelName, 
                     unsigned int numBlocks, 
                     const std::vector<std::vector<LevelPiece*> >& pieces,
                     const std::vector<GameItem::ItemType>& allowedDropTypes, 
                     size_t randomItemProbabilityNum, long* starAwardScores, float paddleStartXPos) :

levelIdx(levelIdx), currentLevelPieces(pieces), allowedDropTypes(allowedDropTypes), 
randomItemProbabilityNum(randomItemProbabilityNum), piecesLeft(numBlocks),
filepath(filepath), levelName(levelName), prevHighScore(0), highScore(0),
levelAlmostCompleteSignaled(false), boss(NULL) {

	assert(!filepath.empty());
	
    this->InitPieces(paddleStartXPos, pieces);

    // Set all of the star reward milestone scores
    for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
        this->starAwardScores[i] = starAwardScores[i];
    }

    this->SetPaddleStartXPos(paddleStartXPos);
}

GameLevel::GameLevel(size_t levelIdx, const std::string& filepath, const std::string& levelName,
                     const std::vector<std::vector<LevelPiece*> >& pieces, Boss* boss,
                     const std::vector<GameItem::ItemType>& allowedDropTypes, size_t randomItemProbabilityNum,
                     float paddleStartXPos) :

levelIdx(levelIdx), currentLevelPieces(pieces), allowedDropTypes(allowedDropTypes), 
randomItemProbabilityNum(randomItemProbabilityNum),
piecesLeft(0), filepath(filepath), levelName(levelName), highScore(0),
levelAlmostCompleteSignaled(false), boss(boss) {

    assert(!filepath.empty());
	assert(boss != NULL);

    this->InitPieces(paddleStartXPos, pieces);
    
    // Place the boss at the center of the level...
    this->boss->Init(this->GetLevelUnitWidth() / 2.0f, this->GetLevelUnitHeight() / 2.0f);

    // Set all of the star reward milestone scores to zero, they aren't used in boss fight levels
    for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
        this->starAwardScores[i] = 0;
    }

    this->SetPaddleStartXPos(paddleStartXPos);
}

// Destructor, clean up heap stuffs
GameLevel::~GameLevel() {
	// Clean up level pieces
	for (size_t i = 0; i < this->currentLevelPieces.size(); i++) {
		for (size_t j = 0; j < this->currentLevelPieces[i].size(); j++) {
			delete this->currentLevelPieces[i][j];
			this->currentLevelPieces[i][j] = NULL;
		}
		this->currentLevelPieces[i].clear();
	}
	this->currentLevelPieces.clear();

    if (this->boss != NULL) {
        delete this->boss;
        this->boss = NULL;
    }
}

void GameLevel::InitPieces(float paddleStartXPos, const std::vector<std::vector<LevelPiece*> >& pieces) {
    assert(pieces.size() > 0);
    
    // Set the dimensions of the level
	this->width = pieces[0].size();
	this->height = pieces.size();

    float unitWidth  = this->GetLevelUnitWidth();
    float unitHeight = this->GetLevelUnitHeight();
    this->levelHypotenuse = sqrt(unitWidth * unitWidth + unitHeight * unitHeight);

    // Initialize triggerable and AI pieces...
    for (size_t row = 0; row < this->height; row++) {
        for (size_t col = 0; col < this->width; col++) {
            LevelPiece* currPiece = this->currentLevelPieces[row][col];
            
            if (currPiece->GetHasTriggerID()) {
                this->triggerablePieces.insert(std::make_pair(currPiece->GetTriggerID(), currPiece));
            }
            if (currPiece->IsAIPiece()) {
                aiEntities.insert(currPiece);
            }
        }
    }

    // When a no-entry block is in the paddle area we make its partially visible to indicate
    // that the paddle can move through it
    int paddleStartingIdx;
    if (paddleStartXPos < 0) {
        paddleStartingIdx = this->width / 2;
    }
    else {
        paddleStartingIdx = paddleStartXPos / LevelPiece::PIECE_WIDTH;
    }

    for (int i = paddleStartingIdx+1; i < static_cast<int>(this->width); i++) {
        
        LevelPiece* currPiece = this->currentLevelPieces[0][i];
        if (!currPiece->IsNoBoundsPieceType() && currPiece->GetType() != LevelPiece::NoEntry) {
            break;
        }
        else if (currPiece->GetType() == LevelPiece::NoEntry) {
            ColourRGBA currColour = currPiece->GetColour();
            currColour[3] = 0.5f;
            currPiece->SetColour(currColour);
        }

        if (this->height > 1) {
            LevelPiece* pieceAbove = this->currentLevelPieces[1][i];
            if (pieceAbove->GetType() == LevelPiece::NoEntry) {
                ColourRGBA currColour = pieceAbove->GetColour();
                currColour[3] = 0.5f;
                pieceAbove->SetColour(currColour);
            }
        }

    }

    for (int i = paddleStartingIdx-1; i >= 0; i--) {
        LevelPiece* currPiece = this->currentLevelPieces[0][i];
        if (!currPiece->IsNoBoundsPieceType() && currPiece->GetType() != LevelPiece::NoEntry) {
            break;
        }
        else if (currPiece->GetType() == LevelPiece::NoEntry) {
            ColourRGBA currColour = currPiece->GetColour();
            currColour[3] = 0.5f;
            currPiece->SetColour(currColour);
        }

        if (this->height > 1) {
            LevelPiece* pieceAbove = this->currentLevelPieces[1][i];
            if (pieceAbove->GetType() == LevelPiece::NoEntry) {
                ColourRGBA currColour = pieceAbove->GetColour();
                currColour[3] = 0.5f;
                pieceAbove->SetColour(currColour);
            }
        }
    }
}

void GameLevel::SetPaddleStartXPos(float xPos) {
    // Setup the paddle start x position...
    float minBound = this->GetPaddleMinBound();
    float maxBound = this->GetPaddleMaxBound();
    if (xPos < 0) {
        this->paddleStartXPos = (maxBound + minBound) / 2.0f;
    }
    else {
        assert(xPos >= minBound + PlayerPaddle::PADDLE_HALF_WIDTH && xPos <= maxBound - PlayerPaddle::PADDLE_HALF_WIDTH);
        this->paddleStartXPos = xPos;
    }
}

GameLevel* GameLevel::CreateGameLevelFromFile(const GameWorld::WorldStyle& style, size_t levelIdx, std::string filepath) {
	std::stringstream* inFile = ResourceManager::GetInstance()->FilepathToInOutStream(filepath);
	if (inFile == NULL) {
		assert(false);
		return NULL;
	}

    std::string levelName("");

    // Attempt to read in the boss keyword (it may or may not be there, if it isn't then the
    // level is not a boss level by default).
    bool levelHasBoss = false;
    std::string bossKeyword("");
    while (bossKeyword.empty()) {
		if (!std::getline(*inFile, bossKeyword)) {
			debug_output("ERROR: Error reading the start of the level file: " << filepath);
			delete inFile;
			inFile = NULL;
			return NULL;
		}
		bossKeyword = stringhelper::trim(bossKeyword);
	}
    if (bossKeyword.compare(BOSS_LEVEL_KEYWORD) == 0) {
        levelHasBoss = true;

	    // Read in the level name
	    while (levelName.empty()) {
		    if (!std::getline(*inFile, levelName)) {
			    debug_output("ERROR: Error reading in level name for file: " << filepath);
			    delete inFile;
			    inFile = NULL;
			    return NULL;
		    }
		    levelName = stringhelper::trim(levelName);
	    }

    }
    else {
        // Not the boss keyword... that means that the there is no boss keyword and instead we
        // have just read in the level name
        levelName = bossKeyword;
    }

	// Read in the file width and height
	int width = 0;
	int height = 0;
	if (!(*inFile >> width && *inFile >> height)) {
		debug_output("ERROR: Error reading in width/height for file: " << filepath);
		delete inFile;
		inFile = NULL;
		return NULL;
	}
	
	// Ensure width and height are valid
	if (width <= 0 || height <= 0) {
		debug_output("ERROR: Invalid width/height values for file: " << filepath);
		delete inFile;
		inFile = NULL;
		return NULL;	
	}

	std::vector<std::vector<LevelPiece*> > levelPieces;
	unsigned int numVitalPieces = 0;

	// Keep track of named portal blocks...
	std::map<char, PortalBlock*> portalBlocks;
	// Reset the colours for the portal blocks
	PortalBlock::ResetPortalColourGenerator();

	// Keep track of named tesla blocks...
	std::map<char, TeslaBlock*> teslaBlocks;

	// Read in the values that make up the level
	for (int h = 0; h < height; h++) {
		std::vector<LevelPiece*> currentRowPieces;

		for (int w = 0; w < width; w++) {
			char currBlock;
			
			// Read in the current level piece / block
			if (!(*inFile >> currBlock)) {
					debug_output("ERROR: Could not properly read level interior value at width = " << w << ", height = " << h);
					delete inFile;
					inFile = NULL;
					return NULL;	
			}

			// Validate the block type and create teh appropriate object for that block type...
			LevelPiece* newPiece = NULL;
			unsigned int pieceWLoc = static_cast<unsigned int>(w);
			unsigned int pieceHLoc = static_cast<unsigned int>(height - 1 - h);

			switch (currBlock) {
				case EMPTY_SPACE_CHAR:
					newPiece = new EmptySpaceBlock(pieceWLoc, pieceHLoc);
					break;
				case SOLID_BLOCK_CHAR:
					newPiece = new SolidBlock(pieceWLoc, pieceHLoc);
					break;
				case GREEN_BREAKABLE_CHAR:
				case YELLOW_BREAKABLE_CHAR:
				case ORANGE_BREAKABLE_CHAR:
				case RED_BREAKABLE_CHAR:
					newPiece = new BreakableBlock(currBlock, pieceWLoc, pieceHLoc);
					break;
				case BOMB_CHAR:
					newPiece = new BombBlock(pieceWLoc, pieceHLoc);
					break;
				case INKBLOCK_CHAR:
					newPiece = new InkBlock(pieceWLoc, pieceHLoc);
					break;
				case PRISM_BLOCK_CHAR:
					newPiece = new PrismBlock(pieceWLoc, pieceHLoc);
					break;
				case COLLATERAL_BLOCK_CHAR:
					newPiece = new CollateralBlock(pieceWLoc, pieceHLoc);
					break;

				case CANNON_BLOCK_CHAR: {
						// C(d[-e]) - Cannon block
                        // d: The direction to always fire the cannon block in (or specify random firing direction)
                        //    the value can be any degree angle starting at 0 going to 359.
                        // The angle starts by firing directly upwards and moves around the circle
                        // of angles clockwise (e.g., 90 will always fire the ball perfectly to the right of the cannon, 180 will always
                        // fire the ball downwards from the cannon, ...). 
                        // [-e]: is optional - it allows the specification of a angle range from d to e, inclusive.
						char tempChar;

						// Beginning bracket
						*inFile >> tempChar;
						if (tempChar != '(') {
							debug_output("ERROR: poorly formed cannon block syntax, missing the beginning '('");
							break;
						}

						// Degree angle value (or random -1)
						int rotationValue1 = 0;
						int rotationValue2 = 0;
                        *inFile >> rotationValue1;
						if (rotationValue1 != -1 && (rotationValue1 < 0 || rotationValue1 > 720)) {
							debug_output("ERROR: poorly formed cannon block syntax, first degree angle must either be -1 or in [0,720]");
							break;
						}

                        if (rotationValue1 == -1) {
                            rotationValue1 = 0;
                            rotationValue2 = 359;

						    // Closing bracket
						    *inFile >> tempChar;
						    if (tempChar != ')') {
							    debug_output("ERROR: poorly formed cannon block syntax, missing the beginning '('");
							    break;
						    }
                        }
                        else {
                            // Check for '-'
						    *inFile >> tempChar;
						    if (tempChar == '-') {
                                *inFile >> rotationValue2;
						        if (rotationValue2 != -1 && (rotationValue2 < 0 || rotationValue2 > 720)) {
							        debug_output("ERROR: poorly formed cannon block syntax, second degree angle must either be -1 or in [0,720]");
							        break;
						        }

                                *inFile >> tempChar;
                                if (tempChar != ')') {
							        debug_output("ERROR: poorly formed cannon block syntax, missing the beginning '('");
							        break;
                                }
						    }
                            else {
                                if (tempChar != ')') {
							        debug_output("ERROR: poorly formed cannon block syntax, missing the beginning '('");
							        break;
                                }
                                else {
                                    rotationValue2 = rotationValue1;
                                }
                            }
                        }

                        newPiece = new CannonBlock(pieceWLoc, pieceHLoc, std::make_pair(rotationValue1, rotationValue2));
					}
					break;

				case TESLA_BLOCK_CHAR: {
                        // A([1|0], [1|0], a, b) - Tesla Block (When active with another tesla block, forms an arc of lightning between the two)
                        // [1|0]: A '1' or a '0' to indicated whether it starts on (1) or off (0).
                        // [1|0]: A '1' or a '0' to indicated whether it can be turned on/off (1) or is unchangable (0).
                        // a : The single character name of this tesla block.
                        // b : One or more other named tesla blocks seperated by commas.

						char tempChar;

						// Beginning bracket
						*inFile >> tempChar;
						if (tempChar != '(') {
							debug_output("ERROR: poorly formed tesla block syntax, missing the beginning '('");
							break;
						}

						// Read whether the block is on or off
						*inFile >> tempChar;
						if (tempChar != '1' && tempChar != '0') {
							debug_output("ERROR: poorly formed tesla block syntax, missing the [1|0] to indicate whether the block starts on or off");
							break;
						}
						bool startsOn = (tempChar == '1') ? true : false;


						// Should be a comma after the on/off...
						*inFile >> tempChar;
						if (tempChar != ',') {
							debug_output("ERROR: poorly formed tesla block syntax, missing ',' after the on/off option.");
							break;
						}

						// Read whether the block is changable or not
						*inFile >> tempChar;
						if (tempChar != '1' && tempChar != '0') {
							debug_output("ERROR: poorly formed tesla block syntax, missing the [1|0] to indicate whether the block is changable or not");
							break;
						}
						bool isChangable = (tempChar == '1') ? true : false;

						// Should be a comma after the changable...
						*inFile >> tempChar;
						if (tempChar != ',') {
							debug_output("ERROR: poorly formed tesla block syntax, missing ',' after the changable option.");
							break;
						}

						// Read the single character name of the tesla block
						char teslaBlockName;
						*inFile >> teslaBlockName;
						if ((teslaBlockName < 'A' && teslaBlockName > 'z') || (teslaBlockName < '0' && teslaBlockName > '9')) {
							debug_output("ERROR: tesla block name must be a character A-Z, a-z or 0-9");
							break;
						}

						// Should be a comma after the name...
						*inFile >> tempChar;
						if (tempChar != ',') {
							debug_output("ERROR: poorly formed tesla block syntax, missing ',' after the name.");
							break;
						}

						// Read in all the names of the brother tesla blocks that this one can fire lightning to
						std::string errorStr;
						std::list<char> connectedNameList;
						for (;;) {
							if (*inFile >> tempChar) {
								if ((tempChar >= 'A' && tempChar <= 'z') || (teslaBlockName < '0' && teslaBlockName > '9')) {
									connectedNameList.push_back(tempChar);

									*inFile >> tempChar;
									// Check for the end bracket
									if (tempChar == ')') {
										break;
									}
									if (tempChar == ',') {
										continue;
									}

									errorStr = "invalid connecting tesla block list formatting.";
									break;
								}
								else {
									errorStr = "invalid tesla block name in connecting tesla block list.";
									break;						
								}
							}
							else {
								errorStr = "failed to read connecting tesla block list.";
								break;
							}
						}

						if (!errorStr.empty() || connectedNameList.empty()) {
							debug_output("ERROR: poorly formed tesla block syntax, " << errorStr);
							break;							
						}

						// Now we need to connect the tesla blocks...
						TeslaBlock* currentTeslaBlock = NULL;
						std::list<TeslaBlock*> siblingTeslaBlocks;

						std::map<char, TeslaBlock*>::iterator findIter = teslaBlocks.find(teslaBlockName);
						std::pair<std::map<char, TeslaBlock*>::iterator, bool> insertResult;

						if (findIter != teslaBlocks.end()) {
							currentTeslaBlock = findIter->second;
							assert(currentTeslaBlock != NULL);
						}

						// Go through each sibling and try to find it in the list of tesla blocks already created,
						// if it does not exist then build a place holder for it
						for (std::list<char>::const_iterator iter = connectedNameList.begin(); iter != connectedNameList.end(); ++iter) {
							
							const char& siblingName = *iter;
							findIter = teslaBlocks.find(siblingName);
							TeslaBlock* siblingTeslaBlock = NULL;

							if (findIter != teslaBlocks.end()) {
								siblingTeslaBlock = findIter->second;
								assert(siblingTeslaBlock != NULL);
								assert(siblingTeslaBlock != currentTeslaBlock);
								siblingTeslaBlocks.push_back(siblingTeslaBlock);
							}
							else {
								assert(siblingTeslaBlock == NULL);
								// No sibling tesla block with the current name exists yet, create one and add it to the list
								siblingTeslaBlock = new TeslaBlock(false, false, 0, 0);
								siblingTeslaBlocks.push_back(siblingTeslaBlock);
								insertResult = teslaBlocks.insert(std::make_pair(siblingName, siblingTeslaBlock));
								if (!insertResult.second) {
									// Tesla block with that name already existed... fail!
									delete siblingTeslaBlock;
									siblingTeslaBlock = NULL;
									errorStr = std::string("Duplicate tesla block name: '") + siblingName + std::string("'");
									break;
								}
							}
						}

						if (!errorStr.empty()) {
							debug_output("ERROR: Poorly formed tesla block syntax, " << errorStr);
							break;
						}

						if (currentTeslaBlock == NULL) {
							// No tesla block has been created for the current name yet, create one.
							currentTeslaBlock = new TeslaBlock(startsOn, isChangable, pieceWLoc, pieceHLoc);
							currentTeslaBlock->SetConnectedTeslaBlockList(siblingTeslaBlocks);
							insertResult = teslaBlocks.insert(std::make_pair(teslaBlockName, currentTeslaBlock));
							if (!insertResult.second) {
								delete currentTeslaBlock;
								currentTeslaBlock = NULL;
								debug_output("ERROR: Poorly formed tesla block syntax, " << "Duplicate tesla block name: '" << teslaBlockName << "'");
								break;
							}
						}
						else {
							// The tesla block has previously been created and inserted into the map...
							// Use that tesla block, make sure it has the proper location and sibling
							currentTeslaBlock->SetWidthAndHeightIndex(pieceWLoc, pieceHLoc);
							assert(currentTeslaBlock->GetConnectedTeslaBlockList().empty());
							currentTeslaBlock->SetConnectedTeslaBlockList(siblingTeslaBlocks);
							currentTeslaBlock->SetElectricityIsActive(startsOn);
							currentTeslaBlock->SetIsChangable(isChangable);
						}
						
						// We need to inform the level about any initially 'on' tesla blocks!!!
						// TODO

						newPiece = currentTeslaBlock;
					}
					break;

				case PORTAL_BLOCK_CHAR: {
						// X(a,b) - Portal block:
						// a: A single character name for this portal block
						// b: The single character name of the sibling portal block that this portal block is
						// the enterance and exit for.
						char tempChar;
						*inFile >> tempChar;
						if (tempChar != '(') {
							debug_output("ERROR: poorly formed portal block syntax, missing '('");
							break;
						}
						
						char portalName;
						*inFile >> portalName;
						if ((portalName < 'A' && portalName > 'z') || (portalName < '0' && portalName > '9')) {
							debug_output("ERROR: Portal block name must be a character A-Z or a-z");
							break;
						}

						*inFile >> tempChar;
						if (tempChar != ',') {
							debug_output("ERROR: poorly formed portal block syntax, missing ','");
							break;
						}				

						char siblingName;
						*inFile >> siblingName;
						if ((portalName < 'A' && portalName > 'z') || (portalName < '0' && portalName > '9')) {
							debug_output("ERROR: Portal block name must be a character A-Z or a-z");
							break;
						}

						*inFile >> tempChar;
						if (tempChar != ')') {
							debug_output("ERROR: poorly formed portal block syntax, missing ')'");
							break;
						}

						// First try to find either portal block in the current mapping
						PortalBlock* currentPortalBlock = NULL;
						PortalBlock* siblingPortalBlock = NULL;
						std::map<char, PortalBlock*>::iterator findIter = portalBlocks.find(portalName);
						std::pair<std::map<char, PortalBlock*>::iterator, bool> insertResult;

						if (findIter != portalBlocks.end()) {
							currentPortalBlock = findIter->second;
							assert(currentPortalBlock != NULL);
						}
						findIter = portalBlocks.find(siblingName);
						if (findIter != portalBlocks.end()) {
							siblingPortalBlock = findIter->second;
						}

						if (siblingPortalBlock == NULL) {
							// No sibling exists yet, create one
							siblingPortalBlock = new PortalBlock(0, 0, NULL);
							insertResult = portalBlocks.insert(std::make_pair(siblingName, siblingPortalBlock));
							assert(insertResult.second);
						}

						if (currentPortalBlock == NULL) {
							// No portal block has been created for the current name yet, create one.
							currentPortalBlock = new PortalBlock(pieceWLoc, pieceHLoc, siblingPortalBlock);
							insertResult = portalBlocks.insert(std::make_pair(portalName, currentPortalBlock));
							assert(insertResult.second);
						}
						else {
							// The portal block has previously been created and inserted into the map...
							// Use that portal block, make sure it has the proper location and sibling
							currentPortalBlock->SetWidthAndHeightIndex(pieceWLoc, pieceHLoc);
							assert(currentPortalBlock->GetSiblingPortal() == NULL);
							currentPortalBlock->SetSiblingPortal(siblingPortalBlock);

							// Set the same colour for both the current and sibling portal blocks
							const Colour& portalBlockColour = PortalBlock::GeneratePortalColour();
							currentPortalBlock->SetColour(ColourRGBA(portalBlockColour, 1.0f));
							siblingPortalBlock->SetColour(ColourRGBA(portalBlockColour, 1.0f));
						}

						newPiece = currentPortalBlock;
					}
					break;

				case TRIANGLE_BLOCK_CHAR: {
						// T(x,p) - Triangle block, 
						// x: type of block from the above, can be any of the following: {R, O, Y, G, S}
						// p: the orientation of the triangle (where the outer corner is located), can be any of the following: {ul, ur, ll, lr}
						// i.e., upper-left = ul, lower-right = lr, etc.
						char tempChar;
						*inFile >> tempChar;
						if (tempChar != '(') {
							debug_output("ERROR: poorly formed triangle block syntax, missing '('");
							break;
						}

						// Read in 'x'
						char typeOfBlock;
						*inFile >> typeOfBlock;
						if (!BreakableBlock::IsValidBreakablePieceType(typeOfBlock) && typeOfBlock != SOLID_BLOCK_CHAR && typeOfBlock != PRISM_BLOCK_CHAR) {
							debug_output("ERROR: Triangle block has invalid block type specified in descriptor: " << typeOfBlock);
							break;
						}

						*inFile >> tempChar;
						if (tempChar != ',') {
							debug_output("ERROR: poorly formed triangle block syntax, missing ','");
							break;
						}

						// Read in 'p'
						char orient1, orient2;
						*inFile >> orient1;
						*inFile >> orient2;

						TriangleBlock::Orientation orientation = TriangleBlock::UpperRight;
						if (orient1 == TRI_UPPER_CORNER) {
							if (orient2 == TRI_RIGHT_CORNER) {
								// Upper-right
								orientation = TriangleBlock::UpperRight;
							}
							else if (orient2 == TRI_LEFT_CORNER) {
								// Upper-left
								orientation = TriangleBlock::UpperLeft;
							}
						}
						else if (orient1 == TRI_LOWER_CORNER) {
							if (orient2 == TRI_RIGHT_CORNER) {
								// Lower-right
								orientation = TriangleBlock::LowerRight;
							}
							else if (orient2 == TRI_LEFT_CORNER) {
								// Lower-left
								orientation = TriangleBlock::LowerLeft;
							}
						}

						// Read in the closing bracket
						*inFile >> tempChar;
						if (tempChar != ')') {
							debug_output("ERROR: poorly formed triangle block syntax, missing ')'");
							break;
						}

						// Create a new class for the triangle block based on its type...
						if (typeOfBlock == SOLID_BLOCK_CHAR) {
							newPiece = new SolidTriangleBlock(orientation, pieceWLoc, pieceHLoc);
						}
						else if (typeOfBlock == PRISM_BLOCK_CHAR) {
							newPiece = new PrismTriangleBlock(orientation, pieceWLoc, pieceHLoc);
						}
						else {
							newPiece = new BreakableTriangleBlock(typeOfBlock, orientation, pieceWLoc, pieceHLoc);
						}

					}
					break;

				case GameLevel::ITEM_DROP_BLOCK_CHAR: {
						// D(i0, i1, i2, ...) - Item drop block:
						// i0, i1, i2, ... : The names of all the item types that the block is allowed to drop, names can be 
						//                   repeated in order to make higher probabilities of drops. Also, there are several special key
						//                   words that can be used: (powerups, powerdowns, powerneutrals), which cause the block to drop 
						//                   all items of those respective designations.
						
						std::vector<GameItem::ItemType> itemTypes;
						if (!ReadItemList(*inFile, itemTypes)) {
							break;
						}

						newPiece = new ItemDropBlock(itemTypes, pieceWLoc, pieceHLoc);
					}
					break;

                case GameLevel::SWITCH_BLOCK_CHAR: {
                        // W(a) - Switch block:
                        // a : The trigger ID of the block that gets triggered by the switch 
                        //     (switch is turned on when a ball/projectile hits it).

                        char tempChar;
                        *inFile >> tempChar;
					    if (tempChar != '(') {
					        debug_output("ERROR: poorly formed switch block syntax, missing '('");
						    break;
					    }
                        
                        // The next value will be a triggerID
                        LevelPiece::TriggerID switchTriggerID = LevelPiece::NO_TRIGGER_ID;
                        *inFile >> switchTriggerID;
                        if (switchTriggerID < 0) {
					        debug_output("ERROR: poorly formed switch block syntax, switch trigger ID was invalid.");
						    break;
                        }

					    // Read in the closing bracket
					    *inFile >> tempChar;
					    if (tempChar != ')') {
						    debug_output("ERROR: poorly formed switch block syntax, missing ')'");
						    break;
					    }
                        
                        newPiece = new SwitchBlock(switchTriggerID, pieceWLoc, pieceHLoc);
                    }
                    break;

                case GameLevel::ONE_WAY_BLOCK_CHAR:
                    {
                        // F(a) - One-way block:
                        // a : The direction that the block allows the ball to travel through it, allowable options are:
                        //     {u, d, l, r} for up, down, left and right, respectively.
                        
                        char tempChar;
                        *inFile >> tempChar;
					    if (tempChar != '(') {
					        debug_output("ERROR: poorly formed one-way block syntax, missing '('");
						    break;
					    }


                        // The next value will be the direction of the one-way...
                        char oneWayChar;
                        OneWayBlock::OneWayDir oneWayEnum;
                        *inFile >> oneWayChar;
                        if (!OneWayBlock::ConvertCharToOneWayDir(oneWayChar, oneWayEnum)) {
					        debug_output("ERROR: illegal character found in one-way block syntax, character must be one of 'u', 'd', 'l' or 'r'.");
						    break;
                        }

					    // Read in the closing bracket
					    *inFile >> tempChar;
					    if (tempChar != ')') {
						    debug_output("ERROR: poorly formed one-way block syntax, missing ')'");
						    break;
					    }

                        newPiece = new OneWayBlock(oneWayEnum, pieceWLoc, pieceHLoc);
                    }
                    break;

                case GameLevel::NO_ENTRY_BLOCK_CHAR:
                    // N - No-entry block
                    newPiece = new NoEntryBlock(pieceWLoc, pieceHLoc);
                    break;

                case GameLevel::LASER_TURRET_BLOCK_CHAR:
                    // L - Laser Turret Block
                    newPiece = new LaserTurretBlock(pieceWLoc, pieceHLoc);
                    break;

                case GameLevel::ROCKET_TURRET_BLOCK_CHAR:
                    // J - Rocket Turret Block
                    newPiece = new RocketTurretBlock(pieceWLoc, pieceHLoc);
                    break;

                case GameLevel::MINE_TURRET_BLOCK_CHAR:
                    // M - Mine Turret Block
                    newPiece = new MineTurretBlock(pieceWLoc, pieceHLoc);
                    break;

                case GameLevel::ALWAYS_DROP_BLOCK_CHAR: {
                    // K(i0, i1, i2, ...) - Always drop block (similar to Item drop block, but is destroyed after 1 hit):
                    // i0, i1, i2, ... : The names of all the item types that the block is allowed to drop, names can be 
                    //                   repeated in order to make higher probabilities of drops. Also, there are several special key
                    //                   words that can be used: (all, powerups, powerdowns, powerneutrals), which cause the block to drop 
                    //                   all items of those respective designations.

					std::vector<GameItem::ItemType> itemTypes;
					if (!ReadItemList(*inFile, itemTypes)) {
						break;
					}

					newPiece = new AlwaysDropBlock(itemTypes, pieceWLoc, pieceHLoc);
                    break;
                }

                case GameLevel::REGEN_BLOCK_CHAR: {
                    // Q([f|i]) - Regen block
                    // [f|i] - 'f' means that the block has finite life, 'i' means that the block has infinite life

                    char tempChar;

                    // Read the opening bracket
                    *inFile >> tempChar;
				    if (tempChar != '(') {
				        debug_output("ERROR: poorly formed regen block syntax, missing '('");
					    break;
				    }

                    // Read whether it has infinite life or finite life
                    *inFile >> tempChar;
                    bool hasInfiniteLife = false;
                    if (tempChar == GameLevel::FINITE_LIFE_CHAR) {
                        hasInfiniteLife = false;
                    }
                    else if (tempChar == GameLevel::INFINITE_LIFE_CHAR) {
                        hasInfiniteLife = true;
                    }
                    else {
					    debug_output("ERROR: poorly formed regen block syntax '" << GameLevel::FINITE_LIFE_CHAR << "' or '" <<
                            GameLevel::INFINITE_LIFE_CHAR << "' must be used an no other characters.");
					    break;
                    }

                    // Read the closing bracket
				    *inFile >> tempChar;
				    if (tempChar != ')') {
					    debug_output("ERROR: poorly formed regen block syntax, missing ')'");
					    break;
				    }

                    newPiece = new RegenBlock(hasInfiniteLife, pieceWLoc, pieceHLoc);
                    break;
                }

				default:
					debug_output("ERROR: Invalid level interior value: " << currBlock << " at width = " << pieceWLoc << ", height = " << pieceHLoc);
					delete inFile;
					inFile = NULL;
					GameLevel::CleanUpFileReadData(levelPieces);
					return NULL;
			}

            // Check to see if there's a trigger ID for the block...
            if (newPiece != NULL) {
                char nextChar = inFile->peek();
                if (nextChar == '{') {
                    // There's a trigger ID available, read it!
                    std::string triggerIDWithBraces;
                    if (!(*inFile >> triggerIDWithBraces)) {
                        debug_output("ERROR: Trigger ID could not be properly read from file.");
                        delete newPiece;
                        newPiece = NULL;
                    }
                    else {
                        std::string triggerIDStr = triggerIDWithBraces.substr(1, triggerIDWithBraces.size()-2);
                        if (triggerIDStr.empty()) {
                            debug_output("ERROR: Invalid (empty) trigger ID found in level file.");
                            delete newPiece;
                            newPiece = NULL;
                        }
                        else {
                            LevelPiece::TriggerID triggerID = atoi(triggerIDStr.c_str());
                            if (triggerID < 0) {
                                debug_output("ERROR: Invalid (< 0) trigger ID found in level file.");
                                delete newPiece;
                                newPiece = NULL;
                            }
                            else {
                                newPiece->SetTriggerID(triggerID);
                            }
                        }
                    }
                }
            }

			if (newPiece == NULL) {
				assert(false);
				debug_output("ERROR: Invalid level piece found.");
				delete inFile;
				inFile = NULL;
				GameLevel::CleanUpFileReadData(levelPieces);
				return NULL;
			}

			currentRowPieces.push_back(newPiece);
			if (newPiece->MustBeDestoryedToEndLevel()) {
				numVitalPieces++;
			}
		}

		levelPieces.insert(levelPieces.begin(), currentRowPieces);
	}

    // Get the star milestone totals
    std::string tempReadStr;
    long pointAmount;
    long starAwardScores[GameLevel::MAX_STARS_PER_LEVEL];
    if ((*inFile >> tempReadStr) && tempReadStr.compare(GameLevel::STAR_POINT_MILESTONE_KEYWORD) == 0) {
        for (int i = 0; i < GameLevel::MAX_STARS_PER_LEVEL; i++) {
            if (*inFile >> pointAmount) {
                starAwardScores[i] = pointAmount;
            }
            else {
                debug_output("ERROR: No star point milestone for star #" << i << " was found.");
	            GameLevel::CleanUpFileReadData(levelPieces);
	            delete inFile;
	            inFile = NULL;
	            return NULL;
            }
        }
    }
    else {
        debug_output("ERROR: No '" << GameLevel::STAR_POINT_MILESTONE_KEYWORD << "' keyword was found.");
	    GameLevel::CleanUpFileReadData(levelPieces);
	    delete inFile;
	    inFile = NULL;
	    return NULL;
    }

	// Finished reading in all the blocks for the level, now read in the allowed item drops and their probabilities...
	std::string itemTypeName;
	int probabilityNum = 0;
	size_t randomItemProbabilityNum = 0;
	std::vector<GameItem::ItemType> allowedDropTypes;
	while (*inFile >> itemTypeName) {
        if (itemTypeName.compare(GameLevel::PADDLE_STARTING_X_POS) == 0) {
            for (int i = 0; i < static_cast<int>(itemTypeName.size()); i++) {
                inFile->unget();
            }
            break;
        }

		// There are some special keywords that we need to check for first...
		if (itemTypeName.compare(ALL_ITEM_TYPES_KEYWORD) == 0) {
			// Add all item types...
			std::set<GameItem::ItemType> allItemTypes = GameItemFactory::GetInstance()->GetAllItemTypes();
			allItemTypes.erase(GameItem::RandomItem);
			randomItemProbabilityNum++;
			allowedDropTypes.insert(allowedDropTypes.end(), allItemTypes.begin(), allItemTypes.end());
		}
		else if (itemTypeName.compare(POWERUP_ITEM_TYPES_KEYWORD) == 0) {
			// Add all power-up item types
			const std::set<GameItem::ItemType>& powerUpItemTypes = GameItemFactory::GetInstance()->GetPowerUpItemTypes();
			allowedDropTypes.insert(allowedDropTypes.end(), powerUpItemTypes.begin(), powerUpItemTypes.end());
		}
		else if (itemTypeName.compare(POWERNEUTRAL_ITEM_TYPES_KEYWORD) == 0) {
			// Add all power-neutral item types
			std::set<GameItem::ItemType> powerNeutralItemTypes = GameItemFactory::GetInstance()->GetPowerNeutralItemTypes();
			powerNeutralItemTypes.erase(GameItem::RandomItem);
			randomItemProbabilityNum++;
			allowedDropTypes.insert(allowedDropTypes.end(), powerNeutralItemTypes.begin(), powerNeutralItemTypes.end());
		}
		else if (itemTypeName.compare(POWERDOWN_ITEM_TYPES_KEYWORD) == 0) {
			// Add all power-down item types
			const std::set<GameItem::ItemType>& powerDownItemTypes = GameItemFactory::GetInstance()->GetPowerDownItemTypes();
			allowedDropTypes.insert(allowedDropTypes.end(), powerDownItemTypes.begin(), powerDownItemTypes.end());
		}
		else {
			if (!GameItemFactory::GetInstance()->IsValidItemTypeName(itemTypeName)) {
				debug_output("ERROR: Invalid item type name found in allowable item drop probability list: '" << itemTypeName << "'");
				GameLevel::CleanUpFileReadData(levelPieces);
				delete inFile;
				inFile = NULL;
				return NULL;
			}

			if (*inFile >> probabilityNum) {
				if (probabilityNum < 0) {
					debug_output("ERROR: Poorly formated item drop probability number - must be >= 0.");
					GameLevel::CleanUpFileReadData(levelPieces);
					delete inFile;
					inFile = NULL;
					return NULL;
				}
				else {
					GameItem::ItemType currItemType = GameItemFactory::GetInstance()->GetItemTypeFromName(itemTypeName);
					// Ignore random item types...
					if (currItemType == GameItem::RandomItem) {
						randomItemProbabilityNum = probabilityNum;
						continue;
					}
					for (int i = 0; i < probabilityNum; i++) {
						allowedDropTypes.push_back(currItemType);
					}
				}
			}
			else {
				debug_output("ERROR: Poorly formated item drop probability list - missing probability number.");
				GameLevel::CleanUpFileReadData(levelPieces);
				delete inFile;
				inFile = NULL;
				return NULL;
			}	
		}
	}

	// If there are no allowed drop types then there should be a zero random item drop probability
	if (allowedDropTypes.empty()) {
		randomItemProbabilityNum = 0;
	}

    // Get the x-coordinate where the paddle starts the level (and goes to when the ball dies), 
    // if this is missing then the default (center of the entire level) will be used
    float paddleStartXPos = GameLevel::DEFAULT_PADDLE_START_IDX;
    if ((*inFile >> tempReadStr) && tempReadStr.compare(GameLevel::PADDLE_STARTING_X_POS) == 0) {
        if (!(*inFile >> paddleStartXPos)) {
            debug_output("ERROR: No paddle starting block index value was provided with keyword!");
            GameLevel::CleanUpFileReadData(levelPieces);
            delete inFile;
            inFile = NULL;
            return NULL;
        }
    }

	delete inFile;
	inFile = NULL;

	// Go through all of the portal blocks and make sure they loaded properly...
	for (std::map<char, PortalBlock*>::iterator iter = portalBlocks.begin(); iter != portalBlocks.end(); ++iter) {
		if (iter->second == NULL || iter->second->GetSiblingPortal() == NULL || iter->second->GetWidthIndex() < 0
				|| iter->second->GetHeightIndex() < 0) {
			debug_output("ERROR: Poorly formatted portal blocks.");
			GameLevel::CleanUpFileReadData(levelPieces);
			return NULL;
		}
	}

	// Go through all the pieces and initialize their bounding values appropriately
	for (size_t h = 0; h < levelPieces.size(); h++) {
		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			GameLevel::UpdatePiece(levelPieces, h, w);
		}
	}

    // Build the level based on whether it has a boss or not
    if (levelHasBoss) {
        Boss* boss = Boss::BuildStyleBoss(style);
        if (boss == NULL) {
            assert(false);
            GameLevel::CleanUpFileReadData(levelPieces);
            return NULL;
        }
    
        return new GameLevel(levelIdx, filepath, levelName, levelPieces, boss, allowedDropTypes, randomItemProbabilityNum, paddleStartXPos);

    }
    else {
	    return new GameLevel(levelIdx, filepath, levelName, numVitalPieces, levelPieces, allowedDropTypes, randomItemProbabilityNum, starAwardScores, paddleStartXPos);
    }
}

/**
 * Reads a list of items from a level file. The list has the following format:
 * "(i0,i1,i2,...)"
 * Where ... means the list can continue indefinitely. There are also possible keywords in 
 * the list for allpowerups, allpowerdowns, allpowerneutrals, and all.
 */
bool GameLevel::ReadItemList(std::stringstream& inFile, std::vector<GameItem::ItemType>& items) {
    
    char tempChar;
    inFile >> tempChar;
    if (tempChar != '(') {
	    debug_output("ERROR: poorly formed item drop block syntax, missing '('");
	    return false;
    }		

    // Read each of the item type names and set the proper possible item drops for the item drop block...
    std::string tempStr;
    inFile >> tempStr;

    std::vector<std::string> itemNames;

    // Careful of the case where there's a curly bracket - we want to get rid of the curly bracket in that case
    size_t curlyBracketPos = tempStr.find_first_of("{");
    if (curlyBracketPos != std::string::npos) {
        stringhelper::Tokenize(tempStr, itemNames, "{");
        std::string putBackIntoStream = tempStr.substr(curlyBracketPos);
        inFile.seekg(-static_cast<int>(putBackIntoStream.size()), std::ios_base::cur);
        tempStr = itemNames.front();
        itemNames.clear();
    }
    stringhelper::Tokenize(tempStr, itemNames, ",) \t\r\n"); 

    // Make sure there's at least one item defined
    if (itemNames.empty()) {
	    debug_output("ERROR: poorly formed item list syntax, no item types defined");
	    return false;
    }

    // Figure out the item types from the names...
    for (std::vector<std::string>::const_iterator iter = itemNames.begin(); iter != itemNames.end(); ++iter) {
	    std::string currItemName = stringhelper::trim(*iter);

	    // There are some special keywords that we need to check for first...
	    if (currItemName.compare(ALL_ITEM_TYPES_KEYWORD) == 0) {
		    // Add all item types...
		    const std::set<GameItem::ItemType>& allItemTypes = GameItemFactory::GetInstance()->GetAllItemTypes();
		    items.insert(items.end(), allItemTypes.begin(), allItemTypes.end());
	    }
	    else if (currItemName.compare(POWERUP_ITEM_TYPES_KEYWORD) == 0) {
		    // Add all power-up item types
		    const std::set<GameItem::ItemType>& powerUpItemTypes = GameItemFactory::GetInstance()->GetPowerUpItemTypes();
		    items.insert(items.end(), powerUpItemTypes.begin(), powerUpItemTypes.end());
	    }
	    else if (currItemName.compare(POWERNEUTRAL_ITEM_TYPES_KEYWORD) == 0) {
		    // Add all power-neutral item types
		    const std::set<GameItem::ItemType>& powerNeutralItemTypes = GameItemFactory::GetInstance()->GetPowerNeutralItemTypes();
		    items.insert(items.end(), powerNeutralItemTypes.begin(), powerNeutralItemTypes.end());
	    }
	    else if (currItemName.compare(POWERDOWN_ITEM_TYPES_KEYWORD) == 0) {
		    // Add all power-up item types
		    const std::set<GameItem::ItemType>& powerDownItemTypes = GameItemFactory::GetInstance()->GetPowerDownItemTypes();
		    items.insert(items.end(), powerDownItemTypes.begin(), powerDownItemTypes.end());
	    }
	    else {
		    // Check for manual entry of a specific item type name...
		    bool isValid = GameItemFactory::GetInstance()->IsValidItemTypeName(currItemName);
		    if (!isValid) {
			    debug_output("ERROR: poorly formed item list syntax, no item drop type found for item \"" + currItemName + "\"");
			    return false;
		    }
    		
		    items.push_back(GameItemFactory::GetInstance()->GetItemTypeFromName(currItemName));
	    }
    }

    return true;
}

void GameLevel::CleanUpFileReadData(std::vector<std::vector<LevelPiece*> >& levelPieces) {
	// Clean up any already loaded pieces...
	for (std::vector<std::vector<LevelPiece*> >::iterator iter1 = levelPieces.begin(); iter1 != levelPieces.end(); ++iter1) {
		std::vector<LevelPiece*>& currPieceRow = *iter1;
		for (std::vector<LevelPiece*>::iterator iter2 = currPieceRow.begin(); iter2 != currPieceRow.end(); ++iter2) {
			LevelPiece* currPiece = *iter2;
			delete currPiece;
			currPiece = NULL;
		}
		currPieceRow.clear();
	}
	levelPieces.clear();
}

/**
 * Updates the level piece at the given index.
 */
void GameLevel::UpdatePiece(const std::vector<std::vector<LevelPiece*> >& pieces, size_t hIndex, size_t wIndex) {
	
	// Make sure the provided indices are in the correct range
	if (wIndex < 0 || wIndex >= pieces[0].size() || hIndex < 0 || hIndex >= pieces.size()) {
		return;
	}
	
	LevelPiece* leftNeighbor	= NULL;
	LevelPiece* bottomNeighbor	= NULL;
	LevelPiece* rightNeighbor	= NULL;
	LevelPiece* topNeighbor		= NULL;
	LevelPiece* topLeftNeighbor	= NULL;
	LevelPiece* topRightNeighbor	= NULL;
	LevelPiece* bottomLeftNeighbor	= NULL;
	LevelPiece* bottomRightNeighbor	= NULL;


	if (wIndex != 0) {
		int leftIndex = wIndex-1;
		leftNeighbor = pieces[hIndex][leftIndex];

		if (hIndex != 0) {
			bottomLeftNeighbor = pieces[hIndex-1][leftIndex];
		}
		if (hIndex != pieces.size()-1) {
			topLeftNeighbor = pieces[hIndex+1][leftIndex];
		}
	}

	if (wIndex != pieces[hIndex].size()-1) {
		int rightIndex = wIndex+1;
		rightNeighbor = pieces[hIndex][rightIndex];

		if (hIndex != 0) {
			bottomRightNeighbor = pieces[hIndex-1][rightIndex];
		}
		if (hIndex != pieces.size()-1) {
			topRightNeighbor = pieces[hIndex+1][rightIndex];
		}
	}

	if (hIndex != 0) {
		bottomNeighbor = pieces[hIndex-1][wIndex];
	}

	if (hIndex != pieces.size()-1) {
		topNeighbor = pieces[hIndex+1][wIndex];
	}

	pieces[hIndex][wIndex]->UpdateBounds(leftNeighbor, bottomNeighbor, rightNeighbor, topNeighbor, 
                                         topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, 
                                         bottomLeftNeighbor);
}

void GameLevel::SignalLevelAlmostCompleteEvent() {
    // If we've already signaled it then just exit
    if (this->levelAlmostCompleteSignaled) {
        return;
    }

    if (this->IsLevelAlmostComplete()) {
        // EVENT: The level just hit the 'Almost Complete' mark
        GameEventManager::Instance()->ActionLevelAlmostComplete(*this);
        this->levelAlmostCompleteSignaled = true;
    }
}

/**
 * Call this when a level piece changes in this level. This function is meant to 
 * change the piece and manage the other level pieces accordingly.
 */
void GameLevel::PieceChanged(GameModel* gameModel, LevelPiece* pieceBefore, 
                             LevelPiece* pieceAfter, const LevelPiece::DestructionMethod& method) {
	assert(gameModel != NULL);
	assert(pieceBefore != NULL);
	assert(pieceAfter != NULL);

    // Add whatever number of points are acquired for the piece change to the player's score
    // NOTE: Make sure this is done before incrementing the number of interim
    // blocks destroyed - otherwise the multiplier will be applied before the incremented score!
    PointAward ptAward(pieceBefore->GetPointsOnChange(*pieceAfter), ScoreTypes::UndefinedBonus, pieceBefore->GetCenter());
    ptAward.SetDestructionMethod(method);
    gameModel->IncrementScore(ptAward);

	// EVENT: Level piece has changed...
	GameEventManager::Instance()->ActionLevelPieceChanged(*pieceBefore, *pieceAfter);

	if (pieceBefore != pieceAfter) {
		// Find out if a vital piece was destroyed (i.e., the player is closer to finishing the level)
		bool isVitalBeforeCollision = pieceBefore->MustBeDestoryedToEndLevel();
		bool isVitalAfterCollision	= pieceAfter->MustBeDestoryedToEndLevel();

		// Update the number of pieces that need to be destroyed to end the level
		if (isVitalBeforeCollision && !isVitalAfterCollision) {
			// In this case a block that was vital has been destroyed
			this->piecesLeft--;
			assert(this->piecesLeft >= 0);
		}

		// Replace the old piece with the new one...
		assert(pieceBefore->GetHeightIndex() == pieceAfter->GetHeightIndex());
		assert(pieceBefore->GetWidthIndex()  == pieceAfter->GetWidthIndex());
		unsigned int hIndex = pieceAfter->GetHeightIndex();
		unsigned int wIndex = pieceAfter->GetWidthIndex();
		this->currentLevelPieces[hIndex][wIndex] = pieceAfter;

		// Update the neighbour's bounds...
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex-1);   // left
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex-1, wIndex);   // bottom
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex+1);   // right
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex);   // top
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex-1); // top-left
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex-1, wIndex-1); // bottom-left
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex+1); // top-right
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex+1); // top-right

        // Check to see if the piece is inside the list of triggerables...
        if (pieceBefore->GetHasTriggerID()) {
            // Remove it from the trigger list
            std::map<LevelPiece::TriggerID, LevelPiece*>::iterator findIter =
                this->triggerablePieces.find(pieceBefore->GetTriggerID());
            assert(findIter != this->triggerablePieces.end());
            this->triggerablePieces.erase(findIter);
        }

        // Check to see if the piece is inside the list of AI entities...
        if (pieceBefore->IsAIPiece()) {
            size_t numErased = this->aiEntities.erase(pieceBefore);
            assert(numErased == 1);
            UNUSED_VARIABLE(numErased);
        }

		// If the piece is in any auxillary lists within the game model then we need to remove it
		gameModel->WipePieceFromAuxLists(pieceBefore);

	    // The replaced piece has officially been destroyed, increase the number of destroyed blocks in the model
        if (pieceBefore->GetType() != LevelPiece::Empty && pieceAfter->GetType() == LevelPiece::Empty) {
            gameModel->IncrementNumInterimBlocksDestroyed(pieceBefore->GetCenter());
        }
	}
	else {
		// Inline: in this case there was a change within the piece object itself
		// since both the before and after objects are the same.
	}
}

void GameLevel::UpdateBoundsOnPieceAndSurroundingPieces(LevelPiece* piece) {
	unsigned int hIndex = piece->GetHeightIndex();
	unsigned int wIndex = piece->GetWidthIndex();

	// Update the neighbour's bounds...
    GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex);     // center
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex-1);   // left
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex-1, wIndex);   // bottom
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex+1);   // right
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex);   // top
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex-1); // top-left
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex-1, wIndex-1); // bottom-left
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex+1); // top-right
	GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex+1); // top-right
}

/**
 * When a rocket explodes... a lot of stuff goes boom.
 * If a rocket hits block 'x' then it plus all other blocks 'o' are destroyed 
 * (if they can be destroyed by a ball).
 *                          b
 *                        a o a
 *                        o o o
 *                      b o x o b 
 *                        o o o
 *                        a o a
 *                          b
 * From the above ('o' is less than normal size, add 'a' for normal, add 'b' for bigger) is the normal scenario .
 */
LevelPiece* GameLevel::RocketExplosion(GameModel* gameModel, const RocketProjectile* rocket, LevelPiece* hitPiece) {
	
	// Destroy the hit piece if we can...
	LevelPiece* centerPieceAfterDestruction = hitPiece->Destroy(gameModel, LevelPiece::RocketDestruction);

	// Grab all the pieces that are going to be affected around the central given hit piece
	// NOTE: If a piece doesn't exist (i.e., the bounds of the level are hit then the piece
	// will be populated as NULL and accounted for while iterating through the affected pieces).
	float rocketSizeFactor = rocket->GetHeight() / rocket->GetDefaultHeight();
	
    std::set<LevelPiece*> affectedPieces = this->GetExplosionAffectedLevelPieces(rocket, rocketSizeFactor, centerPieceAfterDestruction);
    std::set<LevelPiece*> ignorePieces;
    bool canChangeLevelOnHit = false;

	// Go through each affected piece and destroy it if we can
	for (std::set<LevelPiece*>::iterator iter = affectedPieces.begin(); iter != affectedPieces.end();) {

		LevelPiece* currAffectedPiece = *iter;
        if (currAffectedPiece != NULL && ignorePieces.find(currAffectedPiece) == ignorePieces.end()) {

			canChangeLevelOnHit = currAffectedPiece->CanChangeSelfOrOtherPiecesWhenHit();
            LevelPiece* resultPiece = currAffectedPiece->Destroy(gameModel, LevelPiece::RocketDestruction);
            ignorePieces.insert(resultPiece);

            if (canChangeLevelOnHit || resultPiece != currAffectedPiece) {
			    
                // Update all the affected pieces again...
			    affectedPieces = this->GetExplosionAffectedLevelPieces(rocket, rocketSizeFactor, centerPieceAfterDestruction);
			    iter = affectedPieces.begin();
                continue;
            }
		}
		++iter;
	}

	// EVENT: Rocket exploded!!
	const PaddleRocketProjectile* rocketProjectile = static_cast<const PaddleRocketProjectile*>(rocket);
	assert(rocketProjectile != NULL);
	GameEventManager::Instance()->ActionRocketExploded(*rocketProjectile);

	return centerPieceAfterDestruction;
}
void GameLevel::RocketExplosionNoPieces(const RocketProjectile* rocket) {
	// EVENT: Rocket exploded!!
	GameEventManager::Instance()->ActionRocketExploded(*rocket);
}


std::set<LevelPiece*> GameLevel::GetExplosionAffectedLevelPieces(const Projectile* projectile, float sizeFactor, LevelPiece* centerPiece) {
    assert(centerPiece != NULL);

    int hIndex = static_cast<int>(centerPiece->GetHeightIndex());
    int wIndex = static_cast<int>(centerPiece->GetWidthIndex());

	std::set<LevelPiece*> affectedPieces;

    LevelPiece* innerCircleTop      = this->GetLevelPieceFromCurrentLayout(hIndex+1, wIndex);
    LevelPiece* innerCircleMidLeft  = this->GetLevelPieceFromCurrentLayout(hIndex, wIndex-1);
    LevelPiece* innerCircleMidRight = this->GetLevelPieceFromCurrentLayout(hIndex, wIndex+1);
    LevelPiece* innerCircleBottom   = this->GetLevelPieceFromCurrentLayout(hIndex-1, wIndex);
    
    // Start with the base pieces that will get destroyed
    affectedPieces.insert(innerCircleTop);
    affectedPieces.insert(innerCircleMidLeft);
    affectedPieces.insert(innerCircleMidRight);
    affectedPieces.insert(innerCircleBottom);
    
    // Mines don't have as large an explosion radius as rockets...
    if (projectile->IsMine() && sizeFactor <= 1.0f) {
        return affectedPieces;
    }

    LevelPiece* innerCircleTopLeft  = this->GetLevelPieceFromCurrentLayout(hIndex+1, wIndex-1);
    LevelPiece* innerCircleTopRight = this->GetLevelPieceFromCurrentLayout(hIndex+1, wIndex+1);
    LevelPiece* innerCircleBottomRight = this->GetLevelPieceFromCurrentLayout(hIndex-1, wIndex+1);
    LevelPiece* innerCircleBottomLeft  = this->GetLevelPieceFromCurrentLayout(hIndex-1, wIndex-1);

    affectedPieces.insert(innerCircleTopRight);
    affectedPieces.insert(innerCircleTopLeft);
    affectedPieces.insert(innerCircleBottomRight);
    affectedPieces.insert(innerCircleBottomLeft);

    const Point2D& explosionCenter = centerPiece->GetCenter();
    bool isCenterStopped = centerPiece->IsExplosionStoppedByPiece(explosionCenter);

    LevelPiece* outerCircleTop = this->GetLevelPieceFromCurrentLayout(hIndex+2, wIndex);
    bool innerCircleTopIsStopped = isCenterStopped || innerCircleTop == NULL || innerCircleTop->IsExplosionStoppedByPiece(explosionCenter);
    if (!innerCircleTopIsStopped) {
        affectedPieces.insert(outerCircleTop);
    }

    LevelPiece* outerCircleBottom = this->GetLevelPieceFromCurrentLayout(hIndex-2, wIndex);
    bool innerCircleBottomIsStopped = isCenterStopped || innerCircleBottom == NULL || 
        innerCircleBottom->IsExplosionStoppedByPiece(explosionCenter);
    if (!innerCircleBottomIsStopped) {
        affectedPieces.insert(outerCircleBottom);
    }

    // Mines don't have as large an explosion as rockets do, in general
    if (projectile->IsMine() && sizeFactor < 1.4) {
        return affectedPieces;
    }

	// If the rocket's at least a normal size then the corner pieces are also destroyed
	if (sizeFactor >= 1.0f) {

        bool innerCircleTopLeftIsStopped = isCenterStopped || innerCircleTopLeft == NULL || 
            innerCircleTopLeft->IsExplosionStoppedByPiece(explosionCenter);
        if (!(innerCircleTopIsStopped && innerCircleTopLeftIsStopped)) {
            affectedPieces.insert(this->GetLevelPieceFromCurrentLayout(hIndex+2, wIndex-1));
        }
        
        bool innerCircleTopRightIsStopped = isCenterStopped || innerCircleTopRight == NULL || 
            innerCircleTopRight->IsExplosionStoppedByPiece(explosionCenter);
        if (!(innerCircleTopIsStopped && innerCircleTopRightIsStopped)) {
            affectedPieces.insert(this->GetLevelPieceFromCurrentLayout(hIndex+2, wIndex+1));
        }

        bool innerCircleBottomLeftIsStopped = isCenterStopped || innerCircleBottomLeft == NULL || 
            innerCircleBottomLeft->IsExplosionStoppedByPiece(explosionCenter);
        if (!(innerCircleBottomIsStopped && innerCircleBottomLeftIsStopped)) {
		    affectedPieces.insert(this->GetLevelPieceFromCurrentLayout(hIndex-2, wIndex-1));
        }

        bool innerCircleBottomRightIsStopped = isCenterStopped || innerCircleBottomRight == NULL || 
            innerCircleBottomRight->IsExplosionStoppedByPiece(explosionCenter);
        if (!(innerCircleBottomIsStopped && innerCircleBottomRightIsStopped)) {
            affectedPieces.insert(this->GetLevelPieceFromCurrentLayout(hIndex-2, wIndex+1));
        }

        
        if (projectile->IsMine()) {
            return affectedPieces;
        }
		
	    // If the explosion will be a bit bigger than usual then more pieces are destroyed as well...
	    if (sizeFactor > 1.0f + EPSILON) {
            
            bool outerCircleTopIsStopped = outerCircleTop == NULL || 
                outerCircleTop->IsExplosionStoppedByPiece(explosionCenter);
            if (!innerCircleTopIsStopped && !outerCircleTopIsStopped) {
                affectedPieces.insert(this->GetLevelPieceFromCurrentLayout(hIndex+3, wIndex));
            }
            
            bool outerCircleBottomIsStopped = outerCircleBottom == NULL || 
                outerCircleBottom->IsExplosionStoppedByPiece(explosionCenter);
            if (!innerCircleBottomIsStopped && !outerCircleBottomIsStopped) {
                affectedPieces.insert(this->GetLevelPieceFromCurrentLayout(hIndex-3, wIndex));
            }

            bool innerCircleMidLeftIsStopped = isCenterStopped || innerCircleMidLeft == NULL || 
                innerCircleMidLeft->IsExplosionStoppedByPiece(explosionCenter);
            if (!innerCircleMidLeftIsStopped) {
                affectedPieces.insert(this->GetLevelPieceFromCurrentLayout(hIndex, wIndex-2));
            }

            bool innerCircleMidRightIsStopped = isCenterStopped || innerCircleMidRight == NULL || 
                innerCircleMidRight->IsExplosionStoppedByPiece(explosionCenter);
            if (!innerCircleMidRightIsStopped) {
                affectedPieces.insert(this->GetLevelPieceFromCurrentLayout(hIndex, wIndex+2));
            }
	    }
    }

	return affectedPieces;
}

LevelPiece* GameLevel::MineExplosion(GameModel* gameModel, const MineProjectile* mine, LevelPiece* hitPiece) {
	// Destroy the hit piece if we can...
	LevelPiece* resultPiece = hitPiece->Destroy(gameModel, LevelPiece::MineDestruction);

    this->MineExplosion(gameModel, mine);

    return resultPiece;
}

void GameLevel::MineExplosion(GameModel* gameModel, const MineProjectile* mine) {
    assert(gameModel != NULL);
    assert(mine != NULL);

    const Point2D& minePosition = mine->GetPosition();
    float mineSizeFactor = mine->GetVisualScaleFactor();

    std::vector<LevelPiece*> closestPieces = this->GetLevelPieceCollisionCandidatesNotMoving(minePosition, mine->GetWidth());
    if (closestPieces.empty()) {
        return;
    }

    LevelPiece* centerPieceAfterDestruction = closestPieces.front()->Destroy(gameModel, LevelPiece::MineDestruction);
    
    std::set<LevelPiece*> affectedPieces =
        this->GetExplosionAffectedLevelPieces(mine, mineSizeFactor, centerPieceAfterDestruction);
    std::set<LevelPiece*> ignorePieces;
    bool canChangeLevelOnHit = false;

	// Go through each affected piece and destroy it if we can
	for (std::set<LevelPiece*>::iterator iter = affectedPieces.begin(); iter != affectedPieces.end(); ) {
		
        LevelPiece* currAffectedPiece = *iter;
        if (currAffectedPiece != NULL && ignorePieces.find(currAffectedPiece) == ignorePieces.end()) {

            canChangeLevelOnHit = currAffectedPiece->CanChangeSelfOrOtherPiecesWhenHit();
			LevelPiece* resultPiece = currAffectedPiece->Destroy(gameModel, LevelPiece::MineDestruction);
            ignorePieces.insert(resultPiece);

            if (canChangeLevelOnHit || resultPiece != currAffectedPiece) {
			    // Update all the affected pieces again...
			    affectedPieces = this->GetExplosionAffectedLevelPieces(mine, mineSizeFactor, centerPieceAfterDestruction);
			    iter = affectedPieces.begin();
                continue;
            }

		}
		++iter;
	}

    // If there's a boss then it needs to be informed of the explosion as well
    if (this->GetHasBoss()) {
        this->boss->MineExplosionOccurred(gameModel, mine);
    }

	// EVENT: Mine exploded!!
	GameEventManager::Instance()->ActionMineExploded(*mine);
}

/**
 * Activates the triggerable level piece with the given trigger ID (if it exists).
 */
void GameLevel::ActivateTriggerableLevelPiece(const LevelPiece::TriggerID& triggerID, GameModel* gameModel) {
    std::map<LevelPiece::TriggerID, LevelPiece*>::iterator findIter = this->triggerablePieces.find(triggerID);
    if (findIter == this->triggerablePieces.end()) {
        return;
    }

    LevelPiece* triggerPiece = findIter->second;
    assert(triggerPiece != NULL);
    triggerPiece->Triggered(gameModel);
}

/**
 * Collides the given boss AABB with this level's simplified boundaries. If there's
 * a collision the function returns true and the correction vector required to move the boss
 * so that it is on the boundary (without colliding) of the level.
 */
bool GameLevel::CollideBossWithLevel(const Collision::AABB2D& bossAABB, Vector2D& correctionVec) const {
    
    // Get the simplified boundaries of the level
    float minXBound = this->GetPaddleMinBound();
    float maxXBound = this->GetPaddleMaxBound();
    float minYBound = LevelPiece::PIECE_HEIGHT;
    float maxYBound = this->GetLevelUnitHeight() - LevelPiece::PIECE_HEIGHT;

    Collision::LineSeg2D leftBoundary(Point2D(minXBound, minYBound), Point2D(minXBound, maxYBound));
    Collision::LineSeg2D rightBoundary(Point2D(maxXBound, minYBound), Point2D(maxXBound, maxYBound));
    Collision::LineSeg2D topBoundary(Point2D(minXBound, maxYBound), Point2D(maxXBound, maxYBound));

    correctionVec[0] = 0.0f;
    correctionVec[1] = 0.0f;

    // Check each of the simplified level boundaries against the bosses' AABB
    bool didCollide = false;
    Point2D collisionPt;
    
    if (Collision::GetCollisionPoint(bossAABB, leftBoundary, collisionPt)) {
        correctionVec[0] += collisionPt[0] - bossAABB.GetMin()[0] - EPSILON;
        didCollide = true;
    }
    else if (Collision::GetCollisionPoint(bossAABB, rightBoundary, collisionPt)) {
        correctionVec[0] += collisionPt[0] - bossAABB.GetMax()[0] - EPSILON;
        didCollide = true;
    }

    if (Collision::GetCollisionPoint(bossAABB, topBoundary, collisionPt)) {
        correctionVec[1] += collisionPt[1] - bossAABB.GetMax()[1] - EPSILON;
        didCollide = true;
    }

    // TODO
    //if (didCollide) {
        // EVENT: The boss hit one of the level's walls
        //GameEventManager::Instance()->ActionBossHitWall();
    //}

    return didCollide;
}

// Tick any active AI entities in this level
void GameLevel::TickAIEntities(double dT, GameModel* gameModel) {
    for (std::set<LevelPiece*>::const_iterator iter = this->aiEntities.begin();
         iter != this->aiEntities.end(); ++iter) {
        LevelPiece* currAIEntity = *iter;
        assert(currAIEntity != NULL);
        currAIEntity->AITick(dT, gameModel);
    }

    // Update any active boss in this level
    if (this->GetHasBoss()) {
        this->boss->Tick(dT, gameModel);
    }
}

/**
 * Private helper function for finding a set of levelpieces within the given range of values
 * indexing along the x and y axis.
 * Return: Set of levelpieces included in the given bounds.
 */
std::set<LevelPiece*> GameLevel::IndexCollisionCandidates(float xIndexMin, float xIndexMax, 
                                                          float yIndexMin, float yIndexMax) const {
	std::set<LevelPiece*> colliders;

	// Check to see if we're completely out of bounds first...
	if (xIndexMin >= static_cast<float>(this->width) || yIndexMin >= static_cast<float>(this->height) ||
		  xIndexMax < 0.0f || yIndexMax < 0.0f) {
		return colliders;
	}

	xIndexMin = std::max<float>(0.0f, xIndexMin);
	yIndexMin = std::max<float>(0.0f, yIndexMin);
	xIndexMax = std::min<float>(static_cast<float>(this->width-1), xIndexMax);
	yIndexMax = std::min<float>(static_cast<float>(this->height-1), yIndexMax);

	assert(xIndexMin <= xIndexMax);
	assert(yIndexMin <= yIndexMax);
	
	for (int x = xIndexMin; x <= xIndexMax; x++) {
		for (int y = yIndexMin; y <= yIndexMax; y++) {
			colliders.insert(this->currentLevelPieces[y][x]);
		}
	}

	return colliders;
}

// Used for sorting (using STL)...
struct PieceAndSqrDist {
    LevelPiece* piece;
    float sqrDist;
    PieceAndSqrDist(LevelPiece* piece, float sqrDist) : piece(piece), sqrDist(sqrDist) {}

};

bool ComparePieceAndSqrDist(const PieceAndSqrDist& e1, const PieceAndSqrDist& e2) {
  return e1.sqrDist < e2.sqrDist;
}

std::vector<LevelPiece*> GameLevel::GetLevelPieceCollisionCandidatesNotMoving(const Point2D& center, float radius) const {

	float xNonAdjustedIndex = center[0] / LevelPiece::PIECE_WIDTH;
	float xIndexMax = floorf(xNonAdjustedIndex + radius); 
	float xIndexMin = floorf(xNonAdjustedIndex - radius);
	
	float yNonAdjustedIndex = center[1] / LevelPiece::PIECE_HEIGHT;
	float yIndexMax = floorf(yNonAdjustedIndex + radius);
	float yIndexMin = floorf(yNonAdjustedIndex - radius);

    std::set<LevelPiece*> candidateSet = this->IndexCollisionCandidates(xIndexMin, xIndexMax, yIndexMin, yIndexMax);
    std::vector<PieceAndSqrDist> tempSortVec;
    tempSortVec.reserve(candidateSet.size());

    for (std::set<LevelPiece*>::const_iterator iter = candidateSet.begin(); iter != candidateSet.end(); ++iter) {
        LevelPiece* currPiece = *iter;
        tempSortVec.push_back(PieceAndSqrDist(currPiece, Point2D::SqDistance(center, currPiece->GetBounds().ClosestPoint(center))));
    }
    
    std::sort(tempSortVec.begin(), tempSortVec.end(), ComparePieceAndSqrDist);
    
    std::vector<LevelPiece*> result;
    for (std::vector<PieceAndSqrDist>::const_iterator iter = tempSortVec.begin(); iter != tempSortVec.end(); ++iter) {
        result.push_back(iter->piece);
    }

    return result;
}

/** 
 * Public function for obtaining the level pieces that may currently be
 * in collision with the given gameball. Note: dT is the time delta that the ball has yet-to-travel in this tick.
 * Returns: array of unique LevelPieces that are possibly colliding with b.
 */
std::vector<LevelPiece*> GameLevel::GetLevelPieceCollisionCandidates(double dT, const Point2D& center, 
                                                                     float radius, float velocityMagnitude) const {

    radius += dT * velocityMagnitude;

	float xNonAdjustedIndex = center[0] / LevelPiece::PIECE_WIDTH;
	float xIndexMax = floorf(xNonAdjustedIndex + radius); 
	float xIndexMin = floorf(xNonAdjustedIndex - radius);
	
	float yNonAdjustedIndex = center[1] / LevelPiece::PIECE_HEIGHT;
	float yIndexMax = floorf(yNonAdjustedIndex + radius);
	float yIndexMin = floorf(yNonAdjustedIndex - radius);

    std::set<LevelPiece*> candidateSet =
        this->IndexCollisionCandidates(xIndexMin, xIndexMax, yIndexMin, yIndexMax);

    std::vector<PieceAndSqrDist> tempSortVec;
    tempSortVec.reserve(candidateSet.size());

    for (std::set<LevelPiece*>::const_iterator iter = candidateSet.begin(); iter != candidateSet.end(); ++iter) {
        LevelPiece* currPiece = *iter;
        tempSortVec.push_back(PieceAndSqrDist(currPiece, Point2D::SqDistance(center, currPiece->GetBounds().ClosestPoint(center))));
    }
    
    std::sort(tempSortVec.begin(), tempSortVec.end(), ComparePieceAndSqrDist);
    
    std::vector<LevelPiece*> result;
    for (std::vector<PieceAndSqrDist>::const_iterator iter = tempSortVec.begin(); iter != tempSortVec.end(); ++iter) {
        result.push_back(iter->piece);
    }

    return result;
}

std::set<LevelPiece*> GameLevel::GetLevelPieceCollisionCandidatesNoSort(const Point2D& center, float radius) const {
	// Get the ball boundry and use it to figure out what levelpieces are relevant
	// Find the non-rounded max and min indices to look at along the x and y axis
	float xNonAdjustedIndex = center[0] / LevelPiece::PIECE_WIDTH;
	float xIndexMax = floorf(xNonAdjustedIndex + radius); 
	float xIndexMin = floorf(xNonAdjustedIndex - radius);
	
	float yNonAdjustedIndex = center[1] / LevelPiece::PIECE_HEIGHT;
	float yIndexMax = floorf(yNonAdjustedIndex + radius);
	float yIndexMin = floorf(yNonAdjustedIndex - radius);

	return this->IndexCollisionCandidates(xIndexMin, xIndexMax, yIndexMin, yIndexMax);
}

/** 
 * Public function for obtaining the level pieces that may currently be
 * in collision with the given projectile.
 * Returns: array of unique LevelPieces that are possibly colliding with p.
 */
std::set<LevelPiece*> GameLevel::GetLevelPieceCollisionCandidates(double dT, const Point2D& center,
                                                                  const BoundingLines& bounds, float velocityMagnitude) const {

    Collision::AABB2D boundsAABB = bounds.GenerateAABBFromLines();
    float radius = std::max<float>(boundsAABB.GetWidth(), boundsAABB.GetHeight()) / 2.0f + dT * velocityMagnitude;

    float xNonAdjustedIndex = center[0] / LevelPiece::PIECE_WIDTH;
    float xIndexMax = floorf(xNonAdjustedIndex + radius); 
    float xIndexMin = floorf(xNonAdjustedIndex - radius);

    float yNonAdjustedIndex = center[1] / LevelPiece::PIECE_HEIGHT;
    float yIndexMax = floorf(yNonAdjustedIndex + radius);
    float yIndexMin = floorf(yNonAdjustedIndex - radius);

	return this->IndexCollisionCandidates(xIndexMin, xIndexMax, yIndexMin, yIndexMax);
}

/**
 * Obtain the level pieces that may currently be colliding with the given paddle.
 * Returns: array of unique LevelPieces that are possibly colliding with p.
 */
std::set<LevelPiece*> GameLevel::GetLevelPieceCollisionCandidates(const PlayerPaddle& p, bool includeAttachedBall) const {
	Collision::AABB2D paddleAABB = p.GetPaddleAABB(includeAttachedBall);
	const Point2D& maxPt = paddleAABB.GetMax();
	const Point2D& minPt = paddleAABB.GetMin();
	
	float minIndexX = floorf(minPt[0] / LevelPiece::PIECE_WIDTH);
	float maxIndexX = ceilf(maxPt[0]  / LevelPiece::PIECE_WIDTH);
	float minIndexY = floorf(minPt[1] / LevelPiece::PIECE_HEIGHT);
	float maxIndexY = ceilf(maxPt[1]  / LevelPiece::PIECE_HEIGHT);
	
	return this->IndexCollisionCandidates(minIndexX, maxIndexX, minIndexY, maxIndexY);
}

void GameLevel::BuildCollisionBoundsCombinationAndMap(const std::vector<LevelPiece*>& pieces,
                                                      std::map<size_t, LevelPiece*>& boundsIdxMap,
                                                      BoundingLines& combinationBounds) {

    combinationBounds.Clear();
    boundsIdxMap.clear();
    
    int idxCount = 0;
    for (int i = 0; i < static_cast<int>(pieces.size()); i++) {
        const BoundingLines& currPieceBounds = pieces[i]->GetBounds();
        combinationBounds.AddBounds(currPieceBounds);

        for (int j = 0; j < static_cast<int>(currPieceBounds.GetNumLines()); j++) { 
            boundsIdxMap.insert(std::make_pair(idxCount, pieces[i]));
            idxCount++;
        }
    }
}

/**
 * Get the first piece in the level to collide with the given ray, if the ray does not collide
 * with any piece in the level then NULL is returned.
 * The given ray is in game world space.
 * A tolerance radius may be given, this will allow for checking around the ray.
 * Returns: The first piece closest to the origin of the given ray that collides with it, NULL
 * if no collision found.
 */
LevelPiece* GameLevel::GetLevelPieceFirstCollider(const Collision::Ray2D& ray,
                                                  const std::set<const LevelPiece*>& ignorePieces,
                                                  float& rayT, float toleranceRadius) const {

	// Step along the ray - not a perfect algorithm but will result in something very reasonable
	// NOTE: if the step size is too large then the ray might skip over entire sections of blocks - BECAREFUL!
	const float STEP_SIZE = 0.5f * std::min<float>(LevelPiece::PIECE_WIDTH, LevelPiece::PIECE_HEIGHT);
	int NUM_STEPS = static_cast<int>(this->levelHypotenuse / STEP_SIZE);

	LevelPiece* returnPiece = NULL;
	Collision::Circle2D toleranceCircle(Point2D(0,0), toleranceRadius);
    Point2D currSamplePoint;

	for (int i = 0; i < NUM_STEPS; i++) {
		currSamplePoint = ray.GetPointAlongRayFromOrigin(i * STEP_SIZE);
		
		// Indices of the sampled level piece can be found using the point...
		std::set<LevelPiece*> collisionCandidates = this->GetLevelPieceCollisionCandidatesNoSort(currSamplePoint, toleranceRadius);
		float minRayT = FLT_MAX;
		for (std::set<LevelPiece*>::iterator iter = collisionCandidates.begin(); iter != collisionCandidates.end(); ++iter) {
			
			LevelPiece* currSamplePiece = *iter;
			assert(currSamplePiece != NULL);

			// Check to see if the piece can be collided with, if so try to collide the ray with
			// the actual block bounds, if there's a collision we get out of here and just return the piece
			if (ignorePieces.find(currSamplePiece) == ignorePieces.end()) {
				if (currSamplePiece->CollisionCheck(ray, rayT)) {
					// Make sure the piece is along the direction of the ray and not behind it
					if (rayT < minRayT) {
						returnPiece = currSamplePiece;
						minRayT = rayT;
					}
				}
				else if (toleranceRadius != 0.0f) {
					toleranceCircle.SetCenter(currSamplePoint);
                    if (currSamplePiece->CollisionCheck(toleranceCircle, ray.GetUnitDirection())) {
						// TODO: Project the center onto the ray...
						minRayT = i * STEP_SIZE;
						returnPiece = currSamplePiece;
					}
				}
			}
		}

		// If we managed to find a suitable piece (with the lowest ray t), then return it
		if (returnPiece != NULL) {
			rayT = minRayT;
			return returnPiece;
		}
	}

	return NULL;
}

void GameLevel::GetLevelPieceColliders(const Collision::Ray2D& ray, const std::set<const LevelPiece*>& ignorePieces,
                                       const std::set<LevelPiece::LevelPieceType>& ignorePieceTypes,
                                       std::list<LevelPiece*>& result, float toleranceRadius) const {
    result.clear();

	// Step along the ray - not a perfect algorithm but will result in something very reasonable
	const float LEVEL_WIDTH	 = this->GetLevelUnitWidth();
	const float LEVEL_HEIGHT = this->GetLevelUnitHeight();
	const float LONGEST_POSSIBLE_RAY = sqrt(LEVEL_WIDTH*LEVEL_WIDTH + LEVEL_HEIGHT*LEVEL_HEIGHT);

	// NOTE: if the step size is too large then the ray might skip over entire sections of blocks - BECAREFUL!
	const float STEP_SIZE = 0.5f * std::min<float>(LevelPiece::PIECE_WIDTH, LevelPiece::PIECE_HEIGHT);
	int NUM_STEPS = static_cast<int>(LONGEST_POSSIBLE_RAY / STEP_SIZE);

	Collision::Circle2D toleranceCircle(Point2D(0,0), toleranceRadius);

    Point2D currSamplePoint;
    float rayT;
	for (int i = 0; i < NUM_STEPS; i++) {
		currSamplePoint = ray.GetPointAlongRayFromOrigin(i * STEP_SIZE);

        // Exit the loop if the ray is out of bounds of all level pieces
        if (currSamplePoint[0] > this->GetLevelUnitWidth() || currSamplePoint[0] < 0.0f ||
            currSamplePoint[1] > this->GetLevelUnitHeight() || currSamplePoint[1] < 0.0f) {
            break;
        }

		// Indices of the sampled level piece can be found using the point...
		std::set<LevelPiece*> collisionCandidates = this->GetLevelPieceCollisionCandidatesNoSort(currSamplePoint, toleranceRadius);
		for (std::set<LevelPiece*>::iterator iter = collisionCandidates.begin(); iter != collisionCandidates.end(); ++iter) {
			
			LevelPiece* currSamplePiece = *iter;
			assert(currSamplePiece != NULL);

			// Check to see if the piece can be collided with, if so try to collide the ray with
			// the actual block bounds, if there's a collision we get out of here and just return the piece
			if (ignorePieces.find(currSamplePiece) == ignorePieces.end() &&
                ignorePieceTypes.find(currSamplePiece->GetType()) == ignorePieceTypes.end()) {

				if (currSamplePiece->CollisionCheck(ray, rayT)) {
					// Make sure the piece is along the direction of the ray and not behind it
					result.push_back(currSamplePiece);
				}
				else if (toleranceRadius != 0.0f) {
					toleranceCircle.SetCenter(currSamplePoint);
                    if (currSamplePiece->CollisionCheck(toleranceCircle, ray.GetUnitDirection())) {
						result.push_back(currSamplePiece);
					}
				}
			}
		}

	}
}

// Add a newly activated lightning barrier for the tesla block
void GameLevel::AddTeslaLightningBarrier(GameModel* gameModel, const TeslaBlock* block1, const TeslaBlock* block2) {
	// Check to see if the barrier already exists, if it does then just exit with no change
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, Collision::LineSeg2D>::iterator findIter =
		this->teslaLightning.find(std::make_pair(block1, block2));
	if (findIter != this->teslaLightning.end()) {
		return;
	}
	findIter = this->teslaLightning.find(std::make_pair(block2, block1));
	if (findIter != this->teslaLightning.end()) {
		return;
	}

	// Build a bounding line for the tesla block lightning arc
	Collision::LineSeg2D teslaBoundry(block1->GetLightningOrigin(), block2->GetLightningOrigin());
	this->teslaLightning.insert(std::make_pair(std::make_pair(block1, block2), teslaBoundry));

	// EVENT: Lightning arc/barrier was just added to the level
	GameEventManager::Instance()->ActionTeslaLightningBarrierSpawned(*block1, *block2);

	// Destroy any destroyable blocks in the arc path...
	if (gameModel != NULL) {
		Collision::Ray2D rayFromBlock(block1->GetCenter(), Vector2D::Normalize(block2->GetCenter() - block1->GetCenter()));
		float rayT;
		LevelPiece* pieceInArc;
		std::set<const LevelPiece*> ignorePieces;
		ignorePieces.insert(block1);

		for (;;) {
			pieceInArc = this->GetLevelPieceFirstCollider(rayFromBlock, ignorePieces, rayT, TeslaBlock::LIGHTNING_ARC_RADIUS);
			// We get out once we've reached the 2nd tesla block (which MUST happen!)
			if (pieceInArc == block2) {
				break;
			}
			assert(pieceInArc != NULL);
			assert(pieceInArc != block1);
			assert(rayT >= 0.0f);

			// Destroy the piece in the lightning arc...
            ignorePieces.insert(pieceInArc->Destroy(gameModel, LevelPiece::TeslaDestruction));
			// Reevaluate the next collider using a new ray from the new ignore piece
			rayFromBlock.SetOrigin(rayFromBlock.GetPointAlongRayFromOrigin(rayT));
		}
	}

}

// Remove any currently active tesla lightning arcs
void GameLevel::RemoveTeslaLightningBarrier(const TeslaBlock* block1, const TeslaBlock* block2) {
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, Collision::LineSeg2D>::iterator findIter =
		this->teslaLightning.find(std::make_pair(block1, block2));
	if (findIter == this->teslaLightning.end()) {
		findIter = this->teslaLightning.find(std::make_pair(block2, block1));

		// The arc have better existed... we shouldn't be calling this otherwise
		assert(findIter != this->teslaLightning.end());
	}

	this->teslaLightning.erase(findIter);

	// EVENT: Lightning arc/barrier was just removed from the level
	GameEventManager::Instance()->ActionTeslaLightningBarrierRemoved(*block1, *block2);
}


bool GameLevel::TeslaLightningCollisionCheck(const GameBall& b, double dT, Vector2D& n, 
											 Collision::LineSeg2D& collisionLine, 
                                             double& timeUntilCollision) const {
	// Fast exit if there's no tesla stuffs
	if (this->teslaLightning.empty()) {
		return false;
	}

    static const int NUM_COLLISON_SAMPLES = 15;

    bool zeroVelocity = (b.GetSpeed() < EPSILON);
    int numCollisionSamples;
    if (zeroVelocity) {
        numCollisionSamples = 1;
    }
    else {
        numCollisionSamples = NUM_COLLISON_SAMPLES;
    }
    
    // Figure out the distance along the vector travelled since the last collision
    // to take each sample at...
    Vector2D sampleIncDist = dT * b.GetVelocity() / static_cast<float>(numCollisionSamples+1);
    double   sampleIncTime = dT / static_cast<double>(numCollisionSamples+1);

    Point2D currSamplePt = b.GetCenterPosition2D() - (dT * b.GetVelocity()) + sampleIncDist;
    double currTimeUntilCollision = dT - sampleIncTime;

    // Keep track of all the indices collided with and the collision point collided at
    Point2D collisionPt;
    bool isCollision = false;

    const TeslaBlock* teslaBlock1 = NULL;
    const TeslaBlock* teslaBlock2 = NULL;

	const Collision::Circle2D& ballBounds = b.GetBounds();
    float fullTimeRadius   = (ballBounds.Radius() + ballBounds.Radius() + (dT * b.GetVelocity()).Magnitude()) / 2.0f;
	float sqfullTimeRadius =  fullTimeRadius * fullTimeRadius;
	
	// Find the first collision between the ball and a tesla lightning arc
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, Collision::LineSeg2D>::const_iterator iter = this->teslaLightning.begin();
	for (; iter != this->teslaLightning.end(); ++iter) {

		const Collision::LineSeg2D& currLineSeg = iter->second;
		float lineToBallCenterSqDist = Collision::SqDistFromPtToLineSeg(currLineSeg, b.GetCenterPosition2D());

		// Collision if the square radius is greater or equal to the sq distance between the line
		// segment and the ball's center
		if (lineToBallCenterSqDist <= sqfullTimeRadius) {
			collisionLine = iter->second;
			isCollision = true;
            teslaBlock1 = iter->first.first;
            teslaBlock2 = iter->first.second;
            break;
        }
    }

	// If there was no collision then just exit
	if (!isCollision) {
		return false;
	}

    // There was a collision...

    // Go through all of the samples starting with the first (which is just a bit off from the previous
    // tick location) and moving towards the circle's current location, when a collision is found we exit
    bool stillFindingACollision = false;
    for (int i = 0; i < numCollisionSamples; i++) {

        if (Collision::GetCollisionPoint(
            Collision::Circle2D(currSamplePt, b.GetBounds().Radius()),
            collisionLine, collisionPt)) {

            stillFindingACollision = true;
		    break;
        }

        currSamplePt = currSamplePt + sampleIncDist;
        currTimeUntilCollision -= sampleIncTime;
    }

    if (!stillFindingACollision) {
        return false;
    }

    timeUntilCollision = currTimeUntilCollision;

	// Figure out what the normal of the collision was
	// and set all the other relevant parameter values
	
	// Get the vector from the line to the ball's center (the center of the ball dT time ago)
	const Point2D previousBallPos = b.GetCenterPosition2D() - dT * b.GetVelocity();
	const Point2D& linePt1        = collisionLine.P1();
	const Point2D& linePt2        = collisionLine.P2();

	Vector2D fromLineToBall = previousBallPos - linePt1;
	if (Vector2D::Dot(fromLineToBall, fromLineToBall) < EPSILON) {
		fromLineToBall = previousBallPos - linePt2;
	}
	assert(Vector2D::Dot(fromLineToBall, fromLineToBall) >= EPSILON);

	// Use the vector from the line to the ball to determine what side of the line
	// the ball is on and what the normal should be for the collision
	Vector2D lineVec = linePt2 - linePt1;
	n[0] = -lineVec[1];
	n[1] =  lineVec[0];
	if (Vector2D::Dot(n, fromLineToBall) < 0) {
		n[0] =  lineVec[1];
		n[1] = -lineVec[0];
	}
	n.Normalize();

	// Change the normal slightly to make the ball reflection a bit random - make it harder on the player...
	// Tend towards changing the normal to make a bigger reflection not a smaller one...
    static const float MIN_ANGLE_FOR_CHANGE_REFLECTION_RADS = Trig::degreesToRadians(60);	// Angle from the normal that's allowable
	static const float MAX_ANGLE_LESS_REFLECTION_DEGS = 15.0f;
	static const float MAX_ANGLE_MORE_REFLECTION_DEGS = 30.0f;
	const Vector2D& ballVelocityDir = b.GetDirection();
	// First make sure the ball velocity direction is reasonably off from the line/ reasonably close to the normal
	// or modification could cause the ball to collide multiple times or worse
    float radiansBetweenNormalAndBall = acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-ballVelocityDir, n))));
	if (radiansBetweenNormalAndBall <= MIN_ANGLE_FOR_CHANGE_REFLECTION_RADS) {
		float rotateDir = NumberFuncs::SignOf(Vector3D::cross(Vector3D(n), Vector3D(ballVelocityDir))[2]);
		if (Randomizer::GetInstance()->RandomUnsignedInt() % 3 == 0) {
			// Rotate in the opposite direction (make the ball not reflect as much)
			rotateDir *= -1;
			n.Rotate(rotateDir * Randomizer::GetInstance()->RandomNumZeroToOne() * MAX_ANGLE_LESS_REFLECTION_DEGS);
		}
		else {
			n.Rotate(rotateDir * Randomizer::GetInstance()->RandomNumZeroToOne() * MAX_ANGLE_MORE_REFLECTION_DEGS);
		}
	}

	// EVENT: Ball hit a tesla lightning arc!
	GameEventManager::Instance()->ActionBallHitTeslaLightningArc(b, *teslaBlock1, *teslaBlock2);

	return true;
}

/**
 * Checks if the given bounding lines collide with any tesla lightning arcs currently active in this level.
 */
bool GameLevel::TeslaLightningCollisionCheck(const BoundingLines& bounds) const {
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, Collision::LineSeg2D>::const_iterator iter = this->teslaLightning.begin();
	for (; iter != this->teslaLightning.end(); ++iter) {
		const Collision::LineSeg2D& currLineSeg = iter->second;
        if (bounds.CollisionCheck(currLineSeg)) {
            return true;
        }
    }
    return false;
}

// Whether or not the given projectile is destroyed by collision with a tesla lightning arc
bool GameLevel::IsDestroyedByTelsaLightning(const Projectile& p) const {
    switch (p.GetType()) {

        case Projectile::BossOrbBulletProjectile:
        case Projectile::BossLaserBulletProjectile:
        case Projectile::PaddleLaserBulletProjectile:
        case Projectile::BallLaserBulletProjectile:
        case Projectile::LaserTurretBulletProjectile:
        case Projectile::FireGlobProjectile:
            return false;
        
        case Projectile::CollateralBlockProjectile:
        case Projectile::PaddleRocketBulletProjectile:
        case Projectile::PaddleRemoteCtrlRocketBulletProjectile:
        case Projectile::RocketTurretBulletProjectile:
        case Projectile::BossRocketBulletProjectile:
        case Projectile::PaddleMineBulletProjectile:
        case Projectile::MineTurretBulletProjectile:
            return true;

        default:
            assert(false);
            break;
    }

    return false;
}

void GameLevel::InitAfterLevelLoad(GameModel* model) {
	// Initialize the tesla lightning barriers already in existance...
	for (size_t h = 0; h < this->currentLevelPieces.size(); h++) {
		for (size_t w = 0; w < this->currentLevelPieces[h].size(); w++) {
			LevelPiece* currPiece = this->currentLevelPieces[h][w];
			if (currPiece->GetType() == LevelPiece::Tesla) {
				TeslaBlock* currTeslaBlk = static_cast<TeslaBlock*>(currPiece);
				assert(currTeslaBlk != NULL);
				std::list<TeslaBlock*> activeConnectedBlks = currTeslaBlk->GetLightningArcTeslaBlocks();
				for (std::list<TeslaBlock*>::const_iterator iter = activeConnectedBlks.begin(); iter != activeConnectedBlks.end(); ++iter) {
					this->AddTeslaLightningBarrier(model, currTeslaBlk, *iter);
				}
			}
		}
	}
}

LevelPiece* GameLevel::GetMinPaddleBoundPiece() const {
    int col = static_cast<int>(this->currentLevelPieces[0].size()) / 2;
    for (; col > 0; col--) {
        const LevelPiece* currPiece = this->currentLevelPieces[0][col];
        if (currPiece->GetType() == LevelPiece::Solid ||
            currPiece->GetType() == LevelPiece::SolidTriangle ||
            currPiece->GetType() == LevelPiece::Prism ||
            currPiece->GetType() == LevelPiece::PrismTriangle ||
            currPiece->GetType() == LevelPiece::Switch) {
            break;
        }
        else if (currPiece->GetType() == LevelPiece::OneWay) {
            const OneWayBlock* oneWayBlock = static_cast<const OneWayBlock*>(currPiece);
            if (oneWayBlock->GetDirType() != OneWayBlock::OneWayLeft) {
                break;
            }
        }
    }
    return this->currentLevelPieces[0][col];
}

LevelPiece* GameLevel::GetMaxPaddleBoundPiece() const {
    int col = static_cast<int>(this->currentLevelPieces[0].size()) / 2;
    for (; col < static_cast<int>(this->currentLevelPieces[0].size())-1; col++) {
        const LevelPiece* currPiece = this->currentLevelPieces[0][col];
        if (currPiece->GetType() == LevelPiece::Solid ||
            currPiece->GetType() == LevelPiece::SolidTriangle ||
            currPiece->GetType() == LevelPiece::Prism ||
            currPiece->GetType() == LevelPiece::PrismTriangle ||
            currPiece->GetType() == LevelPiece::Switch) {
            break;
        }
        else if (currPiece->GetType() == LevelPiece::OneWay) {
            const OneWayBlock* oneWayBlock = static_cast<const OneWayBlock*>(currPiece);
            if (oneWayBlock->GetDirType() != OneWayBlock::OneWayRight) {
                break;
            }
        }
    }

    return this->currentLevelPieces[0][col];
}

float GameLevel::GetPaddleMinBound() const {
	LevelPiece* temp = this->GetMinPaddleBoundPiece();
    if (temp->IsNoBoundsPieceType()) {
        return temp->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH;
    }
    else {
	    return temp->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH;
    }
}

float GameLevel::GetPaddleMaxBound() const {
    LevelPiece* temp = this->GetMaxPaddleBoundPiece();

    if (temp->IsNoBoundsPieceType()) {
        return temp->GetCenter()[0] + LevelPiece::HALF_PIECE_WIDTH;
    }
    else {
	    return temp->GetCenter()[0] - LevelPiece::HALF_PIECE_WIDTH;
    }
}
