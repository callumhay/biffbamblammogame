#ifndef __MESH_H__
#define __MESH_H__

#include "BasicIncludes.h"

#include "Matrix.h"
#include "Point.h"
#include "Vector.h"
#include "CgFxEffect.h"

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
	void Transform(const Matrix4x4& m);
};

class MaterialGroup {

private:
	PolygonGroup* polyGrp;
	CgFxMaterialEffect* material;
	GLint displayListID;

public:
	MaterialGroup(CgFxMaterialEffect* mat) : material(mat), displayListID(-1), polyGrp(NULL) {}
	
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
	void Draw(const Camera& camera, CgFxEffectBase* replacementMat) const {
		replacementMat->Draw(camera, this->displayListID);
	}

	void FastDraw() const {
		glCallList(this->displayListID);
	}

	CgFxMaterialEffect* GetMaterial() const {
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

	/**
	 * Typical draw function for a mesh.
	 */
	virtual void Draw(const Camera& camera) const {
		// Draw each material group
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); matGrpIter++) {
			matGrpIter->second->Draw(camera);
		}
	}

	/**
	 * Special override of the draw function - this will take the given material
	 * and apply it to the entire mesh, regardless of the material groups.
	 */
	virtual void Draw(const Camera& camera, CgFxEffectBase* replacementMat) const {
		// In case the replacement material is NULL then we just do default draw...
		if (replacementMat == NULL) {
			this->Draw(camera);
		}
		else {
			// Draw each material group
			std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
			for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); matGrpIter++) {
				matGrpIter->second->Draw(camera, replacementMat);
			}
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

	// Obtain a reference to the listing of material groups for this mesh.
	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const {
		return this->matGrps;
	}

	void SetTextureForMaterial(const std::string& matGrpName, Texture2D* texToSet);
	void SetColourForMaterial(const std::string& matGrpName, const Colour& c);
	void SetColour(const Colour& c);
};
#endif