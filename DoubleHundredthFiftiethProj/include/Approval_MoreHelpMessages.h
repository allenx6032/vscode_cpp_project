#pragma once

#include <string>
#include "Approval_ApprovalTestNamer.h"

namespace ApprovalTests
{
    class MoreHelpMessages
    {
    public:
        static void deprecatedFunctionCalled(const std::string& message,
                                             const std::string& file,
                                             int lineNumber);
    };
}
