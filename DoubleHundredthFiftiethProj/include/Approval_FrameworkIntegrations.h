#pragma once

#include "Approval_ApprovalTestNamer.h"
#include "Approval_FileApprover.h"

namespace ApprovalTests
{
    class FrameworkIntegrations
    {
    public:
        static void
        setTestPassedNotification(FileApprover::TestPassedNotification notification);

        static void setCurrentTest(ApprovalTests::TestName* currentTest);
    };
}
