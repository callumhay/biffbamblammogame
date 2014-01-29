/**
 * Mesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "Mesh.h"

// Copy constructor
PolygonGroup::PolygonGroup(const PolygonGroup& other) : polyType(other.polyType), vertexStream(other.vertexStream),
normalStream(other.normalStream), texCoordStream(other.texCoordStream), indices(other.indices) {
}

PolygonGroup::PolygonGroup(const PolyGrpIndexer& faceIndexer, const std::vector<Point3D>& vertexStream, 
													 const std::vector<Vector3D>& normalStream, const std::vector<Point2D>& texCoordStream) {
	assert(faceIndexer.vertexIndices.size () >= 3);
	assert(faceIndexer.normalIndices.size() == faceIndexer.vertexIndices.size());
	assert(faceIndexer.vertexIndices.size() == faceIndexer.texCoordIndices.size());

	this->polyType   = faceIndexer.polyType;
	assert(faceIndexer.vertexIndices.size() <= USHRT_MAX);
	for (size_t i = 0; i < faceIndexer.vertexIndices.size(); i++) {
		this->vertexStream.push_back(vertexStream[faceIndexer.vertexIndices[i]]);
		this->normalStream.push_back(normalStream[faceIndexer.normalIndices[i]]);
		this->texCoordStream.push_back(texCoordStream[faceIndexer.texCoordIndices[i]]);
		this->indices.push_back(static_cast<GLushort>(i));
	}

}

void PolygonGroup::Draw() const {
	glBegin(this->polyType);
	std::vector<Point3D>::const_iterator vertexIter = this->vertexStream.begin();
	std::vector<Vector3D>::const_iterator normalIter = this->normalStream.begin();
	std::vector<Point2D>::const_iterator texIter = this->texCoordStream.begin();
	for (; vertexIter != this->vertexStream.end(); ++vertexIter, ++normalIter, ++texIter) {
		const Point3D& currVertex = *vertexIter;
		const Vector3D& currNormal = *normalIter;
		const Point2D& currTexCoord = *texIter;
		glNormal3f(currNormal[0], currNormal[1], currNormal[2]);
		glTexCoord2f(currTexCoord[0], currTexCoord[1]);
		glVertex3f(currVertex[0], currVertex[1], currVertex[2]);

	}
	glEnd();

/*
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &this->vertexStream[0]);

	if (this->normalStream.size() > 0) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, &this->normalStream[0]);
	}
	if (this->texCoordStream.size() > 0) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, &this->texCoordStream[0]);
	}

	glDrawRangeElements(this->polyType, 0, this->indices.size(), this->indices.size(), GL_UNSIGNED_SHORT, &this->indices[0]);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
*/
}

/**
 * Translate this polygon group by the given vector.
 */
void PolygonGroup::Translate(const Vector3D& t) {
	for (size_t i = 0; i < this->vertexStream.size(); ++i) {
		Point3D& currPt = this->vertexStream[i];
		currPt = currPt + t;
	}
}

void PolygonGroup::Transform(const Matrix4x4& m) {
	for (size_t i = 0; i < this->vertexStream.size(); ++i) {
		Point3D& currPt = this->vertexStream[i];
		currPt = m * currPt;
	}
	for (size_t i = 0; i < this->normalStream.size(); ++i) {
		Vector3D& currNormal = this->normalStream[i];
		currNormal = m * currNormal;
	}
}

void MaterialGroup::AddFaces(const PolyGrpIndexer& indexer, 
														 const std::vector<Point3D>& vertexStream, 
														 const std::vector<Vector3D>& normalStream,
														 const std::vector<Point2D>& texCoordStream) {
	if (this->polyGrp != NULL) {
		delete this->polyGrp;
	}
	if (this->displayListID != 0) {
		this->DeleteDisplayList();
	}

	this->polyGrp = new PolygonGroup(indexer, vertexStream, normalStream, texCoordStream);

	// Create a display list for the polygon group
	this->displayListID = glGenLists(1);
	glNewList(this->displayListID, GL_COMPILE);
	this->polyGrp->Draw();
	glEndList();
}

Mesh::Mesh(const std::string name, const std::map<std::string, MaterialGroup*> &matGrps):
name(name), matGrps(matGrps) {
}

Mesh::~Mesh(){
	this->Flush();
}

/**
 * Clean out all the stuff that currently makes up this mesh.
 */
void Mesh::Flush() {
	// Delete the material groups
	std::map<std::string, MaterialGroup*>::iterator matGrpIter = this->matGrps.begin();
	for (; matGrpIter != this->matGrps.end(); matGrpIter++) {
		delete matGrpIter->second;
		matGrpIter->second = NULL;
	}
	this->matGrps.clear();
}

/**
 * Set the overall colour of this mesh.
 */
void Mesh::SetColour(const Colour& c) {
	
	// Go through each of the material groups in this mesh and
	// change their diffuse colour.
	std::map<std::string, MaterialGroup*>::iterator matGrpIter = this->matGrps.begin();
	for (; matGrpIter != matGrps.end(); matGrpIter++) {
		CgFxAbstractMaterialEffect* currMaterial = matGrpIter->second->GetMaterial();
        currMaterial->SetDiffuseColour(c);
	}
}

void Mesh::SetAlpha(float alpha) {
    for (std::map<std::string, MaterialGroup*>::const_iterator iter = this->matGrps.begin();
         iter != this->matGrps.end(); ++iter) {

	    MaterialGroup* matGrp = iter->second;
        matGrp->GetMaterial()->SetAlphaMultiplier(alpha);
    }
}

/**
 * Set the texture for the material group of the given name.
 */
void Mesh::SetTextureForMaterial(const std::string& matGrpName, Texture* texToSet) {

	// Make sure the material group exists
	std::map<std::string, MaterialGroup*>::iterator foundMatGrpIter = this->matGrps.find(matGrpName);
	assert(foundMatGrpIter != this->matGrps.end());

	// Change the texture for the found material group
	MaterialGroup* foundMatGrp = foundMatGrpIter->second;
	assert(foundMatGrp != NULL);
	CgFxAbstractMaterialEffect* matEffect = foundMatGrp->GetMaterial();
    matEffect->SetDiffuseTexture(texToSet);
}

/**
 * Set the colour for the specified material group of the given name.
 */
void Mesh::SetColourForMaterial(const std::string& matGrpName, const Colour& c) {
	// Make sure the material group exists
	std::map<std::string, MaterialGroup*>::iterator foundMatGrpIter = this->matGrps.find(matGrpName);
	assert(foundMatGrpIter != this->matGrps.end());

	// Change the texture for the found material group
	MaterialGroup* foundMatGrp = foundMatGrpIter->second;
	assert(foundMatGrp != NULL);

	CgFxAbstractMaterialEffect* matEffect = foundMatGrp->GetMaterial();
    matEffect->SetDiffuseColour(c);
}

/**
 * Replace the one and only material group in this mesh with the given material group.
 */
void Mesh::ReplaceMaterial(CgFxAbstractMaterialEffect* replacementMat) {
	assert(replacementMat != NULL);
	assert(this->matGrps.size() == 1);
	this->matGrps.begin()->second->ReplaceMaterialEffect(replacementMat);
}