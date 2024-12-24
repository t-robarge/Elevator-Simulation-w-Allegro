#include "Controller.h"

using namespace std;

// constructor
ECElevatorSimController :: ECElevatorSimController(int time, int floors, vector<ECElevatorSimRequest>& requestList) : simTime(time),
        curTime(0),Ticker(60), numFloors(floors), requests(requestList),fSpaceBarPressed(false) {
            gui = new ECGraphicViewImp(1200,1540);
            Model = new ECElevatorSim(numFloors, requests);
            View = new ElevatorObserver(*gui, *Model, numFloors);
        }
// destructor
ECElevatorSimController ::  ~ECElevatorSimController() {
            delete View;
            delete Model;
            delete gui;
        }
// main function. Inits simulation
void ECElevatorSimController :: Run(){
    // allows us to control gui updates
    View->SetController(this);
    // so we can update gui in this object
    gui->Attach(this);
    // so observer can update gui
    gui->Attach(View);
    // start gui
    gui->Show();

}
// Periodic Update
void ECElevatorSimController :: Update() {
    // space bar listener
    ECGVEventType keyEvent = gui->GetCurrEvent();
    if (keyEvent == ECGV_EV_KEY_UP_SPACE){
        fSpaceBarPressed = (fSpaceBarPressed) ? false : true;
    }
    else if (keyEvent == ECGV_EV_TIMER){
    // always check if we are in a paused state
        if (fSpaceBarPressed) {
            View->Pause(); 
            return;
        }
        else View->Unpause();
        // use a ticker to call TimeStep once per second
        if (Ticker == 60){
            // ensure animation is complete
            if (!View->HasArrived()) return;
            // update time
            curTime = Model->GetCurTime();
            // update backend
            Model->TimeStep();
            // floor, direction for observers
            View->SetCurFloor(Model->GetCurrFloor());
            View->SetDirection(Model->GetCurrDir());
            // reset cords for animation
            View->ResetCords();
            // update waiting and cabin
            UpdatePassengers();
            // reset ticker
            Ticker = 0;
        }
        else {
            // update passengers to check for new valid requests
            UpdatePassengers();
            Ticker++;
        }
    }
}
// Get waiting requests and passengers in cabin
void ECElevatorSimController :: UpdatePassengers(){
    waiting.clear();
    cabin.clear();
    int floor = Model->GetCurrFloor();

    for (int i = 0; i < requests.size(); ++i) {
        if (requests[i].GetTime() <= curTime && !requests[i].IsFloorRequestDone()) {
            waiting.push_back(i);
        
        } 
        else if ((requests[i].IsFloorRequestDone() && !requests[i].IsServiced()) || requests[i].GetJustServiced()) {
            cabin.push_back(i);
        }
    }
    // sort the cabin so people with closer destinations stand closer to exit
    sort(cabin.begin(),cabin.end(),[floor](int a, int b){
        return std::abs(a - floor) > std::abs(b - floor);});
}
