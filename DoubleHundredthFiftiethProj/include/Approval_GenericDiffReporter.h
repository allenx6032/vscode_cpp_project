#pragma once

#include "Approval_DiffPrograms.h"
#include "Approval_CommandReporter.h"
#include "Approval_SystemLauncher.h"
#include <string>

namespace ApprovalTests
{
    class GenericDiffReporter : public CommandReporter
    {
    public:
        SystemLauncher launcher;

    public:
        explicit GenericDiffReporter(const std::string& program);

        explicit GenericDiffReporter(const DiffInfo& info);
    };
}
