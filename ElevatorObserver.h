


#ifndef ElevatorObserver_h
#define ElevatorObserver_h

#include "ECObserver.h"
#include "ECGraphicViewImp.h"
#include "Controller.h"
using namespace std;

// init for declarations
class ECElevatorSimController; 

class ElevatorObserver: public ECObserver
{
    public:
        // constructor
        ElevatorObserver(ECGraphicViewImp &viewIn,ECElevatorSim &model,int floors);
        // destructor
        ~ElevatorObserver() {}
        // main update
        void Update() override;
        // update currrent floor
        void SetCurFloor(int floor){curFloor = floor;}
        // get current floor
        int GetCurFloor() {return curFloor;}
        // update direction
        void SetDirection(EC_ELEVATOR_DIR direct){direction = direct;}
        // get floor size
        double GetFloorSize() {return floorSize;}
        // set controller that updates observer w/ new back end update
        void SetController(ECElevatorSimController* control) {controller = control;}
        // resets animation coordinates for passengers
        void ResetCords();
        // get current direction
        EC_ELEVATOR_DIR GetDirection() {return direction;}
        // return gui object
        ECGraphicViewImp& getView() const {return myView;};
        void DrawProgressBar();
        void DrawBackground();
        void DrawPassenger(int increment, int x1, int x2, int x3, double y, const char* flr);
        void MovePassenger(int index, int increment, bool direction, const char* label);
        void DrawPassengers();
        void DrawCabin();
        // update animation status
        bool HasArrived() {return Arrived;}
        double GetYCabin() {return y1_cabin;}
        // pause all updates
        void Pause() {pauseSim = true;}
        // continue updates
        void Unpause() {pauseSim = false;}
    private:
        // gui
        ECGraphicViewImp& myView;
        // controller
        ECElevatorSimController* controller;
        // backend
        ECElevatorSim &Model;
        // keeps track of animation status 
        // we dont want to update back end until animation is complete
        bool Arrived;
        // status of simulation (are we paused or not)
        bool pauseSim;
        // keep track of curFloor from back end
        // updated through controller
        int curFloor;
        // y coords of cabin
        double y1_cabin, y2_cabin;
        // coords used for passenger animation
        double dx1, dx2, dx3, sx1, sx2, sx3;
        // need to keep track of this for coordinate calculations
        int numFloors;  
        // create a reasonable size based on amount of floors
        double floorSize = 1500 / numFloors;
        // speed is distance / time (frames)
         double speed = floorSize/60;
        // keep track of direction
        // updated through controller
        EC_ELEVATOR_DIR direction;

};
#endif 
