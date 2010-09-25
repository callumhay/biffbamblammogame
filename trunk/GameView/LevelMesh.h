#ifndef __LEVELMESH_H__
#define __LEVELMESH_H__

#include "../BlammoEngine/CgFxEffect.h"
#include "../BlammoEngine/ObjReader.h"
#include "../BlammoEngine/Light.h"

#include "../GameModel/GameWorld.h"
#include "../GameModel/GameLevel.h"
#include "../GameModel/LevelPiece.h"

class GameWorldAssets;
class GameItemAssets;
class BallSafetyNetMesh;
class Mesh;
class PrismBlockMesh;
class PortalBlockMesh;
class CannonBlockMesh;
class CollateralBlockMesh;
class TeslaBlockMesh;
class ItemDropBlockMesh;
class MaterialGroup;
class ESPEmitter;
class ItemDropBlock;

class LevelMesh {
public:
	LevelMesh(const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level);
	~LevelMesh();
	
	void ChangePiece(const LevelPiece& pieceBefore, const LevelPiece& pieceAfter);
	void RemovePiece(const LevelPiece& piece);
	
	void DrawPieces(const Vector3D& worldTranslation, double dT, const Camera& camera, bool lightsAreOut, const BasicPointLight& keyLight, 
									const BasicPointLight& fillLight, const BasicPointLight& ballLight, const Texture2D* sceneTexture);
	void DrawSafetyNet(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) const;

	void LoadNewLevel(const GameWorldAssets& gameWorldAssets, const GameItemAssets& gameItemAssets, const GameLevel& level);

	void BallSafetyNetCreated();
	void BallSafetyNetDestroyed(const Point2D& pos);

	void PaddleCameraActiveToggle(bool isActive);

	void SetLevelAlpha(float alpha);

	void UpdateItemDropBlock(const GameItemAssets& gameItemAssets, const ItemDropBlock& block);

private:
	const GameLevel* currLevel;

	// Meshes for all types of level pieces
	Mesh* basicBlock;
	Mesh* bombBlock;
	Mesh* triangleBlockUR;								// Triangle block (default position in upper-right)
	Mesh* inkBlock;
	PortalBlockMesh* portalBlock;
	PrismBlockMesh* prismBlockDiamond;		// Prism diamond block
	PrismBlockMesh* prismBlockTriangleUR;	// Prism triangle block in upper-right corner position
	CannonBlockMesh* cannonBlock;
	CollateralBlockMesh* collateralBlock;
	TeslaBlockMesh* teslaBlock;
	ItemDropBlockMesh* itemDropBlock;

	// Style-specific level pieces
	Mesh* styleBlock;

	// Misc. display lists and meshes for other geometry activated by items and such
	BallSafetyNetMesh* ballSafetyNet;

	// The unique identifiers of, and associated materials of the level piece meshes
	std::map<std::string, CgFxMaterialEffect*> levelMaterials;

	// Which display lists correspond to a given material
	std::map<CgFxMaterialEffect*, std::vector<GLuint> > displayListsPerMaterial;
	// The display lists associated with each level piece
	std::map<const LevelPiece*, std::map<CgFxMaterialEffect*, GLuint> > pieceDisplayLists;
	// Special effects always present for specific level pieces
	std::map<const LevelPiece*, std::list<ESPEmitter*> > pieceEmitterEffects;

	const std::map<std::string, MaterialGroup*>* GetMaterialGrpsForPieceType(LevelPiece::LevelPieceType type) const;
	void CreateDisplayListsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation);
	void CreateEmitterEffectsForPiece(const LevelPiece* piece, const Vector3D &worldTranslation);
	void CreateDisplayListForBallSafetyNet(float levelWidth);
	void Flush();	
};
#endif