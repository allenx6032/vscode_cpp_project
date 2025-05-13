#pragma once

#include "Approval_Reporter.h"
#include "Approval_AutoApproveReporter.h"
#include "Approval_FileUtils.h"

namespace ApprovalTests
{
    class AutoApproveIfMissingReporter : public Reporter
    {
    public:
        bool report(std::string received, std::string approved) const override;
    };
}
