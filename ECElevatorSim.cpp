//
//  ECElevatorSim.cpp
//  
//
//  Created by Yufeng Wu on 6/27/23.
//  Elevator simulation


#include "ECElevatorSim.h"

using namespace std;

// Constructor 
ECElevatorSim::ECElevatorSim(int numFloors, std::vector<ECElevatorSimRequest> &listRequests) : numFloors(numFloors), requests(listRequests), curFloor(1),nextFloor(0), direction(EC_ELEVATOR_STOPPED), directionHistory(EC_ELEVATOR_STOPPED),
curTime(0),justLoaded(false), curState(nullptr) {}
// Destructor
ECElevatorSim::~ECElevatorSim() {if (curState != nullptr) delete curState;}

// Main simulate function 
void ECElevatorSim :: Simulate(int lenSim){
    // we dont want to load twice in a row 
    // because the elevator always starts moving after a load/unload
    // so we create this boolean to track if we just loaded
    bool justLoaded = false;
    // iterate through each time step
    for (;curTime < lenSim; curTime++){
        // if there are passengers to load/unload do it and go to next time step
        if ((!justLoaded) && (Load()|Unload())){
            justLoaded = true;
            continue;
        }
        else justLoaded = false;
        // get highest priority floor
        nextFloor = GetPriority();
        // if there is a request move 1 floor towards it in each time step
        if (nextFloor) Move(nextFloor);
    }
} 
// Progress 1 TimeStep
void ECElevatorSim :: TimeStep(){
    if(justLoaded) {
        // no longer just loaded
        direction = directionHistory;
        justLoaded = false;
        // update variables needed for visualization
        for (auto &req : requests){

            if (req.GetJustServiced()) req.SetJustServiced(false);
            if (req.GetJustBoarded()) req.SetJustBoarded(false);

        }
    }
    // check for load/unload 
    // cant load twice in a row to adhere to rules of Part 1
    else if(Load()|Unload()){
        justLoaded = true;
        // keep track of direction before unload/load stoppage to maintain proper direction
        directionHistory = direction;
        // stop the elevator for passengers to load/unload
        direction = EC_ELEVATOR_STOPPED;
        curTime++;
        return;
    }
    // get next floor to go to
    nextFloor = GetPriority();
    // if there is a request move 1 floor towards it in each time step
    if (nextFloor) Move(nextFloor);
    curTime++;
}


// Finds the floor with the highest priority (where the elevator will go next)
int ECElevatorSim :: GetPriority(){
    // call appropriate state
    if (direction == EC_ELEVATOR_DOWN){
        SetCurState(new MovingDown(requests, curFloor, curTime));
        return GetCurState()->getNextFloor();
    }
    else if (direction == EC_ELEVATOR_UP){
        SetCurState(new MovingUp(requests, curFloor, curTime));
        return GetCurState()->getNextFloor();
    }
    else{
        SetCurState(new Stopped(requests, curFloor, curTime, justLoaded));
        return GetCurState()->getNextFloor();
    }
}
// Load all passengers on curFloor
bool ECElevatorSim :: Load(){
    bool load = false;
    for (auto &req : requests){
        // dont load people who have already been loaded
        if (req.IsServiced() || req.IsFloorRequestDone()) continue;
        // check conditions for loading
        else if (req.GetRequestedFloor() == curFloor && req.GetTime() <= curTime) {
            // update variables
            load = true;
            req.SetFloorRequestDone(true);
            req.SetJustBoarded(true);
            nextFloor = req.GetRequestedFloor();
        }
    }
    // this tells the main simulate function whether a loading event occured
    return load; 
}
// Unload all passengers on curFloor
bool ECElevatorSim :: Unload(){
    bool load = false;
    for (auto &req : requests){
        // dont unload passengers who have arrived
        if (req.IsServiced()) continue;
        // check conditions for unloading
        else if ((req.IsFloorRequestDone()) && (req.GetFloorDest() == curFloor)){
            // update variables
            req.SetArriveTime(curTime);
            load = true;
            req.SetServiced(true);
            req.SetJustServiced(true);
            nextFloor = req.GetRequestedFloor();
        }
    }
    return load; 
}
// Move 1 floor
void ECElevatorSim :: Move(int dest){
    // dont move if our targ is the floor we're on
    if (dest == curFloor) return;
    // change direction accordingly
    direction = (dest > curFloor) ? EC_ELEVATOR_UP : EC_ELEVATOR_DOWN;
    // increment accordingly
    curFloor += (direction == EC_ELEVATOR_UP) ? 1 : -1;
}


