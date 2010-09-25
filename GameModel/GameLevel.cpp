/**
 * GameLevel.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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

#include "PaddleRocketProjectile.h"

#include "../BlammoEngine/StringHelper.h"

#include "../ResourceManager.h"

const char GameLevel::EMPTY_SPACE_CHAR				= 'E';
const char GameLevel::SOLID_BLOCK_CHAR				= 'S';
const char GameLevel::GREEN_BREAKABLE_CHAR		= 'G';
const char GameLevel::YELLOW_BREAKABLE_CHAR		= 'Y';
const char GameLevel::ORANGE_BREAKABLE_CHAR		= 'O';
const char GameLevel::RED_BREAKABLE_CHAR			= 'R';
const char GameLevel::BOMB_CHAR								= 'B';
const char GameLevel::INKBLOCK_CHAR						= 'I';
const char GameLevel::PRISM_BLOCK_CHAR				= 'P';
const char GameLevel::PORTAL_BLOCK_CHAR				= 'X';
const char GameLevel::CANNON_BLOCK_CHAR				= 'C';
const char GameLevel::COLLATERAL_BLOCK_CHAR		= 'L';
const char GameLevel::TESLA_BLOCK_CHAR        = 'A';
const char GameLevel::ITEM_DROP_BLOCK_CHAR		= 'D';

const char GameLevel::TRIANGLE_BLOCK_CHAR	= 'T';
const char GameLevel::TRI_UPPER_CORNER		= 'u';
const char GameLevel::TRI_LOWER_CORNER		= 'l';
const char GameLevel::TRI_LEFT_CORNER			= 'l';
const char GameLevel::TRI_RIGHT_CORNER		= 'r';

const char* GameLevel::ALL_ITEM_TYPES_KEYWORD						= "all";
const char* GameLevel::POWERUP_ITEM_TYPES_KEYWORD				= "powerups";
const char* GameLevel::POWERNEUTRAL_ITEM_TYPES_KEYWORD	= "powerneutrals";
const char* GameLevel::POWERDOWN_ITEM_TYPES_KEYWORD			= "powerdowns";

// Private constructor, requires all the pieces that make up the level
GameLevel::GameLevel(const std::string& filepath, const std::string& levelName, unsigned int numBlocks, std::vector<std::vector<LevelPiece*> > pieces): currentLevelPieces(pieces),
piecesLeft(numBlocks), ballSafetyNetActive(false), filepath(filepath), levelName(levelName) {
	assert(!filepath.empty());
	assert(pieces.size() > 0);
	
	// Set the dimensions of the level
	this->width = pieces[0].size();
	this->height = pieces.size();

	// Create the safety net bounding line for this level
	std::vector<Collision::LineSeg2D> lines;
	lines.reserve(1);
	Collision::LineSeg2D safetyNetLine(Point2D(0.0f, -LevelPiece::HALF_PIECE_HEIGHT), Point2D(this->GetLevelUnitWidth(), -LevelPiece::HALF_PIECE_HEIGHT));
	lines.push_back(safetyNetLine);

	std::vector<Vector2D> normals;
	normals.reserve(1);
	normals.push_back(Vector2D(0, 1));
	
	this->safetyNetBounds = BoundingLines(lines, normals);

	// Initialize the tesla lightning barriers already in existance...
	for (size_t h = 0; h < this->currentLevelPieces.size(); h++) {
		for (size_t w = 0; w < this->currentLevelPieces[h].size(); w++) {
			LevelPiece* currPiece = this->currentLevelPieces[h][w];
			if (currPiece->GetType() == LevelPiece::Tesla) {
				TeslaBlock* currTeslaBlk = dynamic_cast<TeslaBlock*>(currPiece);
				assert(currTeslaBlk != NULL);
				std::list<TeslaBlock*> activeConnectedBlks = currTeslaBlk->GetLightningArcTeslaBlocks();
				for (std::list<TeslaBlock*>::const_iterator iter = activeConnectedBlks.begin(); iter != activeConnectedBlks.end(); ++iter) {
					this->AddTeslaLightningBarrier(NULL, currTeslaBlk, *iter);
				}
			}
		}
	}

	// Set the quad tree for the level
	//Point2D levelMax(this->GetLevelUnitWidth(), this->GetLevelUnitHeight());
	//this->levelTree = new QuadTree(Collision::AABB2D(Point2D(0, 0), levelMax), Vector2D(LevelPiece::PIECE_WIDTH, LevelPiece::PIECE_HEIGHT));
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
}

GameLevel* GameLevel::CreateGameLevelFromFile(std::string filepath) {
	std::istringstream* inFile = ResourceManager::GetInstance()->FilepathToInStream(filepath);
	if (inFile == NULL) {
		assert(false);
		return NULL;
	}

	// Read in the level name
	std::string levelName("");
	while (levelName.empty()) {
		if (!std::getline(*inFile, levelName)) {
			debug_output("ERROR: Error reading in level name for file: " << filepath);
			delete inFile;
			inFile = NULL;
			return NULL;
		}
		levelName = stringhelper::trim(levelName);
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
				case CANNON_BLOCK_CHAR:
					newPiece = new CannonBlock(pieceWLoc, pieceHLoc);
					break;
				case COLLATERAL_BLOCK_CHAR:
					newPiece = new CollateralBlock(pieceWLoc, pieceHLoc);
					break;

				case TESLA_BLOCK_CHAR: {
						// A([1|0], a, B) - Tesla Block (When active with another tesla block, forms an arc of lightning between the two)
					  // [1|0]: A '1' or a '0' to indicated whether it starts on (1) or off (0).
						// a : The single character name of this tesla block.
						// B : One or more other named tesla blocks seperated by commas.

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
							debug_output("ERROR: poorly formed tesla block syntax, missing ',' after the name.");
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
						while (true) {
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
								siblingTeslaBlock = new TeslaBlock(false, 0, 0);
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
							currentTeslaBlock = new TeslaBlock(startsOn, pieceWLoc, pieceHLoc);
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
							currentPortalBlock->SetColour(portalBlockColour);
							siblingPortalBlock->SetColour(portalBlockColour);
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

						TriangleBlock::Orientation orientation;
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

						// Read in the closing bracket
						*inFile >> tempChar;
						if (tempChar != ')') {
							debug_output("ERROR: poorly formed triangle block syntax, missing ')'");
							break;
						}
					}
					break;

				case GameLevel::ITEM_DROP_BLOCK_CHAR: {
						// D(i0, i1, i2, ...) - Item drop block:
						// i0, i1, i2, ... : The names of all the item types that the block is allowed to drop, names can be 
						//                   repeated in order to make higher probabilities of drops. Also, there are several special key
						//                   words that can be used: (powerups, powerdowns, powerneutrals), which cause the block to drop 
						//                   all items of those respective designations.

						char tempChar;
						*inFile >> tempChar;
						if (tempChar != '(') {
							debug_output("ERROR: poorly formed item drop block syntax, missing '('");
							break;
						}		

						// Read each of the item type names and set the proper possible item drops for the item drop block...
						std::string tempStr;
						*inFile >> tempStr;
						if (tempStr.at(tempStr.size()-1) != ')') {
							debug_output("ERROR: poorly formed item drop block syntax, missing ')'");
							break;
						}

						std::vector<std::string> itemNames;
						stringhelper::Tokenize(tempStr, itemNames, ",) \t\r\n"); 
						
						// Make sure there's at least one item defined
						if (itemNames.empty()) {
							debug_output("ERROR: poorly formed item drop block syntax, no item drop types defined");
							break;
						}

						// Figure out the item types from the names...
						bool success = true;
						std::vector<GameItem::ItemType> itemTypes;
						
						for (std::vector<std::string>::const_iterator iter = itemNames.begin(); iter != itemNames.end(); ++iter) {
							std::string currItemName = stringhelper::trim(*iter);

							// There are some special keywords that we need to check for first...
							if (currItemName.compare(ALL_ITEM_TYPES_KEYWORD) == 0) {
								// Add all item types...
								const std::set<GameItem::ItemType>& allItemTypes = GameItemFactory::GetInstance()->GetAllItemTypes();
								itemTypes.insert(itemTypes.end(), allItemTypes.begin(), allItemTypes.end());
							}
							else if (currItemName.compare(POWERUP_ITEM_TYPES_KEYWORD) == 0) {
								// Add all power-up item types
								const std::set<GameItem::ItemType>& powerUpItemTypes = GameItemFactory::GetInstance()->GetPowerUpItemTypes();
								itemTypes.insert(itemTypes.end(), powerUpItemTypes.begin(), powerUpItemTypes.end());
							}
							else if (currItemName.compare(POWERNEUTRAL_ITEM_TYPES_KEYWORD) == 0) {
								// Add all power-neutral item types
								const std::set<GameItem::ItemType>& powerNeutralItemTypes = GameItemFactory::GetInstance()->GetPowerNeutralItemTypes();
								itemTypes.insert(itemTypes.end(), powerNeutralItemTypes.begin(), powerNeutralItemTypes.end());
							}
							else if (currItemName.compare(POWERDOWN_ITEM_TYPES_KEYWORD) == 0) {
								// Add all power-up item types
								const std::set<GameItem::ItemType>& powerDownItemTypes = GameItemFactory::GetInstance()->GetPowerDownItemTypes();
								itemTypes.insert(itemTypes.end(), powerDownItemTypes.begin(), powerDownItemTypes.end());
							}
							else {
								// Check for manual entry of a specific item type name...
								bool isValid = GameItemFactory::GetInstance()->IsValidItemTypeName(currItemName);
								if (!isValid) {
									debug_output("ERROR: poorly formed item drop block syntax, no item drop type found for item \"" + currItemName + "\"");
									success = false;
									continue;
								}
								
								itemTypes.push_back(GameItemFactory::GetInstance()->GetItemTypeFromName(currItemName));
							}
						}
						if (!success) {
							break;
						}

						newPiece = new ItemDropBlock(itemTypes, pieceWLoc, pieceHLoc);
					}
					break;

				default:
					debug_output("ERROR: Invalid level interior value: " << currBlock << " at width = " << pieceWLoc << ", height = " << pieceHLoc);
					delete inFile;
					inFile = NULL;

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

					return NULL;
			}
			assert(newPiece != NULL);
			currentRowPieces.push_back(newPiece);
	
			if (newPiece->MustBeDestoryedToEndLevel()) {
				numVitalPieces++;
			}
		}

		levelPieces.insert(levelPieces.begin(), currentRowPieces);
	}

	delete inFile;
	inFile = NULL;

	// Go through all of the portal blocks and make sure they loaded properly...
	for (std::map<char, PortalBlock*>::iterator iter = portalBlocks.begin(); iter != portalBlocks.end(); ++iter) {
		if (iter->second == NULL || iter->second->GetSiblingPortal() == NULL || iter->second->GetWidthIndex() < 0
				|| iter->second->GetHeightIndex() < 0) {
			debug_output("ERROR: Poorly formatted portal blocks.");
			return NULL;
		}
	}

	// Go through all the pieces and initialize their bounding values appropriately
	for (size_t h = 0; h < levelPieces.size(); h++) {
		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			GameLevel::UpdatePiece(levelPieces, h, w);
		}
	}

	return new GameLevel(filepath, levelName, numVitalPieces, levelPieces);
}

/**
 * Updates the level piece at the given index.
 */
void GameLevel::UpdatePiece(const std::vector<std::vector<LevelPiece*> >& pieces, size_t hIndex, size_t wIndex) {
	
	// Make sure the provided indices are in the correct range
	if (wIndex < 0 || wIndex >= pieces[0].size() || hIndex <0 || hIndex >= pieces.size()) {
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

	if (wIndex != pieces[wIndex].size()-1) {
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
																			 topRightNeighbor, topLeftNeighbor, bottomRightNeighbor, bottomLeftNeighbor);
}

/**
 * Call this when a level piece changes in this level. This function is meant to 
 * change the piece and manage the other level pieces accordingly.
 */
void GameLevel::PieceChanged(LevelPiece* pieceBefore, LevelPiece* pieceAfter) {
	assert(pieceBefore != NULL);
	assert(pieceAfter != NULL);

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
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex-1); // left
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex-1, wIndex); // bottom
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex, wIndex+1); // right
		GameLevel::UpdatePiece(this->currentLevelPieces, hIndex+1, wIndex); // top
	}
	else {
		// Inline: in this case there was a change within the piece object itself
		// since both the before and after objects are the same.
	}
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
LevelPiece* GameLevel::RocketExplosion(GameModel* gameModel, const Projectile* rocket, LevelPiece* hitPiece) {
	// Destroy the hit piece if we can...
	LevelPiece* resultPiece = hitPiece;
	if (hitPiece->CanBeDestroyedByBall()) {
		resultPiece = hitPiece->Destroy(gameModel);
	}

	// Grab all the pieces that are going to be affected around the central given hit piece
	// NOTE: If a piece doesn't exist (i.e., the bounds of the level are hit then the piece
	// will be populated as NULL and accounted for while iterating through the affected pieces).
	unsigned int hIndex = resultPiece->GetHeightIndex();
	unsigned int wIndex = resultPiece->GetWidthIndex();

	float rocketSizeFactor = rocket->GetHeight() / PaddleRocketProjectile::PADDLEROCKET_HEIGHT_DEFAULT;

	std::vector<LevelPiece*> affectedPieces = this->GetRocketExplosionAffectedLevelPieces(rocketSizeFactor, hIndex, wIndex);
	// Go through each affected piece and destroy it if we can
	for (std::vector<LevelPiece*>::iterator iter = affectedPieces.begin(); iter != affectedPieces.end(); ) {
		LevelPiece* currAffectedPiece = *iter;
		if (currAffectedPiece != NULL && currAffectedPiece->CanBeDestroyedByBall()) {
			currAffectedPiece->Destroy(gameModel);
			// Update all the affected pieces again...
			affectedPieces = this->GetRocketExplosionAffectedLevelPieces(rocketSizeFactor, hIndex, wIndex);
			iter = affectedPieces.begin();
			continue;
		}
		++iter;
	}

	return resultPiece;
}

std::vector<LevelPiece*> GameLevel::GetRocketExplosionAffectedLevelPieces(float rocketSizeFactor, size_t hIndex, size_t wIndex) {
	std::vector<LevelPiece*> affectedPieces;
	affectedPieces.reserve(18);

	// Start with the base pieces that will get destroyed
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex+2, wIndex));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex+1, wIndex-1));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex+1, wIndex));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex+1, wIndex+1));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex, wIndex-1));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex, wIndex+1));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex-1, wIndex-1));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex-1, wIndex));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex-1, wIndex+1));
	affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex-2, wIndex));

	// If the rocket's at least a normal size then the corner pieces are also destroyed
	if (rocketSizeFactor >= 1.0f) {
		affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex+2, wIndex-1));
		affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex+2, wIndex+1));
		affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex-2, wIndex-1));
		affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex-2, wIndex+1));
	}

	// If the rocket's a bit bigger than usual then more pieces are destroyed as well...
	if (rocketSizeFactor > 1.0f + EPSILON) {
		affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex-3, wIndex));
		affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex+3, wIndex));
		affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex, wIndex+2));
		affectedPieces.push_back(this->GetLevelPieceFromCurrentLayout(hIndex, wIndex-2));
	}

	return affectedPieces;
}

/**
 * Private helper function for finding a set of levelpieces within the given range of values
 * indexing along the x and y axis.
 * Return: Set of levelpieces included in the given bounds.
 */
std::set<LevelPiece*> GameLevel::IndexCollisionCandidates(float xIndexMin, float xIndexMax, float yIndexMin, float yIndexMax) const {
	std::set<LevelPiece*> colliders;

	// Do some correction of x-axis index values if the ball goes out of bounds...
	if (xIndexMin < 0 || xIndexMin >= static_cast<float>(this->width)) {
		if (xIndexMax >= static_cast<float>(this->width) || xIndexMax < 0) {
			return colliders;
		}
		xIndexMin = xIndexMax;
	}
	else if (xIndexMax >= static_cast<float>(this->width) || xIndexMax < 0) {
		xIndexMax = xIndexMin;
	}

	// Do some correction of y-axis index values if the ball goes out of bounds...
	if (yIndexMin < 0 || yIndexMin >= static_cast<float>(this->height)) {
		if (yIndexMax >= static_cast<float>(this->height) || yIndexMax < 0) {
			return colliders;
		}
		yIndexMin = yIndexMax;
	}
	else if (yIndexMax >= static_cast<float>(this->height) || yIndexMax < 0) {
		yIndexMax = yIndexMin;
	}
	
	assert(xIndexMin <= xIndexMax);
	assert(yIndexMin <= yIndexMax);
	
	for (int x = xIndexMin; x <= xIndexMax; x++) {
		for (int y = yIndexMin; y <= yIndexMax; y++) {
			colliders.insert(this->currentLevelPieces[y][x]);
		}
	}

	return colliders;
}

/** 
 * Public function for obtaining the level pieces that may currently be
 * in collision with the given gameball.
 * Returns: array of unique LevelPieces that are possibly colliding with b.
 */
std::set<LevelPiece*> GameLevel::GetLevelPieceCollisionCandidates(const Point2D& center, float radius) const {

	// Get the ball boundry and use it to figure out what levelpieces are relevant
	// Find the non-rounded max and min indices to look at along the x and y axis
	float xNonAdjustedIndex = center[0] / LevelPiece::PIECE_WIDTH;
	int xIndexMax = static_cast<int>(floorf(xNonAdjustedIndex + radius)); 
	int xIndexMin = static_cast<int>(floorf(xNonAdjustedIndex - radius));
	
	float yNonAdjustedIndex = center[1] / LevelPiece::PIECE_HEIGHT;
	int yIndexMax = static_cast<int>(floorf(yNonAdjustedIndex + radius));
	int yIndexMin = static_cast<int>(floorf(yNonAdjustedIndex - radius));

	return this->IndexCollisionCandidates(xIndexMin, xIndexMax, yIndexMin, yIndexMax);
}

/** 
 * Public function for obtaining the level pieces that may currently be
 * in collision with the given projectile.
 * Returns: array of unique LevelPieces that are possibly colliding with p.
 */
std::set<LevelPiece*> GameLevel::GetLevelPieceCollisionCandidates(const Projectile& p) const {
	Point2D projectileCenter = p.GetPosition();

	// Find the non-rounded max and min indices to look at along the x and y axis
	float xDelta = p.GetHalfWidth() * fabs(p.GetRightVectorDirection()[0]) + p.GetHalfHeight() * fabs(p.GetVelocityDirection()[0]);
	int xIndexMax = static_cast<int>(floorf((projectileCenter[0] + xDelta) / LevelPiece::PIECE_WIDTH)); 
	int xIndexMin = static_cast<int>(floorf((projectileCenter[0] - xDelta) / LevelPiece::PIECE_WIDTH));
	
	float yDelta = p.GetHalfWidth() * fabs(p.GetRightVectorDirection()[1]) + p.GetHalfHeight() * fabs(p.GetVelocityDirection()[1]);
	int yIndexMax = static_cast<int>(floorf((projectileCenter[1] + yDelta) / LevelPiece::PIECE_HEIGHT));
	int yIndexMin = static_cast<int>(floorf((projectileCenter[1] - yDelta) / LevelPiece::PIECE_HEIGHT));

	return this->IndexCollisionCandidates(xIndexMin, xIndexMax, yIndexMin, yIndexMax);
}

/**
 * Get the first piece in the level to collide with the given ray, if the ray does not collide
 * with any piece in the level then NULL is returned.
 * The given ray is in game world space.
 * A tolerance radius may be given, this will allow for checking around the ray.
 * Returns: The first piece closest to the origin of the given ray that collides with it, NULL
 * if no collision found.
 */
LevelPiece* GameLevel::GetLevelPieceFirstCollider(const Collision::Ray2D& ray, std::set<const LevelPiece*> ignorePieces, float& rayT, float toleranceRadius) const {
	// Step along the ray - not a perfect algorithm but will result in something very reasonable
	const float LEVEL_WIDTH					 = this->GetLevelUnitWidth();
	const float LEVEL_HEIGHT				 = this->GetLevelUnitHeight();
	const float LONGEST_POSSIBLE_RAY = sqrt(LEVEL_WIDTH*LEVEL_WIDTH + LEVEL_HEIGHT*LEVEL_HEIGHT);

	// NOTE: if the step size is too large then the ray might skip over entire sections of blocks - BECAREFUL!
	const float STEP_SIZE = 0.5f * std::min<float>(LevelPiece::PIECE_WIDTH, LevelPiece::PIECE_HEIGHT);
	int NUM_STEPS = static_cast<int>(LONGEST_POSSIBLE_RAY / STEP_SIZE);

	LevelPiece* returnPiece = NULL;
	Collision::Circle2D toleranceCircle(Point2D(0,0), toleranceRadius);

	for (int i = 0; i < NUM_STEPS; i++) {
		Point2D currSamplePoint = ray.GetPointAlongRayFromOrigin(i * STEP_SIZE);
		

		// Indices of the sampled level piece can be found using the point...
		std::set<LevelPiece*> collisionCandidates = this->GetLevelPieceCollisionCandidates(currSamplePoint, toleranceRadius);
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
					if (currSamplePiece->CollisionCheck(toleranceCircle)) {
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

/**
 * Do a collision check with the ball safety net if it's active.
 * Returns: true on collision (when active) and the normal and distance values, false otherwise.
 */
bool GameLevel::BallSafetyNetCollisionCheck(const GameBall& b, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) {
	// Fast exit if there's no safety net active
	if (!this->ballSafetyNetActive){ 
		return false;
	}

	// Test for collision, if there was one then we kill the safety net
	bool didCollide = this->safetyNetBounds.Collide(dT, b.GetBounds(), b.GetVelocity(), n, collisionLine, timeSinceCollision);
	if (didCollide) {
		this->ballSafetyNetActive = false;
		GameEventManager::Instance()->ActionBallSafetyNetDestroyed(b);
	}

	return didCollide;
}

bool GameLevel::PaddleSafetyNetCollisionCheck(const PlayerPaddle& p) {
	// Fast exit if there's no safety net active
	if (!this->ballSafetyNetActive){ 
		return false;
	}
	
	// Test for collision, if there was one then we kill the safety net
	bool didCollide = p.CollisionCheck(this->safetyNetBounds);
	if (didCollide) {
		this->ballSafetyNetActive = false;
		GameEventManager::Instance()->ActionBallSafetyNetDestroyed(p);
	}

	return didCollide;
}

// Add a newly activated lightning barrier for the tesla block
void GameLevel::AddTeslaLightningBarrier(GameModel* gameModel, const TeslaBlock* block1, const TeslaBlock* block2) {
	assert(gameModel != NULL);

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

		while (true) {
			pieceInArc = this->GetLevelPieceFirstCollider(rayFromBlock, ignorePieces, rayT, TeslaBlock::LIGHTNING_ARC_RADIUS);
			// We get out once we've reached the 2nd tesla block (which MUST happen!)
			if (pieceInArc == block2) {
				break;
			}
			assert(pieceInArc != NULL);
			assert(pieceInArc != block1);
			assert(rayT >= 0.0f);

			// Destroy the piece in the lightning arc...
			ignorePieces.insert(pieceInArc->Destroy(gameModel));
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
																						 Collision::LineSeg2D& collisionLine, double& timeSinceCollision) const {
	// Fast exit if there's no tesla stuffs
	if (this->teslaLightning.empty()) {
		return false;
	}

	const Point2D& currentBallPos	= b.GetCenterPosition2D();
	const Collision::Circle2D& ballBounds = b.GetBounds();
	float sqBallRadius = ballBounds.Radius() * ballBounds.Radius();
	
	// Find the first collision between the ball and a tesla lightning arc
	float lineToBallCenterSqDist = FLT_MAX;
	bool isCollision = false;
	std::map<std::pair<const TeslaBlock*, const TeslaBlock*>, Collision::LineSeg2D>::const_iterator iter = this->teslaLightning.begin();
	for (; iter != this->teslaLightning.end(); ++iter) {

		const Collision::LineSeg2D& currLineSeg = iter->second;
		lineToBallCenterSqDist = Collision::SqDistFromPtToLineSeg(currLineSeg, currentBallPos);

		// Collision if the square radius is greater or equal to the sq distance between the line
		// segment and the ball's center
		if (lineToBallCenterSqDist <= sqBallRadius) {
			collisionLine = iter->second;
			isCollision = true;
			break;
		}
	}

	// If there was no collision then just exit
	if (!isCollision) {
		return false;
	}

	// inline: There was a collision, figure out what the normal of the collision was
	// and set all the other relevant parameter values
	
	// Get the vector from the line to the ball's center (the center of the ball dT time ago)
	const Point2D previousBallPos = currentBallPos - dT * b.GetVelocity();
	const Point2D& linePt1			  = collisionLine.P1();
	const Point2D& linePt2				= collisionLine.P2();

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

	// Now we need to calculate the time since the collision
	double collisionOverlapDist = fabs(ballBounds.Radius() - sqrt(lineToBallCenterSqDist));
	timeSinceCollision = collisionOverlapDist / b.GetSpeed();
	assert(timeSinceCollision >= 0.0);

	// Change the normal slightly to make the ball reflection a bit random - make it harder on the player...
	// Tend towards changing the normal to make a bigger reflection not a smaller one...
	static const float MIN_ANGLE_FOR_CHANGE_REFLECTION_RADS = static_cast<float>(M_PI / 4.0);	// Angle from the normal that's allowable
	static const float MAX_ANGLE_LESS_REFLECTION_DEGS = 15.0f;
	static const float MAX_ANGLE_MORE_REFLECTION_DEGS = 30.0f;
	const Vector2D& ballVelocityDir = b.GetDirection();
	// First make sure the ball velocity direction is reasonably off from the line/ reasonably close to the normal
	// or modification could cause the ball to collide multiple times or worse
	if (acos(std::min<float>(1.0f, std::max<float>(-1.0f, Vector2D::Dot(-ballVelocityDir, n)))) <= MIN_ANGLE_FOR_CHANGE_REFLECTION_RADS) {
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
	GameEventManager::Instance()->ActionBallHitTeslaLightningArc(b, *iter->first.first, *iter->first.second);

	return true;
}