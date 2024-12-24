#include "ElevatorObserver.h"
#include "ECElevatorSim.h"
#include "Controller.h"
#include <fstream>
#include <string>
#include <sstream>
using namespace std;


int main(int argc, char** argv){
    // check arguments
    if (argc != 2){
        cerr << "Incorrect Usage: " << argv[0] << "<filename" << endl;
        return -1;
    }
    // open file
    ifstream file(argv[1]);
    string line;
    string word;
    // check for empty file
    if (!getline(file, line)) {
        std::cerr << "Error: File is empty or could not read the first line" << std::endl;
        return -1;
    }
    // skip any comments to get to real first line
    while(line[0] == '#'){
        getline(file,line);
    }
    // get Time and NumFloors from first non comment line
    istringstream iss(line);
    iss >> word;
    int numFloors = stoi(word);
    iss >> word;
    int Time = stoi(word);
    // get requests from all sequential lines
    vector<ECElevatorSimRequest> requests;
    while (getline(file,line)){
        // ignore comment lines
        if (line[0] == '#') continue;
        istringstream iss(line);
        // convert strings to ints
        vector<int> args;
        while (iss >> word){
            args.push_back(stoi(word));
        }
        // check amount of arguments
        if (args.size() != 3){
            cerr << "Bad Argument: Requests should be in format (time) (src) (dest)" << endl;
            return -1;
        }
        // add request
        ECElevatorSimRequest new_request(args[0],args[1],args[2]);
        requests.push_back(new_request);
    }
    // sort requests by time
    sort(requests.begin(),requests.end(),[](ECElevatorSimRequest a, ECElevatorSimRequest b){
        return a.GetTime() < b.GetTime();
    });
    // begin simulation
    ECElevatorSimController control(Time, numFloors,requests);
    control.Run();
    return 0;
}