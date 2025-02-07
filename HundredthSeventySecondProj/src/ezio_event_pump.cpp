/*
 @ 0xCCCCCCCC
*/

#include "ezio_event_pump.h"

#include "kbase_basic_macros.h"
#include "kbase_error_exception_util.h"

#include "ezio_notifier.h"

#if defined(OS_APPLE)
#include "ezio_event_pump_impl_apple.h"
#elif defined(OS_POSIX)
#include "ezio_event_pump_impl_posix.h"
#elif defined(OS_WIN)
#include "ezio_event_pump_impl_win.h"
#endif

namespace ezio {

EventPump::EventPump(EventLoop* loop)
    : impl_(std::make_unique<Impl>(loop))
{
#if defined(OS_POSIX) && !defined(OS_APPLE)
    impl_->EnableWakeupNotification();
#endif
}

EventPump::~EventPump()
{
#if defined(OS_POSIX) && !defined(OS_APPLE)
    impl_->DisableWakeupNotification();
#endif
}

TimePoint EventPump::Pump(std::chrono::milliseconds timeout,
                          std::vector<IONotification>& notifications)
{
    FORCE_AS_NON_CONST_FUNCTION();

    ENSURE(CHECK, notifications.empty()).Require();
    return impl_->Pump(timeout, notifications);
}

void EventPump::Wakeup()
{
    FORCE_AS_NON_CONST_FUNCTION();

    impl_->Wakeup();
}

void EventPump::RegisterNotifier(Notifier* notifier)
{
    FORCE_AS_NON_CONST_FUNCTION();

    impl_->RegisterNotifier(notifier);
}

void EventPump::UnregisterNotifier(Notifier* notifier)
{
    FORCE_AS_NON_CONST_FUNCTION();

    ENSURE(CHECK, notifier->WatchNoneEvent())(notifier->watching_events()).Require();
    impl_->UnregisterNotifier(notifier);
}

}   // namespace ezio
