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

        struct VertexDataLayout
        {
            friend class VertexDataLayoutBuilder;

            private:
                VertexDataLayout(GLsizei stride, GLintptr offset, std::vector<VertexAttribute> &&vertexAttributes)
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

        class VertexDataLayoutBuilder
        {
            private:
                std::vector<VertexAttribute> vertexAttributes;
                GLsizei stride = 0;
                GLintptr offset = 0;
            public:
                VertexDataLayoutBuilder &addAttribute(const std::string &attributeName, GLint size, GLenum type, GLboolean normalized, GLuint relativeOffset)
                {
                    vertexAttributes.push_back(VertexAttribute{attributeName, size, type, normalized, relativeOffset});
                    return *this;
                }

                VertexDataLayoutBuilder &setStride(GLsizei stride)
                {
                    this->stride = stride;
                    return *this;
                }

                VertexDataLayoutBuilder &setOffset(GLintptr offset)
                {
                    this->offset = offset;
                    return *this;
                }

                VertexDataLayout build()
                {
                    VertexDataLayout result = VertexDataLayout(stride, offset, std::move(vertexAttributes));
                    vertexAttributes = std::vector<VertexAttribute>();
                    stride = 0;
                    offset = 0;

                    return result;
                }
        };

    //     struct UniformBufferObjectAttribute
    //     {
    //         std::string name;
    //         GLint size = 0;
    //         GLenum type;
    //     };
    //
    //     struct UniformBufferObjectLayout
    //     {
    //     };
    //
    //     class UniformBufferObjectLayoutBuilder
    //     {
    //         private:
    //             std::vector<UniformBufferObjectAttribute> uboAttributes;
    //         public:
    //             UniformBufferObjectLayoutBuilder &addAttribute(const std::string &name, GLint size, GLenum type)
    //             {
    //                 uboAttributes.push_back(UniformBufferAttribute{name, size, type});
    //
    //                 return *this;
    //             }
    //
    //             UniformBufferObjectLayout build()
    //             {
    //                 //TODO: construct uniform buffer object layout using the rules of
    //                 //UBO layout(std140). that object will autocalc where each parameter
    //                 //should be placed based on its size, type, and the alignment rules
    //                 //of layout(std140). that object can be queried for where each attribute
    //                 //is laid out. It would be fantastic if this could be a template class that generates
    //                 //strongly-typed object layouts that don't have to be runtime queried.
    //             }
    //     };
    }
}

#endif
