#include "Mesh.h"


void MaterialGroup::AddFaces(const PolyGrpIndexer& indexer, 
														 const std::vector<Point3D>& vertexStream, 
														 const std::vector<Vector3D>& normalStream,
														 const std::vector<Point2D>& texCoordStream ) {

	assert(indexer.normalIndices.size() == indexer.texCoordIndices.size());
	assert(indexer.normalIndices.size() == indexer.vertexIndices.size());
	assert(indexer.vertexIndices.size() == indexer.texCoordIndices.size());
		
	unsigned int numIndices = indexer.vertexIndices.size();
	unsigned int polygrpArrayLen = numIndices * INTERLEAVED_MULTIPLIER;
	this->polygroupArray = new float[polygrpArrayLen];
	
	// Go through each possible vertex and add it to the polygon group array
	for (unsigned int i = 0, j = 0; i < polygrpArrayLen; i += INTERLEAVED_MULTIPLIER, j++) {
		const Point3D& vertex		= vertexStream[indexer.vertexIndices[j]];
		const Vector3D& normal	= normalStream[indexer.normalIndices[j]];
		const Point2D& texCoord = texCoordStream[indexer.texCoordIndices[j]];

		this->polygroupArray[i]			= texCoord[0];
		this->polygroupArray[i + 1]	= texCoord[1];
		this->polygroupArray[i + 2]	= normal[0];
		this->polygroupArray[i + 3]	= normal[1];
		this->polygroupArray[i + 4]	= normal[2];
		this->polygroupArray[i + 5]	= vertex[0];
		this->polygroupArray[i + 6]	= vertex[1];
		this->polygroupArray[i + 7]	= vertex[2];
	}

	// Create a display list for the polygon group
	this->displayListID = glGenLists(1);
	glNewList(this->displayListID, GL_COMPILE);
	glInterleavedArrays(GL_INTERLEAVED_FORMAT, INTERLEAVED_STRIDE, this->polygroupArray);
	glDrawArrays(GL_TRIANGLES, 0, numIndices);
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
		CgFxEffect* currMaterial = matGrpIter->second->GetMaterial();
		MaterialProperties* currMatProps = currMaterial->GetProperties();
		currMatProps->diffuse = c;
	}

}