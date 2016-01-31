#ifndef _LOAD_SHADERS_H_
#define _LOAD_SHADERS_H_

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#include <string>

namespace ar
{

GLuint LoadShadersFromFiles(const char* vertex_file_path, const char* fragment_file_path);
GLuint LoadShadersFromSource(const std::string vertex_source, const std::string fragment_source);

} // namespace ar

#endif // _LOAD_SHADERS_H_
