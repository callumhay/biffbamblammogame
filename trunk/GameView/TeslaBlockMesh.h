/**
 * TeslaBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TESLABLOCKMESH_H__
#define __TESLABLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

#include "../ESPEngine/ESPParticleScaleEffector.h"

class TeslaBlock;
class ESPPointEmitter;
class Texture2D;
class CgFxPostRefract;

class TeslaBlockMesh {
public:
	TeslaBlockMesh();
	~TeslaBlockMesh();

	void Flush();
	void AddTeslaBlock(const TeslaBlock* cannonBlock);
	void RemoveTeslaBlock(const TeslaBlock* cannonBlock);
	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void SetAlphaMultiplier(float alpha);

private:
	static const float COIL_ROTATION_SPEED_DEGSPERSEC;

	std::map<std::string, MaterialGroup*> materialGroups;	// Material groups for the static parts of the tesla block mesh
	
	std::set<const TeslaBlock*> teslaBlocks;														// A list of all the tesla blocks that are currently present in the game
	std::map<const TeslaBlock*, std::pair<Vector3D, float>> rotations;	// Rotations of the various tesla coils

	Mesh* teslaBaseMesh;
	Mesh* teslaCoilMesh;

	ESPParticleScaleEffector flarePulse;
	//ESPParticleAccelEffector sparkGravity;
	ESPPointEmitter* teslaCenterFlare;
	//ESPPointEmitter* teslaSparks;
	Texture2D* flareTex;
	//Texture2D* sparkTex;
	Texture2D* shieldTex;
	float shieldAlpha;

	void DrawTeslaShield(const Matrix4x4& screenAlignMatrix);

	void LoadMesh();

	// Disallow copy and assign
	TeslaBlockMesh(const TeslaBlockMesh& t);
	TeslaBlockMesh& operator=(const TeslaBlockMesh& t);
};

inline void TeslaBlockMesh::Flush() {
	this->teslaBlocks.clear();
}

inline void TeslaBlockMesh::AddTeslaBlock(const TeslaBlock* teslaBlock) {
	assert(teslaBlock != NULL);
	std::pair<std::set<const TeslaBlock*>::iterator, bool> insertResult = this->teslaBlocks.insert(teslaBlock);
	assert(insertResult.second);
	Vector3D randomRotVec(Randomizer::GetInstance()->RandomNumNegOneToOne(),
												Randomizer::GetInstance()->RandomNumNegOneToOne(), 0.0f);
	if (randomRotVec == Vector3D(0,0,0)) {
		randomRotVec[0] = 1.0f;
		randomRotVec[1] = 1.0f;
	}
	randomRotVec.Normalize();
	this->rotations.insert(std::make_pair(teslaBlock, std::make_pair(randomRotVec, static_cast<float>(Randomizer::GetInstance()->RandomNumNegOneToOne() * 360.0f))));
}	

inline void TeslaBlockMesh::RemoveTeslaBlock(const TeslaBlock* teslaBlock) {
	size_t numRemoved = this->teslaBlocks.erase(teslaBlock);
	assert(numRemoved == 1);
	this->rotations.erase(teslaBlock);
}

inline const std::map<std::string, MaterialGroup*>& TeslaBlockMesh::GetMaterialGroups() const {
	return this->materialGroups;
}

#endif // __TESLABLOCKMESH_H__