#include "Approval_ReporterFactory.h"

#include "Approval_AutoApproveIfMissingReporter.h"
#include "Approval_AutoApproveReporter.h"
#include "Approval_BlockingReporter.h"
#include "Approval_CIBuildOnlyReporter.h"
#include "Approval_ClipboardReporter.h"
#include "Approval_CombinationReporter.h"
#include "Approval_CrossPlatformReporters.h"
#include "Approval_DefaultFrontLoadedReporter.h"
#include "Approval_DefaultReporter.h"
#include "Approval_DiffReporter.h"
#include "Approval_EnvironmentVariableReporter.h"
#include "Approval_LinuxReporters.h"
#include "Approval_MacReporters.h"
#include "Approval_QuietReporter.h"
#include "Approval_TextDiffReporter.h"
#include "Approval_WindowsReporters.h"

#include <map>
#include <functional>

#define APPROVAL_TESTS_REGISTER_REPORTER(name)                                           \
    map[#name] = []() { return std::unique_ptr<Reporter>(new name); }

namespace ApprovalTests
{

    std::string getOsPrefix()
    {
        if (SystemUtils::isMacOs())
        {
            return "Mac::";
        }

        if (SystemUtils::isWindowsOs())
        {
            return "Windows::";
        }

        return "Linux::";
    }

    ReporterFactory::ReporterFactory() : map(createMap())
    {
    }

    ReporterFactory::Reporters ReporterFactory::createMap()
    {
        Reporters map;

        APPROVAL_TESTS_REGISTER_REPORTER(AutoApproveIfMissingReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(AutoApproveReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(CIBuildOnlyReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(ClipboardReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(DefaultFrontLoadedReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(DefaultReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(DiffReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(EnvironmentVariableReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(QuietReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(TextDiffReporter);

        APPROVAL_TESTS_REGISTER_REPORTER(Linux::BeyondCompareReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Linux::MeldReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Linux::SublimeMergeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Linux::KDiff3Reporter);

        // begin-snippet: register_reporter_with_factory
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::AraxisMergeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::BeyondCompareReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::DiffMergeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::KaleidoscopeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::P4MergeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::SublimeMergeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::KDiff3Reporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::TkDiffReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::VisualStudioCodeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Mac::CLionDiffReporter);
        // end-snippet

        APPROVAL_TESTS_REGISTER_REPORTER(Windows::TortoiseDiffReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Windows::TortoiseGitDiffReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Windows::BeyondCompareReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Windows::WinMergeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Windows::AraxisMergeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Windows::CodeCompareReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Windows::SublimeMergeReporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Windows::KDiff3Reporter);
        APPROVAL_TESTS_REGISTER_REPORTER(Windows::VisualStudioCodeReporter);

        APPROVAL_TESTS_REGISTER_REPORTER(CrossPlatform::VisualStudioCodeReporter);

        return map;
    }

    std::unique_ptr<Reporter>
    ReporterFactory::createReporter(const std::string& reporterName) const
    {
        auto osPrefix = getOsPrefix();

        auto key = findReporterName(osPrefix, reporterName);

        if (!key.empty())
        {
            return map.at(key)();
        }

        return std::unique_ptr<Reporter>();
    }

    std::vector<std::string> ReporterFactory::allSupportedReporterNames() const
    {
        std::vector<std::string> result;

        for (auto& p : map)
        {
            result.push_back(p.first);
        }

        return result;
    }

    std::string ReporterFactory::findReporterName(const std::string& osPrefix,
                                                  const std::string& reporterName) const
    {
        auto trimmedReporterName = StringUtils::trim(reporterName);
        trimmedReporterName = StringUtils::toLower(trimmedReporterName);

        std::vector<std::string> candidateNames = {
            trimmedReporterName,
            // Allow program names to be specified without Reporter suffix
            trimmedReporterName + "reporter",
            // Allow names without os namespace
            StringUtils::toLower(osPrefix) + trimmedReporterName,
            StringUtils::toLower(osPrefix) + trimmedReporterName + "reporter",
        };

        for (auto& candidateName : candidateNames)
        {
            auto iter = std::find_if(
                map.begin(), map.end(), [&](const Reporters::value_type pair) {
                    return StringUtils::toLower(pair.first) == candidateName;
                });

            if (iter != map.end())
            {
                return iter->first;
            }
        }

        return std::string{};
    }
}
