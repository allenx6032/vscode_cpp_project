#ifndef __SGE_FILE_LOCATOR_HPP
#define __SGE_FILE_LOCATOR_HPP

#include <sge_locator.hpp>

namespace sge
{
    class FileLocator : public AssetLocator
    {
        public:
            FileLocator(const std::string &location);

            virtual SDL_RWops *locate(const std::string &assetname, bool binary);

        private:
            std::string _location;
    };
}

#endif /* __SGE_FILE_LOCATOR_HPP */
