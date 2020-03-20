#include <stdexcept>

#include <flare/gl/buffer.h>

namespace Flare
{
    namespace GL
    {
        Buffer::Buffer(const std::string &name, const RenderSystem::VertexDataLayout& bufferContentDescription, RenderSystem::BufferType bufferType)
        :
            name(name),
            bufferContentDescription(bufferContentDescription),
            bufferType(bufferType)
        {
            if (bufferType == RenderSystem::BufferType::BASIC) {
                glCreateBuffers(1, &glBuffer);
            } else if (bufferType == RenderSystem::BufferType::TRANSFORMFEEDBACK) {
                glCreateTransformFeedbacks(1, &glBuffer);
            }
        }

        Buffer::~Buffer()
        {
            destroy();
        }

        Buffer::Buffer(Buffer&& other)
        :
            name(std::exchange(other.name, std::string{})),
            bufferContentDescription(other.bufferContentDescription),
            bufferType(other.bufferType),
            usageFlags(std::exchange(other.usageFlags, UsageFlags{})),
            glBuffer(std::exchange(other.glBuffer, 0)),
            dataCapacityBytes(std::exchange(other.dataCapacityBytes, 0)),
            isGPUStorageInitialized(std::exchange(other.isGPUStorageInitialized, false))
        {
        }

        Buffer& Buffer::operator=(Buffer&& other)
        {
            name = std::exchange(other.name, std::string{});
            bufferContentDescription = other.bufferContentDescription;
            bufferType = other.bufferType;
            usageFlags = std::exchange(other.usageFlags, UsageFlags{});
            glBuffer = std::exchange(other.glBuffer, 0);
            dataCapacityBytes = std::exchange(other.dataCapacityBytes, 0);
            isGPUStorageInitialized = std::exchange(other.isGPUStorageInitialized, false);

            return *this;
        }

        void Buffer::clearNamedBufferSubData(GLenum internalFormat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data)
        {
            if (isGPUStorageInitialized == false) {
                auto message = "Attempting to clear buffer " + std::to_string(static_cast<unsigned int>(glBuffer)) +
                    " before it has been initialized.";

                throw std::runtime_error(message);
                return;
            }

            glClearNamedBufferSubData(glBuffer, internalFormat, offset, size, format, type, data);
        }

        void Buffer::copyNamedBufferSubData(const Buffer& readBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
        {
            if (isGPUStorageInitialized == false || readBuffer.isGPUStorageInitialized == false ) {
                auto message = "Attempting to copy data to or from an uninitialized buffer (buffers " +
                    std::to_string(static_cast<unsigned int>(glBuffer)) + " and " +
                    std::to_string(static_cast<unsigned int>(readBuffer.glBuffer)) + ").";

                throw std::runtime_error(message);
                return;
            }

            glCopyNamedBufferSubData(readBuffer.glBuffer, glBuffer, readOffset, writeOffset, size);
        }

        MappedBufferRange* Buffer::mapNamedBufferRange(GLintptr offset, GLsizeiptr length)
        {
            if (isCurrentlyMapped) {
                std::string message = "Mapping buffer " + std::to_string(glBuffer) + " while it "
                    "is already mapped is not allowed. Unmap the buffer first";
                throw std::runtime_error(message);

                return nullptr;
            }

            GLbitfield mapFlags = 0;
            if (usageFlags.mapRead) { mapFlags |= GL_MAP_READ_BIT; }
            if (usageFlags.mapWrite) { mapFlags |= GL_MAP_WRITE_BIT; }
            if (usageFlags.mapPersistent) { mapFlags |= GL_MAP_PERSISTENT_BIT; }
            if (usageFlags.mapCoherent) { mapFlags |= GL_MAP_COHERENT_BIT; }

            mappedBuffer = nullptr;
            auto mappedData = glMapNamedBufferRange(glBuffer, offset, length, mapFlags);
            mappedBuffer = std::unique_ptr<MappedBufferRange>(new MappedBufferRange(this, offset, length, mappedData));

            isCurrentlyMapped = true;

            return mappedBuffer.get();
        }

        Buffer::UsageFlags Buffer::getInternalUsageFlags() const
        {
            return usageFlags;
        }

        void Buffer::namedBufferData(GLsizei size, const void* data, GLenum usage)
        {
            checkDynamicStorageFlagBeforeWrite();
            glNamedBufferData(glBuffer, size, data, usage);
        }

        void Buffer::namedBufferSubData(GLintptr offset, GLsizeiptr size, const void* data)
        {
            checkDynamicStorageFlagBeforeWrite();
            glNamedBufferSubData(glBuffer, offset, size, data);
        }

        void Buffer::namedBufferStorage(GLsizei size, const void* data, GLbitfield flags)
        {
            if (isGPUStorageInitialized) {
                auto message = "Allocating storage for buffer " + std::to_string(static_cast<unsigned int>(glBuffer)) +
                    " which has already had storage allocated previously. Create a new buffer or explicitly destroy"
                    " the storage before allocating.";
                
                throw std::runtime_error(message);
                return;
            }

            glNamedBufferStorage(glBuffer, size, data, flags);
            isGPUStorageInitialized = true;
            dataCapacityBytes = size;

            usageFlags = UsageFlags{
                static_cast<bool>(flags & GL_DYNAMIC_STORAGE_BIT),
                static_cast<bool>(flags & GL_MAP_READ_BIT),
                static_cast<bool>(flags & GL_MAP_WRITE_BIT),
                static_cast<bool>(flags & GL_MAP_PERSISTENT_BIT),
                static_cast<bool>(flags & GL_MAP_COHERENT_BIT),
                static_cast<bool>(flags & GL_CLIENT_STORAGE_BIT)
            };
        }

        void Buffer::checkDynamicStorageFlagBeforeWrite()
        {
            if (usageFlags.dynamicStorage == false) {
                throw std::domain_error("Attempting to write data to buffer " + std::to_string(static_cast<unsigned int>(glBuffer))
                    + " which is not marked for dynamic storage."
                );
            }
        }

        void Buffer::bind(RenderSystem::RSenum target)
        {
            glBindBuffer(target, glBuffer);
        }

        void Buffer::destroy()
        {
            if (bufferType == RenderSystem::BufferType::BASIC) {
                glDeleteBuffers(1, &glBuffer);
            } else if (bufferType == RenderSystem::BufferType::TRANSFORMFEEDBACK) {
                glDeleteTransformFeedbacks(1, &glBuffer);
            }

            isGPUStorageInitialized = false;
            dataCapacityBytes = 0;
            usageFlags = UsageFlags{};
        }

        void Buffer::unmap()
        {
            if (!isCurrentlyMapped) {
                return;
            }

            glUnmapNamedBuffer(glBuffer);

            //Leave the mapped buffer marked invalid incase anything monitoring it
            //needs to update. It will be destroyed completely at the next call to
            //mapNamedBufferRange();
            mappedBuffer->valid = false;
            isCurrentlyMapped = false;
        }

        void Buffer::clearRange(RenderSystem::RSenum internalFormat, RenderSystem::RSintptr offset, RenderSystem::RSsizeiptr size, RenderSystem::RSenum format, RenderSystem::RSenum type, const void *data)
        {
            clearNamedBufferSubData(internalFormat, offset, size, format, type, data);
        }

        void Buffer::copyRange(const RenderSystem::Buffer &readBuffer, RenderSystem::RSintptr readOffset, RenderSystem::RSintptr writeOffset, RenderSystem::RSsizeiptr size)
        {
            copyNamedBufferSubData(static_cast<const Buffer &>(readBuffer), readOffset, writeOffset, size);
        }

        const RenderSystem::VertexDataLayout &Buffer::getContentDescription() const
        {
            return bufferContentDescription;
        }

        size_t Buffer::getSizeInBytes() const
        {
            return dataCapacityBytes;
        }

        size_t Buffer::getSizeInElements() const
        {
            if (bufferContentDescription.stride == 0) {
                const std::string errorMessage = "Attempted to calculate buffer capacity in elements, stride was 0. To fix this error, provide an explicit stride value.";
                throw std::runtime_error(errorMessage);
            }

            return dataCapacityBytes / bufferContentDescription.stride;
        }

        RenderSystem::RSbitfield Buffer::getUsageFlags() const
        {
            auto result = RenderSystem::RSbitfield{};
            if (usageFlags.dynamicStorage) { result |= GL_DYNAMIC_STORAGE_BIT; }
            if (usageFlags.mapRead) { result |= GL_MAP_READ_BIT; }
            if (usageFlags.mapWrite) { result |= GL_MAP_WRITE_BIT; }
            if (usageFlags.mapPersistent) { result |= GL_MAP_PERSISTENT_BIT; }
            if (usageFlags.mapCoherent) { result |= GL_MAP_COHERENT_BIT; }
            if (usageFlags.clientStorage) { result |= GL_CLIENT_STORAGE_BIT; }

            return result;
        }

        RenderSystem::MappedBufferRange *Buffer::mapRange(RenderSystem::RSintptr offset, RenderSystem::RSsizeiptr length)
        {
            return mapNamedBufferRange(offset, length);
        }

        void Buffer::bufferData(RenderSystem::RSsizei size, const void *data, RenderSystem::RSenum usage)
        {
            namedBufferData(size, data, usage);
        }

        void Buffer::bufferRange(RenderSystem::RSintptr offset, RenderSystem::RSsizeiptr size, const void *data)
        {
            namedBufferSubData(offset, size, data);
        }

        void Buffer::allocateBufferStorage(RenderSystem::RSsizei size, const void *data, RenderSystem::RSbitfield flags)
        {
            namedBufferStorage(size, data, flags);
        }

        MappedBufferRange::MappedBufferRange(const Buffer *sourceBuffer, GLintptr offset, GLsizeiptr length, void* mappedData)
        :
            sourceBuffer(sourceBuffer),
            offset(offset),
            length(length),
            mappedData(mappedData)
        {}

        MappedBufferRange::~MappedBufferRange()
        {
            valid = false;
        }

        void *MappedBufferRange::get() const
        {
            if (valid == false) {
                return nullptr;
            }

            return mappedData;
        }

        bool MappedBufferRange::hasReadAccess() const
        {
            if (valid == false) {
                return false;
            }

            return sourceBuffer->getInternalUsageFlags().mapRead;
        }

        bool MappedBufferRange::hasWriteAccess() const
        {
            if (valid == false) {
                return false;
            }

            return sourceBuffer->getInternalUsageFlags().mapWrite;
        }

        bool MappedBufferRange::isPersistent() const
        {
            if (valid == false) {
                return false;
            }

            return sourceBuffer->getInternalUsageFlags().mapPersistent;
        }

        bool MappedBufferRange::isCoherent() const
        {
            if (valid == false) {
                return false;
            }

            return sourceBuffer->getInternalUsageFlags().mapCoherent;
        }

        bool MappedBufferRange::isValid() const
        {
            return valid;
        }
    }
}
