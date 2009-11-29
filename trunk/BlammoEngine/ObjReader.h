/**
 * ObjReader.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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