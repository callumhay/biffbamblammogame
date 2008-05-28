#include "MtlReader.h"
#include "Texture2D.h"

#include "../Utils/Includes.h"

#include <iostream>
#include <fstream>

const std::string MtlReader::MTL_NEWMATERIAL	= "newmtl";
const std::string MtlReader::MTL_AMBIENT			= "Ka";
const std::string MtlReader::MTL_DIFFUSE			= "Kd";
const std::string MtlReader::MTL_SPECULAR			= "Ks";
const std::string MtlReader::MTL_SHININESS		= "Ns";
const std::string MtlReader::MTL_DIFF_TEXTURE	= "map_Kd";

MtlReader::MtlReader() {
}

MtlReader::~MtlReader() {
}

/**
 * Read a .mtl file and create a celshading material from it.
 * filepath should be to an .mtl file on disk.
 * Postcondition: Returns a map of celshadingmaterials with the keys equal
 * to their respective names if all goes well, NULL otherwise.
 */
std::map<std::string, CelShadingMaterial*> MtlReader::ReadMaterialFile(const std::string &filepath) {
	std::map<std::string, CelShadingMaterial*> materials;
	
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
			materials[matName] = new CelShadingMaterial();
		}
		else if (currStr == MTL_AMBIENT) {
			// Ignore this for now...
		}
		else if (currStr == MTL_DIFFUSE) {
			Colour diff;
			if (!(inFile >> diff[0] && inFile >> diff[1] && inFile >> diff[2])) {
				debug_output("ERROR: could not read diffuse colour properly from mtl file: " << filepath); 
				return materials;				
			}
			materials[matName]->SetDiffuse(diff);
		}
		else if (currStr == MTL_SPECULAR) {
			Colour spec;
			if (!(inFile >> spec[0] && inFile >> spec[1] && inFile >> spec[2])) {
				debug_output("ERROR: could not read specular colour properly from mtl file: " << filepath); 
				return materials;				
			}
			materials[matName]->SetSpecular(spec);		
		}
		else if (currStr == MTL_SHININESS) {
			float shininess;
			if (!(inFile >> shininess)) {
				debug_output("ERROR: could not read shininess properly from mtl file: " << filepath); 
				return materials;		
			}
			materials[matName]->SetShininess(shininess);
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
			materials[matName]->SetTexture(texturePath);
		}
	}

	return materials;
}