#ifndef __LEVELMESH_H__
#define __LEVELMESH_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/ObjReader.h"
#include "../BlammoEngine/Light.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"

class GameWorldAssets;
class BallSafetyNetMesh;
class Mesh;
class MaterialGroup;

class LevelMesh {

private:
	const GameLevel* currLevel;

	// Meshes for all types of level pieces
	Mesh* basicBlock;
	Mesh* bombBlock;
	Mesh* triangleBlockUR;	// Triangle block (default position in upper-right)
	Mesh* inkBlock;
	Mesh* styleBlock;

	// Misc. display lists and meshes for other geometry activated by items and such
	BallSafetyNetMesh* ballSafetyNet;

	// The unique identifiers of, and associated materials of the level piece meshes
	std::map<std::string, CgFxMaterialEffect*> levelMaterials;

	// Which display lists correspond to a given material
	std::map<CgFxMaterialEffect*, std::vector<GLuint>> displayListsPerMaterial;
	// The display lists associated with each level piece
	std::map<const LevelPiece*, std::map<CgFxMaterialEffect*, GLuint>> pieceDisplayLists;
	
	std::map<std::string, MaterialGroup*> GetMaterialGrpsForPieceType(LevelPiece::LevelPieceType type) const;
	void CreateDisplayListsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation);
	void CreateDisplayListForBallSafetyNet(float levelWidth);
	void Flush();	

public:
	LevelMesh(const GameWorldAssets* gameWorldAssets, const GameLevel* level);
	~LevelMesh();
	
	void ChangePiece(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void Draw(double dT, const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) const;

	void LoadNewLevel(const GameWorldAssets* gameWorldAssets, const GameLevel* level);

	void BallSafetyNetCreated();
	void BallSafetyNetDestroyed(const GameBall& ball);
};
#endif