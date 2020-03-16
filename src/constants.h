#ifndef __CONSTANTS__H
#define __CONSTANTS__H 

#include <cmath>

class Constants
{
    public:
    // 2D function for which the Particle Swarm Optimization is done
    static inline double f(double x, double y) { return x * x - 8 * x + y * y + 7; }

    // Arena bounds
    static inline int const BOUND_X = 320;
    static inline int const BOUND_Y = 175;

    // Total number of particles in the simulation
    static inline int const NUM_PARTICLES = 100;

    // Maximum number of iterations
    static inline int const MAX_ITERATIONS = 10;
};

#endif