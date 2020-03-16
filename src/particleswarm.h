 
#ifndef __PARTICLE_SWARM_AGENT__H
#define __PARTICLE_SWARM_AGENT__H 

#include <cmath>
#include <iostream>
#include <limits>
#include <set>
#include "constants.h"
#include "enviro.h"

using namespace enviro;

class ParticleSwarmController : public Process, public AgentInterface {

    public:
    ParticleSwarmController() : Process(), AgentInterface() {
        // Seed the random based on the current time
        srand(time(0));
    }

    void init() {
        // Initialize the number of particles that are done
        particles_done.clear();
        current_iteration = 0;
        min = std::numeric_limits<double>::max();

        // Create particles
        for (int i = 0; i < Constants::NUM_PARTICLES; i++) {
            double x = -Constants::BOUND_X + ( std::rand() % ( 2 * Constants::BOUND_X + 1 ) );
            double y = -Constants::BOUND_Y + ( std::rand() % ( 2 * Constants::BOUND_Y + 1 ) );
            double val = Constants::f(x, y);

            Agent& a = add_agent("Particle", x, y, 0, {{"fill","gray"},{"stroke","black"}});
            a.set_client_id("particle_" + std::to_string(i));

            if (val < min) {
                best_x = x;
                best_y = y;
                min = val;
                best_particle_id = a.get_id();
            }

            std::cout << "Adding particle " << a.get_id() << std::endl;
        }

        std::cout << "Finished adding the particles. Best X = " << best_x << "; Best Y = " << best_y << "; Min = " << min << std::endl;

        // Subscribe for events from the particles
        watch("particle_arrived", [&](Event& e) {
            if ( ! e.value()["client_id"].is_null() ) {
                auto particle_id = e.value()["id"].get<int>();
                auto particle_client_id = e.value()["client_id"].get<std::string>();
                std::cout << "Particle " << particle_client_id << " has arrived to its destination.\n";
                particles_done.insert(particle_client_id);

                // Get the coordinates of the particle
                auto new_x = e.value()["x"].get<double>();
                auto new_y = e.value()["y"].get<double>();
                auto new_value = Constants::f(new_x, new_y);

                // See if the new position of the particle leads to a minimum for the entire swarm
                if (new_value < min) {
                    best_x = new_x;
                    best_y = new_y;
                    min = new_value;
                    best_particle_id = particle_id;
                }

                if (particles_done.size() == Constants::NUM_PARTICLES) {
                    std::cout << "Iteration " << current_iteration << " done!" << std::endl;
                    current_iteration++;
                    if (current_iteration < Constants::MAX_ITERATIONS)
                    {
                        particles_done.clear();
                        std::cout << "Starting next iteration. Best X = " << best_x << "; Best Y = " << best_y << "; Min = " << min << std::endl;
                        emit(Event("move_next", { 
                            { "iteration", current_iteration },
                            { "x", best_x }, 
                            { "y", best_y } 
                        }));
                    }
                    else
                    {
                        // Highlight the best agent
                        find_agent(best_particle_id).decorate("<circle x='-5' y='5' r='5' style='fill: red'></circle>");

                        // Done swarming, emit an event with the result
                        emit(Event("swarm_complete", { 
                            { "value", min },
                            { "x", best_x }, 
                            { "y", best_y } 
                        }));
                    }
                }
            }
        });

        // Start the first iteration
        emit(Event("move_next", { 
            { "iteration", current_iteration },
            { "x", best_x }, 
            { "y", best_y } 
        }));
    }
    void start() {}
    void update() {}
    void stop() {}

    private:
    int best_particle_id;
    std::set<std::string> particles_done;
    int current_iteration;
    double best_x, best_y;
    double min;
    
};

class ParticleSwarm : public Agent {
    public:
    ParticleSwarm(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    ParticleSwarmController c;
};

DECLARE_INTERFACE(ParticleSwarm)

#endif