#pragma once

#include <utility>
#include "Approval_ApprovalNamer.h"
#include "Approval_Options.h"

namespace ApprovalTests
{
    class ExistingFileNamer : public ApprovalNamer
    {
        std::string filePath;
        const Options& options_;

    public:
        explicit ExistingFileNamer(std::string filePath_, const Options& options);

        ExistingFileNamer(const ExistingFileNamer& x);

        ExistingFileNamer(ExistingFileNamer&& x) noexcept;

        virtual std::string getApprovedFile(std::string extensionWithDot) const override;

        virtual std::string
            getReceivedFile(std::string /*extensionWithDot*/) const override;
    };
}
