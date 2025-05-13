#pragma once

#include <utility>
#include <memory>
#include "Approval_CommandLauncher.h"
#include "Approval_Reporter.h"
#include "Approval_ConvertForCygwin.h"
#include "Approval_DiffInfo.h"

namespace ApprovalTests
{
    // Generic reporter to launch arbitrary command
    class CommandReporter : public Reporter
    {
    private:
        std::string cmd;
        std::string arguments = DiffInfo::getDefaultArguments();
        CommandLauncher* l;
        std::shared_ptr<ConvertForCygwin> converter;

        std::string assembleFullCommand(const std::string& received,
                                        const std::string& approved) const;

    protected:
        CommandReporter(std::string command, CommandLauncher* launcher);

        CommandReporter(std::string command, std::string args, CommandLauncher* launcher);

    public:
        static bool exists(const std::string& command);

        bool report(std::string received, std::string approved) const override;

        std::string getCommandLine(const std::string& received,
                                   const std::string& approved) const;

    public:
        void checkForCygwin();

        // Seam for testing
        void useCygwinConversions(bool useCygwin);
    };
}
