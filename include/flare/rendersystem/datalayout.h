#ifndef FLARE_RENDERSYSTEM_DATALAYOUT_H
#define FLARE_RENDERSYSTEM_DATALAYOUT_H

#include <algorithm>
#include <string>
#include <vector>
#include <variant>

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
        };

        struct MatrixVertexAttribute
        {
            std::string name;
            RSint rows = 0;
            RSint cols = 0;
            RSenum type;
            RSboolean normalized = GL_FALSE;
            RSuint relativeOffset = 0;
        };

        struct VertexDataLayout
        {
            friend class VertexDataLayoutBuilder;

            private:
                //if stride is set to 0, glVertexArrayVertexBuffer() will autocalculate it
                //based on the assigned vertex attributes
                RSsizei stride = 0;
                //always set to zero unless interleaving data blocks within a single buffer
                RSintptr offset = 0;
                RSuint divisor = 0;
                std::vector<std::variant<VertexAttribute, MatrixVertexAttribute>> vertexAttributes;

                VertexDataLayout(RSsizei stride, RSintptr offset, RSuint divisor, std::vector<std::variant<VertexAttribute, MatrixVertexAttribute>> &&vertexAttributes)
                : stride(stride), offset(offset), divisor(divisor), vertexAttributes(vertexAttributes)
                {
                }

            public:
                ~VertexDataLayout() {}
                VertexDataLayout(const VertexDataLayout &other);
                VertexDataLayout &operator=(const VertexDataLayout &other);
                VertexDataLayout(VertexDataLayout &&other);
                VertexDataLayout &operator=(VertexDataLayout &&other);

                const std::variant<VertexAttribute, MatrixVertexAttribute> *getAttribute(const std::string &attributeName) const;
                RSuint getDivisor() const;
                RSintptr getOffset() const;
                RSsizei getStride() const;
                const std::vector<std::variant<VertexAttribute, MatrixVertexAttribute>> &getVertexAttributes() const;
        };

        class VertexDataLayoutBuilder
        {
            private:
                std::vector<std::variant<VertexAttribute, MatrixVertexAttribute>> vertexAttributes;
                RSsizei stride = 0;
                RSintptr offset = 0;
                RSuint divisor = 0;
            public:
                VertexDataLayoutBuilder &addAttribute(const std::string &attributeName, RSint size, RSenum type, RSboolean normalized, RSuint relativeOffset);
                VertexDataLayoutBuilder &addMatrixAttribute(const std::string &attributeName, RSint rows, RSint cols, RSenum type, RSboolean normalized, RSuint relativeOffset);
                VertexDataLayoutBuilder &setStride(RSsizei stride);
                VertexDataLayoutBuilder &setOffset(RSintptr offset);
                VertexDataLayoutBuilder &setDivisor(RSuint divisor);
                VertexDataLayout build();
        };

        struct VertexBufferVertexDataLayout
        {
            std::string bufferName;
            VertexDataLayout bufferContentDescription;
        };
    }
}

#endif
