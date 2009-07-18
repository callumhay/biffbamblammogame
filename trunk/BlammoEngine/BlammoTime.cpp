#include "BlammoTime.h"

#ifdef WIN32
typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)( int );
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
#endif

/**
 * Platform independent method of setting VSync On/Off.
 */
void BlammoTime::SetVSync(bool vSyncOn) {
	const char *extensions = (const char*)(glGetString(GL_EXTENSIONS));

	if(strstr( extensions, "WGL_EXT_swap_control" ) == 0)
		return; // Error: WGL_EXT_swap_control extension not supported on your computer.\n");
	else
	{
#ifdef WIN32
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if( wglSwapIntervalEXT ) {
			wglSwapIntervalEXT(vSyncOn ? 1 : 0);
		}
#endif
		// TODO: other ways of setting VSync...
	}
}