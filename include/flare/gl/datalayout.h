#ifndef FLARE_GL_DATALAYOUT_H
#define FLARE_GL_DATALAYOUT_H

#include <algorithm>
#include <string>
#include <vector>

namespace Flare
{
    namespace GL
    {
        struct VertexAttribute
        {
            std::string name;
            GLint size = 0;
            GLenum type;
            GLboolean normalized = GL_FALSE;
            GLuint relativeOffset = 0;
        };

        struct DataLayout
        {
            friend class DataLayoutBuilder;

            private:
                DataLayout(GLsizei stride, GLintptr offset, std::vector<VertexAttribute> &&vertexAttributes)
                : stride(stride), offset(offset), vertexAttributes(vertexAttributes)
                {
                }

            public:
                //if stride is set to 0, glVertexArrayVertexBuffer() will autocalculate it
                //based on the assigned vertex attributes
                const GLsizei stride = 0;
                //always set to zero unless interleaving data blocks within a single buffer
                const GLintptr offset = 0;
                const std::vector<VertexAttribute> vertexAttributes;

                const VertexAttribute *getAttribute(const std::string &attributeName)
                {
                    const auto& result = std::find_if(
                        vertexAttributes.begin(),
                        vertexAttributes.end(),
                        [attributeName](const auto& entry) {
                            return entry.name == attributeName;
                        }
                    );

                    if (result == std::end(vertexAttributes)) {
                        return nullptr;
                    }

                    return &(*result);
                }
        };

        class DataLayoutBuilder
        {
            private:
                std::vector<VertexAttribute> vertexAttributes;
                GLsizei stride = 0;
                GLintptr offset = 0;
            public:
                DataLayoutBuilder &addVertexAttribute(const std::string &attributeName, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset)
                {
                    vertexAttributes.push_back(VertexAttribute{attributeName, size, type, normalized, relativeOffset});
                    return *this;
                }

                DataLayoutBuilder &setStride(GLsizei stride)
                {
                    this->stride = stride;
                    return *this;
                }

                DataLayoutBuilder &setOffset(GLintptr offset)
                {
                    this->offset = offset;
                    return *this;
                }

                DataLayout build()
                {
                    DataLayout result = DataLayout(stride, offset, std::move(vertexAttributes));
                    vertexAttributes = std::vector<VertexAttribute>();
                    stride = 0;
                    offset = 0;

                    return result;
                }
        };
    }
}

#endif
