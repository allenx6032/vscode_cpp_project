#pragma once

#include "Approval_DefaultReporterFactory.h"
#include "Approval_Macros.h"

namespace ApprovalTests
{
    //! Implementation detail of Approvals::useAsDefaultReporter()
    class APPROVAL_TESTS_NO_DISCARD DefaultReporterDisposer
    {
    private:
        std::shared_ptr<Reporter> previous_result;

    public:
        explicit DefaultReporterDisposer(const std::shared_ptr<Reporter>& reporter);

        ~DefaultReporterDisposer();
    };
}
