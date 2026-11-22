#ifndef GLLOADER_H
#define GLLOADER_H
#include <Windows.h>
#include <GL/gl.h>
#include <cstddef>

#define GL_ARRAY_BUFFER                        0x8892
#define GL_ELEMENT_ARRAY_BUFFER                0x8893
#define GL_STATIC_DRAW                         0x88E4
#define GL_FRAGMENT_SHADER                     0x8B30
#define GL_VERTEX_SHADER                       0x8B31
#define GL_TEXTURE0                            0x84C0
#define GL_BGR                                 0x80E0
#define GL_UNPACK_ALIGNMENT                    0x0CF5
#define GL_DEPTH_TEST                          0x0B71
#define GL_COMPILE_STATUS                      0x8B81
#define GL_LINK_STATUS                         0x8B82

#define WGL_CONTEXT_MAJOR_VERSION_ARB          0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB          0x2092
#define WGL_CONTEXT_FLAGS_ARB                  0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB           0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB       0x00000001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef UINT (WINAPI * PFNGLCREATESHADERPROC) (UINT type);
typedef void (WINAPI * PFNGLSHADERSOURCEPROC) (UINT shader, int count, const char* const* string, const int* length);
typedef void (WINAPI * PFNGLCOMPILESHADERPROC) (UINT shader);
typedef void (WINAPI * PFNGLDELETESHADERPROC) (UINT shader);
typedef UINT (WINAPI * PFNGLCREATEPROGRAMPROC) (void);
typedef void (WINAPI * PFNGLATTACHSHADERPROC) (UINT program, UINT shader);
typedef void (WINAPI * PFNGLLINKPROGRAMPROC) (UINT program);
typedef void (WINAPI * PFNGLUSEPROGRAMPROC) (UINT program);
typedef void (WINAPI * PFNGLDELETEPROGRAMPROC) (UINT program);
typedef void (WINAPI * PFNGLGENVERTEXARRAYSPROC) (int n, UINT* arrays);
typedef void (WINAPI * PFNGLBINDVERTEXARRAYPROC) (UINT array);
typedef void (WINAPI * PFNGLGENBUFFERSPROC) (int n, UINT* buffers);
typedef void (WINAPI * PFNGLBINDBUFFERPROC) (UINT target, UINT buffer);
typedef void (WINAPI * PFNGLBUFFERDATAPROC) (UINT target, ptrdiff_t size, const void* data, UINT usage);
typedef void (WINAPI * PFNGLVERTEXATTRIBPOINTERPROC) (UINT index, int size, UINT type, GLboolean normalized, int stride, const void* pointer);
typedef void (WINAPI * PFNGLENABLEVERTEXATTRIBARRAYPROC) (UINT index);
typedef int  (WINAPI * PFNGLGETUNIFORMLOCATIONPROC) (UINT program, const char* name);
typedef void (WINAPI * PFNGLUNIFORMMATRIX4FVPROC) (int location, int count, GLboolean transpose, const float* value);
typedef void (WINAPI * PFNGLACTIVETEXTUREPROC) (UINT texture);
typedef void (WINAPI * PFNGLDELETEBUFFERSPROC) (int n, const UINT* buffers);
typedef void (WINAPI * PFNGLDELETEVERTEXARRAYSPROC) (int n, const UINT* arrays);
typedef void (WINAPI * PFNGLGETSHADERIVPROC) (UINT shader, UINT pname, int* params);
typedef void (WINAPI * PFNGLGETSHADERINFOLOGPROC) (UINT shader, int bufSize, int* length, char* infoLog);
typedef void (WINAPI * PFNGLGETPROGRAMIVPROC) (UINT program, UINT pname, int* params);
typedef void (WINAPI * PFNGLGETPROGRAMINFOLOGPROC) (UINT program, int bufSize, int* length, char* infoLog);
typedef void (WINAPI * PFNGLUNIFORM1IPROC) (int location, int v0);
typedef void (WINAPI * PFNGLUNIFORM2IPROC) (int location, int v0, int v1);
typedef void (WINAPI * PFNGLUNIFORM3IPROC) (int location, int v0, int v1, int v2);
typedef void (WINAPI * PFNGLUNIFORM4IPROC) (int location, int v0, int v1, int v2, int v3);
typedef void (WINAPI * PFNGLUNIFORM1FPROC) (int location, float v0);
typedef void (WINAPI * PFNGLUNIFORM2FPROC) (int location, float v0, float v1);
typedef void (WINAPI * PFNGLUNIFORM3FPROC) (int location, float v0, float v1, float v2);
typedef void (WINAPI * PFNGLUNIFORM4FPROC) (int location, float v0, float v1, float v2, float v3);

extern PFNGLCREATESHADERPROC glCreateShader;
extern PFNGLSHADERSOURCEPROC glShaderSource;
extern PFNGLCOMPILESHADERPROC glCompileShader;
extern PFNGLDELETESHADERPROC glDeleteShader;
extern PFNGLCREATEPROGRAMPROC glCreateProgram;
extern PFNGLATTACHSHADERPROC glAttachShader;
extern PFNGLLINKPROGRAMPROC glLinkProgram;
extern PFNGLUSEPROGRAMPROC glUseProgram;
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
extern PFNGLGENBUFFERSPROC glGenBuffers;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM2IPROC glUniform2i;
extern PFNGLUNIFORM3IPROC glUniform3i;
extern PFNGLUNIFORM4IPROC glUniform4i;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM2FPROC glUniform2f;
extern PFNGLUNIFORM3FPROC glUniform3f;
extern PFNGLUNIFORM4FPROC glUniform4f;

void loadExtensions();

#endif