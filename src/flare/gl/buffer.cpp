#include <flare/gl/buffer.h>

namespace Flare
{
    namespace GL
    {
        Buffer::Buffer()
        {
            glCreateBuffers(1, &glBuffer);
        }

        Buffer::~Buffer()
        {
            destroy();
        }

        Buffer::Buffer(Buffer&& other)
        {
        }

        Buffer::Buffer& operator=(Buffer&& other)
        {
        }

        Buffer::bind()
        {
            glBindBuffer(target, glBuffer);
        }

        Buffer::namedBufferStorage(GLsizei size, const void* data, GLbitfield flags)
        {
            glNamedBufferStorage(glBuffer, size, data, flags);
            isGPUStorageInitialized = true;
            dataCapacityBytes = size;

            //TODO: store flags in usageflags struct
        }

        Buffer::destroy()
        {
            glDeleteBuffers(1, &glBuffer);
            isGPUStorageInitialized = false;
            dataCapacityBytes = 0;
        }
    }
}
