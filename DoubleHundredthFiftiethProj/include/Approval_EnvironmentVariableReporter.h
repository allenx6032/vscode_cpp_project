#pragma once

#include "Approval_Reporter.h"
#include "Approval_ReporterFactory.h"
#include <memory>

namespace ApprovalTests
{
    class EnvironmentVariableReporter : public Reporter
    {
    public:
        bool report(std::string received, std::string approved) const override;

        bool report(const std::string& envVar,
                    const std::string& received,
                    const std::string& approved) const;

        static std::string environmentVariableName();

    private:
        ReporterFactory factory;
    };
}
