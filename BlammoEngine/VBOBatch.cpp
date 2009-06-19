#include "VBOBatch.h"

#include "Matrix.h"
#include "Mesh.h"

VBOBatch::VBOBatch() : vboBuffers(VBOBatch::NUM_VBO_BUFFERS), nextID(1), numDrawIndices(0) {
	// Setup the vbo buffers
	assert(this->vboBuffers.size() == VBOBatch::NUM_VBO_BUFFERS);
	glGenBuffers(VBOBatch::NUM_VBO_BUFFERS, &this->vboBuffers[0]);

	debug_opengl_state();
}

VBOBatch::~VBOBatch() {
	// Clear / release the vbo buffers
	glDeleteBuffers(VBOBatch::NUM_VBO_BUFFERS, &this->vboBuffers[0]);
	this->vboBuffers.clear();

	this->idMapping.clear();

	debug_opengl_state();
}

/**
 * Add geometry data to this vbo batch - this will add the geometry for the number of transforms given,
 * so that each will be drawn with the same material as the rest of this batch.
 * The given colour will be applied to the geometry data as well.
 * Returns: An array of IDs that index the geometry given within this batch - these
 * IDs can be used later to remove / manipulate the geometry.
 */
std::vector<unsigned int> VBOBatch::SetInitialGeometryData(const std::vector<PolygonGroup*>& polyGrps, 
																													 const std::vector<ColourRGBA>& colours, 
																													 const std::vector<Matrix4x4>& transforms) {
	assert(transforms.size() > 0);
	assert(transforms.size() == colours.size());
	assert(colours.size() == polyGrps.size());

	std::vector<float> vertexDataArray;
	std::vector<float> normalDataArray;
	std::vector<float> texCoordDataArray;
	std::vector<float> colourDataArray;
	std::vector<unsigned int> indexDataArray;

	unsigned int totalVertices = 0;

	// Create an array for storing the IDs that will be generated
	std::vector<unsigned int> assignedIDs;
	assignedIDs.reserve(colours.size());

	// Grab the geometry data in the polygon group and for each transform we append
	// the transformed polygon group data into a set of larger arrays of geometry data
	std::vector<Matrix4x4>::const_iterator xFormIter = transforms.begin();
	std::vector<ColourRGBA>::const_iterator colourIter = colours.begin(); 
	std::vector<PolygonGroup*>::const_iterator polyGrpIter = polyGrps.begin(); 

	for (; xFormIter != transforms.end(); ++colourIter, ++xFormIter, ++polyGrpIter) {
		
		std::vector<float> currVertexDataArray;
		std::vector<float> currNormalDataArray;
		std::vector<float> currTexCoordDataArray;

		// Temporarily transform the polygon group with the current transform
		// then obtain its geometry data
		const Matrix4x4& currTransform	= *xFormIter;
		const ColourRGBA& currColour		= *colourIter;
		const PolygonGroup* currPolyGrp	= *polyGrpIter;

		unsigned int numVerts = currPolyGrp->GetDataArrays(currVertexDataArray, currNormalDataArray, currTexCoordDataArray);

		// Transform the geometry data and insert it into the larger data arrays
		for (unsigned int i = 0; i < currVertexDataArray.size(); i += 3) {
			Point3D tempVert(currVertexDataArray[i], currVertexDataArray[i + 1], currVertexDataArray[i + 2]);
			Point3D resultVert = currTransform * tempVert;

			currVertexDataArray[i]			= resultVert[0]; 
			currVertexDataArray[i + 1]	= resultVert[1];
			currVertexDataArray[i + 2]	= resultVert[2];
		}

		for (unsigned int i = 0; i < currNormalDataArray.size(); i += 3) {
			Vector3D tempNorm(currNormalDataArray[i], currNormalDataArray[i + 1], currNormalDataArray[i + 2]);
			Vector3D resultNorm = currTransform * tempNorm;

			currNormalDataArray[i]			= resultNorm[0];
			currNormalDataArray[i + 1]	= resultNorm[1];
			currNormalDataArray[i + 2]	= resultNorm[2];
		}

		// Make sure we keep track of where exactly we are inserting the new data
		IDData currLocData;
		currLocData.vertexOffset		= vertexDataArray.size() * sizeof(float);
		currLocData.normalOffset		= normalDataArray.size() * sizeof(float);
		currLocData.texCoordOffset	= texCoordDataArray.size() * sizeof(float);
		currLocData.colourOffset		= colourDataArray.size() * sizeof(float);

		currLocData.vertexSize		= currVertexDataArray.size() * sizeof(float);
		currLocData.normalSize		= currNormalDataArray.size() * sizeof(float);
		currLocData.texCoordSize	= currTexCoordDataArray.size() * sizeof(float);
		currLocData.colourSize		= 4 * numVerts * sizeof(float);

		// Insert the new geometry data...
		vertexDataArray.reserve(vertexDataArray.size() + currVertexDataArray.size());
		normalDataArray.reserve(normalDataArray.size() + currNormalDataArray.size());
		texCoordDataArray.reserve(texCoordDataArray.size() + currTexCoordDataArray.size());
		colourDataArray.reserve(colourDataArray.size() + 4 * numVerts);

		vertexDataArray.insert(vertexDataArray.end(), currVertexDataArray.begin(), currVertexDataArray.end());
		normalDataArray.insert(normalDataArray.end(), currNormalDataArray.begin(), currNormalDataArray.end());
		texCoordDataArray.insert(texCoordDataArray.end(), currTexCoordDataArray.begin(), currTexCoordDataArray.end());

		for (unsigned int i = 0; i < numVerts; i++) {
			colourDataArray.push_back(currColour.R());
			colourDataArray.push_back(currColour.G());
			colourDataArray.push_back(currColour.B());
			colourDataArray.push_back(currColour.A());
		}

		totalVertices += numVerts;

		// Keep track of the geometry by assigning an ID to it
		unsigned int currID = this->nextID;
		++this->nextID;

		assignedIDs.push_back(currID);
		this->idMapping.insert(std::make_pair(currID, currLocData));
	}

	assert(vertexDataArray.size() > 0);
	assert(normalDataArray.size() > 0);
	assert(texCoordDataArray.size() > 0);

	// Setup the indices (this is easy since its just a sequential set of numbers from 0 to totalVertices)
	assert(totalVertices <= UINT_MAX);
	for (unsigned int i = 0; i < totalVertices; i++) {
		indexDataArray.push_back(i);
	}

	// Bind the vbo buffers and set the data for each one, respectively:
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// Vertex data
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffers[VBOBatch::VertexData]);
	glBufferData(GL_ARRAY_BUFFER, vertexDataArray.size() * sizeof(float), &vertexDataArray[0], GL_STATIC_DRAW);
	glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
	
	// Normal data
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffers[VBOBatch::NormalData]);
	glBufferData(GL_ARRAY_BUFFER, normalDataArray.size() * sizeof(float), &normalDataArray[0], GL_STATIC_DRAW);
	glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));
	
	// Texture coordinate data
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffers[VBOBatch::TexCoordData]);
	glBufferData(GL_ARRAY_BUFFER, texCoordDataArray.size() * sizeof(float), &texCoordDataArray[0], GL_STATIC_DRAW);
	glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

	// Colour data
	glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffers[VBOBatch::ColourData]);
	glBufferData(GL_ARRAY_BUFFER, colourDataArray.size() * sizeof(float), &colourDataArray[0], GL_STATIC_DRAW);
	glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(0));

	// Index data
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboBuffers[VBOBatch::IndexData]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataArray.size() * sizeof(unsigned int), &indexDataArray[0], GL_STATIC_DRAW);

	this->numDrawIndices = indexDataArray.size();

	// Unbind all the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	return assignedIDs;
}

/**
 * Removes the given id corresponding to geometry data from this VBO Batch.
 * Returns: true if there is still geometry in this batch, false otherwise.
 */
bool VBOBatch::RemoveGeometryData(unsigned int id) {
	// TODO
	assert(false);
	return false;
}