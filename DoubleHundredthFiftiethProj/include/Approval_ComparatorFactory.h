#pragma once

#include <memory>
#include "Approval_ApprovalComparator.h"
#include "Approval_ComparatorDisposer.h"

namespace ApprovalTests
{

    class ComparatorFactory
    {
    private:
        static ComparatorContainer& comparators();

    public:
        static ComparatorDisposer
        registerComparator(const std::string& extensionWithDot,
                           std::shared_ptr<ApprovalComparator> comparator);

        static std::shared_ptr<ApprovalComparator>
        getComparatorForFile(const std::string& receivedPath);

        static std::shared_ptr<ApprovalComparator>
        getComparatorForFileExtensionWithDot(const std::string& fileExtensionWithDot);
    };
}
