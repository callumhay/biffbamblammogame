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

#include "GameEventManager.h"
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

const char GameLevel::TRIANGLE_BLOCK_CHAR	= 'T';
const char GameLevel::TRI_UPPER_CORNER		= 'u';
const char GameLevel::TRI_LOWER_CORNER		= 'l';
const char GameLevel::TRI_LEFT_CORNER			= 'l';
const char GameLevel::TRI_RIGHT_CORNER		= 'r';

// Private constructor, requires all the pieces that make up the level
GameLevel::GameLevel(unsigned int numBlocks, std::vector<std::vector<LevelPiece*>> pieces): currentLevelPieces(pieces),
piecesLeft(numBlocks), ballSafetyNetActive(false) {
	assert(pieces.size() > 0);
	
	// Set the dimensions of the level
	this->width = pieces[0].size();
	this->height = pieces.size();

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

	// Clean up the quad tree
	//delete this->levelTree;
	//this->levelTree = NULL;
}

GameLevel* GameLevel::CreateGameLevelFromFile(std::string filepath) {
	
#ifdef _DEBUG
	// In debug mode we read directly off disk and not out of the resource manager
	std::stringstream* inFile = NULL;
	{
		std::ifstream tempFileStream(filepath.c_str());
		inFile = new std::stringstream();
		*inFile << tempFileStream.rdbuf();
	}
#else
	std::istringstream* inFile = ResourceManager::GetInstance()->FilepathToInStream(filepath);
#endif
	if (inFile == NULL) {
		assert(false);
		return NULL;
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

	std::vector<std::vector<LevelPiece*>> levelPieces;
	unsigned int numVitalPieces = 0;

	// Keep track of named portal blocks...
	std::map<char, PortalBlock*> portalBlocks;
	// Reset the colours for the portal blocks
	PortalBlock::ResetPortalColourGenerator();

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

						*inFile >> tempChar;
						if (tempChar != ',') {
							debug_output("ERROR: poorly formed portal block syntax, missing ','");
							break;
						}				

						char siblingName;
						*inFile >> siblingName;

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
						}
						findIter = portalBlocks.find(siblingName);
						if (findIter != portalBlocks.end()) {
							siblingPortalBlock = findIter->second;
						}

						if (siblingPortalBlock == NULL) {
							// No sibling exists yet, create one
							siblingPortalBlock = new PortalBlock(-1, -1, NULL);
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
							Colour portalBlockColour = PortalBlock::GeneratePortalColour();
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
				default:
					debug_output("ERROR: Invalid level interior value: " << currBlock << " at width = " << pieceWLoc << ", height = " << pieceHLoc);
					delete inFile;
					inFile = NULL;
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
			GameLevel* temp = new GameLevel(numVitalPieces, levelPieces);
			delete temp;
			temp = NULL;
			return NULL;
		}
	}

	// Go through all the pieces and initialize their bounding values appropriately
	for (size_t h = 0; h < levelPieces.size(); h++) {
		for (size_t w = 0; w < levelPieces[h].size(); w++) {
			GameLevel::UpdatePiece(levelPieces, h, w);
		}
	}

	return new GameLevel(numVitalPieces, levelPieces);
}

/**
 * Updates the level piece at the given index.
 */
void GameLevel::UpdatePiece(const std::vector<std::vector<LevelPiece*>>& pieces, size_t hIndex, size_t wIndex) {
	
	// Make sure the provided indices are in the correct range
	if (wIndex < 0 || wIndex >= pieces[0].size() || hIndex <0 || hIndex >= pieces.size()) {
		return;
	}
	
	LevelPiece* leftNeighbor				= NULL;
	LevelPiece* bottomNeighbor			= NULL;
	LevelPiece* rightNeighbor				= NULL;
	LevelPiece* topNeighbor					= NULL;
	LevelPiece* topLeftNeighbor			= NULL;
	LevelPiece* topRightNeighbor		= NULL;
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
	
	if (xIndexMax == xIndexMin) {
		if (yIndexMax == yIndexMin) {
			// Only one collision point
			colliders.insert(this->currentLevelPieces[yIndexMin][xIndexMin]);
		}
		else {
			// Some number of collisions along the y-axis
			for (int y = yIndexMin; y <= yIndexMax; y++) {
				colliders.insert(this->currentLevelPieces[y][xIndexMin]);
			}
		}
	}
	else {
		// No matter what there are some number of collisions along the x-axis
		for (int x = xIndexMin; x <= xIndexMax; x++) {
			colliders.insert(this->currentLevelPieces[yIndexMin][x]);
		}

		if (yIndexMax != yIndexMin) {
			// Add the left overs...
			for (int x = xIndexMin; x <= xIndexMax; x++) {
				colliders.insert(this->currentLevelPieces[yIndexMax][x]);
			}
		}
	}

	return colliders;
}

/** 
 * Public function for obtaining the level pieces that may currently be
 * in collision with the given gameball.
 * Returns: array of unique LevelPieces that are possibly colliding with b.
 */
std::set<LevelPiece*> GameLevel::GetLevelPieceCollisionCandidates(const GameBall& b) const {

	// Get the ball boundry and use it to figure out what levelpieces are relevant
	Collision::Circle2D ballBounds = b.GetBounds();
	Point2D ballCenter = ballBounds.Center();

	// Find the non-rounded max and min indices to look at along the x and y axis
	float xNonAdjustedIndex = ballCenter[0] / LevelPiece::PIECE_WIDTH;
	int xIndexMax = static_cast<int>(floorf(xNonAdjustedIndex + ballBounds.Radius())); 
	int xIndexMin = static_cast<int>(floorf(xNonAdjustedIndex - ballBounds.Radius()));
	
	float yNonAdjustedIndex = ballCenter[1] / LevelPiece::PIECE_HEIGHT;
	int yIndexMax = static_cast<int>(floorf(yNonAdjustedIndex + ballBounds.Radius()));
	int yIndexMin = static_cast<int>(floorf(yNonAdjustedIndex - ballBounds.Radius()));

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
 * Returns: The first piece closest to the origin of the given ray that collides with it, NULL
 * if no collision found.
 */
LevelPiece* GameLevel::GetLevelPieceFirstCollider(const Collision::Ray2D& ray, const LevelPiece* ignorePiece, float& rayT) const {
	// Step along the ray - not a perfect algorithm but will result in something very reasonable
	const float LEVEL_WIDTH					 = this->GetLevelUnitWidth();
	const float LEVEL_HEIGHT				 = this->GetLevelUnitHeight();
	const float LONGEST_POSSIBLE_RAY = sqrt(LEVEL_WIDTH*LEVEL_WIDTH + LEVEL_HEIGHT*LEVEL_HEIGHT);

	// NOTE: if the step size is too large then the ray might skip over entire sections of blocks - BECAREFUL!
	const float STEP_SIZE = 0.5f * std::min<float>(LevelPiece::PIECE_WIDTH, LevelPiece::PIECE_HEIGHT);

	int NUM_STEPS = static_cast<int>(LONGEST_POSSIBLE_RAY / STEP_SIZE);
	for (int i = 0; i < NUM_STEPS; i++) {
		Point2D currSamplePoint = ray.GetPointAlongRayFromOrigin(i * STEP_SIZE);

		// Indices of the sampled level piece can be found using the point...
		int wSampleIndex = static_cast<int>(currSamplePoint[0] / LevelPiece::PIECE_WIDTH);
		int hSampleIndex = static_cast<int>(currSamplePoint[1] / LevelPiece::PIECE_HEIGHT);
		
		// Make sure the ray hasn't gone out of bounds
		if (wSampleIndex < 0 || hSampleIndex < 0) {
			continue;
		}
		else if (static_cast<size_t>(wSampleIndex) >= this->width || static_cast<size_t>(hSampleIndex) >= this->height) {
			continue;
		}

		// Grab the piece at that index
		LevelPiece* currSamplePiece = this->currentLevelPieces[hSampleIndex][wSampleIndex];
		assert(currSamplePiece != NULL);

		// Check to see if the piece can be collided with, if so try to collide the ray with
		// the actual block bounds, if there's a collision we get out of here and just return the piece
		if (currSamplePiece != ignorePiece && currSamplePiece->CollisionCheck(ray, rayT)) {
			return currSamplePiece;
		}

	}

	return NULL;
}

/**
 * Do a collision check with the ball safety net if it's active.
 * Returns: true on collision (when active) and the normal and distance values, false otherwise.
 */
bool GameLevel::BallSafetyNetCollisionCheck(const GameBall& b, double dT, Vector2D& n, Collision::LineSeg2D& collisionLine, double& timeSinceCollision) {
	if (!this->ballSafetyNetActive){ 
		return false;
	}

	// Create the safety net bounding line and check the ball against it
	std::vector<Collision::LineSeg2D> lines;
	lines.reserve(1);
	Collision::LineSeg2D safetyNetLine(Point2D(0.0f, -LevelPiece::HALF_PIECE_HEIGHT), Point2D(this->GetLevelUnitWidth(), -LevelPiece::HALF_PIECE_HEIGHT));
	lines.push_back(safetyNetLine);

	std::vector<Vector2D> normals;
	normals.reserve(1);
	normals.push_back(Vector2D(0, 1));
	
	BoundingLines safetyNetBounds(lines, normals);

	// Test for collision, if there was one then we kill the safety net
	bool didCollide = safetyNetBounds.Collide(dT, b.GetBounds(), b.GetVelocity(), n, collisionLine, timeSinceCollision);
	if (didCollide) {
		this->ballSafetyNetActive = false;
		GameEventManager::Instance()->ActionBallSafetyNetDestroyed(b);
	}

	return didCollide;
}