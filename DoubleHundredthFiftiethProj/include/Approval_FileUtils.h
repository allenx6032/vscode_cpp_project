#pragma once

#include <string>
#include "Approval_EmptyFileCreatorFactory.h"
#include "Approval_EmptyFileCreatorDisposer.h"
namespace ApprovalTests
{
    class FileUtils
    {
    public:
        static bool fileExists(const std::string& path);

        static int fileSize(const std::string& path);

        static EmptyFileCreatorDisposer useEmptyFileCreator(EmptyFileCreator creator);

        static void ensureFileExists(const std::string& fullFilePath);

        static std::string getDirectory(const std::string& filePath);

        static std::string getExtensionWithDot(const std::string& filePath);

        static std::string readFileThrowIfMissing(const std::string& fileName);

        static std::string readFileReturnEmptyIfMissing(const std::string& fileName);

        static void writeToFile(const std::string& filePath, const std::string& content);
    };
}
