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

        Buffer& Buffer::operator=(Buffer&& other)
        {
        }

        void Buffer::bind(GLenum target)
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

        Buffer::UsageFlags Buffer::getUsageFlags() const
        {
            return usageFlags;
        }

        void Buffer::namedBufferStorage(GLsizei size, const void* data, GLbitfield flags)
        {
            if (isGPUStorageInitialized) {
                //TODO: figure out tests for error cases
                return;
            }

            glNamedBufferStorage(glBuffer, size, data, flags);
            isGPUStorageInitialized = true;
            dataCapacityBytes = size;

            usageFlags = UsageFlags{
                flags & GL_DYNAMIC_STORAGE_BIT,
                flags & GL_MAP_READ_BIT,
                flags & GL_MAP_WRITE_BIT,
                flags & GL_MAP_PERSISTENT_BIT,
                flags & GL_MAP_COHERENT_BIT,
                flags & GL_CLIENT_STORAGE_BIT
            };
        }
    }
}
