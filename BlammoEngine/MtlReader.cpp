#include "MtlReader.h"
#include "Texture2D.h"
#include "CgFxEffect.h"
#include "../GameView/CgFxCelShading.h"
#include "../GameView/CgFxOutlinedPhong.h"

#include "BasicIncludes.h"

#include <iostream>
#include <fstream>

const std::string MtlReader::MTL_NEWMATERIAL	= "newmtl";
const std::string MtlReader::MTL_AMBIENT			= "Ka";
const std::string MtlReader::MTL_DIFFUSE			= "Kd";
const std::string MtlReader::MTL_SPECULAR			= "Ks";
const std::string MtlReader::MTL_SHININESS		= "Ns";
const std::string MtlReader::MTL_DIFF_TEXTURE	= "map_Kd";

// Custom tags
const std::string MtlReader::CUSTOM_MTL_MATTYPE			= "type";
const std::string MtlReader::CUSTOM_MTL_OUTLINESIZE = "outlinesize";

MtlReader::MtlReader() {
}

MtlReader::~MtlReader() {
}

/**
 * Read a .mtl file and create a celshading material from it.
 * filepath should be to an .mtl file on disk.
 * Postcondition: Returns a map of CgFxCelShading with the keys equal
 * to their respective names if all goes well, NULL otherwise.
 */
std::map<std::string, CgFxMaterialEffect*> MtlReader::ReadMaterialFile(const std::string &filepath) {
	
	
	std::map<std::string, CgFxMaterialEffect*> materials;
	std::map<std::string, MaterialProperties*> matProperties;
	
	// Start reading in the file
	std::ifstream inFile;
	inFile.open(filepath.c_str());
	
	// Make sure the file opened properly
	if (!inFile.is_open()) {
		debug_output("ERROR: Could not open file: " << filepath); 
		return materials;
	}

	std::string currStr;
	std::string matName = "";

	while (inFile >> currStr) {

		if (currStr == MTL_NEWMATERIAL) {
			// Read the material name
			if (!(inFile >> matName)) {
				debug_output("ERROR: Material name not provided with proper syntax in mtl file: " << filepath); 
				return materials;
			}
			matProperties[matName] = new MaterialProperties();
		}
		else if (currStr == MTL_AMBIENT) {
			// Ignore this
		}
		else if (currStr == MTL_DIFFUSE) {
			Colour diff;
			if (!(inFile >> diff[0] && inFile >> diff[1] && inFile >> diff[2])) {
				debug_output("ERROR: could not read diffuse colour properly from mtl file: " << filepath); 
				return materials;				
			}
			matProperties[matName]->diffuse = diff;
		}
		else if (currStr == MTL_SPECULAR) {
			Colour spec;
			if (!(inFile >> spec[0] && inFile >> spec[1] && inFile >> spec[2])) {
				debug_output("ERROR: could not read specular colour properly from mtl file: " << filepath); 
				return materials;				
			}
			matProperties[matName]->specular = spec;		
		}
		else if (currStr == MTL_SHININESS) {
			float shininess;
			if (!(inFile >> shininess)) {
				debug_output("ERROR: could not read shininess properly from mtl file: " << filepath); 
				return materials;		
			}
			matProperties[matName]->shininess = shininess;
		}
		else if (currStr == MTL_DIFF_TEXTURE) {
			// Read in the path to the texture file
			std::string texturePath;
			if (!(inFile >> texturePath)) {
				debug_output("ERROR: could not read texture path properly from mtl file: " << filepath); 
				return materials;
			}

			// Concatenate the directory to the texture
			size_t pos = filepath.find_last_of("/");
			if (pos == std::string::npos) {
				pos = filepath.find_last_of("\\");
			}
			texturePath.insert(0, filepath.substr(0, pos+1));

			// Create the texture and add it to the material on success
			// TODO: options for texture filtering...
			matProperties[matName]->diffuseTexture = Texture2D::CreateTexture2DFromImgFile(texturePath, Texture::Trilinear);
		}
		else if (currStr == CUSTOM_MTL_MATTYPE) {
			std::string matTypeName;
			if (!(inFile >> matTypeName)) {
				debug_output("ERROR: could not read material type properly from mtl file: " << filepath); 
				return materials;
			}
			matProperties[matName]->materialType = matTypeName;
		}
		else if (currStr == CUSTOM_MTL_OUTLINESIZE) {
			float outlineSize = 1.0f;
			if (!(inFile >> outlineSize)) {
				debug_output("ERROR: could not read material outline size properly from mtl file: " << filepath); 
				return materials;
			}
			matProperties[matName]->outlineSize = outlineSize;
		}
	}

	// Create materials with the corresponding properties
	std::map<std::string, MaterialProperties*>::iterator matPropIter;
	for (matPropIter = matProperties.begin(); matPropIter != matProperties.end(); matPropIter++) {
		CgFxMaterialEffect* currMaterial = NULL;
	
		// Figure out what material we should be using for each material group
		if (matPropIter->second->materialType == MaterialProperties::MATERIAL_CELBASIC_TYPE) {
			currMaterial = new CgFxCelShading(matPropIter->second);
		}
		else if (matPropIter->second->materialType == MaterialProperties::MATERIAL_CELPHONG_TYPE) {
			currMaterial = new CgFxOutlinedPhong(matPropIter->second);
		}
		else {
			// Default to using the cel shader for now
			currMaterial = new CgFxCelShading(matPropIter->second);
		}

		// Inline: The material is set
		assert(currMaterial != NULL);
		materials[matPropIter->first] = currMaterial;
	}

	return materials;
}