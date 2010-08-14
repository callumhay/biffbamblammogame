/**
 * Mesh.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MESH_H__
#define __MESH_H__

#include "BasicIncludes.h"

#include "Matrix.h"
#include "Point.h"
#include "Vector.h"
#include "CgFxEffect.h"
#include "Light.h"

class Camera;

// Represents the set of indices 
struct PolyGrpIndexer {
	GLenum polyType;
	std::vector<unsigned short> vertexIndices;
	std::vector<unsigned short> normalIndices;
	std::vector<unsigned short> texCoordIndices;

	PolyGrpIndexer() : polyType(GL_TRIANGLES) {};
};

// Holds all of the indexed vertices, normals, etc. for a polygon group
class PolygonGroup {
private:

	GLenum polyType;
	//size_t numIndices;
	std::vector<Point3D>  vertexStream;
	std::vector<Vector3D> normalStream;
	std::vector<Point2D>  texCoordStream;

	std::vector<GLushort> indices;

public:
	PolygonGroup(const PolygonGroup& other);
	PolygonGroup(const PolyGrpIndexer& faceIndexer, 
							 const std::vector<Point3D>& vertexStream, 
							 const std::vector<Vector3D>& normalStream,
							 const std::vector<Point2D>& texCoordStream);

	~PolygonGroup() {};

	void Draw() const;

	GLuint GenerateDisplayList() const {
		GLuint displayListID = glGenLists(1);
		glNewList(displayListID, GL_COMPILE);
		this->Draw();
		glEndList();
		return displayListID;
	}

	void Translate(const Vector3D& t);
	void Transform(const Matrix4x4& m);
};

class Mesh;

class MaterialGroup {

private:
	PolygonGroup* polyGrp;
	CgFxMaterialEffect* material;
	GLuint displayListID;

	// Deletes only the effect/material for this material group
	void ReplaceMaterialEffect(CgFxMaterialEffect* material) {
		assert(material != NULL);
		delete this->material;
		this->material = material;
	}

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
		if (this->displayListID != 0) {
			glDeleteLists(this->displayListID, 1);
			this->displayListID = 0;
		}
	}

	inline void Draw(const Camera& camera) const {
		assert(this->displayListID != 0);
		this->material->Draw(camera, this->displayListID);
	}
	inline void Draw(const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight) {
		this->material->SetKeyLight(keyLight);
		this->material->SetFillLight(fillLight);
		this->Draw(camera);
	}
	inline void Draw(const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
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

	friend class Mesh;
};

// Represents a mesh, made up of faces, grouped by material.
class Mesh {

protected:
	std::string name;
	std::map<std::string, MaterialGroup*> matGrps;

	Mesh(const std::string name) : name(name) {}

public:
	Mesh(const std::string name, const std::map<std::string, MaterialGroup*> &matGrps);
	~Mesh();

	/**
	 * Typical draw function for a mesh.
	 */
	void Draw(const Camera& camera) const {
		// Draw each material group
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); ++matGrpIter) {
			matGrpIter->second->Draw(camera);
		}
	}

	/**
	 * Draw function for a mesh, with given key and fill lights.
	 */
	void Draw(const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight) {
		// Draw each material group
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); ++matGrpIter) {
			matGrpIter->second->Draw(camera, keyLight, fillLight);
		}
	}

	/**
	 * Draw function for a mesh, with given key, fill and ball lights.
	 */
	void Draw(const Camera& camera, const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
		// Draw each material group
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); ++matGrpIter) {
			matGrpIter->second->Draw(camera, keyLight, fillLight, ballLight);
		}
	}

	/**
	 * Special override of the draw function - this will take the given material
	 * and apply it to the entire mesh, regardless of the material groups.
	 */
	void Draw(const Camera& camera, CgFxEffectBase* replacementMat) {
		// In case the replacement material is NULL then we just do default draw...
		if (replacementMat == NULL) {
			this->Draw(camera);
		}
		else {
			// Draw each material group
			std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
			for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); ++matGrpIter) {
				matGrpIter->second->Draw(camera, replacementMat);
			}
		}
	}

	/**
	 * Special override of the draw function - this will take the given material
	 * and apply it to the entire mesh, regardless of the material groups, also applies given lights
	 */
	void Draw(const Camera& camera, CgFxEffectBase* replacementMat, const BasicPointLight& keyLight, const BasicPointLight& fillLight) {
		// In case the replacement material is NULL then we just do default draw...
		if (replacementMat == NULL) {
			this->Draw(camera, keyLight, fillLight);
		}
		else {
			// Draw each material group
			std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
			for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); ++matGrpIter) {
				matGrpIter->second->Draw(camera, replacementMat);
			}
		}
	}

	/**
	 * Draw all of the mesh without the material.
	 */
	void FastDraw() const {
		std::map<std::string, MaterialGroup*>::const_iterator matGrpIter = this->matGrps.begin();
		for (matGrpIter = this->matGrps.begin(); matGrpIter != this->matGrps.end(); ++matGrpIter) {
			matGrpIter->second->FastDraw();
		}
	}

	// Obtain a reference to the listing of material groups for this mesh.
	const std::map<std::string, MaterialGroup*>& GetMaterialGroups() const {
		return this->matGrps;
	}

	void SetTextureForMaterial(const std::string& matGrpName, Texture* texToSet);
	void SetColourForMaterial(const std::string& matGrpName, const Colour& c);
	void SetColour(const Colour& c);

	void ReplaceMaterial(CgFxMaterialEffect* replacementMat);
	void Flush();
};
#endif