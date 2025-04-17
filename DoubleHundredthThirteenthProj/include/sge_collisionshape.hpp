#ifndef __SGE_COLLISION_SHAPE_NODE_HPP
#define __SGE_COLLISION_SHAPE_NODE_HPP

#include <sge_position.hpp>
#include <sge_shape.hpp>

namespace sge
{
    class CollisionShapeNode : public PositionNode
    {
        using PositionNode::PositionNode;

        public:
            virtual std::vector<std::string> mro() const;

            void set_shape(const Shape &shape);
            Shape get_shape();

        private:
            Shape _shape;
    };
}

#endif /* __SGE_COLLISION_SHAPE_NODE_HPP */
