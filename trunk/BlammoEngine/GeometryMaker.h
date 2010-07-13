/**
 * GeometryMaker.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#ifndef __GEOMETRYMAKER_H__
#define __GEOMETRYMAKER_H__

#include "BasicIncludes.h"
#include "Colour.h"
#include "Camera.h"

class PolygonGroup;

/**
 * Singleton class for creating pre-made geometry (e.g., quad,
 * cube, etc.).
 */
class GeometryMaker {

private:
	static const unsigned int NUM_SPHERE_STACKS = 12;
	static const unsigned int NUM_SPHERE_SLICES = 12;

	static GeometryMaker* instance;

	GeometryMaker();
	~GeometryMaker();
	
	// Display list IDs
	GLuint quadDL;
	GLuint cubeDL;
	GLuint sphereDL;

	bool InitializeQuadDL();
	bool InitializeCubeDL();
	bool InitializeSphereDL();

	static const int TUNNEL_TESSELATION;
	static const float TUNNEL_WALL_SIZE;	// Have width and height?
	static const float HALF_TUNNEL_WALL_SIZE;

	enum TunnelOrientation { PosX, NegX, PosY, NegY, PosZ, NegZ };
	Vector3D TunnelOrientToVector(GeometryMaker::TunnelOrientation orientation);
	GeometryMaker::TunnelOrientation VectorToTunnelOrient(const Vector3D& vec);
	void DrawTunnelWallSection(const Point3D& center, GeometryMaker::TunnelOrientation orientation);
	void DrawFullTunnel(const Point3D& startCenterPt, int units, GeometryMaker::TunnelOrientation dir);

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
	inline GLuint GetCubeDL() const { return this->cubeDL; }

	inline void DrawSphere() const { glCallList(this->sphereDL); }
	inline GLuint GetSphereDL() const { return this->sphereDL; }
	static GLuint CreateSphereDL(float horizRadius, float vertRadius, unsigned int stacks, unsigned int slices);

	inline void DrawFullScreenQuad(int width, int height, float depth = 0.0f, const ColourRGBA& colour = ColourRGBA(1,1,1,1)) {
		glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT);
		Camera::PushWindowCoords();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT, GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_ALWAYS);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor4f(colour.R(), colour.G(), colour.B(), colour.A());
		glBegin(GL_QUADS);
				glTexCoord2i(0, 0); glVertex3f(0, 0, depth);
				glTexCoord2i(1, 0); glVertex3f(width, 0, depth);
				glTexCoord2i(1, 1); glVertex3f(width, height, depth);
				glTexCoord2i(0, 1); glVertex3f(0, height, depth);
		glEnd();

		glPopMatrix();

		Camera::PopWindowCoords();
		glPopAttrib();

		debug_opengl_state();
	};


};
#endif