#pragma once

#include "Approval_SectionNameDisposer.h"

#include <string>

namespace ApprovalTests
{
    struct NamerFactory
    {
        static SectionNameDisposer appendToOutputFilename(const std::string& sectionName);
    };
}
