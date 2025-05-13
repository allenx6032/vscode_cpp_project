#include "Approval_SubdirectoryDisposer.h"
#include "Approval_ApprovalTestNamer.h"

namespace ApprovalTests
{
    SubdirectoryDisposer::SubdirectoryDisposer(std::string subdirectory)
    {
        previous_result = ApprovalTestNamer::testConfiguration().subdirectory;
        ApprovalTestNamer::testConfiguration().subdirectory = std::move(subdirectory);
    }

    SubdirectoryDisposer::~SubdirectoryDisposer()
    {
        ApprovalTestNamer::testConfiguration().subdirectory = previous_result;
    }
}
