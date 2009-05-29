#include "DecoSkybox.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"

#include "../ResourceManager.h"

DecoSkybox::DecoSkybox() : 
Skybox(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STARFIELD, Texture::Trilinear)),
decoSkyboxEffect(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_STARFIELD, Texture::Trilinear)),
skyboxDispList(0) {

	// Create a display list for the skybox geometry
	this->skyboxDispList = glGenLists(1);
	assert(this->skyboxDispList != 0);

	glNewList(this->skyboxDispList, GL_COMPILE);
	Skybox::DrawSkyboxGeometry(4, (Camera::FAR_PLANE_DIST - 1.0f) / SQRT_2);
	glEndList();
}

DecoSkybox::~DecoSkybox() {
	// Clean up the display list for the skybox
	glDeleteLists(this->skyboxDispList, 1);
	this->skyboxDispList = 0;
}

void DecoSkybox::Draw(const Camera& camera) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
  glLoadIdentity();

	Vector3D camView = camera.GetNormalizedViewVector();
	Matrix4x4 camTransform = camera.GetViewTransform();
	camTransform.setTranslation(Point3D(0,0,0));
	glMultMatrixf(camTransform.begin());

	glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
  glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	
	this->skyboxTex->BindTexture();
	glTexParameteri(this->skyboxTex->GetTextureType(), GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(this->skyboxTex->GetTextureType(), GL_TEXTURE_WRAP_T, GL_REPEAT);
	this->decoSkyboxEffect.Draw(camera, this->skyboxDispList);
	this->skyboxTex->UnbindTexture();

	glPopAttrib();
	glPopMatrix();

	debug_cg_state();
}