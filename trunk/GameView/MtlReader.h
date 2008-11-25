#ifndef __MTLREADER_H__
#define __MTLREADER_H__

#include "../Utils/Colour.h"
#include "Texture2D.h"

#include <string>
#include <map>

class CgFxEffect;

/**
 * Reads a material file for an .obj defined mesh.
 */
class MtlReader {
private:

	// Typical MTL statements
	static const std::string MTL_NEWMATERIAL;
	static const std::string MTL_AMBIENT;
	static const std::string MTL_DIFFUSE;
	static const std::string MTL_SPECULAR;
	static const std::string MTL_SHININESS;
	static const std::string MTL_DIFF_TEXTURE;

	// Custom MTL statements
	static const std::string CUSTOM_MTL_MATTYPE;
	static const std::string CUSTOM_MTL_OUTLINESIZE;

	MtlReader();

public:
	~MtlReader();

	static std::map<std::string, CgFxEffect*> ReadMaterialFile(const std::string &filepath);
	
};

#endif