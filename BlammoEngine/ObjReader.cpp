#include "ObjReader.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "CgFxEffect.h"

const std::string ObjReader::OBJ_EXTENSION = "obj";
const std::string ObjReader::MTL_EXTENSION = "mtl";

const std::string ObjReader::OBJ_MTLFILE = "mtllib";
const std::string ObjReader::OBJ_USE_MATERIAL = "usemtl";

const std::string ObjReader::OBJ_GROUP = "g";
const std::string ObjReader::OBJ_VERTEX_COORD = "v";
const std::string ObjReader::OBJ_VERTEX_NORMAL = "vn";
const std::string ObjReader::OBJ_VERTEX_TEXCOORD = "vt";
const std::string ObjReader::OBJ_FACE = "f";

/*
 * Read in a mesh from an Obj file.
 * Precondition: true.
 * Returns: A loaded mesh or NULL in case of error.
 */
Mesh* ObjReader::ReadMesh(const std::string &filepath) {
	
	// Start reading in the file
	std::ifstream inFile;
	inFile.open(filepath.c_str());
	
	// Make sure the file opened properly
	if (!inFile.is_open()) {
		debug_output("ERROR: Could not open file: " << filepath); 
		return NULL;
	}

	Mesh* temp = ObjReader::ReadMeshFromStream(filepath, inFile);
	inFile.close();
	return temp;
}

/**
 * Read the mesh using a PHYSFS file handle - for reading from zipped archives.
 * Returns: A loaded mesh object, NULL on error.
 */
Mesh* ObjReader::ReadMesh(const std::string &filepath, PHYSFS_File* fileHandle) {
	
	// Get a byte buffer of the entire file and convert it into a string stream so it
	// can be read in using the STL
	PHYSFS_sint64 fileLength = PHYSFS_fileLength(fileHandle);
	char* fileBuffer = new char[fileLength];
	
	int readResult = PHYSFS_read(fileHandle, fileBuffer, sizeof(char), fileLength);
	if (readResult == NULL) {
		delete[] fileBuffer;
		fileBuffer = NULL;
		debug_output("Error reading obj file to bytes: " << filepath);
		assert(false);
		return NULL;
	}

	// Convert the bytes to a string stream 
	std::istringstream fileSS(std::string(fileBuffer), std::ios_base::in | std::ios_base::binary);
	delete[] fileBuffer;
	fileBuffer = NULL;

	return ObjReader::ReadMeshFromStream(filepath, fileSS);
}


/**
 * Private helper function for reading meshes with a given input stream.
 * Returns: Mesh on successful read, NULL otherwise.
 */
Mesh* ObjReader::ReadMeshFromStream(const std::string &filepath, std::istream &inFile) {
	std::string currStr = "";

	// We first start by finding and reading the material file
	std::string mtlFilepath = "";
	while (inFile >> currStr) {
		if (currStr == OBJ_MTLFILE) {
			inFile >> mtlFilepath;
			// Add the path to the obj to the path... the mtl file SHOULD be in the same dir
			size_t pos = filepath.find_last_of("/");
			if (pos == std::string::npos) {
				pos = filepath.find_last_of("\\");
			}
			mtlFilepath.insert(0, filepath.substr(0, pos+1));
			break;
		}
	}
	
	// Figure out what materials to make and make them
	std::map<std::string, CgFxMaterialEffect*> meshMaterials;
	assert(mtlFilepath != "");
	meshMaterials = ResourceManager::GetInstance()->GetMtlMeshResource(mtlFilepath);

	std::string grpName = "";
	std::string matName = "";
	
	std::vector<Point3D> vertices;
	std::vector<Vector3D> normals;
	std::vector<Point2D> texCoords;
	std::map<std::string, MaterialGroup*> matGrps;		// Material groups, mapped by their group name
	std::map<std::string, PolyGrpIndexer> polyGrps;		// Set of polygons associated with material group names


	while (inFile >> currStr) {
		
		if (currStr == OBJ_GROUP) {
			// Read in a new group - TODO: We don't do anything with this... we assume each obj file makes
			// up a mesh unto itself
			if (!(inFile >> grpName)) {
				debug_output("ERROR: Group name not provided with proper syntax in obj file: " << filepath); 
				return NULL;
			}
		}
		else if (currStr == OBJ_VERTEX_COORD) {
			// Read in a vertex coordinate
			Point3D pt;
			if (!(inFile >> pt[0] && inFile >> pt[1] && inFile >> pt[2])) {
				debug_output("ERROR: could not read vertex/point properly from obj file: " << filepath); 
				return NULL;				
			}
			vertices.push_back(pt);
		}
		else if (currStr == OBJ_VERTEX_NORMAL) {
			// Read in a normal vector
			Vector3D normal;
			if (!(inFile >> normal[0] && inFile >> normal[1] && inFile >> normal[2])) {
				debug_output("ERROR: could not read normal properly from obj file: " << filepath); 
				return NULL;				
			}
			normals.push_back(normal);
		}
		else if (currStr == OBJ_VERTEX_TEXCOORD) {
			// Read in a texture coordinate
			Point2D texCoord;
			if (!(inFile >> texCoord[0] && inFile >> texCoord[1])) {
				debug_output("ERROR: could not read texture coordinate properly from obj file: " << filepath); 
				return NULL;				
			}
			texCoords.push_back(texCoord);
		}
		else if (currStr == OBJ_FACE) {
			assert(matName != "");
			char trash;

			for (unsigned int i = 0; i < 3; i++) {
				unsigned int vertexIndex;
				unsigned int normalIndex;
				unsigned int texCoordIndex;

				inFile >> vertexIndex;
				inFile >> trash;
				inFile >> texCoordIndex;
				inFile >> trash;
				inFile >> normalIndex;

				// Since indices read from an obj file start at index 1, we need to decrement
				// for our zero index system
				vertexIndex--;
				texCoordIndex--;
				normalIndex--;

				// Add the indices to their respective lists
				polyGrps[matName].vertexIndices.push_back(vertexIndex);
				polyGrps[matName].normalIndices.push_back(normalIndex);
				polyGrps[matName].texCoordIndices.push_back(texCoordIndex);
			}

		}
		else if (currStr == OBJ_USE_MATERIAL) {
			// Obtain the material group name
			if (!(inFile >> matName)) {
				debug_output("ERROR: Material name not provided with proper syntax in obj file: " << filepath); 
				return NULL;
			}
			
			// Ensure the material group doesn't already exist (so we don't overwrite it)
			std::map<std::string, MaterialGroup*>::iterator matGrpCheck = matGrps.find(matName);
			if (matGrpCheck == matGrps.end()) {
				// Inline: We are initializing the material for the first time

				// Look up the material name from the list of materials
				std::map<std::string, CgFxMaterialEffect*>::iterator valIter = meshMaterials.find(matName);
				if (valIter == meshMaterials.end()) {
					// Not good: the material was not found...
					debug_output("ERROR: Material name in obj file with no matching material in mtl file: " << filepath); 
					return NULL;
				}

				matGrps[matName] = new MaterialGroup(valIter->second);
			}
		}
	}

	// Go through all the polygon groups and set the material groups
	std::map<std::string, PolyGrpIndexer>::iterator polyGrpIter;
	for (polyGrpIter = polyGrps.begin(); polyGrpIter != polyGrps.end(); polyGrpIter++) {
		MaterialGroup* currMatGrp = matGrps[polyGrpIter->first];
		currMatGrp->AddFaces(polyGrpIter->second, vertices, normals, texCoords);
	}

	return new Mesh(filepath, matGrps);
}

/**
 * Read in ONLY a polygon group from a given file, this does not care about materials
 * or anything else of the sort and will simply put it all together into
 * a single polygon group and hand it back.
 */
PolygonGroup* ObjReader::ReadPolygonGroup(const std::string &filepath) {
	// Start reading in the file
	std::ifstream inFile;
	inFile.open(filepath.c_str());
	
	// Make sure the file opened properly
	if (!inFile.is_open()) {
		debug_output("ERROR: Could not open file: " << filepath); 
		return NULL;
	}

	std::string currStr = "";
	std::vector<Point3D> vertices;
	std::vector<Vector3D> normals;
	std::vector<Point2D> texCoords;
	PolyGrpIndexer polyGrpIndexer;	

	while (inFile >> currStr) {
		if (currStr == OBJ_VERTEX_COORD) {
			// Read in a vertex coordinate
			Point3D pt;
			if (!(inFile >> pt[0] && inFile >> pt[1] && inFile >> pt[2])) {
				debug_output("ERROR: could not read vertex/point properly from obj file: " << filepath); 
				return NULL;				
			}
			vertices.push_back(pt);
		}
		else if (currStr == OBJ_VERTEX_NORMAL) {
			// Read in a normal vector
			Vector3D normal;
			if (!(inFile >> normal[0] && inFile >> normal[1] && inFile >> normal[2])) {
				debug_output("ERROR: could not read normal properly from obj file: " << filepath); 
				return NULL;				
			}
			normals.push_back(normal);
		}
		else if (currStr == OBJ_VERTEX_TEXCOORD) {
			// Read in a texture coordinate
			Point2D texCoord;
			if (!(inFile >> texCoord[0] && inFile >> texCoord[1])) {
				debug_output("ERROR: could not read texture coordinate properly from obj file: " << filepath); 
				return NULL;				
			}
			texCoords.push_back(texCoord);
		}
		else if (currStr == OBJ_FACE) {
			char trash;

			for (unsigned int i = 0; i < 3; i++) {
				unsigned int vertexIndex;
				unsigned int normalIndex;
				unsigned int texCoordIndex;

				inFile >> vertexIndex;
				inFile >> trash;
				inFile >> texCoordIndex;
				inFile >> trash;
				inFile >> normalIndex;

				// Since indices read from an obj file start at index 1, we need to decrement
				// for our zero index system
				vertexIndex--;
				texCoordIndex--;
				normalIndex--;

				// Add the indices to their respective lists
				polyGrpIndexer.vertexIndices.push_back(vertexIndex);
				polyGrpIndexer.normalIndices.push_back(normalIndex);
				polyGrpIndexer.texCoordIndices.push_back(texCoordIndex);
			}
		}
	}
	
	PolygonGroup* newPolyGrp = new PolygonGroup(polyGrpIndexer, vertices, normals, texCoords);
	return newPolyGrp;
}