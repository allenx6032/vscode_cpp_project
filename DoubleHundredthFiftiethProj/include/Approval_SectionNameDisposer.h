#pragma once

#include "Approval_ApprovalTestNamer.h"
#include "Approval_Macros.h"

namespace ApprovalTests
{
    class APPROVAL_TESTS_NO_DISCARD SectionNameDisposer
    {
    public:
        SectionNameDisposer(TestName& currentTest_, const std::string& scope_name);
        SectionNameDisposer(const SectionNameDisposer&) = default;

        ~SectionNameDisposer();

    private:
        TestName& currentTest;
    };
}
