#ifndef FLARE_GL_UNIFORMBLOCK_H
#define FLARE_GL_UNIFORMBLOCK_H

namespace Flare
{
    namespace GL
    {
        class UniformBlock
        {
            private:
                const DataLayout dataLayout;
            public:
                UniformBlock(const DataLayout& dataLayout);
        };
    }
}

#endif
