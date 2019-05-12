#ifndef FLARE_UTILITY_FILE_H
#define FLARE_UTILITY_FILE_H

#include <string>
#include <vector>

namespace Utility
{
    namespace File
    {
        std::vector<uint8_t> getFileContents(const std::string &filePath);
    }
}


#endif
