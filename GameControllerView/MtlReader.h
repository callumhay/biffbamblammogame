#ifndef __MTLREADER_H__
#define __MTLREADER_H__

#include <string>
#include <map>

#include "CgFxCelShading.h"

/**
 * Reads a material file for an .obj defined mesh.
 */
class MtlReader {
private:

	static const std::string MTL_NEWMATERIAL;
	static const std::string MTL_AMBIENT;
	static const std::string MTL_DIFFUSE;
	static const std::string MTL_SPECULAR;
	static const std::string MTL_SHININESS;
	static const std::string MTL_DIFF_TEXTURE;

	MtlReader();

public:
	~MtlReader();
	static std::map<std::string, CgFxCelShading*> ReadMaterialFile(const std::string &filepath);
	
};

#endif