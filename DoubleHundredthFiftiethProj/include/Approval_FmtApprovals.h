#pragma once
#ifdef FMT_VERSION
#include "Approval_FmtToString.h"
#include "Approval_Approvals.h"
namespace ApprovalTests
{
    using FmtApprovals =
        ApprovalTests::TApprovals<ApprovalTests::ToStringCompileTimeOptions<FmtToString>>;
}
#endif
