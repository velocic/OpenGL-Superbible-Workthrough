#ifndef FLARE_GL_BUFFER_H
#define FLARE_GL_BUFFER_H

namespace Flare
{
    namespace GL
    {
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

                GLuint glBuffer = 0;
                GLsizei dataCapacityBytes = 0;
                bool isGPUStorageInitialized = false;
                UsageFlags usageFlags;
            public:
                Buffer();
                ~Buffer();
                Buffer(Buffer&& other);
                Buffer& operator=(Buffer&& other);
                Buffer(const Buffer& other) = delete;
                Buffer& operator=(const Buffer& other) = delete;

                bind(GLenum target);
                destroy();

                /*Flags:
                 * GL_DYNAMIC_STORAGE_BIT - Buffer contents can be updated directly
                 * GL_MAP_READ_BIT - Buffer data store will be mapped for reading
                 * GL_MAP_WRITE_BIT - Buffer data store will be mapped for writing
                 * GL_MAP_PERSISTENT_BIT - Buffer data store can be mapped persistently
                 * GL_MAP_COHERENT_BIT - Buffer maps are to be coherent
                 * GL_CLIENT_STORAGE_BIT - If all other conditions can be met, prefer CPU storage over GPU storage*/ 
                namedBufferStorage(GLsizeiptr size, const void* data, GLbitfield flags);
        };
    }
}

#endif
