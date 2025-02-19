#include "al_config.h"

#include "al_al_utils.h"

#include <cassert>
#include <exception>

#include "al_common_alstring.h"
#include "al_core_logging.h"


void eax_log_exception(std::string_view message) noexcept
{
    const auto exception_ptr = std::current_exception();
    assert(exception_ptr);

    try {
        std::rethrow_exception(exception_ptr);
    }
    catch(const std::exception& ex) {
        ERR("%.*s %s\n", al::sizei(message), message.data(), ex.what());
    }
    catch(...) {
        ERR("%.*s %s\n", al::sizei(message), message.data(), "Generic exception.");
    }
}
