#include <mrr/graphics/waypoint.hxx>

namespace mrr {
namespace graphics {
namespace gl {

waypoint::waypoint(::glm::vec3 location, double radius)
	: location_(location), radius_(radius)
{
}

void waypoint::set_location(::glm::vec3 const& location)
{
	location_ = location;
}

::glm::vec3 const& waypoint::get_location() const
{
	return location_;
}

void waypoint::set_radius(double radius)
{
	radius_ = radius;
}

double waypoint::get_radius() const
{
	return radius_;
}

void waypoint::add_action(model& m, action_type const& action)
{
	actions_[&m].push_back(action);
}

auto waypoint::get_actions() const
	-> ::std::map<model*, std::vector<action_type> > const&
{
	return actions_;
}

} // namespace gl
} // namespace graphics
} // namespace mrr
