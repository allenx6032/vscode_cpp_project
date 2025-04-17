#ifndef __SGE_ASSET_LOCATOR_HPP
#define __SGE_ASSET_LOCATOR_HPP

#include <sge_error.hpp>

#include <SDL.h>
#include <string>

namespace sge
{
    class AssetLocatorError : public Exception
    {
        using Exception::Exception;
    };

    class AssetLocator
    {
        public:
            virtual SDL_RWops *locate(const std::string &assetname, bool binary) = 0;
    };
}

#endif /* __SGE_ASSET_LOCATOR_HPP */
