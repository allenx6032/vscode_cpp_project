#include "Approval_NamerFactory.h"

namespace ApprovalTests
{
    SectionNameDisposer
    NamerFactory::appendToOutputFilename(const std::string& sectionName)
    {
        return SectionNameDisposer(ApprovalTestNamer::currentTest(), sectionName);
    }
}
