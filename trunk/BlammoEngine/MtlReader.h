/**
 * MtlReader.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __MTLREADER_H__
#define __MTLREADER_H__

#include "BasicIncludes.h"
#include "Colour.h"
#include "Texture2D.h"

class CgFxMaterialEffect;

/**
 * Reads a material file for an .obj defined mesh.
 */
class MtlReader {
private:

	// Typical MTL statements
	static const char* MTL_NEWMATERIAL;
	static const char* MTL_AMBIENT;
	static const char* MTL_DIFFUSE;
	static const char* MTL_SPECULAR;
	static const char* MTL_SHININESS;
	static const char* MTL_DIFF_TEXTURE;

	// Custom MTL statements
	static const char* CUSTOM_MTL_MATTYPE;
	static const char* CUSTOM_MTL_OUTLINESIZE;
	static const char* CUSTOM_MTL_GEOMETRYTYPE;

	MtlReader();

public:
	~MtlReader();

	static std::map<std::string, CgFxMaterialEffect*> ReadMaterialFileFromStream(const std::string &filepath, std::istream &inFile);
	static std::map<std::string, CgFxMaterialEffect*> ReadMaterialFile(const std::string &filepath);
	static std::map<std::string, CgFxMaterialEffect*> ReadMaterialFile(const std::string &filepath, PHYSFS_File* fileHandle);
	
};

#endif