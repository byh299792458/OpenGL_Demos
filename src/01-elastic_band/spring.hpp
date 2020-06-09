#ifndef __SPRING_HPP__
#define __SPRING_HPP__

#include "mass.hpp"

class Spring {
public:
    Spring(Mass* a, Mass* b, double k)
        : m1_(a), m2_(b), k_(k), rest_length_(glm::distance(a->position_ , b->position_)) 
        {}

    float k_;
    float rest_length_;

    Mass* m1_;
    Mass* m2_;

};

#endif // !__SPRING_HPP__
