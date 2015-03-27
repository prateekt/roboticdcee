--Introduction--

The Robotic DCEE open source project aims to provide software support for robotic sensor networks. The open source project provides a software implementation of the DCEE framework. The codebase was built as a research platform by the USC Teamcore group.

The Robotic DCEE project provides software for coordination on a robotic sensor network. Robots connected on a network can move to optimize their network links. The platform contains implementations of key DCEE algorithms that allow robots to coordinate their movements to improve their signal strengths over time.

DCEE (Distributed Constraint Exploration and Exploitation) is a general purpose framework for multi-agent coordination. DCEE was the result of the application of machine learning techniques to the more general Distributed Constraint Optimization Problems (DCOP) framework.

See publications:

All algorithms, software, and publications that comprise the Robotic DCEE project are property of the USC Teamcore Research group. Under the GPL license, all code is available for free to download, modify, and redistribute for non-commercial purposes.

--System Requirements--

Operating System:
-Any linux/unix distro (recommended: Ubuntu)
-Any Mac OSX version (recommended: Leopard or higher)
-Windows w/ Cygwin and GNU tools (not recommended, but possible to use)

Required Tools:
-GCC compiler with GNU Make
-Java 1.5 or higher
-PlayerStage platform (optional: for robotic deployment)

Note: The software supports running DCEE both on actual robots and in simulation on a single computer.

--Installation Instructions--

1. Unzip RoboticDcee.zip into your favorite directory (i.e. /home/user/me/RoboticDCEE).

From the command line:
unzip RoboticDcee.zip /home/user/me/RoboticDcee

2. Go to the RoboticDcee directory

From the command line:
cd /home/user/me/RoboticDcee

3.The easy-to-use DCEE Configuration Utility provides an intuitive interface for setting up your robotic sensor network. To Launch DCEE Configuration Utility, double click on DCEEConfig.jar.

From the command line:
java -jar DCEEConfig.jar

The software supports running DCEE both on actual robots and in simulation. If you want a demo of the platform in simulation mode on a single computer, continue reading. If you want to deploy DCEE on a network of robots, skip to the "Configuring DCEE: Robot Deployment" section.

IMPORTANT: If you are a newbie to the platform, its recommended that you first try the platform in simulation before deploying it on a robot.

--Configuring DCEE: Simulation mode--

You should see a GUI load with the default configuration. Reading this section means you want a demo of the software in simulation on a single computer.

1. On the bottom of the GUI, click "Default Configuration."

2. Click "Save Configuration."

You should receive a "Configuration Successful" message. If instead, an error message shows up, that means there was a problem compiling and building the platform. To see the errors, go to the RoboticDcee directory and do a manual make. The errors in compilation should show up and you will need to continue debugging on your own. Good luck.

3. The default configuration contains a simulation of two robots connected to each other. The simulation will be run on local host with arbitrary MAC addresses for the robots.

Notice that the robots have ids "0" and "1" respectively. This is their dcop id.

To run, open up two new terminals or terminal tabs (one for each robot). In both terminals, go to the RoboticDCEE  directory.

From the command line:
cd /home/user/me/RoboticDcee

When the robots start, they open up TCP sockets to listen for each other. The order of the robots starting does not matter because a robot will automatically wait until it receives messages from the other robot(s) before starting and begin the simulation only when the other robot(s) have connected.

Once the robots start up, they will start generating their log file on stdout. The simulation will execute for the number of rounds set in the configuration manager (in our case, 10). The simulation will run MPGM as the algorithm for the robots. There are several configurable parameters described in the next section "DCEE Configurable Parameters".

The simulation is SUCCESSFUL once the robots have gone through all their rounds and agreed to terminate the simulation. At this point, both traces will stop.

To start the simulation and see what I'm talking about--

In one terminal, execute:
./dcop 0

In the other terminal, execute:
./dcop 1

Note: You can pipe the robot logs to files.
(i.e. ./dcop 0 >> Robot0.txt)
You can then later parse these files for simulation data.

--DCEE Configurable Parameters--

The DCEE Configuration Utility supports the following operations:

1. To add a robot to the network, type in an IP address for the robot and the robot's hardware MAC address.
> Then click "Add Robot."

Once you've added at least two robots, you can define network properties for your configuration.

2. To set up a network topology, there are two methods.

> A. You can link up robots manually using their robot IDs. For example to connect robot "0" and robot "2," in the "Modify Robot Topologies" section, choose "0" and "2" in the dropdown boxes and click "Add Link."

> B. You can use a topology generator for key graph topologies. For example, if you want to link up the robots in a chain, you can specify the order of Chain Nodes using a comma delimited format.

> Example: 0,2,3 means robots 0 and 2 share a link and robots 2 and 3 share a link.

> Once you have a chain topology, hit "Make Chain" to autogenerate the necessary links.

> If you want to generate a fully-connected graph, hit "Make Full" which makes every robot in the network
> connected to every other robot.

3. If you make a mistake, you can always remove robots or links in the "Remove Network Elements" section.

4. To configure DCEE algorithm parameters, see "DCEE settings." The following parameters can be customized:

> DCEE Algorithm - The algorithm to be run on the robots. The supported algorithms are MPGM1 (also known
> as SE-Optimistic-1), MGMMean1 (also known as SE-Mean-1), BeRebid1, MPGM2 (also known as SE-Optimistic-2),
> MGMMean2 (also known as SE-mean-2), BeRebid2, SE-i-1, and SE-i-2.

> Number of Rounds - The number of rounds the robots agree to run for.

> Maximum Signal Strength - In MPGM1 and MPGM2, this is "max signal strength" parameter. For other algorithms
> this parameter is ignored.

> Average Signal Strength - In MGMMean1 and MGMMean2, this is the "average signal strength" parameter. For other algorithms this parameter is ignored.

> I Parameter - In SE-i-1 and SE-i-2, this is the "i" parameter. For other algorithms this parameter is ignored.

> Movement Amount - The amount a robot should move if it has the green light to move. The default parameter is 0.06 m.

> Player Driver Location - Path to the **.cfg file is for the robot. (i.e. /player/create.cfg).**

6. Hitting save causes saving of the configuration and recompilation of the code. If there are no compilation errors, the build will succeed. If there are compilation errors, the build will fail and you will need to do a "make" manually in the project directory. You will need to investigate and correct the errors manually.

---Configuring DCEE: Robot Deployment---

Finally, time to deploy DCEE on robots! Make sure you have gone through and set all the "DCEE Configurable Parameters"
(see above). Once you have a working build and are ready, follow these instructions:

1. You only need to configure/compile the build once if your robot operating systems are identical on all your robots. Otherwise you will need to configure a copy of the DCOP source on every machine.

Once you are ready, port your entire RoboticDcee directory from your build machine to each of your robots.

2. To Run on each robot, from the RoboticDcee project directory do

> ./dcop [id](id.md) where id is the id of the robot (i.e. ./dcop 0).

> Do this on all the robots.

> The DCOP program will start on the machine soon after. It will try to connect to other robots. Once it is connected to all its neighbors, the DCOP rounds will start. Watch the robots coordinate their movements and enjoy as the distributed algorithm executes.

Congratulations!