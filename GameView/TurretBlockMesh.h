/**
 * TurretBlockMesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __TURRETBLOCKMESH_H__
#define __TURRETBLOCKMESH_H__

#include "../BlammoEngine/BasicIncludes.h"

class Texture2D;
class MaterialGroup;
class Mesh;
class BasicPointLight;
class Camera;

class TurretBlockMesh {
public:
    virtual ~TurretBlockMesh();

    virtual void Flush() = 0;
    virtual void Draw(double dT, const Camera& camera, const BasicPointLight& keyLight,
        const BasicPointLight& fillLight, const BasicPointLight& ballLight) = 0;
	virtual void SetAlphaMultiplier(float alpha) = 0;

    const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const;

protected:
    TurretBlockMesh();

    std::map<std::string, MaterialGroup*> materialGroups; // Material groups for the static parts of the laser turret block mesh

    std::vector<Texture2D*> smokeTextures;
    Texture2D* glowTexture;
    Texture2D* sparkleTexture;

    Mesh* baseMesh;

private:
    DISALLOW_COPY_AND_ASSIGN(TurretBlockMesh);
};

inline const std::map<std::string, MaterialGroup*>& TurretBlockMesh::GetMaterialGroups() const {
	return this->materialGroups;
}

#endif // __TURRETBLOCKMESH_H__