#include "Approval_QuietReporter.h"

namespace ApprovalTests
{
    bool QuietReporter::report(std::string, std::string) const
    {
        return true;
    }
}
