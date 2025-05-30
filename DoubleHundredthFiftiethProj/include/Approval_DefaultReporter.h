#pragma once

#include "Approval_Reporter.h"

#include <string>

namespace ApprovalTests
{
    class DefaultReporter : public Reporter
    {
    public:
        virtual bool report(std::string received, std::string approved) const override;
    };
}
