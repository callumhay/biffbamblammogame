/**
 * MtlReader.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
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

#include "MtlReader.h"
#include "Texture2D.h"
#include "CgFxEffect.h"

#include "../GameView/CgFxCelShading.h"
#include "../GameView/CgFxPhong.h"
#include "../GameView/CgFxPrism.h"

#include "BasicIncludes.h"

#include <iostream>
#include <fstream>

const char* MtlReader::MTL_NEWMATERIAL	= "newmtl";
const char* MtlReader::MTL_AMBIENT			= "Ka";
const char* MtlReader::MTL_DIFFUSE			= "Kd";
const char* MtlReader::MTL_SPECULAR			= "Ks";
const char* MtlReader::MTL_SHININESS		= "Ns";
const char* MtlReader::MTL_DIFF_TEXTURE	= "map_Kd";

// Custom tags
const char* MtlReader::CUSTOM_MTL_MATTYPE				= "type";
const char* MtlReader::CUSTOM_MTL_OUTLINESIZE		= "outlinesize";
const char* MtlReader::CUSTOM_MTL_GEOMETRYTYPE	= "geomtype";

MtlReader::MtlReader() {
}

MtlReader::~MtlReader() {
}

/**
 * Read a .mtl file and create a celshading material from it.
 * filepath should be to an .mtl file on disk.
 * Returns: Returns a map of CgFxCelShading with the keys equal
 * to their respective names if all goes well, empty map otherwise.
 */
std::map<std::string, CgFxMaterialEffect*> MtlReader::ReadMaterialFile(const std::string &filepath) {
	// Start reading in the file
	std::ifstream inFile;
	inFile.open(filepath.c_str());
	
	std::map<std::string, CgFxMaterialEffect*> materials;

	// Make sure the file opened properly
	if (!inFile.is_open()) {
		debug_output("ERROR: Could not open file: " << filepath);
		assert(false);
		return materials;
	}

	materials = MtlReader::ReadMaterialFileFromStream(filepath, inFile);
	inFile.close();

	return materials;
}

/**
 * Read a .mtl file using the given physfs file handle and create a celshading material from it.
 * Returns: Returns a map of CgFxCelShading with the keys equal
 * to their respective names if all goes well, empty map otherwise.
 */
std::map<std::string, CgFxMaterialEffect*> MtlReader::ReadMaterialFile(const std::string &filepath, PHYSFS_File* fileHandle) {
	// Get a byte buffer of the entire file and convert it into a string stream so it
	// can be read in using the STL
	PHYSFS_sint64 fileLength = PHYSFS_fileLength(fileHandle);
	char* fileBuffer = new char[fileLength];
	
	int readResult = PHYSFS_read(fileHandle, fileBuffer, sizeof(char), fileLength);
	if (readResult == 0) {
		delete[] fileBuffer;
		fileBuffer = NULL;
		debug_output("Error reading mtl file to bytes: " << filepath);
		assert(false);
		std::map<std::string, CgFxMaterialEffect*> materials;
		return materials;
	}

	// Convert the bytes to a string stream 
	std::istringstream fileSS(std::string(fileBuffer), std::ios_base::in | std::ios_base::binary);
	delete[] fileBuffer;
	fileBuffer = NULL;

	return MtlReader::ReadMaterialFileFromStream(filepath, fileSS);
}

/**
 * Private helper function that does most of the work using a given istream
 * representing the file information.
 * Returns: list of material effects on success, empty map otherwise.
 */
std::map<std::string, CgFxMaterialEffect*> MtlReader::ReadMaterialFileFromStream(const std::string &filepath, std::istream &inFile) {
    UNUSED_PARAMETER(filepath);

	std::map<std::string, CgFxMaterialEffect*> materials;
	std::map<std::string, MaterialProperties*> matProperties;
	
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

			// Create the texture and add it to the material on success
			// TODO: options for texture filtering...
			matProperties[matName]->diffuseTexture = ResourceManager::GetInstance()->GetImgTextureResource(texturePath, Texture::Trilinear, GL_TEXTURE_2D);
		}
		else if (currStr == CUSTOM_MTL_MATTYPE) {
			// Material type definition
			std::string matTypeName;
			if (!(inFile >> matTypeName)) {
				debug_output("ERROR: could not read material type properly from mtl file: " << filepath);
				return materials;
			}
			matProperties[matName]->materialType = matTypeName;
		}
		else if (currStr == CUSTOM_MTL_OUTLINESIZE) {
			// Outline size definition
			float outlineSize = 1.0f;
			if (!(inFile >> outlineSize)) {
				debug_output("ERROR: could not read material outline size properly from mtl file: " << filepath);
				return materials;
			}
			matProperties[matName]->outlineSize = outlineSize;
		}
		else if (currStr == CUSTOM_MTL_GEOMETRYTYPE) {
			// Geometry type (foreground/background)
			std::string geomTypeName;
			if (!(inFile >> geomTypeName)) {
				debug_output("ERROR: could not read geometry type properly from mtl file: " << filepath);
				return materials;
			}
			matProperties[matName]->geomType = geomTypeName;
		}
	}

	// Create materials with the corresponding properties
	std::map<std::string, MaterialProperties*>::iterator matPropIter;
	for (matPropIter = matProperties.begin(); matPropIter != matProperties.end(); ++matPropIter) {
		CgFxMaterialEffect* currMaterial = NULL;
	
		// Figure out what material we should be using for each material group
		if (matPropIter->second->materialType == MaterialProperties::MATERIAL_CELBASIC_TYPE) {
			currMaterial = new CgFxCelShading(matPropIter->second);
		}
		else if (matPropIter->second->materialType == MaterialProperties::MATERIAL_PHONG_TYPE) {
			currMaterial = new CgFxPhong(matPropIter->second);
		}
		else if (matPropIter->second->materialType == MaterialProperties::MATERIAL_PRISM_TYPE) {
			currMaterial = new CgFxPrism(matPropIter->second);
		}
        else if (matPropIter->second->materialType == MaterialProperties::MATERIAL_ICE_TYPE) {
            currMaterial = new CgFxPrism(matPropIter->second);
            currMaterial->SetTechnique(CgFxPrism::ICE_TECHNIQUE_NAME);
        }
		else {
			// Default to using the cel-shader for now
			matPropIter->second->materialType = MaterialProperties::MATERIAL_CELBASIC_TYPE;
			currMaterial = new CgFxCelShading(matPropIter->second);
		}

		// Inline: The material is set
		assert(currMaterial != NULL);
		materials[matPropIter->first] = currMaterial;
	}

	return materials;
}