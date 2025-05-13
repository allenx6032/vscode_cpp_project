#pragma once

#include "Approval_Reporter.h"
#include "Approval_DefaultFrontLoadedReporter.h"

#include <memory>

namespace ApprovalTests
{
    //! Implementation detail of Approvals::useAsFrontLoadedReporter()
    class FrontLoadedReporterFactory
    {
        static std::shared_ptr<Reporter>& frontLoadedReporter();

    public:
        static std::shared_ptr<Reporter> getFrontLoadedReporter();

        static void setFrontLoadedReporter(const std::shared_ptr<Reporter>& reporter);
    };
}
