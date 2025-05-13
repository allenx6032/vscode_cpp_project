#pragma once

#include "Approval_QuietReporter.h"
#include "Approval_FrontLoadedReporterDisposer.h"

namespace ApprovalTests
{
    namespace CIBuildOnlyReporterUtils
    {
        FrontLoadedReporterDisposer
        useAsFrontLoadedReporter(const std::shared_ptr<Reporter>& reporter);
    }
} // namespace ApprovalTests
