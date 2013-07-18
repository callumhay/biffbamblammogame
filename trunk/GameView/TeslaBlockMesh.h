/**
 * TeslaBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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
	void AddTeslaBlock(TeslaBlock* teslaBlock);
	void RemoveTeslaBlock(TeslaBlock* teslaBlock);
	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
    void DrawPostEffects(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void SetAlphaMultiplier(float alpha);

private:
	static const float COIL_ROTATION_SPEED_DEGSPERSEC;

    typedef std::set<TeslaBlock*> TeslaBlockSet;
    typedef TeslaBlockSet::iterator TeslaBlockSetIter;
    typedef TeslaBlockSet::const_iterator TeslaBlockSetConstIter;
    
	std::map<std::string, MaterialGroup*> materialGroups;  // Material groups for the static parts of the tesla block mesh
	TeslaBlockSet teslaBlocks;                             // A list of all the tesla blocks that are currently present in the game

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

    DISALLOW_COPY_AND_ASSIGN(TeslaBlockMesh);
};

inline void TeslaBlockMesh::Flush() {
	this->teslaBlocks.clear();
}

inline void TeslaBlockMesh::AddTeslaBlock(TeslaBlock* teslaBlock) {
	assert(teslaBlock != NULL);
	std::pair<TeslaBlockSetIter, bool> insertResult = this->teslaBlocks.insert(teslaBlock);
	assert(insertResult.second);
}	

inline void TeslaBlockMesh::RemoveTeslaBlock(TeslaBlock* teslaBlock) {
	size_t numRemoved = this->teslaBlocks.erase(teslaBlock);
    UNUSED_VARIABLE(numRemoved);
	assert(numRemoved == 1);
}

inline const std::map<std::string, MaterialGroup*>& TeslaBlockMesh::GetMaterialGroups() const {
	return this->materialGroups;
}

#endif // __TESLABLOCKMESH_H__