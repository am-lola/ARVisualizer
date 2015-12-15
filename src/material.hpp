#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <vector>
#include "color.hpp"
#include "ShaderProgram.hpp"

namespace ar
{
  /*
    Stores mesh-specific parameters which need to be passed to a shader
  */
  class Material
  {
  public:
    // sends all data to shader
    virtual void Apply() = 0;

    ShaderProgram* GetShader() { return _shader; }
    void SetShader(ShaderProgram* s) {
      _shader = s;
      Validate();
    }

  protected:
    ShaderProgram* _shader;
    std::vector<std::string> _uniformNames; // used to ensure shader can support this material

    // checks that all uniforms this material require are in the given shader
    void Validate() {
      for (auto u : _uniformNames)
      {
        try
        {
          _shader->getUniform(u);
        }
        catch (ShaderProgram::UniformDoesNotExistException e)
        {
          std::cout << "Incompatible material/shader pair! " << std::endl << e.what() << std::endl;
          throw e;
        }
      }
    }
  };

  /*
    Simplest material: Just stores a single color value for the whole mesh
  */
  class FlatColorMaterial : public Material
  {
  public:
    FlatColorMaterial (Color color) {
      _color = color;
      _uniformNames = {"color"};
    }

    virtual void Apply() {
      glUniform4f(_shader->getUniform("color"), _color.r, _color.g, _color.b, _color.a);
    }

  private:
    Color _color;
  };

} // namespace ar

#endif // _MATERIAL_H
