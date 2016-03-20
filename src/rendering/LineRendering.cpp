#include "LineRendering.hpp"
#include "ShaderSources.g.hpp"

namespace ar
{

LineRenderer::LineRenderer()
{
}

void LineRenderer::Init()
{
  Base::Init();

  _shader.loadAndLink(ShaderSources::sh_line_vert, ShaderSources::sh_line_frag);
  SetDefaultShader(&_shader);
}

void LineRenderer::RenderPass(const SceneInfo& sceneInfo)
{
  _shader.enable();

  glUniformMatrix4fv(_shader.getUniform("P"), 1, GL_FALSE, &(sceneInfo.projectionMatrix[0][0]));
  glUniform1f(_shader.getUniform("nearPlane"), sceneInfo.nearClip);
  glUniform1f(_shader.getUniform("aspect"), sceneInfo.aspect);

  for (const auto& m : _meshes)
  {
    const LineMesh* mesh = static_cast<LineMesh*>(m.get());

    const glm::mat4 mv = sceneInfo.viewMatrix * mesh->GetTransform();
    glUniformMatrix4fv(_shader.getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
    glUniform1f(_shader.getUniform("lineThickness"), mesh->GetThickness());

    mesh->GetMaterial()->Apply();

    glBindVertexArray(_vertexBuffer._vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer._vio);

    glDrawElementsBaseVertex(sceneInfo.renderType,
                             mesh->IndexCount(),
                             GL_UNSIGNED_INT,
                             (void*)(mesh->GetIndexOffset() * sizeof(GLuint)),
                             mesh->GetVertexOffset());

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
}

} // namespace ar
