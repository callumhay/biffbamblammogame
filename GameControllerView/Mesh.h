#ifndef __MESH_H__
#define __MESH_H__

#include "CgFxCelShading.h"

#include "../Utils/Includes.h"
#include "../Utils/Point.h"
#include "../Utils/Vector.h"

#include <vector>
#include <map>
#include <string>

// Represents the set of indices 
struct PolyGrpIndexer {
	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> normalIndices;
	std::vector<unsigned int> texCoordIndices;
};

class MaterialGroup {

private:
	static const int INTERLEAVED_MULTIPLIER = 8;
	static const GLint GL_INTERLEAVED_FORMAT = GL_T2F_N3F_V3F;
	static const GLint INTERLEAVED_STRIDE = INTERLEAVED_MULTIPLIER * sizeof(float);

	float* polygroupArray;
	CgFxCelShading* material;
	GLint displayListID;

public:
	MaterialGroup(CgFxCelShading* mat) : material(mat), polygroupArray(NULL), displayListID(-1) {}
	~MaterialGroup() {
		delete this->material;
		if (this->polygroupArray != NULL) {
			delete[] this->polygroupArray;
		}
		glDeleteLists(this->displayListID, 1);
	}

	void AddFaces(const PolyGrpIndexer& faceIndexer, 
							  const std::vector<Point3D>& vertexStream, 
								const std::vector<Vector3D>& normalStream,
								const std::vector<Point2D>& texCoordStream);

	void Draw() const {
		//this->material->DrawMaterial();
		//glCallList(this->displayListID);
		this->material->Draw(this->displayListID);
	}

};

// Represents a mesh, made up of faces, grouped by material.
class Mesh {

private:
	std::string name;
	std::map<std::string, MaterialGroup*> matGrps;

public:
	Mesh(const std::string name, const std::map<std::string, MaterialGroup*> &matGrps);
	virtual ~Mesh();

	virtual void Draw() const {
		// Draw each material group
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); matGrpIter++) {
			matGrpIter->second->Draw();
		}
	}


	//Vector3D CalculateDimensions();
};
#endif