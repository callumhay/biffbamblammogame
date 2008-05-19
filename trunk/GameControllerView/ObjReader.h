#ifndef __OBJREADER_H__
#define __OBJREADER_H__

#include <string>

class Mesh;

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

public:
	static Mesh* ReadMesh(const std::string &filepath);

};
#endif