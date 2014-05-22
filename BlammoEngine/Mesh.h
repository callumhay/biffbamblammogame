/**
 * Mesh.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	void TransformVerticesAndNormals(const Matrix4x4& m);
    void TransformTexCoords(const Matrix4x4& m);
    void TransformVerticesNormalsAndTexCoords(const Matrix4x4& m);
    void RotateTexCoords(float degs);
    //void ReflectTexCoordsInY();
};

class Mesh;

class MaterialGroup {

private:
	PolygonGroup* polyGrp;
	CgFxAbstractMaterialEffect* material;
	GLuint displayListID;

	// Deletes only the effect/material for this material group
	void ReplaceMaterialEffect(CgFxAbstractMaterialEffect* material) {
		assert(material != NULL);
		delete this->material;
		this->material = material;
	}

    void BuildDisplayList();

public:
	MaterialGroup(CgFxAbstractMaterialEffect* mat) : material(mat), displayListID(0), polyGrp(NULL) {}
	
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
    void ResetDisplayList();

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

	CgFxAbstractMaterialEffect* GetMaterial() const {
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
	 * Special override of the draw function - this will take the given material
	 * and apply it to the entire mesh, regardless of the material groups, also applies given lights
	 */
	void Draw(const Camera& camera, CgFxEffectBase* replacementMat, const BasicPointLight& keyLight,
              const BasicPointLight& fillLight, const BasicPointLight& ballLight) {

		// In case the replacement material is NULL then we just do default draw...
		if (replacementMat == NULL) {
			this->Draw(camera, keyLight, fillLight, ballLight);
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
    void SetAlpha(float alpha);

	void ReplaceMaterial(CgFxAbstractMaterialEffect* replacementMat);
	void Flush();


};
#endif