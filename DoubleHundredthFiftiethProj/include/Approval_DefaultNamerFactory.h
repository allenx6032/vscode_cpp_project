#pragma once

#include "Approval_ApprovalNamer.h"

#include <memory>
#include <functional>

namespace ApprovalTests
{

    using NamerCreator = std::function<std::shared_ptr<ApprovalNamer>()>;

    //! Implementation detail of Approvals::useAsDefaultNamer()
    class DefaultNamerFactory
    {
    private:
        static NamerCreator& defaultNamer();

    public:
        static NamerCreator getDefaultNamer();

        static void setDefaultNamer(NamerCreator namer);
    };
}
