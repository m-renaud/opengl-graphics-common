#ifndef MRR_GRAPHICS_GL_WAYPOINT_HXX__
#define MRR_GRAPHICS_GL_WAYPOINT_HXX__

#include <mrr/graphics/gl-common.hxx>

#include <glm/glm.hpp>
#include <functional>
#include <map>
#include <vector>

namespace mrr {
namespace graphics {
namespace gl {

class waypoint
{
public:
	using action_type = ::std::function<void(component&)>;
	waypoint() = default;
	waypoint(waypoint const&) = default;
	waypoint(waypoint&&) = default;

	waypoint& operator =(waypoint const&) = default;
	waypoint& operator =(waypoint&&) = default;

	~waypoint() = default;

	waypoint(::glm::vec3 location, double radius);

	void set_location(::glm::vec3 const& location);
	::glm::vec3 const& get_location() const;

	void set_radius(double radius);
	double get_radius() const;

	void add_action(model& m, action_type const& action);
	::std::map<model*, std::vector<action_type> > const& get_actions() const;

	::glm::vec3 location_;
	double radius_;
	::std::map<model*, std::vector<action_type> > actions_;
};

// bool match_waypoint(waypoint const& wp, mrr::graphics::gl::component const& m)
// {
//	return (fabs(wp.location.x - m.getLocation().x) < 0.03)
//		&& (fabs(wp.location.y - m.getLocation().y) < 0.03)
//		&& (fabs(wp.location.z - m.getLocation().z) < 0.03);
// }


} // namespace gl
} // namespace graphics
} // namespace mrr


#endif // #ifndef MRR_GRAPHICS_GL_WAYPOINT_HXX__
