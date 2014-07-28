This produces videos simulating collisions between marbles.

Main properties:
* we never accumulate lost of small floating point numbers in a larger one, to avoid floating point precision issues
* the precision of the simulation doesn't depend on the frame rate. Actually, you can just generate a log of the events simulation
* we don't use any O(n²) algorithm, so we can simulate a rather large number of marbles. The main issue is writing the frames to the hard drive.

Run-time to simulate marbles with random initial velocities during 1 minute:
* 125 marbles: 1s
* 241 marbles: 3s
* 455 marbles: 17s
* 704 marbles: 60s


Todo
====

* generate a log of the events simulation
* read initial positions from a file
* generate video in main program instead of writing each frame to disk and calling avconv
* separate the frame generators from the video creator
* read command-line options to know what kind of output must be generated, in which resolution, etc.
* create an ouptut with velocity vectors (long as speed, thick as mass)
* create a demo output combining the different types of outputs on different areas of the video
* add sound on collision... "Spouich spouich" or "tick-tick-tick" :)
