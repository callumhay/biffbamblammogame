/**
 * SwitchBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009-2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __SWITCHBLOCKMESH_H__
#define __SWITCHBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"
#include "../BlammoEngine/Texture2D.h"

class SwitchBlock;

class SwitchBlockMesh {
public:
    SwitchBlockMesh();
    ~SwitchBlockMesh();

    void Flush();

	void AddSwitchBlock(const SwitchBlock* switchBlock);
	void RemoveSwitchBlock(const SwitchBlock* switchBlock);

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
        const BasicPointLight& fillLight, const BasicPointLight& ballLight, bool lightsAreOff) const;
	void SetAlphaMultiplier(float alpha);

private:
    Mesh* switchBlockGeometry;
    std::map<std::string, MaterialGroup*> baseMaterialGrp;
    std::set<const SwitchBlock*> switchBlocks;

    MaterialGroup* switchCurrentMaterialGrp;
    MaterialGroup* switchOnMaterialGrp;
    MaterialGroup* switchOffMaterialGrp;

    Texture2D* greenOnSwitchTexture;
    Texture2D* redOnSwitchTexture;
    Texture2D* offSwitchTexture;

    void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(SwitchBlockMesh);
};

inline void SwitchBlockMesh::Flush() {
    this->switchBlocks.clear();
}

inline void SwitchBlockMesh::AddSwitchBlock(const SwitchBlock* switchBlock) {
    assert(switchBlock != NULL);
    std::pair<std::set<const SwitchBlock*>::iterator, bool> insertResult = 
        this->switchBlocks.insert(switchBlock);
    assert(insertResult.second);
}

inline void SwitchBlockMesh::RemoveSwitchBlock(const SwitchBlock* switchBlock) {
    assert(switchBlock != NULL);
    std::set<const SwitchBlock*>::iterator findIter = this->switchBlocks.find(switchBlock);
    assert(findIter != this->switchBlocks.end());
    this->switchBlocks.erase(findIter);
}

inline const std::map<std::string, MaterialGroup*>& SwitchBlockMesh::GetMaterialGroups() const {
    return this->baseMaterialGrp;
}

#endif //__SWITCHBLOCKMESH_H__