#ifndef FLARE_GL_BUFFER_H
#define FLARE_GL_BUFFER_H

#include <memory>

#include <GL/gl3w.h>
#include <flare/gl/datalayout.h>

namespace Flare
{
    namespace GL
    {
        class MappedBufferRange;

        class Buffer
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

                DataLayout bufferContentDescription;
                UsageFlags usageFlags;
                GLuint glBuffer = 0;
                GLsizei dataCapacityBytes = 0;
                std::unique_ptr<MappedBufferRange> mappedBuffer;
                bool isCurrentlyMapped = false;
                bool isGPUStorageInitialized = false;

                void checkDynamicStorageFlagBeforeWrite();
            public:
                Buffer(const DataLayout& bufferContentDescription);
                ~Buffer();
                Buffer(Buffer&& other);
                Buffer& operator=(Buffer&& other);
                Buffer(const Buffer& other) = delete;
                Buffer& operator=(const Buffer& other) = delete;

                void bind(GLenum target);
                void clearNamedBufferSubData(GLenum internalFormat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data);
                void copyNamedBufferSubData(const Buffer& readBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
                void destroy();
                const DataLayout &getBufferContentDescription() const {return bufferContentDescription;}
                GLuint getName() const {return glBuffer;}
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
                void unmap();
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
