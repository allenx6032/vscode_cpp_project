/*
 @ 0xCCCCCCCC
*/

#include "ezio_io_service_context.h"

#include "kbase_at_exit_manager.h"
#include "kbase_error_exception_util.h"

#include "ezio_this_thread.h"

namespace {

ezio::IOServiceContext* instance = nullptr;

}   // namespace

namespace ezio {

IOServiceContext::IOServiceContext()
{
    this_thread::SetName(kMainThreadName, true);
}

// static
void IOServiceContext::Init()
{
    ENSURE(CHECK, instance == nullptr).Require();
    instance = new IOServiceContext();
    kbase::AtExitManager::RegisterCallback([] {
        delete instance;
        instance = nullptr;
    });
}

// static
const IOServiceContext& IOServiceContext::current()
{
    ENSURE(CHECK, instance != nullptr).Require();
    return *instance;
}

}   // namespace ezio
