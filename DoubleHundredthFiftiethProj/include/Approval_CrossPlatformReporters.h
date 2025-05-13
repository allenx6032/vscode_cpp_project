#pragma once

#include "Approval_GenericDiffReporter.h"
#include "Approval_FirstWorkingReporter.h"

namespace ApprovalTests
{
    namespace CrossPlatform
    {
        class VisualStudioCodeReporter : public GenericDiffReporter
        {
        public:
            VisualStudioCodeReporter();
        };

        class CrossPlatformDiffReporter : public FirstWorkingReporter
        {
        public:
            CrossPlatformDiffReporter();
        };
    }
}
