#ifndef __LEVELMESH_H__
#define __LEVELMESH_H__

#include "../Utils/Includes.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"

#include "Mesh.h"
#include "CgFxEffect.h"

#include <vector>
#include <map>

class Camera;

class LevelMesh {

private:
	static const GLint NO_PIECE = -1;

	static const std::string BASIC_BLOCK_MESH_PATH;
	static const std::string DECO_BLOCK_MESH_PATH;
	static const std::string CYBERPUNK_BLOCK_MESH_PATH;

	Mesh* basicBlock;
	Mesh* styleBlock;
	
	std::vector<std::vector<GLint>> pieceMeshes;
	std::map<LevelPiece::LevelPieceType, std::vector<GLint>> piecesByMaterial;

	LevelMesh(GameWorld::WorldStyle worldStyle);

public:
	~LevelMesh();
	
	void ChangePiece(const LevelPiece& piece);
	void Draw(const Camera& camera) const;

	// Static creator
	static LevelMesh* CreateLevelMesh(GameWorld::WorldStyle worldStyle, const GameLevel* level);
														 

};
#endif