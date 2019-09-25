#ifndef FLARE_RENDERSYSTEM_DATALAYOUT_H
#define FLARE_RENDERSYSTEM_DATALAYOUT_H

#include <algorithm>
#include <string>
#include <vector>

#include <flare/rendersystem/datatypes.h>

namespace Flare
{
    namespace RenderSystem
    {
        struct VertexAttribute
        {
            std::string name;
            RSint size = 0;
            RSenum type;
            RSboolean normalized = GL_FALSE;
            RSuint relativeOffset = 0;
            RSuint attribDivisor = 0;
        };

        struct VertexDataLayout
        {
            friend class VertexDataLayoutBuilder;

            private:
                VertexDataLayout(RSsizei stride, RSintptr offset, std::vector<VertexAttribute> &&vertexAttributes)
                : stride(stride), offset(offset), vertexAttributes(vertexAttributes)
                {
                }

            public:
                //if stride is set to 0, glVertexArrayVertexBuffer() will autocalculate it
                //based on the assigned vertex attributes
                const RSsizei stride = 0;
                //always set to zero unless interleaving data blocks within a single buffer
                const RSintptr offset = 0;
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
                RSsizei stride = 0;
                RSintptr offset = 0;
            public:
                VertexDataLayoutBuilder &addAttribute(const std::string &attributeName, RSint size, RSenum type, RSboolean normalized, RSuint relativeOffset)
                {
                    vertexAttributes.push_back(VertexAttribute{attributeName, size, type, normalized, relativeOffset, 0});
                    return *this;
                }

                VertexDataLayoutBuilder &addAttribute(const std::string &attributeName, RSint size, RSenum type, RSboolean normalized, RSuint relativeOffset, RSuint attribDivisor)
                {
                    vertexAttributes.push_back(VertexAttribute{attributeName, size, type, normalized, relativeOffset, attribDivisor});
                    return *this;
                }

                VertexDataLayoutBuilder &setStride(RSsizei stride)
                {
                    this->stride = stride;
                    return *this;
                }

                VertexDataLayoutBuilder &setOffset(RSintptr offset)
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

        struct VertexBufferVertexDataLayout
        {
            std::string bufferName;
            VertexDataLayout bufferContentDescription;
        };
    }
}

#endif
