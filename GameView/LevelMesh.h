#ifndef __LEVELMESH_H__
#define __LEVELMESH_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/ObjReader.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"

class GameWorldAssets;

class LevelMesh {

private:
	Vector2D levelDimensions;

	// Meshes for all types of level pieces
	Mesh* basicBlock;
	Mesh* bombBlock;
	Mesh* styleBlock;

	// The unique identifiers of, and associated materials of the level piece meshes
	std::map<std::string, CgFxMaterialEffect*> levelMaterials;

	// Which display lists correspond to a given material
	std::map<CgFxMaterialEffect*, std::list<GLuint>> displayListsPerMaterial;

	// The display lists associated with each level piece
	std::map<const LevelPiece*, std::map<CgFxMaterialEffect*, GLuint>> pieceDisplayLists;

	std::map<std::string, MaterialGroup*> GetMaterialGrpsForPieceType(LevelPiece::LevelPieceType type) const;
	void CreateDisplayListsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation);
	void Flush();	

public:
	LevelMesh(const GameWorldAssets* gameWorldAssets, const GameLevel* level);
	~LevelMesh();
	
	void ChangePiece(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void Draw(const Camera& camera) const;

	void LoadNewLevel(const GameWorldAssets* gameWorldAssets, const GameLevel* level);
														 
};
#endif