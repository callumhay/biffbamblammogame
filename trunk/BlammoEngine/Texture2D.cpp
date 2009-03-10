#include "Texture2D.h"
#include "Camera.h"
#include "FBOManager.h"

#include "Algebra.h"


// Default constructor for 2D textures
Texture2D::Texture2D(TextureFilterType texFilter) : Texture(texFilter, GL_TEXTURE_2D) {
}

Texture2D::~Texture2D() {
}


/**
 * Renders this texture to a full screen quad in the viewport.
 */
void Texture2D::RenderTextureToFullscreenQuad() {

	glPushAttrib(GL_VIEWPORT_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | 
							 GL_TRANSFORM_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	Camera::PushWindowCoords();

	glDepthMask(GL_FALSE);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT, GL_FILL);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Draw the full screen quad
	this->BindTexture();

	// Set the appropriate parameters for rendering the single fullscreen quad
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(0, 0);
			glTexCoord2i(1, 0); glVertex2i(this->width, 0);
			glTexCoord2i(1, 1); glVertex2i(this->width, this->height);
			glTexCoord2i(0, 1); glVertex2i(0, this->height);
	glEnd();
	this->UnbindTexture();

	glDepthMask(GL_TRUE);
	glPopMatrix();
	glPopAttrib();
	Camera::PopWindowCoords();
}

Texture2D* Texture2D::CreateEmptyTextureRectangle(int width, int height) {


	// First check to see that rectangular textures are supported
	int textureType = GL_TEXTURE_2D;
		/*
	if (glewGetExtension("GL_EXT_texture_rectangle") == GL_TRUE) {
		textureType = GL_TEXTURE_RECTANGLE_EXT;
	}
	else if (glewGetExtension("GL_ARB_texture_rectangle") == GL_TRUE) {
		textureType = GL_TEXTURE_RECTANGLE_ARB;
	}
	else {
		return NULL;
	}
*/
	
	Texture2D* newTex = new Texture2D(Texture::Linear);
	glDisable(GL_TEXTURE_2D);
	newTex->textureType = textureType;
	
	glEnable(newTex->textureType);
	glGenTextures(1, &newTex->texID);
	if (newTex->texID == 0) {
		delete newTex;
		return NULL;
	}

	newTex->width  = width;
	newTex->height = height;
	
	glPushAttrib(GL_TEXTURE_BIT);
	newTex->BindTexture();

	glTexImage2D(newTex->textureType, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	Texture::SetFilteringParams(newTex->texFilter, newTex->textureType);
	
	glTexParameteri(newTex->textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(newTex->textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(newTex->textureType, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(newTex->textureType, GL_TEXTURE_WRAP_T, GL_CLAMP);

	newTex->UnbindTexture();

	glPopAttrib();
	return newTex;
}

/**
 * Static creator for making a 2D texture from a given file path to an image file.
 */
Texture2D* Texture2D::CreateTexture2DFromImgFile(const std::string& filepath, TextureFilterType texFilter) {
	glPushAttrib(GL_TEXTURE_BIT);
	
	Texture2D* newTex = new Texture2D(texFilter);
	if (!newTex->Load2DOr1DTextureFromImg(filepath, texFilter)) {
		delete newTex;
		newTex = NULL;
	}

	glPopAttrib();
	return newTex;
}

/**
 * Static creator, for making a 2D texture given a true font bitmap.
 */
Texture2D* Texture2D::CreateTexture2DFromFTBMP(const FT_Bitmap& bmp, TextureFilterType texFilter) {

	// First create the texture
	Texture2D* newTex = new Texture2D(texFilter);
	glGenTextures(1, &newTex->texID);
	if (newTex->texID == 0) {
		delete newTex;
		return NULL;
	}

	glPushAttrib(GL_TEXTURE_BIT);

	// Obtain the proper power of two width/height of the font
	int width = NumberFuncs::NextPowerOfTwo(bmp.width);
	int height = NumberFuncs::NextPowerOfTwo(bmp.rows);
	newTex->width = width;
	newTex->height = height;

	// Allocate Memory For The Texture Data.
	GLubyte* expandedData = new GLubyte[ 2 * width * height];

	// Use A Two Channel Bitmap (One For Channel Luminosity And One For Alpha)
	// Assign Both Luminosity And Alpha To The Value That We Find In The FreeType Bitmap. 
	for (int j=0; j < height; j++) {
		for (int i=0; i < width; i++){
			// That Value Which We Use Will Be 0 If We Are In The Padding Zone, And Whatever
			// Is The FreeType Bitmap Otherwise.
			expandedData[2*(i+j*width)]= expandedData[2*(i+j*width)+1] = 
				(i>=bmp.width || j>=bmp.rows) ? 0 : bmp.buffer[i + bmp.width*j];
		}
	}
	
	// Bind the texture and create it in all its glory
	glBindTexture(GL_TEXTURE_2D, newTex->texID);

	// Fonts are 2 channel data, hence the use of GL_LUMINANCE_ALPHA
	if (!Texture::IsMipmappedFilter(texFilter)) {
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
									GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData );
	}
	else {
		GLint result = gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expandedData);
		
		if (result != 0) {
			debug_output("Failed to load mipmaps for empty texture.");
			delete newTex;
			newTex = NULL;
		}	
	}
	
	Texture::SetFilteringParams(texFilter, newTex->textureType);
	glPopAttrib();
	delete[] expandedData;
	return newTex;
}