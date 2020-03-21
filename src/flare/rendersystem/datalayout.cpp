#include <flare/rendersystem/datalayout.h>

namespace Flare
{
    namespace RenderSystem
    {
        VertexDataLayout::VertexDataLayout(const VertexDataLayout &other)
        :
            stride(other.stride),
            offset(other.offset),
            divisor(other.divisor),
            vertexAttributes(other.vertexAttributes)
        {
        }

        VertexDataLayout &VertexDataLayout::operator=(const VertexDataLayout &other)
        {
            stride = other.stride;
            offset = other.offset;
            divisor = other.divisor;
            vertexAttributes = other.vertexAttributes;
        }

        VertexDataLayout::VertexDataLayout(VertexDataLayout &&other)
        :
            stride(std::exchange(other.stride, 0)),
            offset(std::exchange(other.offset, 0)),
            divisor(std::exchange(other.divisor, 0)),
            vertexAttributes(std::move(other.vertexAttributes))
        {
        }

        VertexDataLayout &VertexDataLayout::operator=(VertexDataLayout &&other)
        {
            stride = std::exchange(other.stride, 0);
            offset = std::exchange(other.offset, 0);
            divisor = std::exchange(other.divisor, 0);
            vertexAttributes = std::move(other.vertexAttributes);
        }

        const std::variant<VertexAttribute, MatrixVertexAttribute> *VertexDataLayout::getAttribute(const std::string &attributeName) const
        {
            const auto& result = std::find_if(
                vertexAttributes.begin(),
                vertexAttributes.end(),
                [&attributeName](const auto& entry) {
                    auto findResult = false;
                    std::visit([&attributeName, &findResult](const auto &vertexAttribute){ findResult = vertexAttribute.name == attributeName; }, entry);
                    return findResult;
                }
            );

            if (result == std::end(vertexAttributes)) {
                return nullptr;
            }

            return &(*result);
        }

        RSuint VertexDataLayout::getDivisor() const
        {
            return divisor;
        }

        RSintptr VertexDataLayout::getOffset() const
        {
            return offset;
        }

        RSsizei VertexDataLayout::getStride() const
        {
            return stride;
        }

        const std::vector<std::variant<VertexAttribute, MatrixVertexAttribute>> &VertexDataLayout::getVertexAttributes() const
        {
            return vertexAttributes;
        }


        VertexDataLayoutBuilder &VertexDataLayoutBuilder::addAttribute(const std::string &attributeName, RSint size, RSenum type, RSboolean normalized, RSuint relativeOffset)
        {
            vertexAttributes.push_back(VertexAttribute{attributeName, size, type, normalized, relativeOffset});
            return *this;
        }

        VertexDataLayoutBuilder &VertexDataLayoutBuilder::addMatrixAttribute(const std::string &attributeName, RSint rows, RSint cols, RSenum type, RSboolean normalized, RSuint relativeOffset)
        {
            vertexAttributes.push_back(MatrixVertexAttribute{attributeName, rows, cols, type, normalized, relativeOffset});
            return *this;
        }

        VertexDataLayoutBuilder &VertexDataLayoutBuilder::setStride(RSsizei stride)
        {
            this->stride = stride;
            return *this;
        }

        VertexDataLayoutBuilder &VertexDataLayoutBuilder::setOffset(RSintptr offset)
        {
            this->offset = offset;
            return *this;
        }

        VertexDataLayoutBuilder &VertexDataLayoutBuilder::setDivisor(RSuint divisor)
        {
            this->divisor = divisor;
            return *this;
        }

        VertexDataLayout VertexDataLayoutBuilder::build()
        {
            VertexDataLayout result = VertexDataLayout(stride, offset, divisor, std::move(vertexAttributes));
            vertexAttributes = std::vector<std::variant<VertexAttribute, MatrixVertexAttribute>>();
            stride = 0;
            offset = 0;
            divisor = 0;

            return result;
        }
    }
}
