/**
 * AlwaysDropBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __ALWAYSDROPBLOCKMESH_H__
#define __ALWAYSDROPBLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Algebra.h"
#include "../BlammoEngine/Mesh.h"

class AlwaysDropBlock;

class AlwaysDropBlockMesh {
public:
	AlwaysDropBlockMesh();
	~AlwaysDropBlockMesh();

	void Flush();

	void AddAlwaysDropBlock(const AlwaysDropBlock* block, Texture* itemTexture);
	void RemoveAlwaysDropBlock(const AlwaysDropBlock* block);

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

	void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight);
	void SetAlphaMultiplier(float alpha);

private:
	Mesh* blockMesh;

	std::map<const AlwaysDropBlock*, Texture*> blockToItemTexMap;
	MaterialGroup* dropTypeMatGrp;
    MaterialGroup* dropColourMatGrp;
    
	void LoadMesh();

    DISALLOW_COPY_AND_ASSIGN(AlwaysDropBlockMesh);
};

inline void AlwaysDropBlockMesh::Flush() {
	this->blockToItemTexMap.clear();
}


#endif // __ALWAYSDROPBLOCKMESH_H__