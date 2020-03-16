#ifndef __MESSAGE_AGENT__H
#define __MESSAGE_AGENT__H 

#include "constants.h"
#include "enviro.h"

using namespace enviro;

class MessageController : public Process, public AgentInterface {

    public:
    MessageController() : Process(), AgentInterface() {}

    void init() {
        // Subscribe for events when the next iteration is starting
        watch("move_next", [&](Event& e) {
            int iteration = e.value()["iteration"];

            label("Running iteration #" + std::to_string(iteration + 1) + " out of " + std::to_string(Constants::MAX_ITERATIONS), 10, 5);
        });

        // Subscribe for swarm complete events so we can display the final result
        watch("swarm_complete", [&](Event& e) {
            double value = e.value()["value"];
            double x = e.value()["x"];
            double y = e.value()["y"];

            label("Done! Found minimum value of " + std::to_string(value) + " at [" + std::to_string(x) + ", " + std::to_string(y) + "]", 10, 5);
        });
    }
    void start() {}
    void update() {}
    void stop() {}

};

class Message : public Agent {
    public:
    Message(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    MessageController c;
};

DECLARE_INTERFACE(Message)

#endif