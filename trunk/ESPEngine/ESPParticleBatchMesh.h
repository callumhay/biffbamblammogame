#ifndef __ESPPARTICLEBATCHMESH_H__
#define __ESPPARTICLEBATCHMESH_H__

#include "ESPUtil.h"

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Camera.h"

class ESPParticle;

class Texture2D;
class CgFxEffectBase;


/**
 * The composition mesh is meant to provide a very fast way of transferring lots
 * of geometry to the GPU via VBOs with lots of parts that can be updated fast.
 */
/*
class BatchMesh {
public:
	enum MeshDataType { VertexData = 0, NormalData = 1, TexCoordData = 2, ColourData = 3 };

private:
	// VBOs
	std::vector<GLuint> vertexBufferObjs;

	// VBO Buffer data arrays in client memory
	std::vector<Point3D>  vertexData;
	std::vector<Vector3D> normalData;
	std::vector<Point2D>  texCoordData;
	std::vector<Colour4D> colourData;
	
	// VBO usage hints for the buffers
	GLenum vertexHint, normalHint, texCoordHint, colourHint;


public:
	BatchMesh();
	~BatchMesh();

	void SetUsageHints(GLenum vertexHint, GLenum normalHint, GLenum texCoordHint, GLenum colourHint) {
		this->vertexHint = vertexHint;
		this->normalHint = normalHint;
		this->texCoordHint = texCoordHint;
		this->colourHint = colourHint;
	}


	void Flush();
	void Draw();

};
*/

class ESPParticleBatchMesh {
	
private:
	enum MeshDataType { VertexData = 0, NormalData = 1, TexCoordData = 2, ColourData = 3 };
	std::vector<GLuint> vertexBufferObjs;

public:
	ESPParticleBatchMesh();
	~ESPParticleBatchMesh();

	void DrawParticleBatch(const Camera& camera, const ESP::ESPAlignment alignment, std::list<ESPParticle*> aliveParticles /*, Texture2D* texture */);
	//void DrawParticleBatch(const Camera& camera, const ESP::ESPAlignment alignment, std::list<ESPParticle*> aliveParticles /*, CgFxBaseEffect* effect*/);



};

#endif