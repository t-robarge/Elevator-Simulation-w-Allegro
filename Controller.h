#ifndef ECELEVATORSIMCONTROLLER_H
#define ECELEVATORSIMCONTROLLER_H

#include "ElevatorObserver.h"
#include "ECElevatorSim.h"

using namespace std;

// declare class here to avoid dependency issues
class ElevatorObserver;

class ECElevatorSimController : public ECObserver
{
    public:
        // constructor
        ECElevatorSimController(int time, int floors, vector<ECElevatorSimRequest>& requestList);
        // destructor
        ~ECElevatorSimController();
        // main func - run visual sim
        void Run();
        // update cabin and waiting vectors used by observer
        void UpdatePassengers();
        // update based on current key event
        void Update() override;
        // returns passengers waiting for elevator (index based)
        vector<int>& getWaiting() {
            return waiting;}
        // returns passeners in cabin (index based)
        vector<int>& getCabin() {
                return cabin;}
        // get total simulation time
        int getSimTime(){return simTime;}
    private:
        // gui object
        ECGraphicViewImp* gui;
        // elevator observer (updates visuals)
        ElevatorObserver* View;
        // simulation model (updates back end logic)
        ECElevatorSim* Model;
        // store requests to pass onto model and use in our own functions (pass by reference)
        vector<ECElevatorSimRequest>& requests;
        // keep track of space bar key events
        bool fSpaceBarPressed;
        int simTime;
        int curTime;
        int Ticker;
        int numFloors;
        vector<int> waiting;
        vector<int> cabin;
};
#endif