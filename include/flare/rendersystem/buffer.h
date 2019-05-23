#ifndef FLARE_RENDERSYSTEM_BUFFER_H
#define FLARE_RENDERSYSTEM_BUFFER_H

#include <flare/rendersystem/datatypes.h>
#include <flare/rendersystem/datalayout.h>

namespace Flare
{
    namespace RenderSystem
    {
        class MappedBufferRange;

        class Buffer
        {
            public:
                Buffer(const VertexDataLayout& bufferContentDescription) {}
                virtual ~Buffer() = 0;
                Buffer(Buffer&& other) {}
                Buffer& operator=(Buffer&& other) { return *this; }
                Buffer(const Buffer& other) = delete;
                Buffer& operator=(const Buffer& other) = delete;

                virtual void bind(RSenum target) = 0;
                virtual void clearRange(RSenum internalFormat, RSintptr offset, RSsizeiptr size, RSenum format, RSenum type, const void *data) = 0;
                virtual void copyRange(const Buffer &readBuffer, RSintptr readOffset, RSintptr writeOffset, RSsizeiptr size) = 0;
                virtual void destroy() = 0;
                virtual const VertexDataLayout &getContentDescription() const = 0;
                virtual RSuint getName() const = 0;
                virtual MappedBufferRange *mapRange(RSintptr offset, RSsizeiptr length) = 0;
                virtual void bufferData(RSsizei size, const void *data, RSenum usage) = 0;
                virtual void bufferRange(RSintptr offset, RSsizeiptr size, const void *data) = 0;
                virtual void allocateBufferStorage(RSsizei size, const void *data, RSbitfield flags) = 0;
                virtual void unmap() = 0;
        };

        class MappedBufferRange
        {
            protected:
                MappedBufferRange(const Buffer &sourceBuffer, RSintptr offset, RSsizeiptr length, void *mappedData) {}

            public:
                void *mappedData = nullptr;

                virtual ~MappedBufferRange();
                MappedBufferRange(MappedBufferRange&& other) = delete;
                MappedBufferRange &operator=(MappedBufferRange&& other) = delete;
                MappedBufferRange(const MappedBufferRange& other) = delete;
                MappedBufferRange &operator=(const Buffer& other) = delete;

                virtual bool hasReadAccess() = 0;
                virtual bool hasWriteAccess() = 0;
                virtual bool isPersistent() = 0;
                virtual bool isCoherent() = 0;
                virtual bool isValid() = 0;
        };
    }
}

#endif
