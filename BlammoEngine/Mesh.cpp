#include "Mesh.h"

// Copy constructor
PolygonGroup::PolygonGroup(const PolygonGroup& other) {
	this->polygroupArray = new float[other.polygroupArrayLength];
	for (unsigned int i = 0; i < other.polygroupArrayLength; i++) {
		this->polygroupArray[i] = other.polygroupArray[i];
	}
	this->polygroupArrayLength = other.polygroupArrayLength;
	this->numIndices = other.numIndices;
}

PolygonGroup::PolygonGroup(const PolyGrpIndexer& faceIndexer, const std::vector<Point3D>& vertexStream, 
													 const std::vector<Vector3D>& normalStream, const std::vector<Point2D>& texCoordStream) {
	
	assert(faceIndexer.normalIndices.size() == faceIndexer.texCoordIndices.size());
	assert(faceIndexer.normalIndices.size() == faceIndexer.vertexIndices.size());
	assert(faceIndexer.vertexIndices.size() == faceIndexer.texCoordIndices.size());

	this->numIndices = faceIndexer.vertexIndices.size();
	this->polygroupArrayLength = numIndices * INTERLEAVED_MULTIPLIER;
	this->polygroupArray = new float[this->polygroupArrayLength];
	
	// Go through each possible vertex and add it to the polygon group array
	for (unsigned int i = 0, j = 0; i < this->polygroupArrayLength; i += INTERLEAVED_MULTIPLIER, j++) {
		const Point3D& vertex		= vertexStream[faceIndexer.vertexIndices[j]];
		const Vector3D& normal	= normalStream[faceIndexer.normalIndices[j]];
		const Point2D& texCoord = texCoordStream[faceIndexer.texCoordIndices[j]];

		this->polygroupArray[i]			= texCoord[0];
		this->polygroupArray[i + 1]	= texCoord[1];
		this->polygroupArray[i + 2]	= normal[0];
		this->polygroupArray[i + 3]	= normal[1];
		this->polygroupArray[i + 4]	= normal[2];
		this->polygroupArray[i + 5]	= vertex[0];
		this->polygroupArray[i + 6]	= vertex[1];
		this->polygroupArray[i + 7]	= vertex[2];
	}
}

/**
 * Translate this polygon group by the given vector.
 */
void PolygonGroup::Translate(const Vector3D& t) {
	// Only need to transform the vertices
	for (unsigned int i = 0; i < this->polygroupArrayLength; i += INTERLEAVED_MULTIPLIER) {
		this->polygroupArray[i + 5] += t[0];
		this->polygroupArray[i + 6] += t[1];
		this->polygroupArray[i + 7] += t[2];
	}
}

void MaterialGroup::AddFaces(const PolyGrpIndexer& indexer, 
														 const std::vector<Point3D>& vertexStream, 
														 const std::vector<Vector3D>& normalStream,
														 const std::vector<Point2D>& texCoordStream) {

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
		CgFxMaterialEffect* currMaterial = matGrpIter->second->GetMaterial();
		MaterialProperties* currMatProps = currMaterial->GetProperties();
		currMatProps->diffuse = c;
	}
}

/**
 * Set the texture for the material group of the given name.
 */
void Mesh::SetTextureForMaterial(const std::string& matGrpName, Texture2D* texToSet) {

	// Make sure the material group exists
	std::map<std::string, MaterialGroup*>::iterator foundMatGrpIter = this->matGrps.find(matGrpName);
	assert(foundMatGrpIter != this->matGrps.end());

	// Change the texture for the found material group
	MaterialGroup* foundMatGrp = foundMatGrpIter->second;
	assert(foundMatGrp != NULL);
	CgFxMaterialEffect* matEffect = foundMatGrp->GetMaterial();
	MaterialProperties* matProps = matEffect->GetProperties();
	matProps->diffuseTexture = texToSet;
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

	CgFxMaterialEffect* matEffect = foundMatGrp->GetMaterial();
	MaterialProperties* matProps = matEffect->GetProperties();
	matProps->diffuse = c;
}