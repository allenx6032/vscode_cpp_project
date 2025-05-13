#include "Approval_StringWriter.h"
#include "Approval_EmptyFileCreatorFactory.h"
#include "Approval_EmptyFileCreatorByType.h"
namespace ApprovalTests
{

    void EmptyFileCreatorFactory::defaultCreator(std::string fullFilePath)
    {
        StringWriter s("", "");
        s.write(fullFilePath);
    }

    EmptyFileCreator EmptyFileCreatorFactory::currentCreator =
        EmptyFileCreatorByType::createFile;
}
