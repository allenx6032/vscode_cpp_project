#pragma once

#include "Approval_ApprovalTestNamer.h"
#include "Approval_Approvals.h"
#include "Approval_TemplatedCustomNamer.h"

namespace ApprovalTests
{
    class SeparateApprovedAndReceivedDirectoriesNamer : public TemplatedCustomNamer
    {
    public:
        SeparateApprovedAndReceivedDirectoriesNamer();

        virtual ~SeparateApprovedAndReceivedDirectoriesNamer() override = default;

        static DefaultNamerDisposer useAsDefaultNamer();
    };
}
