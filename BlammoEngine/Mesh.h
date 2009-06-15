#ifndef __MESH_H__
#define __MESH_H__

#include "BasicIncludes.h"

#include "Matrix.h"
#include "Point.h"
#include "Vector.h"
#include "CgFxEffect.h"
#include "Light.h"

class Camera;

/*
struct Vertex32 {
	float x, y, z;		// Vertex
	float nx, ny, nz;	// Normal
	float s, t;				// Texture coordinate
	float r, g, b, a;	// Colour
};
*/

// Represents the set of indices 
struct PolyGrpIndexer {
	GLenum polyType;
	std::vector<unsigned int> vertexIndices;
	std::vector<unsigned int> normalIndices;
	std::vector<unsigned int> texCoordIndices;

	PolyGrpIndexer() : polyType(GL_TRIANGLES) {};
};

// Holds all of the indexed vertices, normals, etc. for a polygon group
class PolygonGroup {
private:
	static const int INTERLEAVED_MULTIPLIER = 8;
	static const GLint GL_INTERLEAVED_FORMAT = GL_T2F_N3F_V3F;
	static const GLint INTERLEAVED_STRIDE = INTERLEAVED_MULTIPLIER * sizeof(float);

	GLenum polyType;
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
		glDrawArrays(this->polyType, 0, this->numIndices);
	};

	void Translate(const Vector3D& t);
	void Transform(const Matrix4x4& m);

	unsigned int GetDataArrays(std::vector<float>& vertexArray, std::vector<float>& normalArray, std::vector<float>& texCoordArray) const;
};

class MaterialGroup {

private:
	PolygonGroup* polyGrp;
	CgFxMaterialEffect* material;
	GLuint displayListID;

public:
	MaterialGroup(CgFxMaterialEffect* mat) : material(mat), displayListID(0), polyGrp(NULL) {}
	
	~MaterialGroup() {
		if (this->polyGrp != NULL) {
			delete this->polyGrp;
			this->polyGrp = NULL;
		}

		delete this->material;
		this->material = NULL;

		this->DeleteDisplayList();
	}

	void AddFaces(const PolyGrpIndexer& faceIndexer, 
							  const std::vector<Point3D>& vertexStream, 
								const std::vector<Vector3D>& normalStream,
								const std::vector<Point2D>& texCoordStream);

	void SetPolygonGroup(PolygonGroup* polyGrp) {
		if (this->polyGrp != NULL) {
			delete this->polyGrp;
		}
		this->polyGrp = polyGrp;
	}

	void SetDisplayListID(GLuint dispListID) {
		this->displayListID = dispListID;
	}
	GLuint GetDisplayListID() const {
		return this->displayListID;
	}

	void DeleteDisplayList() {
		glDeleteLists(this->displayListID, 1);
		this->displayListID = 0;	
	}

	inline void Draw(const Camera& camera) const {
		assert(this->displayListID != 0);
		this->material->Draw(camera, this->displayListID);
	}
	inline void Draw(const Camera& camera, const PointLight& keyLight, const PointLight& fillLight) {
		this->material->SetKeyLight(keyLight);
		this->material->SetFillLight(fillLight);
		this->Draw(camera);
	}
	inline void Draw(const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) {
		this->material->SetBallLight(ballLight);
		this->Draw(camera, keyLight, fillLight);
	}
	inline void Draw(const Camera& camera, CgFxEffectBase* replacementMat) const {
		assert(this->displayListID != 0);
		replacementMat->Draw(camera, this->displayListID);
	}

	inline void FastDraw() const {
		assert(this->displayListID != 0);
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

	Mesh(const std::string name) : name(name) {}

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
	 * Draw function for a mesh, with given key and fill lights.
	 */
	virtual void Draw(const Camera& camera, const PointLight& keyLight, const PointLight& fillLight) {
		// Draw each material group
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); matGrpIter++) {
			matGrpIter->second->Draw(camera, keyLight, fillLight);
		}
	}

	/**
	 * Draw function for a mesh, with given key, fill and ball lights.
	 */
	virtual void Draw(const Camera& camera, const PointLight& keyLight, const PointLight& fillLight, const PointLight& ballLight) {
		// Draw each material group
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); matGrpIter++) {
			matGrpIter->second->Draw(camera, keyLight, fillLight, ballLight);
		}
	}

	/**
	 * Special override of the draw function - this will take the given material
	 * and apply it to the entire mesh, regardless of the material groups.
	 */
	virtual void Draw(const Camera& camera, CgFxEffectBase* replacementMat) {
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
	 * Special override of the draw function - this will take the given material
	 * and apply it to the entire mesh, regardless of the material groups, also applies given lights
	 */
	virtual void Draw(const Camera& camera, CgFxEffectBase* replacementMat, const PointLight& keyLight, const PointLight& fillLight) {
		// In case the replacement material is NULL then we just do default draw...
		if (replacementMat == NULL) {
			this->Draw(camera, keyLight, fillLight);
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

	void Flush();
};
#endif