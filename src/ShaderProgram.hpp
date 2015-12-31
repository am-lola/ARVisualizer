#ifndef _SHADER_PROGRAM_HPP
#define _SHADER_PROGRAM_HPP

// The code below was adapted from: http://r3dux.org/2013/08/a-simple-c-opengl-shader-loader/
// Provides a convenient way to load GLSL shaders and deal with uniform values

#include <iostream>
#include <map>

#include "loadShaders.hpp"

namespace ar
{

class ShaderProgram
{
  private:
    GLuint programId;   // The unique ID / handle for the shader program
    GLuint shaderCount; // How many shaders are attached to the shader program

    // Map of attributes and their binding locations
    std::map<std::string,int> attributeLocList;

    // Map of uniforms and their binding locations
    std::map<std::string,int> uniformLocList;

    // retrieves all uniforms available in the currently-linked program
    void populateUniformList()
    {
      // nothing to be done without a linked program
      if (programId == 0)
      {
        return;
      }

      GLint uniformCount; // number of active uniforms in the linked program
      glGetProgramiv(programId, GL_ACTIVE_UNIFORMS, &uniformCount);

      std::vector<GLchar> nameData(256);
      for(int i = 0; i < uniformCount; i++)
      {
        GLint arraySize = 0;
        GLenum type = 0;
        GLsizei nameLength = 0;
        glGetActiveUniform(programId, i, nameData.size(), &nameLength, &arraySize, &type, &nameData[0]);
        addUniform(std::string((char*)&nameData[0], nameLength));
      }
    }

  public:
    // Constructor
    ShaderProgram()
    {
      programId = 0;

      // load nothing by default; total shaders is 0
      shaderCount = 0;
    }

    ShaderProgram(const std::string vertex_source, const std::string fragment_source)
    {
      // load the given shaders
      programId = LoadShadersFromSource(vertex_source, fragment_source);

      // If LoadShaders was successful, we now have two shaders linked in this program
      if (programId != 0)
      {
        shaderCount = 2;
        populateUniformList();
      }
    }

    // Destructor
    ~ShaderProgram()
    {
      // Delete the shader program from the graphics card memory to
      // free all the resources it's been using
      glDeleteProgram(programId);
    }

    void loadAndLink(const std::string vertex_source, const std::string fragment_source)
    {
      // delete old shader program if we have one
      if (shaderCount > 0 && programId > 0)
      {
        glDeleteProgram(programId);
      }

      // load new shaders
      programId = LoadShadersFromSource(vertex_source, fragment_source);

      // if we succeeded, we now have two shaders
      if (programId != 0)
      {
        shaderCount = 2;
        populateUniformList();
      }
    }

    // Method to enable the shader program
    void enable()
    {
      glUseProgram(programId);
    }


    // Method to disable the shader program
    void disable()
    {
      glUseProgram(0);
    }


    // Returns the bound location of a named attribute
    GLuint getAttribute(const std::string &attribute)
    {
      // Create an iterator to look through our attribute map and try to find the named attribute
      std::map<std::string, int>::iterator it = attributeLocList.find(attribute);

      // Found it? Great -return the bound location! Didn't find it? Alert user and halt.
      if ( it != attributeLocList.end() )
      {
        return attributeLocList[attribute];
      }
      else
      {
        throw AttributeDoesNotExistException("Could not find Attribute '" + attribute + "'");
      }
    }


    // Method to returns the bound location of a named uniform
    GLuint getUniform(const std::string &uniform)
    {
      // Create an iterator to look through our uniform map and try to find the named uniform
      std::map<std::string, int>::iterator it = uniformLocList.find(uniform);

      // Found it? Great - pass it back! Didn't find it? Alert user and halt.
      if ( it != uniformLocList.end() )
      {
       return uniformLocList[uniform];
      }
      else
      {
        throw UniformDoesNotExistException("Could not find Uniform '" + uniform + "'");
      }
    }


    // Method to add an attrbute to the shader and return the bound location
    int addAttribute(const std::string &attributeName)
    {
      attributeLocList[attributeName] = glGetAttribLocation( programId, attributeName.c_str() );

      // Check to ensure that the shader contains an attribute with this name
      if (attributeLocList[attributeName] == -1)
      {
        throw AttributeDoesNotExistException("Attribute ''" + attributeName + "'' does not exist in shader!");
      }

      return attributeLocList[attributeName];
    }


    // Method to add a uniform to the shader and return the bound location
    int addUniform(const std::string &uniformName)
    {
      uniformLocList[uniformName] = glGetUniformLocation( programId, uniformName.c_str() );

      // Check to ensure that the shader contains a uniform with this name
      if (uniformLocList[uniformName] == -1)
      {
        throw UniformDoesNotExistException("Uniform '" + uniformName + "' does not exist in shader!");
      }

      return uniformLocList[uniformName];
    }

    class UniformDoesNotExistException : public std::runtime_error
    {
    public:
      UniformDoesNotExistException(const std::string& what_arg) : std::runtime_error(what_arg) {}
    };
    class AttributeDoesNotExistException : public std::runtime_error
    {
    public:
      AttributeDoesNotExistException(const std::string& what_arg) : std::runtime_error(what_arg) {}
    };

};

} // namespace ar

#endif // _SHADER_PROGRAM_HPP
