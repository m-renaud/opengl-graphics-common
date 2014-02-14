#include <mrr/graphics/waypoint.hxx>

namespace mrr {
namespace graphics {
namespace gl {

waypoint::waypoint(::glm::vec4 location)
	: location_(location)
{
}

void waypoint::set_location(::glm::vec4 const& location)
{
	location_ = location;
}

::glm::vec4 const& waypoint::get_location() const
{
	return location_;
}

void waypoint::add_action(model& m, action_type const& action)
{
	actions_[&m].push_back(action);
}


} // namespace gl
} // namespace graphics
} // namespace mrr
