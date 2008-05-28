#ifndef __MESH_H__
#define __MESH_H__

#include "CelShadingMaterial.h"

#include "../Utils/Includes.h"
#include "../Utils/Point.h"
#include "../Utils/Vector.h"

#include <vector>
#include <map>
#include <string>

// Represents a triangle face that makes up part of a material group.
struct TriFace {
	size_t vertexIndices[3];
	size_t normalIndices[3];
	size_t texCoordIndices[3];
};

struct MaterialGroup {
	
	std::vector<TriFace> faces;
	CelShadingMaterial material;

	MaterialGroup() {}
	MaterialGroup(const std::vector<TriFace> &faces): faces(faces) {}
};

// Represents a mesh, made up of faces, grouped by material.
class Mesh {

private:
	GLuint displayListID;
	std::string name;

	std::vector<Point3D> vertices;
	std::vector<Vector3D> normals;
	std::vector<Point2D> texCoords;
	std::map<std::string, MaterialGroup> matGrps;		// Material groups, listed by their face group name

	void CompileDisplayList();

public:
	Mesh(const std::string name, const std::vector<Point3D> &verts, const std::vector<Vector3D> &norms,
		   const std::vector<Point2D> &texCoords, const std::map<std::string, MaterialGroup> &matGrps);
	virtual ~Mesh();

	virtual void Draw() const {
		glCallList(this->displayListID);
	}

	Vector3D CalculateDimensions();

};
#endif