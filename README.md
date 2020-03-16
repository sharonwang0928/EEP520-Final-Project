# EEP520-Final-Project

Overview of what the goal of the project is
---
The goal of this project is to create a 2D simulation of particle swarm optimization (PSO) method, which iteratively find a minimum value with regard to a given function using ENVIRO.

Description of key challenges and how they were addressed
---
- The main challenge of this project is how to coordinate particle’s movement for each iteration. A swarm (particle controller/coordinator) was created to initializes, update, and stop particle’s movement and position. See the following logics:
  - Swarm initializes the particle’s position and velocity with a uniformly distributed random vector.
  - It subscribes for events from particles such as particle arrival, and it gets the position of each particle and determine if such position leads to a minimum for the entire swarm iteratively. Then, it updates the particle’s next position and velocity. 
  - The program will terminate when the number of iterations has performed, and the best particle will be highlighted with a message displays its coordinate. 


Describes how to install and run the code (assuming the user has docker installed)
---

1. Start a Docker container with ENVIRO loaded.

2. Start a web server from which  the browser can access the ENVRIO client, then do:

```bash
esm start
```

Describes how to run and/or use the project
---

- Make the project and start the simulation, do:

```bash
make
enviro
```
- The program will terminate when it reaches the given number of iterations. 

Sources of information or code used
---
- ENVIRO and examples from the lecture 


