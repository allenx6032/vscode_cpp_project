#pragma once

#include "Approval_Macros.h"
#include "Approval_FileNameSanitizerFactory.h"

namespace ApprovalTests
{
    class APPROVAL_TESTS_NO_DISCARD FileNameSanitizerDisposer
    {
    private:
        FileNameSanitizer previous_result;

    public:
        explicit FileNameSanitizerDisposer(FileNameSanitizer sanitizer);
        FileNameSanitizerDisposer(const FileNameSanitizerDisposer&) = default;

        ~FileNameSanitizerDisposer();
    };
}
