/*
 @ 0xCCCCCCCC
*/

#include "ezio_ignore_sigpipe.h"

#include <signal.h>

namespace ezio {

IgnoreSigPipe::IgnoreSigPipe() noexcept
{
    signal(SIGPIPE, SIG_IGN);
}

}   // namespace ezio
