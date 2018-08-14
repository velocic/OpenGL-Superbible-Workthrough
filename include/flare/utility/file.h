#ifndef FLARE_UTILITY_FILE_H
#define FLARE_UTILITY_FILE_H

#include <string>
#include <vector>

namespace Utility
{
    namespace File
    {
        bool getFileContents(std::vector<uint8_t> &fileBuffer, const std::string &filePath);
    }
}


#endif
