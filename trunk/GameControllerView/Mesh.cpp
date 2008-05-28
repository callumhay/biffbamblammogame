#include "Mesh.h"

Mesh::Mesh(const std::string name, const std::vector<Point3D> &verts, const std::vector<Vector3D> &norms,
					 const std::vector<Point2D> &texCoords, const std::map<std::string, MaterialGroup*> &matGrps):
name(name), vertices(verts), normals(norms), texCoords(texCoords), matGrps(matGrps), displayListID(0) {
	this->CompileDisplayList();
}

Mesh::~Mesh(){
	glDeleteLists(this->displayListID, 1);
	this->vertices.clear();
	this->texCoords.clear();
	this->normals.clear();

	// Delete the material groups
	std::map<std::string, MaterialGroup*>::iterator matGrpIter = this->matGrps.begin();
	for (; matGrpIter != this->matGrps.end(); matGrpIter++) {
		delete matGrpIter->second;
	}
	this->matGrps.clear();
}

/*
 * Create a display list that draws this mesh.
 * Precondition: This mesh has been initialized with appropriate indices etc.
 */
void Mesh::CompileDisplayList() {
	// Create a display list for the mesh
	this->displayListID = glGenLists(1);
	glNewList(this->displayListID, GL_COMPILE);


	// for each material group...
	std::map<std::string, MaterialGroup*>::iterator matGrpIter = this->matGrps.begin();
	for (; matGrpIter != this->matGrps.end(); matGrpIter++) {
		MaterialGroup* currMatGrp = matGrpIter->second;
		currMatGrp->material->DrawMaterial();
		
		glBegin(GL_TRIANGLES);
		// for each face in the material group
		for (size_t j = 0; j < currMatGrp->faces.size(); j++) {
			TriFace currFace = currMatGrp->faces[j];
			
			for (int k = 0; k < 3; k++) {
				Point3D &currVert = this->vertices[currFace.vertexIndices[k]];
				Vector3D &currNorm = this->normals[currFace.normalIndices[k]];
				Point2D &currTexCoord = this->texCoords[currFace.texCoordIndices[k]];

				glNormal3f(currNorm[0], currNorm[1], currNorm[2]);
				glTexCoord2f(currTexCoord[0], currTexCoord[1]);
				glVertex3f(currVert[0], currVert[1], currVert[2]);
			}
		}
		glEnd();
	}

	glEndList();
}

/*
 * Calculates the max size of this mesh along the x, y and z axis.
 * Precondition: This mesh has been initialized with appropriate indices etc.
 * Return: A vector with the max sizes along each of the respective axes.
 */
Vector3D Mesh::CalculateDimensions() {
	if (this->vertices.size() < 2) {
		return Vector3D(0.0f,0.0f,0.0f);
	}

	Vector3D minVals(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3D maxVals(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	for (size_t i = 0; i < this->vertices.size(); i++) {
		Point3D currVertex = this->vertices[i];	

		for (int j = 0; j < 3; j++) {
			minVals[j] = min(minVals[j], currVertex[j]);
			maxVals[j] = max(maxVals[j], currVertex[j]);
		}
	}

	return maxVals - minVals;
}