/**
 * ObjReader.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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