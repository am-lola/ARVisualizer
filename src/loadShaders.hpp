#ifndef _LOAD_SHADERS_H_
#define _LOAD_SHADERS_H_

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

namespace ar
{

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

} // namespace ar

#endif // _LOAD_SHADERS_H_
