#include <flare/utility/file.h>

#include <fstream>

namespace Flare
{
    namespace File
    {
        std::vector<uint8_t> getFileContents(const std::string &filePath)
        {
            std::vector<uint8_t> fileBuffer;

            std::ifstream inFileStream(filePath, std::ios::binary);

            if (!inFileStream) {
                return fileBuffer;
            }

            inFileStream.seekg(0, std::ios::end);
            auto fileLength = inFileStream.tellg();
            inFileStream.seekg(0, std::ios::beg);

            fileBuffer.resize(fileLength);
            inFileStream.read(reinterpret_cast<char *>(fileBuffer.data()), fileLength);

            return fileBuffer;
        }
    }
}
