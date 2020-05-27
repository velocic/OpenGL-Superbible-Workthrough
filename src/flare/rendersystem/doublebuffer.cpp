#include <flare/rendersystem/doublebuffer.h>

namespace Flare
{
    namespace RenderSystem
    {
        void DoubleBuffer::destroy()
        {
            readBuffer = nullptr;
            writeBuffer = nullptr;
        }

        void DoubleBuffer::swap()
        {
            readBuffer.swap(writeBuffer);
        }
    }
}
