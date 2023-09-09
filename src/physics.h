#ifndef BARNESHUT_PHYSICS_H
#define BARNESHUT_PHYSICS_H

#include <cmath>
#include "types.h"
#include "quadtree.h"

static constexpr float G = -6.602e-7f;  // [kPc ** -3 kyr ** -2 Sun_mass ** -1]
static constexpr float smoothing = 0.1f;

void integrate(Quadtree &qt, Time dt = 0.01);

#endif
