#ifndef __GEOMETRYMAKER_H__
#define __GEOMETRYMAKER_H__

#include "BasicIncludes.h"

/**
 * Singleton class for creating pre-made geometry (e.g., quad,
 * cube, etc.).
 */
class GeometryMaker {

private:
	static GeometryMaker* instance;

	GeometryMaker();
	~GeometryMaker();
	
	// Display list IDs
	GLuint quadDL;
	GLuint cubeDL;

	bool InitializeQuadDL();
	bool InitializeCubeDL();

public:

	// Creation and deletion of the singleton
	static GeometryMaker* GetInstance() {
		if (GeometryMaker::instance == NULL) {
			GeometryMaker::instance = new GeometryMaker();
		}
		return GeometryMaker::instance;
	}

	static void DeleteInstance() {
		if (GeometryMaker::instance != NULL) {
			delete GeometryMaker::instance;
			GeometryMaker::instance = NULL;
		}
	}

	// Draw a basic unit quad pointing out towards the positive z-axis
	inline void DrawQuad() const { glCallList(this->quadDL); }
	inline GLuint GetQuadDL() const { return this->quadDL; }
	
	// Draw a basic unit cube
	inline void DrawCube() const { glCallList(this->cubeDL); }

};
#endif