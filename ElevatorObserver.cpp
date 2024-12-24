#include "ElevatorObserver.h"
#include "Controller.h"
using namespace std;



ElevatorObserver :: ElevatorObserver(ECGraphicViewImp &viewIn,ECElevatorSim &model, int floors) : myView(viewIn), Model(model),controller(nullptr), curFloor(1),y1_cabin(1520),y2_cabin(1520 - 1500/floors),numFloors(floors),direction(EC_ELEVATOR_STOPPED),
dx1(40),dx2(60),dx3(50),sx1(540),sx2(560),sx3(550),Arrived(true) {}
// Progress Bar Function
void ElevatorObserver :: DrawProgressBar(){
    int xStart = 790;           // Starting x-position
    int yStart = 1300;          // Starting y-position
    int barWidth = 400;        // Total width of the progress bar
    int barHeight = 20;        // Height of the progress bar

    // Calculate progress
    float progress = static_cast<float>(Model.GetCurTime()) / controller->getSimTime();
    if (progress > 1) progress = 1;
    int currentWidth = static_cast<int>(barWidth * progress);

    // Draw Text
    myView.DrawText(xStart+barWidth/2,yStart-50,"Progress: ",ECGV_WHITE);
    // Draw the background of the progress bar
    myView.DrawFilledRectangle(xStart, yStart, xStart + barWidth, yStart + barHeight, ECGV_WHITE);

    // Draw the filled portion representing progress
    myView.DrawFilledRectangle(xStart, yStart, xStart + currentWidth, yStart + barHeight, ECGV_GREEN);

}
// Background Function
void ElevatorObserver :: DrawBackground() {
    // backdrop
    myView.DrawFilledRectangle(0,0,1200,1540,ECGV_GREY);
            // init floor + button cords
            int x1 = 20, x2 = 500, y1 = 20, y2 = 20+floorSize;
            int h = 510, k1 = floorSize/2+5, k2 = floorSize/2+25;
            double r = 10 -  .1*numFloors;
            // update passenger/waiting count
            myView.DrawText(1000,y1,"Passengers: ",ECGV_WHITE);
            myView.DrawText(1150,y1,to_string(controller->getCabin().size()).c_str(),ECGV_BLUE);
            myView.DrawText(1020,y1+40,"Waiting: ",ECGV_WHITE);
            myView.DrawText(1150,y1+40,to_string(controller->getWaiting().size()).c_str(),ECGV_BLUE);
            // draw floors + buttons top to bottom
            for (int i = 0; i < numFloors;++i){
                myView.DrawRectangle(x1,y1,x2,y2,3,ECGV_WHITE);
                myView.DrawCircle(h,k1,r,4,ECGV_WHITE);
                myView.DrawCircle(h,k2,r,4,ECGV_WHITE);
                y1 += floorSize;
                y2 += floorSize;
                k1 += floorSize;
                k2 += floorSize;
            }

            
}
// Draw Individual Passenger
// Customize Passenger Look in this function
void ElevatorObserver :: DrawPassenger(int increment, int x1, int x2, int x3, double y, const char* flr){
    myView.DrawFilledRectangle(x1+40*increment,y,x2+40*increment,y-35,ECGV_WHITE);
    myView.DrawFilledCircle(x3+40*increment,y-45,8,ECGV_WHITE);
    // santa hat for the holidays
    myView.DrawFilledRectangle((x1+5)+40*increment,y-53,(x2-5)+40*increment,y-60,ECGV_RED);
    myView.DrawFilledCircle(x3+40*increment,y-65,4,ECGV_WHITE);
    // passengers destination floor
    myView.DrawText(x1+10+40*increment, y-35, flr, ECGV_BLUE);
}
// Draw All Passengers Waiting For Elevator
void ElevatorObserver :: DrawPassengers(){
    int x1 = 540, x2 = 560, x3 = 550;
    double r = 10 -  .1*numFloors;
    // get waiting passengers
    vector<int> waiting = controller->getWaiting();
    vector<int> floors;
    for (int index : waiting) {
        // get destination floor for label
         auto& req = Model.getRequestByIndex(index);
         // check for other passengers on same floor
         int count = std::count(floors.begin(),floors.end(),req.GetFloorSrc());
         // get label (destination floor)
         std::string label = to_string(index);
         // get correct y level
         int y_targ = 1500-floorSize*(req.GetFloorSrc()-1);
         // call draw, count checks for multiple passengers waiting at same floor
         if (count) DrawPassenger(0, x1+40*count, x2+40*count, x3+40*count, y_targ, label.c_str());
         else DrawPassenger(0, x1, x2, x3, y_targ, label.c_str());
        // signal lights
        if (req.GetFloorDest() > req.GetFloorSrc()){
            myView.DrawFilledCircle(510,(floorSize/2+5)+floorSize*(numFloors - req.GetFloorSrc()),r,ECGV_YELLOW);
        }
        else{
            myView.DrawFilledCircle(510,(floorSize/2+25)+floorSize*(numFloors - req.GetFloorSrc()),r,ECGV_YELLOW);
        }
        // keep track of floor request is on
        floors.push_back(req.GetFloorSrc());
    }
}
// Draw Cabin and all Passengers in it
void ElevatorObserver :: DrawCabin(){
    int x1 = 40, x2 = 60, x3 = 50;
    // This is the actual cabin
    myView.DrawFilledRectangle(20,y1_cabin,500,y2_cabin,ECGV_LIGHTBLUE);
    const auto& cabin = controller->getCabin();
    // counter variable to ensure proper spacing in cabin
    int i = 0;
    for (int index : cabin) {
         auto& req = Model.getRequestByIndex(index);
         // get label for text
         std::string label = to_string(index);
        // animation of getting off elevator
        if (req.GetJustServiced()){
            MovePassenger(index, i, 1, label.c_str());
            
        }
        // animation of getting onto elevator
        else if (req.GetJustBoarded()){
            MovePassenger(index, i, 0, label.c_str());
        }
        // stationary passenger
        else{
            DrawPassenger(i,x1,x2,x3,y1_cabin,label.c_str());
        }
        i++;
     }
    
}
// Passenger Animation is Contained Within This Function
void ElevatorObserver :: MovePassenger(int index, int increment, bool direction, const char* label){
        auto& req = Model.getRequestByIndex(index);
        //MOVE LEFT
        if (direction == 0){
            int y_targ = 1520-floorSize*(req.GetFloorSrc()-1);
            if (sx1 < (40+40*(increment))){
                DrawPassenger(increment,40,60,50,y_targ,label);
                Arrived = true;

            }
            else{
                DrawPassenger(increment,sx1,sx2,sx3,y1_cabin,label);
                if (pauseSim) return;
                Arrived = false;
                sx1 -= 10;
                sx2 -= 10;
                sx3 -= 10;
            }
        }
        // MOVE RIGHT
        else if (direction == 1){
            if(dx1 > 540+40*increment){
                DrawPassenger(increment, 540, 560, 550, y1_cabin,label);
                Arrived = true;
            }
            else{
                DrawPassenger(increment,dx1,dx2,dx3,y1_cabin,label);
                if (pauseSim) return;
                Arrived = false;
                dx1 += 10;
                dx2 += 10;
                dx3 += 10;
            }
        }
}
// Main Update Function
// Update GUI based on state of Model
void ElevatorObserver :: Update(){
    ECGVEventType keyEvent = myView.GetCurrEvent();
    // draw on the timer
    if (keyEvent == ECGV_EV_TIMER){
        // init background, progress, cur passengers, and cabin
        myView.SetRedraw(true);
        DrawBackground();
        DrawProgressBar();
        DrawPassengers();
        DrawCabin();
        // do not change if we are paused
        if (pauseSim) return;
        // move cabin up
        else if(direction == EC_ELEVATOR_UP){
            // cabin destination y coordinates
            double y1_targ = (20+floorSize)+floorSize*(numFloors - curFloor);
            double y2_targ = 20+floorSize*(numFloors - curFloor);
            // move until we reach limit
            if(y1_cabin < y1_targ || y2_cabin < y2_targ){
                // update direction and coords
                direction = EC_ELEVATOR_STOPPED;
                y1_cabin = y1_targ;
                y2_cabin = y2_targ;
                DrawCabin();
                Arrived = true;
                return;
            }
            else{
            DrawCabin();
            Arrived = false;
            y1_cabin -= speed;
            y2_cabin -= speed;
            }
            }
        // move cabin down
        else if(direction == EC_ELEVATOR_DOWN){
            double y1_targ = (20+floorSize)+floorSize*(numFloors - curFloor);
            double y2_targ = 20+floorSize*(numFloors - curFloor);
            if(y1_cabin > y1_targ || y2_cabin  > y2_targ){
                y1_cabin = y1_targ;
                y2_cabin = y2_targ;
                direction = EC_ELEVATOR_STOPPED;
                DrawCabin();
                Arrived = true;
                return;
            }
            else{
            DrawCabin();
            Arrived = false;
            y1_cabin += speed;
            y2_cabin += speed;
            }
            }
        // keep cabin stationary but updates passengers if needed
        else if(direction == EC_ELEVATOR_STOPPED){
            DrawBackground();
            DrawProgressBar();
            DrawPassengers();
            DrawCabin();
            return;
        }
    }
}
// reset passenger animation cords 
// we must reset these cords each TimeStep progression
// controlled by the controller
void ElevatorObserver :: ResetCords(){
            sx1 = 540;
            sx2 = 560;
            sx3 = 550;
            dx1 = 40 + 40*(controller->getCabin().size()-1);
            dx2 = 60 + 40*(controller->getCabin().size()-1);
            dx3 = 50 + 40*(controller->getCabin().size()-1);
        }

