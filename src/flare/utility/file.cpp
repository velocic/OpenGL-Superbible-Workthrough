#include <flare/utility/file.h>

#include <fstream>

namespace Utility
{
    namespace File
    {
        bool getFileContents(std::vector<uint8_t> &fileBuffer, const std::string &filePath)
        {
            std::ifstream inFileStream(filePath, std::ios::binary);

            if (!inFileStream) {
                return false;
            }

            inFileStream.seekg(0, std::ios::end);
            auto fileLength = inFileStream.tellg();
            inFileStream.seekg(0, std::ios::beg);

            fileBuffer.resize(fileLength);
            inFileStream.read(reinterpret_cast<char *>(fileBuffer.data()), fileLength);

            return true;
        }
    }
}
