#ifndef __VBOBATCH_H__
#define __VBOBATCH_H__

#include "BasicIncludes.h"
#include "Colour.h"

class Matrix4x4;
class PolygonGroup;

/**
 * The VBOBatch class is a glorified material group - it acts as a method of tracking
 * multiple groupings of geometry data which all share the same material. Geometry data
 * can be added and removed as needed through its public members.
 */
class VBOBatch {
private:

	/**
	 * Holds the location and size data for a particular ID.
	 */
	struct IDData {
		GLintptr vertexOffset, normalOffset, texCoordOffset, colourOffset;
		GLsizeiptr vertexSize, normalSize, texCoordSize, colourSize;
	};

	// Types of vbo buffer data that exist - these are indices into the array for the vbo buffers
	static const GLuint NUM_VBO_BUFFERS = 5;
	enum VBOBufferType { VertexData = 0, NormalData = 1, TexCoordData = 2, ColourData = 3, IndexData = 4 };
	
	unsigned int nextID;							// The next ID that will be issued for added geometry
	std::vector<GLuint> vboBuffers;		// The actual vbo buffer ids stored in memory
	unsigned int numDrawIndices;			// The number of indices to draw in this vbo batch

	std::map<unsigned int, IDData> idMapping;	// Maps the ID for specific geometry to data used to locate it within the VBO

public:
	static const unsigned int VBO_BATCH_ERROR = 0;

	VBOBatch();
	~VBOBatch();

	std::vector<unsigned int> SetInitialGeometryData(const std::vector<PolygonGroup*>& polyGrps, 
																									 const std::vector<ColourRGBA>& colours, 
																									 const std::vector<Matrix4x4>& transforms);
	//unsigned int AddGeometryData
	bool RemoveGeometryData(unsigned int id);

	/**
	 * Draw the raw geometry for this VBO Batch.
	 */
	inline void Draw() const {
		// Enable the client state for the vbo arrays
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		// Bind appropriate buffers for this VBO...
		glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffers[VBOBatch::VertexData]);
		glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffers[VBOBatch::NormalData]);
		glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffers[VBOBatch::TexCoordData]);
		glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, this->vboBuffers[VBOBatch::ColourData]);
		glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(0));
		
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboBuffers[VBOBatch::IndexData]);
		glDrawArrays(GL_TRIANGLES, 0, this->numDrawIndices);// GL_UNSIGNED_INT, BUFFER_OFFSET(0));

		// Disable the client state for the vbo arrays and unbind vbo
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


};
#endif