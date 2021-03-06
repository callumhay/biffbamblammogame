/**
 * GeometryMaker.h
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
	GLuint lineCircleDL;
    GLuint sphereDL;
    GLuint basicPaddleDL;

	bool InitializeQuadDL();
	bool InitializeCubeDL();
    bool InitializeLineCircleDL();
	bool InitializeSphereDL();
    bool InitializeBasicPaddleDL();

    static const int CIRCLE_TESSELATION;
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
	
    void DrawRawLineCircle(const Point3D& center, float radius) const;
    inline void DrawLineCircle() const { glCallList(this->lineCircleDL); }
    inline GLuint GetLineCircleDL() const { return this->lineCircleDL; }

	// Draw a basic unit cube
	void DrawRawCube(const Point3D& center, const Vector3D& size) const;
	inline void DrawCube() const { glCallList(this->cubeDL); }
	inline GLuint GetCubeDL() const { return this->cubeDL; }

	inline void DrawSphere() const { glCallList(this->sphereDL); }
	inline GLuint GetSphereDL() const { return this->sphereDL; }
	static GLuint CreateSphereDL(float horizRadius, float vertRadius, unsigned int stacks, unsigned int slices);

    void DrawRawBasicPaddle();
    inline void DrawBasicPaddle() const { glCallList(this->basicPaddleDL); }
    inline GLuint GetBasicPaddleDL() const { return this->basicPaddleDL; }

    inline void DrawFullScreenQuadNoDepth(int width, int height, const ColourRGBA& colour = ColourRGBA(1,1,1,1)) {
		glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glPolygonMode(GL_FRONT, GL_FILL);
		glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor4f(colour.R(), colour.G(), colour.B(), colour.A());
		glBegin(GL_QUADS);
				glTexCoord2i(0, 0); glVertex2f(0, 0);
				glTexCoord2i(1, 0); glVertex2f(width, 0);
				glTexCoord2i(1, 1); glVertex2f(width, height);
				glTexCoord2i(0, 1); glVertex2f(0, height);
		glEnd();

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();

		debug_opengl_state();
    }

	inline void DrawFullScreenQuad(int width, int height, float depth = 0.0f, const ColourRGBA& colour = ColourRGBA(1,1,1,1)) {
		glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);

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

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();

		debug_opengl_state();
	};

	inline void DrawFullScreenQuad(int width, int height, float uvX, float uvY, 
                                   float depth = 0.0f, const ColourRGBA& colour = ColourRGBA(1,1,1,1)) {
		glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);

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
				glTexCoord2f(1-uvX,  1-uvY); glVertex3f(0, 0, depth);
				glTexCoord2f(uvX, 1-uvY); glVertex3f(width, 0, depth);
				glTexCoord2f(uvX, uvY); glVertex3f(width, height, depth);
				glTexCoord2f(1-uvX, uvY); glVertex3f(0, height, depth);
		glEnd();

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();

		debug_opengl_state();
	};


	inline void DrawTiledFullScreenQuad(int width, int height, float tilingX, float tilingY,
        const ColourRGBA& colour = ColourRGBA(1,1,1,1), float tS = 0, float tT = 0) {
		glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT |
            GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_DEPTH_BUFFER_BIT);
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, width, 0, height);

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
				glTexCoord2f(tS, tT); glVertex2f(0, 0);
				glTexCoord2f(tS + tilingX, tT); glVertex2f(width, 0);
				glTexCoord2f(tS + tilingX, tT + tilingY); glVertex2f(width, height);
				glTexCoord2f(tS, tT + tilingY); glVertex2f(0, height);
		glEnd();

		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glPopAttrib();

		debug_opengl_state();
	};

};
#endif