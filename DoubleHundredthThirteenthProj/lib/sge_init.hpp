#ifndef __SGE_INIT_HPP
#define __SGE_INIT_HPP

#include <sge_error.hpp>
#include <memory>
#include <vector>

namespace sge
{
    class Initializer
    {
        public:
            Initializer();

            virtual void do_initialize() = 0;
            virtual void do_shutdown() = 0;

            void initialize();
            void shutdown();

            bool is_initialized() const;
            bool is_freed() const;

        private:
            bool initialized;
            bool freed;
    };

    class InitError : public Exception
    {
        using Exception::Exception;
    };

    class Startup
    {
        public:
            void add_initializer(std::shared_ptr<Initializer> initializer);

            void initialize();
            void shutdown();

        private:
            std::vector<std::shared_ptr<Initializer>> initializers;
    };
}

#endif /* __SGE_INIT_HPP */
