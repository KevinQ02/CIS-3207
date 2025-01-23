# Project-4
### *Driving Simulation with Traffic Lights and Road Capacity Limits*  
This project is a simulation of cars driving on roads in a small town. The town has only 7
roads. Each of the roads has a tra;ic light controlling entry onto the road. The residents of
the town have a total of 10 cars that can be running at the same time. Your objective in this
project is to implement a program that allows the cars to drive around on the roads
obeying the constraints that are described below.  

The 10 cars are to drive randomly on 7 di;erent roads. Each road can accommodate up to
5 cars, and additional cars must wait until there is space. That is, wait until there are fewer
than 5 cars on the road, then the car can enter.  

In addition, traffic lights regulate access to the roads, and each road has its own tra;ic
light that switches between red and green at di;erent intervals.  

In the simulation, cars are represented by threads. To control road access and manage
synchronization between car threads, locks (mutexes) and condition variables are used.  

### *Key Features of the Simulation:*   
1. Road Capacity: Each road can have up to 5 cars at a time.  
2. Tra:ic Lights: Each road has a tra;ic light that switches between red and green, but
the duration of the red/green cycle di;ers for each road.  
3. Concurrency Control:  
* Mutexes (Locks): To protect shared resources such as the number of cars on a
road.  
* Condition Variables: To notify waiting cars when space becomes available or
when the tra;ic light turns green.  
### *Components:*  
1. 10 Car Threads: Each car will randomly select a road, check if the road has space
and if the tra;ic light is green, and if both conditions are met, the car enters the
road.  
2. 7 Tra:ic Light Threads: Each road has its own tra;ic light that operates at its own
unique interval. Intervals for green/red can expressed as integers, some the same,
some different in a range such as 5 to 10.  
3. Shared Data:  
* `cars_on_road`: Array to track how many cars are on each road.  
* `tra;ic_light`: Array to track the state of the tra;ic light (1 for green, 0 for red) for
each road.  
### Simulation Breakdown:  
### 1. Define Constants and Global Variables:  
- `NUM_CARS = 10`: Number of car threads.  
- `NUM_ROADS = 7`: Number of roads.  
- `ROAD_CAPACITY = 5`: Maximum number of cars allowed on a road at the same
time.  
- `TRAFFIC_LIGHT_DURATION`: Array storing individual durations for each road's
traffic light cycle (green and red).  
### *2. Car Thread Logic:*  
Each car will:  
- Randomly select a road.  
- Wait if the road is full or if the tra;ic light is red (using condition variables).  
- Enter the road when conditions are met.  
- Leave the road after simulating driving for a random period.*  
### 3. Traffic Light Thread Logic:  
- Each road has its own traffic light that alternates between green and red.  
- The durations of these states are specific to each road.  
### 4. Main Function:  
*The main function initializes the mutexes and condition variables, starts the car and
tra;ic light threads, and runs indefinitely.*  
### *How the Program Works:*  
- Car Threads: Each car repeatedly selects a random road. If the road is full or the
traffic light is red, the car waits using the condition variable. When space is
available and the light is green, it enters the road, simulates driving, and leaves after
some time.  
- Traffic Light Threads: Each road has a separate tra;ic light thread that alternates
between green and red, with intervals specific to each road.  
- Mutexes: Each road has a mutex that ensures only one car modifies the
`cars_on_road` array at a time.  
- Condition Variables: When a road is full or its tra;ic light is red, cars wait on the
corresponding condition variable. Once the road has space or the light turns green,
the condition variable is signaled to allow waiting cars to proceed.*  
## *Output:*  
*As the program executes, you should produce a log of the movement of each car, including
the road it attempts to enter, the number of cars on that road, the condition of the traffic
light and whether it enters the road.  
You should also log the changes of states of the traffic lights.  
You should include timestamps for the events that you log as this will help you in
debugging and determining if all the cars, roads and events are active.*  

## *Example Output:*  
Traffic light on road 0 is GREEN. 13:05:27  
Car 1 entered road 0. Cars on road: 1 13:05:27  
Car 1 left road 0. Cars on road: 0 13:05:34  
Traffic light on road 1 is GREEN. 13:05:35  
Car 3 entered road 1. Cars on road: 0   13:28:51  
 ...   
