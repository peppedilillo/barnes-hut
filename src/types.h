#ifndef BARNESHUT_TYPES_H
#define BARNESHUT_TYPES_H

using Time = float;
using Length = float;
using Velocity = float;
using Acceleration = float;
using Force = float;
using Mass = float;

template<typename type>
struct Vector {
    type x, y;
};

struct Particle {
    Mass mass;
    Vector<Length> r{};
    Vector<Velocity> v{};
    Vector<Acceleration> a = {0., 0.};      // a_i
    Vector<Acceleration> a_ = {0., 0.};     // a_i+1
};

#endif
