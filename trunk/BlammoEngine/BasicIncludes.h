/**
 * BasicIncludes.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

// cross-compatibility issue
#ifdef _WIN32
#include <windows.h>
#endif
// -------------------------

// C Library includes
#include <cstdlib>
#include <cfloat>
#include <ctime>
#include <cassert>
#include <cstdlib>
#include <cmath>

// Memory Leak Tracking Includes
#if defined(_MSC_VER) || defined(_WIN32)
#include <crtdbg.h>
#define CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC 
#endif

#define UNUSED_PARAMETER(p) ((void)p)
#define UNUSED_VARIABLE(p)  ((void)p)

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#define STRINGIFY(x) # x

// STL includes
//#ifdef _SECURE_SCL
//#undef _SECURE_SCL
//#endif

#undef max
#undef min

#include <algorithm>
#include <limits>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <limits>

#define GLUT_DISABLE_ATEXIT_HACK 1

// OpenGL includes and defines
#define GLEW_STATIC
#include <gl/glew.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <gl/gl.h>
#include <gl/glu.h>
#endif//__APPLE__
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
#define ILUT_USE_OPENGL 1
#undef  ILUT_USE_WIN32
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

#ifndef _SORTINGFUNCTIONS_NAMESPACE__
#define _SORTINGFUNCTIONS_NAMESPACE__
namespace SortingFunctions {

// Function for sorting resolutions
inline static bool ResolutionStrCompare(const std::string& res1, const std::string& res2) {
	std::stringstream res1SS(res1);
	std::stringstream res2SS(res2);

	int resolution1Width, resolution2Width, resolution1Height, resolution2Height;
	res1SS >> resolution1Width;
	res2SS >> resolution2Width;

	if (resolution1Width < resolution2Width) {
		return true;
	}
	else if (resolution1Width > resolution2Width) {
		return false;
	}
	else {
		char temp;
		res1SS >> temp;
		res2SS >> temp;

		res1SS >> resolution1Height;
		res2SS >> resolution2Height;

		if (resolution1Height < resolution2Height) {
			return true;
		}
	}
	return false;
}

inline static bool ResolutionPairCompare(const std::pair<int, int>& res1, const std::pair<int, int>& res2) {

    int resolution1Width, resolution2Width, resolution1Height, resolution2Height;
    resolution1Width = res1.first;
    resolution2Width = res2.first;

    if (resolution1Width < resolution2Width) {
        return true;
    }
    else if (resolution1Width > resolution2Width) {
        return false;
    }
    else {
        resolution1Height = res1.second;
        resolution2Height = res2.second;

        if (resolution1Height < resolution2Height) {
            return true;
        }
    }
    return false;
}

};
#endif

// Custom Debug functionality
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef  NDEBUG
#define debug_output(s) ((void)0)
inline void debug_opengl_state() {}
inline void debug_openal_state() {}
inline void debug_physfs_state(int) {}
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
	const GLubyte* errStr = gluGetString(glErr);
	debug_output("OpenGL Error: " << glErr);
	if (errStr != NULL) {
		debug_output(errStr);
	}
	assert(false);
}

//inline void debug_openal_state() {
//	ALenum alErr = alGetError();
//	if (alErr == AL_NO_ERROR) {
//		return;
//	}
//	debug_output("OpenAL Error: " << alErr);
//
//	alErr = alutGetError();
//	if (alErr == AL_NO_ERROR) {
//		return;
//	}
//	debug_output("OpenAL (ALUT) Error: " << alutGetErrorString(alErr) << " (" << alErr << ")");
//}

/**
 * Used to check for errors in the physfs library.
 * This will print off the error in debug mode and pose an assertion.
 */
inline void debug_physfs_state(int physfsReturnVal) {
	if (physfsReturnVal == 0) {
		const char* error = PHYSFS_getLastError();
		if (error == NULL) {
			error = "?";
		}
		debug_output("PHYSFS ERROR: " << error);
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