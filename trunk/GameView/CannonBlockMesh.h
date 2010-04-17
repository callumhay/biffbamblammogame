#ifndef __CANNONBLOCKMESH_H__
#define __CANNONBLOCKMESH_H__

#include "../BlammoEngine/Mesh.h"


class CannonBlockMesh {
public:

	CannonBlockMesh();
	~CannonBlockMesh();

	inline const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const {
		return this->materialGroups;
	}

private:
	Mesh* cannonBlockBaseGeometry;
	Mesh* cannonBlockBarrelGeometry;
	std::map<std::string, MaterialGroup*> materialGroups;

	void LoadMesh();
};

#endif // __CANNONBLOCKMESH_H__