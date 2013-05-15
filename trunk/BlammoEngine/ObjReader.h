/**
 * ObjReader.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __OBJREADER_H__
#define __OBJREADER_H__

#include "BasicIncludes.h"

class Mesh;
class PolygonGroup;

class ObjReader {

private:

	static const char* OBJ_EXTENSION;
	static const char* MTL_EXTENSION;

	static const char* OBJ_MTLFILE;
	static const char* OBJ_USE_MATERIAL;
	static const char* OBJ_GROUP;
	static const char* OBJ_VERTEX_COORD;
	static const char* OBJ_VERTEX_NORMAL;
	static const char* OBJ_VERTEX_TEXCOORD;
	static const char* OBJ_FACE;

	ObjReader();

public:
	static Mesh* ReadMeshFromStream(const std::string &filepath, std::istream &inFile);
	static Mesh* ReadMesh(const std::string &filepath);
	static Mesh* ReadMesh(const std::string &filepath, PHYSFS_File* fileHandle);

	static PolygonGroup* ReadPolygonGroup(const std::string &filepath);

};
#endif