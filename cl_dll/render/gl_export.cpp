/*
gl_export.cpp - OpenGL dynamically linkage
Copyright (C) 2010 Uncle Mike

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#define EXTERN
#include "gl_export.h"
#include "imgui_manager.h"

extern render_api_t gRenderAPI;

static dllfunc_t opengl_110funcs[] =
{
    { "glClearColor"         	, (void **)&pglClearColor },
    { "glClear"              	, (void **)&pglClear },
    { "glAlphaFunc"          	, (void **)&pglAlphaFunc },
    { "glBlendFunc"          	, (void **)&pglBlendFunc },
    { "glCullFace"           	, (void **)&pglCullFace },
    { "glDrawBuffer"         	, (void **)&pglDrawBuffer },
    { "glReadBuffer"         	, (void **)&pglReadBuffer },
    { "glAccum"         		, (void **)&pglAccum },
    { "glEnable"             	, (void **)&pglEnable },
    { "glDisable"            	, (void **)&pglDisable },
    { "glEnableClientState"  	, (void **)&pglEnableClientState },
    { "glDisableClientState" 	, (void **)&pglDisableClientState },
    { "glGetBooleanv"        	, (void **)&pglGetBooleanv },
    { "glGetDoublev"         	, (void **)&pglGetDoublev },
    { "glGetFloatv"          	, (void **)&pglGetFloatv },
    { "glGetIntegerv"        	, (void **)&pglGetIntegerv },
    { "glGetError"           	, (void **)&pglGetError },
    { "glGetString"          	, (void **)&pglGetString },
    { "glFinish"             	, (void **)&pglFinish },
    { "glFlush"              	, (void **)&pglFlush },
    { "glClearDepth"         	, (void **)&pglClearDepth },
    { "glDepthFunc"          	, (void **)&pglDepthFunc },
    { "glDepthMask"          	, (void **)&pglDepthMask },
    { "glDepthRange"         	, (void **)&pglDepthRange },
    { "glFrontFace"          	, (void **)&pglFrontFace },
    { "glDrawElements"       	, (void **)&pglDrawElements },
    { "glDrawArrays"       		, (void **)&pglDrawArrays },
    { "glColorMask"          	, (void **)&pglColorMask },
    { "glIndexPointer"       	, (void **)&pglIndexPointer },
    { "glVertexPointer"      	, (void **)&pglVertexPointer },
    { "glNormalPointer"      	, (void **)&pglNormalPointer },
    { "glColorPointer"       	, (void **)&pglColorPointer },
    { "glTexCoordPointer"    	, (void **)&pglTexCoordPointer },
    { "glArrayElement"       	, (void **)&pglArrayElement },
    { "glColor3f"            	, (void **)&pglColor3f },
    { "glColor3fv"           	, (void **)&pglColor3fv },
    { "glColor4f"            	, (void **)&pglColor4f },
    { "glColor4fv"           	, (void **)&pglColor4fv },
    { "glColor3ub"           	, (void **)&pglColor3ub },
    { "glColor4ub"           	, (void **)&pglColor4ub },
    { "glColor4ubv"          	, (void **)&pglColor4ubv },
    { "glTexCoord1f"         	, (void **)&pglTexCoord1f },
    { "glTexCoord2f"         	, (void **)&pglTexCoord2f },
    { "glTexCoord3f"         	, (void **)&pglTexCoord3f },
    { "glTexCoord4f"         	, (void **)&pglTexCoord4f },
    { "glTexCoord1fv"        	, (void **)&pglTexCoord1fv },
    { "glTexCoord2fv"        	, (void **)&pglTexCoord2fv },
    { "glTexCoord3fv"        	, (void **)&pglTexCoord3fv },
    { "glTexCoord4fv"        	, (void **)&pglTexCoord4fv },
    { "glTexGenf"            	, (void **)&pglTexGenf },
    { "glTexGenfv"           	, (void **)&pglTexGenfv },
    { "glTexGeni"            	, (void **)&pglTexGeni },
    { "glVertex2f"           	, (void **)&pglVertex2f },
    { "glVertex3f"           	, (void **)&pglVertex3f },
    { "glVertex3fv"          	, (void **)&pglVertex3fv },
    { "glNormal3f"           	, (void **)&pglNormal3f },
    { "glNormal3fv"          	, (void **)&pglNormal3fv },
    { "glBegin"              	, (void **)&pglBegin },
    { "glEnd"                	, (void **)&pglEnd },
    { "glLineWidth"          	, (void**)&pglLineWidth },
    { "glPointSize"          	, (void**)&pglPointSize },
    { "glMatrixMode"         	, (void **)&pglMatrixMode },
    { "glOrtho"              	, (void **)&pglOrtho },
    { "glRasterPos2f"        	, (void **) &pglRasterPos2f },
    { "glFrustum"            	, (void **)&pglFrustum },
    { "glViewport"           	, (void **)&pglViewport },
    { "glPushMatrix"         	, (void **)&pglPushMatrix },
    { "glPopMatrix"          	, (void **)&pglPopMatrix },
    { "glPushAttrib"         	, (void **)&pglPushAttrib },
    { "glPopAttrib"          	, (void **)&pglPopAttrib },
    { "glLoadIdentity"       	, (void **)&pglLoadIdentity },
    { "glLoadMatrixd"        	, (void **)&pglLoadMatrixd },
    { "glLoadMatrixf"        	, (void **)&pglLoadMatrixf },
    { "glMultMatrixd"        	, (void **)&pglMultMatrixd },
    { "glMultMatrixf"        	, (void **)&pglMultMatrixf },
    { "glRotated"            	, (void **)&pglRotated },
    { "glRotatef"            	, (void **)&pglRotatef },
    { "glScaled"             	, (void **)&pglScaled },
    { "glScalef"             	, (void **)&pglScalef },
    { "glTranslated"         	, (void **)&pglTranslated },
    { "glTranslatef"         	, (void **)&pglTranslatef },
    { "glReadPixels"         	, (void **)&pglReadPixels },
    { "glDrawPixels"         	, (void **)&pglDrawPixels },
    { "glStencilFunc"        	, (void **)&pglStencilFunc },
    { "glStencilMask"        	, (void **)&pglStencilMask },
    { "glStencilOp"          	, (void **)&pglStencilOp },
    { "glClearStencil"       	, (void **)&pglClearStencil },
    { "glIsEnabled"          	, (void **)&pglIsEnabled },
    { "glIsList"             	, (void **)&pglIsList },
    { "glIsTexture"          	, (void **)&pglIsTexture },
    { "glTexEnvf"            	, (void **)&pglTexEnvf },
    { "glTexEnvfv"           	, (void **)&pglTexEnvfv },
    { "glTexEnvi"            	, (void **)&pglTexEnvi },
    { "glTexParameterf"      	, (void **)&pglTexParameterf },
    { "glTexParameterfv"     	, (void **)&pglTexParameterfv },
    { "glTexParameteri"      	, (void **)&pglTexParameteri },
    { "glHint"               	, (void **)&pglHint },
    { "glPixelStoref"        	, (void **)&pglPixelStoref },
    { "glPixelStorei"        	, (void **)&pglPixelStorei },
    { "glGenTextures"        	, (void **)&pglGenTextures },
    { "glDeleteTextures"     	, (void **)&pglDeleteTextures },
    { "glBindTexture"        	, (void **)&pglBindTexture },
    { "glTexImage1D"         	, (void **)&pglTexImage1D },
    { "glTexImage2D"         	, (void **)&pglTexImage2D },
    { "glTexSubImage1D"      	, (void **)&pglTexSubImage1D },
    { "glTexSubImage2D"      	, (void **)&pglTexSubImage2D },
    { "glCopyTexImage1D"     	, (void **)&pglCopyTexImage1D },
    { "glCopyTexImage2D"     	, (void **)&pglCopyTexImage2D },
    { "glCopyTexSubImage1D"  	, (void **)&pglCopyTexSubImage1D },
    { "glCopyTexSubImage2D"  	, (void **)&pglCopyTexSubImage2D },
    { "glScissor"            	, (void **)&pglScissor },
    { "glGetTexImage"			, (void **)&pglGetTexImage },
    { "glGetTexEnviv"        	, (void **)&pglGetTexEnviv },
    { "glPolygonOffset"      	, (void **)&pglPolygonOffset },
    { "glPolygonMode"        	, (void **)&pglPolygonMode },
    { "glPolygonStipple"     	, (void **)&pglPolygonStipple },
    { "glClipPlane"          	, (void **)&pglClipPlane },
    { "glGetClipPlane"       	, (void **)&pglGetClipPlane },
    { "glShadeModel"         	, (void **)&pglShadeModel },
    { "glGetTexLevelParameteriv"	, (void **)&pglGetTexLevelParameteriv },
    { "glGetTexLevelParameterfv"	, (void **)&pglGetTexLevelParameterfv },
    { "glFogfv"              	, (void **)&pglFogfv },
    { "glFogf"               	, (void **)&pglFogf },
    { "glFogi"               	, (void **)&pglFogi },
    { NULL, NULL }
};

static dllfunc_t opengl_200funcs[] =
{
    { "glCreateShader"				, (void **)&pglCreateShader },
    { "glAttachShader"				, (void **)&pglAttachShader },
    { "glDetachShader"				, (void **)&pglDetachShader },
    { "glCompileShader"				, (void **)&pglCompileShader },
    { "glShaderSource"				, (void **)&pglShaderSource },
    { "glGetShaderSource"			, (void **)&pglGetShaderSource },
    { "glGetShaderiv"				, (void **)&pglGetShaderiv },
    { "glDeleteShader"		        , (void **)&pglDeleteShader },
    { "glUseProgram"				, (void **)&pglUseProgram },
    { "glLinkProgram"				, (void **)&pglLinkProgram },
    { "glValidateProgram"			, (void **)&pglValidateProgram },
    { "glCreateProgram"				, (void **)&pglCreateProgram },
    { "glDeleteProgram"				, (void **)&pglDeleteProgram },
    { "glGetShaderInfoLog"			, (void **)&pglGetShaderInfoLog },
    { "glGetProgramInfoLog"			, (void **)&pglGetProgramInfoLog },
    { "glGetActiveUniform"			, (void **)&pglGetActiveUniform },
    { "glGetUniformLocation"		, (void **)&pglGetUniformLocation },
    { "glGetProgramiv"              , (void **)&pglGetProgramiv },
    { "glBlendEquation"             , (void **)&pglBlendEquation },
    { "glVertexAttribPointer"		, (void **)&pglVertexAttribPointerARB },
    { "glEnableVertexAttribArray"	, (void **)&pglEnableVertexAttribArrayARB },
    { "glDisableVertexAttribArray"	, (void **)&pglDisableVertexAttribArrayARB },
    { "glBindAttribLocation"		, (void **)&pglBindAttribLocationARB },
    { "glGetActiveAttrib"			, (void **)&pglGetActiveAttribARB },
    { "glGetAttribLocation"			, (void **)&pglGetAttribLocationARB },
    { "glTexImage3D"				, (void **)&pglTexImage3D },
    { "glTexSubImage3D"				, (void **)&pglTexSubImage3D },
    { "glCopyTexSubImage3D"			, (void **)&pglCopyTexSubImage3D },
    { "glDrawRangeElements"			, (void **)&pglDrawRangeElements },
    { "glDrawBuffers"				, (void **)&pglDrawBuffersARB },
    { "glMultiTexCoord1f"			, (void **)&pglMultiTexCoord1f },
    { "glMultiTexCoord2f"			, (void **)&pglMultiTexCoord2f },
    { "glMultiTexCoord3f"			, (void **)&pglMultiTexCoord3f },
    { "glMultiTexCoord4f"			, (void **)&pglMultiTexCoord4f },
    { "glActiveTexture"				, (void **)&pglActiveTexture },
    { "glUniformMatrix2fv"			, (void **)&pglUniformMatrix2fvARB },
    { "glUniformMatrix3fv"			, (void **)&pglUniformMatrix3fvARB },
    { "glUniformMatrix4fv"			, (void **)&pglUniformMatrix4fvARB },
    { "glUniform1f"					, (void **)&pglUniform1fARB },
    { "glUniform2f"					, (void **)&pglUniform2fARB },
    { "glUniform3f"					, (void **)&pglUniform3fARB },
    { "glUniform4f"					, (void **)&pglUniform4fARB },
    { "glUniform1i"					, (void **)&pglUniform1iARB },
    { "glUniform2i"					, (void **)&pglUniform2iARB },
    { "glUniform3i"					, (void **)&pglUniform3iARB },
    { "glUniform4i"					, (void **)&pglUniform4iARB },
    { "glUniform1fv"				, (void **)&pglUniform1fvARB },
    { "glUniform2fv"				, (void **)&pglUniform2fvARB },
    { "glUniform3fv"				, (void **)&pglUniform3fvARB },
    { "glUniform4fv"				, (void **)&pglUniform4fvARB },
    { "glUniform1iv"				, (void **)&pglUniform1ivARB },
    { "glUniform2iv"				, (void **)&pglUniform2ivARB },
    { "glUniform3iv"				, (void **)&pglUniform3ivARB },
    { "glUniform4iv"				, (void **)&pglUniform4ivARB },
    { "glVertexAttrib2f"            , (void **)&pglVertexAttrib2fARB },
    { "glVertexAttrib2fv"           , (void **)&pglVertexAttrib2fvARB },
    { "glVertexAttrib3fv"           , (void **)&pglVertexAttrib3fvARB },
    { "glVertexAttrib4fv"           , (void **)&pglVertexAttrib4fvARB },
    { "glVertexAttrib4ubv"			, (void **)&pglVertexAttrib4ubvARB },
    { NULL, NULL }
};

/*
 * ==================
 * GL_LoadFunctions
 * ==================
 */
static void GL_LoadFunctions(const dllfunc_t *funcs)
{
    for (const dllfunc_t *f = funcs; f->name; f++)
    {
        *f->func = (void *)gRenderAPI.GL_GetProcAddress(f->name);
        if (*f->func)
            gEngfuncs.Con_Printf("Loaded %s\n", f->name);
        else
            gEngfuncs.Con_Printf("Not load %s\n", f->name);
    }
}

/*
 * ==================
 * GL_Init
 * ==================
 */
bool GL_Init(void)
{
    gEngfuncs.Con_Printf("Loading OpenGL 1.1 functions...\n");
    GL_LoadFunctions(opengl_110funcs);

    gEngfuncs.Con_Printf("Loading OpenGL 2.0 functions...\n");
    GL_LoadFunctions(opengl_200funcs);

    g_ImGuiManager.Initialize();

    return true;
}

/*
 * ==================
 * GL_Shutdown
 * ==================
 */
void GL_Shutdown(void)
{
    g_ImGuiManager.Terminate();
    gEngfuncs.Con_Printf("OpenGL functions unloaded.\n");
}
