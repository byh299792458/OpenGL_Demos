#ifndef __MASS_HPP__
#define __MASS_HPP__

#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Mass {
public:
    Mass(glm::vec3 position, float mass, bool is_pinned)
        : start_position_(position), position_(position), last_position_(position),
        mass_(mass), is_pinned_(is_pinned) {}

    float mass_;
    bool is_pinned_;

    glm::vec3 start_position_;
    glm::vec3 position_;

    // explicit Verlet integration

    glm::vec3 last_position_;

    // explicit Euler integration

    glm::vec3 velocity_{ glm::vec3(0,0,0) };
    glm::vec3 forces_{ glm::vec3(0,0,0) };

};


#endif // !__MASS_HPP__
