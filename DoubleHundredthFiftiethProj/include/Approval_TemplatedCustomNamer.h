#pragma once

#include "Approval_Approvals.h"
#include "Approval_NamerFactory.h"
#include "Approval_ApprovalNamer.h"
#include "Approval_Path.h"
#include "Approval_DefaultNamerDisposer.h"

namespace ApprovalTests
{
    class TemplatedCustomNamer : public ApprovalTests::ApprovalNamer
    {
    private:
        ApprovalTests::ApprovalTestNamer namer_;
        std::string template_;

    public:
        explicit TemplatedCustomNamer(std::string templateString);

        APPROVAL_TESTS_NO_DISCARD
        Path constructFromTemplate(const std::string& extensionWithDot,
                                   const std::string& approvedOrReceived) const;

        APPROVAL_TESTS_NO_DISCARD
        std::string getApprovedFile(std::string extensionWithDot) const override;

        APPROVAL_TESTS_NO_DISCARD
        std::string getReceivedFile(std::string extensionWithDot) const override;

        APPROVAL_TESTS_NO_DISCARD
        Path getApprovedFileAsPath(std::string extensionWithDot) const;

        APPROVAL_TESTS_NO_DISCARD
        Path getReceivedFileAsPath(std::string extensionWithDot) const;

        APPROVAL_TESTS_NO_DISCARD
        static std::shared_ptr<TemplatedCustomNamer> create(std::string templateString);

        APPROVAL_TESTS_NO_DISCARD
        static DefaultNamerDisposer useAsDefaultNamer(std::string templateString);
    };
}
