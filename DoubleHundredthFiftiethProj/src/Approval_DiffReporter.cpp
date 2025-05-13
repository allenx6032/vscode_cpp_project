#include "Approval_DiffReporter.h"

#include "Approval_EnvironmentVariableReporter.h"
#include "Approval_WindowsReporters.h"
#include "Approval_MacReporters.h"
#include "Approval_LinuxReporters.h"
#include "Approval_CrossPlatformReporters.h"

namespace ApprovalTests
{
    DiffReporter::DiffReporter()
        : FirstWorkingReporter({new EnvironmentVariableReporter(),
                                new Mac::MacDiffReporter(),
                                new Linux::LinuxDiffReporter(),
                                new Windows::WindowsDiffReporter(),
                                new CrossPlatform::CrossPlatformDiffReporter()})
    {
    }
}
