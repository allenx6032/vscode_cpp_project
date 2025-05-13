#include "Approval_DefaultFrontLoadedReporter.h"
#include "Approval_CIBuildOnlyReporter.h"

namespace ApprovalTests
{
    DefaultFrontLoadedReporter::DefaultFrontLoadedReporter()
        : FirstWorkingReporter({new CIBuildOnlyReporter()})
    {
    }
}
