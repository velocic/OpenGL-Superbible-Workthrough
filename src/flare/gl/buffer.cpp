#include <stdexcept>

#include <flare/gl/buffer.h>

namespace Flare
{
    namespace GL
    {
        Buffer::Buffer(const std::string &name, const RenderSystem::VertexDataLayout& bufferContentDescription)
        :
            name(name),
            bufferContentDescription(bufferContentDescription)
        {
            glCreateBuffers(1, &glBuffer);
        }

        Buffer::~Buffer()
        {
            destroy();
        }

        Buffer::Buffer(Buffer&& other)
        :
            name(std::move(other.name)),
            bufferContentDescription(other.bufferContentDescription),
            usageFlags(std::move(other.usageFlags)),
            glBuffer(std::move(other.glBuffer)),
            dataCapacityBytes(std::move(other.dataCapacityBytes)),
            isGPUStorageInitialized(std::move(other.isGPUStorageInitialized))
        {
            other.glBuffer = 0;
            other.dataCapacityBytes = 0;
            other.isGPUStorageInitialized = false;
            other.usageFlags.dynamicStorage = false;
            other.usageFlags.mapRead = false;
            other.usageFlags.mapWrite = false;
            other.usageFlags.mapPersistent = false;
            other.usageFlags.mapCoherent = false;
            other.usageFlags.clientStorage = false;
        }

        Buffer& Buffer::operator=(Buffer&& other)
        {
            name = std::move(other.name);
            glBuffer = std::move(other.glBuffer);
            dataCapacityBytes = std::move(other.dataCapacityBytes);
            isGPUStorageInitialized = std::move(other.isGPUStorageInitialized);
            usageFlags = std::move(other.usageFlags);

            other.glBuffer = 0;
            other.dataCapacityBytes = 0;
            other.isGPUStorageInitialized = false;
            other.usageFlags.dynamicStorage = false;
            other.usageFlags.mapRead = false;
            other.usageFlags.mapWrite = false;
            other.usageFlags.mapPersistent = false;
            other.usageFlags.mapCoherent = false;
            other.usageFlags.clientStorage = false;

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

            auto mappedData = glMapNamedBufferRange(glBuffer, offset, length, mapFlags);
            mappedBuffer = std::unique_ptr<MappedBufferRange>(new MappedBufferRange(*this, offset, length, mappedData));

            isCurrentlyMapped = true;

            return mappedBuffer.get();
        }

        Buffer::UsageFlags Buffer::getUsageFlags() const
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
            glDeleteBuffers(1, &glBuffer);
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

        MappedBufferRange::MappedBufferRange(const Buffer& sourceBuffer, GLintptr offset, GLsizeiptr length, void* mappedData)
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
            return mappedData;
        }

        bool MappedBufferRange::hasReadAccess() const
        {
            return sourceBuffer.getUsageFlags().mapRead;
        }

        bool MappedBufferRange::hasWriteAccess() const
        {
            return sourceBuffer.getUsageFlags().mapWrite;
        }

        bool MappedBufferRange::isPersistent() const
        {
            return sourceBuffer.getUsageFlags().mapPersistent;
        }

        bool MappedBufferRange::isCoherent() const
        {
            return sourceBuffer.getUsageFlags().mapCoherent;
        }

        bool MappedBufferRange::isValid() const
        {
            return valid;
        }
    }
}
