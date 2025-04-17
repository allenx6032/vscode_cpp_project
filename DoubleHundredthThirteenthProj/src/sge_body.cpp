#include <sge_body.hpp>
#include <sge_engine.hpp>

using namespace std;

namespace sge
{
    vector<string> BodyNode::mro() const
    {
        auto _mro = PositionNode::mro();
        _mro.push_back("BodyNode");
        return _mro;
    }

    void BodyNode::colliding(const Manifold &) {}
}
