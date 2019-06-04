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
                virtual void bind(RSenum target) = 0;
                virtual void clearRange(RSenum internalFormat, RSintptr offset, RSsizeiptr size, RSenum format, RSenum type, const void *data) = 0;
                virtual void copyRange(const Buffer &readBuffer, RSintptr readOffset, RSintptr writeOffset, RSsizeiptr size) = 0;
                virtual void destroy() = 0;
                virtual const VertexDataLayout &getContentDescription() const = 0;
                virtual RSuint getId() const = 0;
                virtual size_t getName() const = 0;
                virtual MappedBufferRange *mapRange(RSintptr offset, RSsizeiptr length) = 0;
                virtual void bufferData(RSsizei size, const void *data, RSenum usage) = 0;
                virtual void bufferRange(RSintptr offset, RSsizeiptr size, const void *data) = 0;
                virtual void allocateBufferStorage(RSsizei size, const void *data, RSbitfield flags) = 0;
                virtual void unmap() = 0;
        };

        class MappedBufferRange
        {
            public:
                void *mappedData = nullptr;

                virtual bool hasReadAccess() const = 0;
                virtual bool hasWriteAccess() const = 0;
                virtual bool isPersistent() const = 0;
                virtual bool isCoherent() const = 0;
                virtual bool isValid() const = 0;
        };
    }
}

#endif
