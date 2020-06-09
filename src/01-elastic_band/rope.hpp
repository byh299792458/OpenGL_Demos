#ifndef __ROPE_HPP__
#define __ROPE_HPP__

#include <vector>
#include "mass.hpp"
#include "spring.hpp"

#define vector std::vector

class Rope {
public:
    Rope(vector<Mass*>& masses, vector<Spring*>& springs)
        : masses_(masses), springs_(springs) {}
    Rope(glm::vec3 start, glm::vec3 end, int num_nodes, float node_mass, float k,
        vector<int> pinned_nodes);

    void SimulateVerlet(double delta_t, glm::vec3 gravity, double kd);
    void SimulateEuler(double delta_t, glm::vec3 gravity, double kd);

    vector<Mass*> masses_;
    vector<Spring*> springs_;

};

#undef vector

#endif // !__ROPE_HPP__
