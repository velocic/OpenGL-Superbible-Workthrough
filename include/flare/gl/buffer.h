#ifndef FLARE_GL_BUFFER_H
#define FLARE_GL_BUFFER_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/rendersystem/datatypes.h>
#include <flare/rendersystem/datalayout.h>
#include <flare/rendersystem/buffer.h>

namespace Flare
{
    namespace GL
    {
        class MappedBufferRange;

        class Buffer : public RenderSystem::Buffer
        {
            private:
                struct UsageFlags
                {
                    bool dynamicStorage = false;
                    bool mapRead = false;
                    bool mapWrite = false;
                    bool mapPersistent = false;
                    bool mapCoherent = false;
                    bool clientStorage = false;
                };

                RenderSystem::VertexDataLayout bufferContentDescription;
                UsageFlags usageFlags;
                GLuint glBuffer = 0;
                GLsizei dataCapacityBytes = 0;
                std::unique_ptr<MappedBufferRange> mappedBuffer;
                bool isCurrentlyMapped = false;
                bool isGPUStorageInitialized = false;

                void checkDynamicStorageFlagBeforeWrite();
            public:
                Buffer(const RenderSystem::VertexDataLayout& bufferContentDescription);
                virtual ~Buffer() override;
                Buffer(Buffer&& other);
                Buffer& operator=(Buffer&& other);
                Buffer(const Buffer& other) = delete;
                Buffer& operator=(const Buffer& other) = delete;

                void clearNamedBufferSubData(GLenum internalFormat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);
                void copyNamedBufferSubData(const Buffer& readBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
                const RenderSystem::VertexDataLayout &getBufferContentDescription() const {return bufferContentDescription;}
                UsageFlags getUsageFlags() const;
                MappedBufferRange* mapNamedBufferRange(GLintptr offset, GLsizeiptr length);
                void namedBufferData(GLsizei size, const void* data, GLenum usage);
                void namedBufferSubData(GLintptr offset, GLsizeiptr size, const void* data);

                /*Flags:
                 * GL_DYNAMIC_STORAGE_BIT - Buffer contents can be updated directly
                 * GL_MAP_READ_BIT - Buffer data store will be mapped for reading
                 * GL_MAP_WRITE_BIT - Buffer data store will be mapped for writing
                 * GL_MAP_PERSISTENT_BIT - Buffer data store can be mapped persistently
                 * GL_MAP_COHERENT_BIT - Buffer maps are to be coherent
                 * GL_CLIENT_STORAGE_BIT - If all other conditions can be met, prefer CPU storage over GPU storage*/ 
                void namedBufferStorage(GLsizei size, const void* data, GLbitfield flags);

                virtual void bind(RenderSystem::RSenum target) override;
                virtual void destroy() override;
                virtual void unmap() override;

                virtual void clearRange(RenderSystem::RSenum internalFormat, RenderSystem::RSintptr offset, RenderSystem::RSsizeiptr size, RenderSystem::RSenum format, RenderSystem::RSenum type, const void *data) override;
                virtual void copyRange(const RenderSystem::Buffer &readBuffer, RenderSystem::RSintptr readOffset, RenderSystem::RSintptr writeOffset, RenderSystem::RSsizeiptr size) override ;
                virtual const RenderSystem::VertexDataLayout &getContentDescription() const override;
                virtual RenderSystem::RSuint getName() const override {return glBuffer;};
                virtual RenderSystem::MappedBufferRange *mapRange(RenderSystem::RSintptr offset, RenderSystem::RSsizeiptr length) override;
                virtual void bufferData(RenderSystem::RSsizei size, const void *data, RenderSystem::RSenum usage) override;
                virtual void bufferRange(RenderSystem::RSintptr offset, RenderSystem::RSsizeiptr size, const void *data) override;
                virtual void allocateBufferStorage(RenderSystem::RSsizei size, const void *data, RenderSystem::RSbitfield flags) override;
        };

        class MappedBufferRange
        {
            friend class Buffer;
            private:
                const Buffer& sourceBuffer;
                GLintptr offset = 0;
                GLsizeiptr length = 0;
                bool valid = true;

                MappedBufferRange(const Buffer& sourceBuffer, GLintptr offset, GLsizeiptr length, void* mappedData);
            public:
                void* mappedData = nullptr;

                ~MappedBufferRange();
                MappedBufferRange(MappedBufferRange&& other) = delete;
                MappedBufferRange& operator=(MappedBufferRange&& other) = delete;
                MappedBufferRange(const MappedBufferRange& other) = delete;
                MappedBufferRange& operator=(const Buffer& other) = delete;

                bool hasReadAccess();
                bool hasWriteAccess();
                bool isPersistent();
                bool isCoherent();
                bool isValid();
        };
    }
}

#endif
