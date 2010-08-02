

#include "TeslaBlockMesh.h"
#include "GameViewConstants.h"

#include "../GameModel/TeslaBlock.h"

const float TeslaBlockMesh::COIL_ROTATION_SPEED_DEGSPERSEC = 270;

TeslaBlockMesh::TeslaBlockMesh() : teslaBaseMesh(NULL), teslaCoilMesh(NULL) {
	this->LoadMesh();
}

TeslaBlockMesh::~TeslaBlockMesh() {
	ResourceManager::GetInstance()->ReleaseMeshResource(this->teslaBaseMesh);
	ResourceManager::GetInstance()->ReleaseMeshResource(this->teslaCoilMesh);
}

void TeslaBlockMesh::Draw(double dT, const Camera& camera, const BasicPointLight& keyLight, 
													const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
	
	float rotationAmt = dT * COIL_ROTATION_SPEED_DEGSPERSEC;

	// Go through each tesla block and transform their center's appropriately
	for (std::set<const TeslaBlock*>::const_iterator iter = this->teslaBlocks.begin(); iter != this->teslaBlocks.end(); ++iter) {
		const TeslaBlock* currTeslaBlock = *iter;
		assert(currTeslaBlock != NULL);
		
		std::map<const TeslaBlock*, std::pair<Vector3D, float>>::iterator findIter = this->rotations.find(currTeslaBlock);

		const Vector3D& currRotationAxis = findIter->second.first;
		float& currRotatationAmt = findIter->second.second;
		const Point2D& blockCenter = currTeslaBlock->GetCenter();

		glPushMatrix();
		glTranslatef(blockCenter[0], blockCenter[1], 0.0f);
		//glRotatef(currRotation[0], 1, 0, 0);
		glRotatef(currRotatationAmt, currRotationAxis[0], currRotationAxis[1], currRotationAxis[2]);
		//glRotatef(currRotation[2], 0, 0, 1);

		this->teslaCoilMesh->Draw(camera, keyLight, fillLight, ballLight);

		glPopMatrix();

		currRotatationAmt += rotationAmt;
	}
}

void TeslaBlockMesh::LoadMesh() {
	assert(this->teslaBaseMesh == NULL);
	assert(this->teslaCoilMesh == NULL);
	assert(this->materialGroups.size() == 0);
	
	this->teslaBaseMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TESLA_BLOCK_BASE_MESH);
	assert(this->teslaBaseMesh != NULL);
	const std::map<std::string, MaterialGroup*>&  baseMatGrps = this->teslaBaseMesh->GetMaterialGroups();

	// We don't add the coil's material groups, we will draw those manually for each tesla block
	// so that we can move them around
	this->teslaCoilMesh = ResourceManager::GetInstance()->GetObjMeshResource(GameViewConstants::GetInstance()->TESLA_BLOCK_COIL_MESH);
	assert(this->teslaCoilMesh != NULL);
	
	this->materialGroups.insert(baseMatGrps.begin(), baseMatGrps.end());
	
	assert(this->materialGroups.size() > 0);
}