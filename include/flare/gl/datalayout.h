#ifndef FLARE_GL_DATALAYOUT_H
#define FLARE_GL_DATALAYOUT_H

#include <vector>

namespace Flare
{
    namespace GL
    {
        struct VertexAttribute
        {
            GLuint index = -1;
            GLint size = 0;
            GLenum type;
            GLboolean normalized = GL_FALSE;
            GLuint relativeOffset = 0;
        };

        struct DataLayout
        {
            friend class DataLayoutBuilder;

            private:
                DataLayout(GLsizei stride, std::vector<VertexAttribute> &&vertexAttributes);

            public:
                //if stride is set to 0, glVertexArrayVertexBuffer() will autocalculate it
                //based on the assigned vertex attributes
                const GLsizei stride = 0;
                const std::vector<VertexAttribute> vertexAttributes;
        };

        class DataLayoutBuilder
        {
            private:
                std::vector<VertexAttribute> vertexAttributes;
                GLsizei stride = 0;
            public:
                DataLayoutBuilder& addVertexAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset)
                {
                    vertexAttributes.push_back(VertexAttribute{index, size, type, normalized, relativeOffset});
                    return *this;
                }

                DataLayoutBuilder& setStride(GLsizei stride)
                {
                    this->stride = stride;
                    return *this;
                }

                DataLayout build()
                {
                    DataLayout result = DataLayout(stride, std::move(vertexAttributes));
                    vertexAttributes = std::vector<VertexAttribute>();
                    stride = 0;

                    return result;
                }
        };
    }
}

#endif
