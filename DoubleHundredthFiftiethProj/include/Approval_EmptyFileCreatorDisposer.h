#pragma once

#include "Approval_Macros.h"
#include "Approval_EmptyFileCreatorFactory.h"

namespace ApprovalTests
{
    class APPROVAL_TESTS_NO_DISCARD EmptyFileCreatorDisposer
    {
    private:
        EmptyFileCreator previous_result;

    public:
        explicit EmptyFileCreatorDisposer(EmptyFileCreator creator);
        EmptyFileCreatorDisposer(const EmptyFileCreatorDisposer&) = default;

        ~EmptyFileCreatorDisposer();
    };
}
