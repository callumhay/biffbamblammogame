#ifndef __LEVELMESH_H__
#define __LEVELMESH_H__

#include "../BlammoEngine/BlammoEngine.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"

class GameWorldAssets;

class LevelMesh {

private:
	static const GLint NO_PIECE = -1;

	Mesh* basicBlock;
	Mesh* styleBlock;
	Mesh* bombBlock;
	
	std::vector<std::vector<GLint>> pieceMeshes;
	std::map<LevelPiece::LevelPieceType, std::vector<GLint>> piecesByMaterial;

	LevelMesh(const GameWorldAssets* gameWorldAssets);

public:
	~LevelMesh();
	
	void ChangePiece(const LevelPiece& blockBefore, const LevelPiece& blockAfter);
	void Draw(const Camera& camera) const;

	// Static creator
	static LevelMesh* CreateLevelMesh(const GameWorldAssets* gameWorldAssets, const GameLevel* level);
														 

};
#endif