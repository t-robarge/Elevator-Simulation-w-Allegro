//
//  ECElevatorSim.h
//  
//
//  Created by Yufeng Wu on 6/27/23.
//  Elevator simulation

#ifndef ECElevatorSim_h
#define ECElevatorSim_h

#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <string>
#include <thread>   // For std::this_thread::sleep_for
#include <chrono> 
#include <algorithm>
using namespace std;

//#include "State.h"

//*****************************************************************************
// DON'T CHANGE THIS CLASS
// 
// Elevator simulation request: 
// (i) time: when the request is made
// (ii) floorSrc: which floor the user is at at present
// (iii) floorDest floor: where the user wants to go; we assume floorDest != floorSrc
// 
// Note: a request is in three stages:
// (i) floor request: the passenger is waiting at floorSrc; once the elevator arrived 
// at the floor (and in the right direction), move to the next stage
// (ii) inside request: passenger now requests to go to a specific floor once inside the elevator
// (iii) Once the passenger arrives at the floor, this request is considered to be "serviced"
//
// two sspecial requests:
// (a) maintenance start: floorSrc=floorDest=-1; put elevator into maintenance 
// starting at the specified time; elevator starts at the current floor
// (b) maintenance end: floorSrc=floorDest=0; put elevator back to operation (from the current floor)

class ECElevatorSimRequest
{
public:
    ECElevatorSimRequest(int timeIn, int floorSrcIn, int floorDestIn) : time(timeIn), floorSrc(floorSrcIn), floorDest(floorDestIn), fFloorReqDone(false), fServiced(false), timeArrive(-1),justServiced(false),justBoarded(false) {} 
    ECElevatorSimRequest(const ECElevatorSimRequest &rhs) : time(rhs.time), floorSrc(rhs.floorSrc), floorDest(rhs.floorDest), fFloorReqDone(rhs.fFloorReqDone), fServiced(rhs.fServiced), timeArrive(rhs.timeArrive),justServiced(rhs.justServiced),justBoarded(rhs.justBoarded) {}
    int GetTime() const {return time; }
    int GetFloorSrc() const { return floorSrc; }
    int GetFloorDest() const { return floorDest; }
    bool IsGoingUp() const { return floorDest >= floorSrc; }
    void Display() const {std::cout << time << floorSrc << floorDest << std::endl;}
    // Is this passenger in the elevator or not
    bool IsFloorRequestDone() const { return fFloorReqDone; }
    void SetFloorRequestDone(bool f) { fFloorReqDone = f; }

    // Is this event serviced (i.e., the passenger has arrived at the desstination)?
    bool IsServiced() const { return fServiced; }
    void SetJustServiced(bool bit) {justServiced = bit;}
    bool GetJustServiced() {return justServiced;}
    void SetJustBoarded(bool bit){justBoarded=bit;}
    bool GetJustBoarded() {return justBoarded;}
    void SetServiced(bool f) { fServiced = f; }

    // Get the floor to service
    // If this is in stage (i): waiting at a floor, return that floor waiting at
    // If this is in stage (ii): inside an elevator, return the floor going to
    // Otherwise, return -1
    int GetRequestedFloor() const {
        if( IsServiced() )  {
            return -1;
        }
        else if( IsFloorRequestDone() )   {
            return GetFloorDest();
        }
        else {
            return GetFloorSrc();
        }
    }

    // Wait time: get/set. Note: you need to maintain the wait time yourself!
    int GetArriveTime() const { return timeArrive; }
    void SetArriveTime(int t) { timeArrive = t; }

    // Check if this is the special maintenance start request
    bool IsMaintenanceStart() const { return floorSrc==-1 && floorDest==-1; }
    bool IsMaintenanceEnd() const { return floorSrc==0 && floorDest==0; }
    

private:
    int time;           // time of request made
    int floorSrc;       // which floor the request is made
    int floorDest;      // which floor is going
    bool fFloorReqDone;   // is this passenger passing stage one (no longer waiting at the floor) or not
    bool fServiced;     // is this request serviced already?
    int timeArrive;    // when the user gets to the desitnation floor
    bool justServiced;
    bool justBoarded;
};

//*****************************************************************************
// Elevator moving direction

typedef enum
{
    EC_ELEVATOR_STOPPED = 0,    // not moving
    EC_ELEVATOR_UP,             // moving up
    EC_ELEVATOR_DOWN            // moving down
} EC_ELEVATOR_DIR;

//*****************************************************************************
// we use a state design pattern where each state has its updates based on the direction it represents
class State
{
    public:
        // pure virtual func
        virtual int getNextFloor() = 0;
        // virtual destructor for memory cleanup
        virtual ~State() {}
        // helper function for determining priority floor based on direction
        std::pair<std::vector<int>,std::vector<int>> divideRequests(vector<ECElevatorSimRequest> requests,int curFloor,int curTime){
            vector<int> lower;
            vector<int> higher;
            for (auto &req : requests){
                if (req.IsServiced()) continue;
                // put valid requests above curFloor into higher and below curFloor into lower
                else if (req.GetRequestedFloor() > curFloor && req.GetTime() <= curTime){
                    higher.push_back(req.GetRequestedFloor());
                }
                else if (req.GetRequestedFloor() < curFloor && req.GetTime() <= curTime) lower.push_back(req.GetRequestedFloor());
            }
            return make_pair(lower,higher);
        }

};
// method to find correct floor when elevator is moving up
class MovingUp : public State
{
    public:
        MovingUp(vector<ECElevatorSimRequest> &Requests, int CurFloor, int CurTime) : requests(Requests), curFloor(CurFloor), curTime(CurTime) {}
        int getNextFloor() override {
            // for our lambda func
            // preprocessing
            int floor = curFloor;
            pair<vector<int>,vector<int>> vecs = divideRequests(requests, curFloor, curTime);
            // return 0 if there are no valid requests
            if (vecs.first.size() + vecs.second.size() == 0) return 0;
            // we want targ to be composed off all valid floor requests based on direction
            vector<int> targ = (vecs.second.size() != 0) ? vecs.second : vecs.first;
            return *min_element(targ.begin(),targ.end(),[floor](int a, int b){
                    return std::abs(a - floor) < std::abs(b - floor);});
        }
    private:
        vector<ECElevatorSimRequest>& requests;
        int curFloor;
        int curTime;


};
// method to find correct floor when elevator is moving down
class MovingDown : public State
{
    public:
        MovingDown(vector<ECElevatorSimRequest> &Requests, int CurFloor, int CurTime) : requests(Requests), curFloor(CurFloor), curTime(CurTime) {}
        int getNextFloor() override{
             // for our lambda func
            // preprocessing
            int floor = curFloor;
            pair<vector<int>,vector<int>> vecs = divideRequests(requests, curFloor, curTime);
            // return 0 if there are no valid requests
            if (vecs.first.size() + vecs.second.size() == 0) return 0;
            // we want targ to be composed off all valid floor requests based on direction
            vector<int> targ = (vecs.first.size() != 0) ? vecs.first : vecs.second;
            return *min_element(targ.begin(),targ.end(),[floor](int a, int b){
                    return std::abs(a - floor) < std::abs(b - floor);});
        }
    private:
        vector<ECElevatorSimRequest>& requests;
        int curFloor;
        int curTime;

};
// method to find correct floor when elevator is stopped
class Stopped : public State
{
    public:
        Stopped(vector<ECElevatorSimRequest> &Requests, int CurFloor, int CurTime, bool loaded) : requests(Requests), curFloor(CurFloor), curTime(CurTime),
        justLoaded(loaded) {}
        int getNextFloor() override {
            int floor = curFloor;
            vector<int> targ;
            for (auto &req : requests){
                if (req.GetTime() <= curTime && !req.IsServiced()) targ.push_back(req.GetRequestedFloor());
            }
            // return 0 if there are no valid requests
            if (targ.size() == 0) return 0;
            // we want targ to be composed off all valid floor requests based on direction
            return *min_element(targ.begin(),targ.end(),[floor](int a, int b){
                    // ties go to higher floor
                    if (abs(a - floor) == abs(b - floor)) return a > b;
                    else return abs(a - floor) < abs(b - floor);});
        }
    private:
        vector<ECElevatorSimRequest>& requests;
        int curFloor;
        int curTime;
        bool justLoaded;
};

//*****************************************************************************
// Simulation of elevator

class ECElevatorSim
{
public:
    // numFloors: number of floors serviced (floors numbers from 1 to numFloors)
    ECElevatorSim(int numFloors, std::vector<ECElevatorSimRequest> &listRequests);

    // free buffer
    ~ECElevatorSim();

    // Simulate by going through all requests up to certain period of time (as specified in lenSim)
    // starting from time 0. For example, if lenSim = 10, simulation stops at time 10 (i.e., time 0 to 9)
    // Caution: the list of requests contain all requests made at different time;
    // at a specific time of simulation, some events may be made in the future (which you shouldn't consider these future requests)
    void Simulate(int lenSim);

    // The following methods are about querying/setting states of the elevator
    // which include (i) number of floors of the elevator, 
    // (ii) the current floor: which is the elevator at right now (at the time of this querying). Note: we don't model the tranisent states like when the elevator is between two floors
    // (iii) the direction of the elevator: up/down/not moving

    // Get num of floors
    int GetNumFloors() const { return numFloors; }

    // Get current floor
    int GetCurrFloor() const {return curFloor;}
    
    //Get next floor
    int GetNextFloor() const {return nextFloor;}

    // Set current floor
    void SetCurrFloor(int f) {if (f <= numFloors) curFloor = f;}

    // Get current direction
    EC_ELEVATOR_DIR GetCurrDir() const {return direction;}

    // Set current direction
    void SetCurrDir(EC_ELEVATOR_DIR dir) {direction = dir;}

    // returns request based on index position
    // this function helps us store requests by their index (save memory)
    ECElevatorSimRequest& getRequestByIndex(int index) {
        return requests[index];
    }
    // main simulation functions
    // we include a func that completes one individual timestep (useful for integration with front end graphics)
     void TimeStep();
    int GetPriority();
    void Move(int dest);
    bool Load();
    bool Unload();
    // get reference of request list
    std::vector<ECElevatorSimRequest> &getAllRequests(){return requests;}
    // get current time
    int GetCurTime() {return curTime;}
    // checks for any valid requests
    bool NoRequests(){
        int time = curTime;
        bool res = std::any_of(requests.begin(),requests.end(),[time](ECElevatorSimRequest req){
            if (req.GetTime() <= time && !req.IsServiced()) return true;
            return false;
        });
        return res;
    }
    // get current elevator state
    State* GetCurState() {return curState;}
    // set new elevator state
    void SetCurState(State* newState){
        if (curState != nullptr) delete curState;
        curState = newState;
    }

private:
    // direction trackers
    EC_ELEVATOR_DIR direction;
    // history is useful to store last direction before unloading/loading
    EC_ELEVATOR_DIR directionHistory;
    // state object
    State* curState;
    // cant load twice in a row so we use this variable to track whether we just loaded passengers
    bool justLoaded;
    
    int curTime;
    int numFloors;
    int curFloor;
    int nextFloor;
    std::vector<ECElevatorSimRequest> &requests;
};


#endif /* ECElevatorSim_h */
