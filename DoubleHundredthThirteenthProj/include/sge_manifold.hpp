#ifndef __SGE_MANIFOLD_HPP
#define __SGE_MANIFOLD_HPP

#include <sge_node.hpp>
#include <sge_vector.hpp>
#include <SDL.h>

namespace sge
{
    struct Manifold
    {
        std::shared_ptr<Node> a;
        std::shared_ptr<Node> b;
        Vector mtv;
    };
}

#endif /* __SGE_MANIFOLD_HPP */
