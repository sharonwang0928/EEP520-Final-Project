#ifndef __PARTICLE_AGENT__H
#define __PARTICLE_AGENT__H 

#include <cmath>
#include <iostream>
#include "constants.h"
#include "enviro.h"

using namespace enviro;

class ParticleController : public Process, public AgentInterface {

    public:
    ParticleController() : Process(), AgentInterface() {}

    void init() {
        // Keep track of the best position
        should_move = false;
        best_x = x();
        best_y = y();
        min = Constants::f(best_x, best_y);
        velocity_x = -(2 * Constants::BOUND_X) + (std::rand() % (4 * Constants::BOUND_X + 1 ));
        velocity_y = -(2 * Constants::BOUND_Y) + (std::rand() % (4 * Constants::BOUND_Y + 1 ));

        // Subscribe to events from the coordinator
        watch("move_next", [&](Event& e) {
            // The swarm sent us the best next position
            double swarm_best_x = e.value()["x"];
            double swarm_best_y = e.value()["y"];
            
            // Compute the next velocity 
            double rp = std::rand() * 1.0 / RAND_MAX;
            double rg = std::rand() * 1.0 / RAND_MAX;
            velocity_x = OMEGA * velocity_x + PHI_P * rp * (best_x - x()) + PHI_G * rg * (swarm_best_x - x());
            velocity_y = OMEGA * velocity_y + PHI_P * rp * (best_y - y()) + PHI_G * rg * (swarm_best_y - y());

            // Update the goal of the particle
            goal_x = std::min(Constants::BOUND_X * 1.0, std::max(-Constants::BOUND_X * 1.0, x() + velocity_x));
            goal_y = std::min(Constants::BOUND_Y * 1.0, std::max(-Constants::BOUND_Y * 1.0, y() + velocity_y));

            std::cout << "Coordinator said I (" << get_client_id() << ") need to move from " << x() << "," << y() << " to " << goal_x << "," << goal_y << "!" << std::endl;

            should_move = true;
        });
    }
    void start() {}
    void update() {
        if (should_move) {
            double delta_x = std::abs(goal_x - x());
            double delta_y = std::abs(goal_y - y());

            // Check if we're in the vicinity of our goal
            if (delta_x < EPSILON && delta_y < EPSILON) {
                std::cout << "I (" << get_client_id() << ") have arrived at " << x() << "," << y() << "!" << std::endl;

                // Stop the movement
                damp_movement();

                // See if we've hit a better location for this particle
                double val = Constants::f(x(), y());

                if (val < min) {
                    best_x = x();
                    best_y = y();
                    min = val;

                    std::cout << "I (" << get_client_id() << ") have found a new best " << best_x << "," << best_y << "," << min << "!" << std::endl;
                }

                // Stop moving until instructed by the coordinator
                should_move = false;

                // Tell the coordinator we've reached our goal
                emit(Event("particle_arrived", {
                    { "id", id() },
                    { "client_id", get_client_id() },
                    { "x", x() },
                    { "y", y() }
                }));
            }
            else {
                move_toward(goal_x, goal_y);
            }
        }
        else {
            // Stop the movement
            damp_movement();
        }
    }
    void stop() {}

    private:
    double best_x, best_y;
    double min;
    double velocity_x, velocity_y;
    double goal_x, goal_y;
    bool should_move;

    const double EPSILON = 1;
    const double OMEGA = 0.5;
    const double PHI_P = 1;
    const double PHI_G = 2;
};

class Particle : public Agent {
    public:
    Particle(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    ParticleController c;
};

DECLARE_INTERFACE(Particle)

#endif