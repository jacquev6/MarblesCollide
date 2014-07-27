This produces videos simulating collisions between marbles.

Todo
====

* read initial positions from a file
* generate video in main program instead of writing each frame to disk and calling avconv
* re-organize EventsHandlers; add a Simulation::addHandler method instead of passing one in the constructor
* separate the frame generators from the video creator
* read command-line options to know what kind of output must be generated, in which resolution, etc.
* create an ouptut with velocity vectors (long as speed, thick as mass)
* create a demo output combining the different types of outputs on different areas of the video
