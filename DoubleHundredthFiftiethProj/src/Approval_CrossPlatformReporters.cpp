#include "Approval_CrossPlatformReporters.h"

#include "Approval_DiffPrograms.h"

namespace ApprovalTests
{
    namespace CrossPlatform
    {
        VisualStudioCodeReporter::VisualStudioCodeReporter()
            : GenericDiffReporter(DiffPrograms::CrossPlatform::VS_CODE())
        {
        }

        CrossPlatformDiffReporter::CrossPlatformDiffReporter()
            : FirstWorkingReporter({
                  // begin-snippet: cross_platform_diff_reporters
                  new VisualStudioCodeReporter(),
                  // end-snippet
              })
        {
        }
    }
}
