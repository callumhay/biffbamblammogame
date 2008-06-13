#ifndef __MESH_H__
#define __MESH_H__

#include "CgFxEffect.h"

#include "../Utils/Includes.h"
#include "../Utils/Point.h"
#include "../Utils/Vector.h"

#include <vector>
#include <map>
#include <string>

class Camera;

// Represents the set of indices 
struct PolyGrpIndexer {
	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> normalIndices;
	std::vector<unsigned int> texCoordIndices;
};

// Holds all of the indexed vertices, normals, etc. for a polygon group
class PolygonGroup {
private:
	static const int INTERLEAVED_MULTIPLIER = 8;
	static const GLint GL_INTERLEAVED_FORMAT = GL_T2F_N3F_V3F;
	static const GLint INTERLEAVED_STRIDE = INTERLEAVED_MULTIPLIER * sizeof(float);

	unsigned int polygroupArrayLength;
	unsigned int numIndices;
	float* polygroupArray;

public:
	PolygonGroup(const PolygonGroup& other);
	PolygonGroup(const PolyGrpIndexer& faceIndexer, 
							 const std::vector<Point3D>& vertexStream, 
							 const std::vector<Vector3D>& normalStream,
							 const std::vector<Point2D>& texCoordStream);

	~PolygonGroup() {
		delete[] this->polygroupArray;	
	}

	void Draw() const {
		glInterleavedArrays(GL_INTERLEAVED_FORMAT, INTERLEAVED_STRIDE, this->polygroupArray);
		glDrawArrays(GL_TRIANGLES, 0, this->numIndices);
	};

	void Translate(const Vector3D& t);

};

class MaterialGroup {

private:
	PolygonGroup* polyGrp;
	CgFxEffect* material;
	GLint displayListID;

public:
	MaterialGroup(CgFxEffect* mat) : material(mat), displayListID(-1), polyGrp(NULL) {}
	
	~MaterialGroup() {
		if (this->polyGrp != NULL) {
			delete this->polyGrp;
		}

		delete this->material;
		glDeleteLists(this->displayListID, 1);
	}

	void AddFaces(const PolyGrpIndexer& faceIndexer, 
							  const std::vector<Point3D>& vertexStream, 
								const std::vector<Vector3D>& normalStream,
								const std::vector<Point2D>& texCoordStream);

	void Draw(const Camera& camera) const {
		this->material->Draw(camera, this->displayListID);
	}

	void FastDraw() const {
		glCallList(this->displayListID);
	}

	CgFxEffect* GetMaterial() const {
		return this->material;
	}

	PolygonGroup* GetPolygonGroup() const {
		return this->polyGrp;
	}

};

// Represents a mesh, made up of faces, grouped by material.
class Mesh {

protected:
	std::string name;
	std::map<std::string, MaterialGroup*> matGrps;

public:
	Mesh(const std::string name, const std::map<std::string, MaterialGroup*> &matGrps);
	virtual ~Mesh();

	virtual void Draw(const Camera& camera) const {
		// Draw each material group
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); matGrpIter++) {
			matGrpIter->second->Draw(camera);
		}
	}
	
	/**
	 * Draw all of the mesh without the material.
	 */
	virtual void FastDraw() const {
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); matGrpIter++) {
			matGrpIter->second->FastDraw();
		}
	}

	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const {
		return this->matGrps;
	}

	/**
	 * Set the colour of this mesh.
	 */
	void SetColour(const Colour& c);
};
#endif