#ifndef __OBJREADER_H__
#define __OBJREADER_H__

#include "BasicIncludes.h"

class Mesh;
class PolygonGroup;

class ObjReader {

private:

	static const std::string OBJ_EXTENSION;
	static const std::string MTL_EXTENSION;

	static const std::string OBJ_MTLFILE;
	static const std::string OBJ_USE_MATERIAL;
	static const std::string OBJ_GROUP;
	static const std::string OBJ_VERTEX_COORD;
	static const std::string OBJ_VERTEX_NORMAL;
	static const std::string OBJ_VERTEX_TEXCOORD;
	static const std::string OBJ_FACE;

	ObjReader();

	static Mesh* ReadMeshFromStream(const std::string &filepath, std::istream &inFile);

public:

	static Mesh* ReadMesh(const std::string &filepath);
	static Mesh* ReadMesh(const std::string &filepath, PHYSFS_File* fileHandle);

	static PolygonGroup* ReadPolygonGroup(const std::string &filepath);

};
#endif