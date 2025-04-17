#ifndef __SGE_BODY_NODE_HPP
#define __SGE_BODY_NODE_HPP

#include <sge_position.hpp>
#include <sge_manifold.hpp>

namespace sge
{
    class BodyNode : public PositionNode
    {
        using PositionNode::PositionNode;

        public:
            virtual std::vector<std::string> mro() const;

            virtual void colliding(const Manifold &manifold);
    };
}

#endif /* __SGE_BODY_NODE_HPP */
