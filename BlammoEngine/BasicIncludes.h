/**
 * BasicIncludes.h
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

// cross-compatibility issue
#ifdef WIN32
#include <windows.h>
#endif
// -------------------------

// C Library includes
#include <cstdlib>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

// Memory Leak Tracking Includes
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC 

// STL includes
#include <algorithm>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>

// OpenGL includes
#define GLEW_STATIC
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>

#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))

// SDL Includes
#ifdef WIN32
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif

#include "SDL.h"
#include "SDL_thread.h"
#include "SDL_mutex.h"

// Cg includes
#include "Cg/cg.h"
#include "Cg/cgGL.h"

// DevIL includes ... uses a dll (cross-compatibility issue)
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

// Freetype library for loading free type fonts
#define FREETYPE2_STATIC

#ifdef WIN32
#define generic __identifier(generic)
#endif

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

// Pseudo Random Number Generator (PRNG)
#include "mtrand.h"

// Physfs (ZIP/FileSystem Reader) Includes
#define PHYSFS_SUPPORTS_ZIP 1
#include "physfs.h"

// Custom Debug functionality
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef  NDEBUG
#define debug_output(s) ((void)0)
inline void debug_opengl_state() {}
inline void debug_physfs_state(int physfsReturnVal) {}
inline void debug_cg_state() {}
#else
#define debug_output(s) (std::cout << s << std::endl)


/**
 * Used to check for errors in opengl.
 * This will print off the error in debug mode and pose an assertion.
 */
inline void debug_opengl_state() {
	GLenum glErr = glGetError();
	if (glErr == GL_NO_ERROR) {
		return;
	}
	debug_output("OpenGL Error: " << glErr);
	assert(false);
}

/**
 * Used to check for errors in the physfs library.
 * This will print off the error in debug mode and pose an assertion.
 */
inline void debug_physfs_state(int physfsReturnVal) {
	if (physfsReturnVal == NULL) {
		const char* error = PHYSFS_getLastError();
		if (error == NULL) {
			error = "?";
		}
		debug_output("PHYSFS ERROR: " << error);
		assert(false);
	}
}

/**
 * Used to check for errors in the Cg runtime.
 * This will print off the error in debug mode and pose an assertion.
 */
inline void debug_cg_state() {
	CGerror error = CG_NO_ERROR;
	const char* errorStr = cgGetLastErrorString(&error);
	
	if (error != CG_NO_ERROR) {
		debug_output("Cg Error: " << error << " - " << errorStr);
		assert(false);
	}
}

#endif
#endif