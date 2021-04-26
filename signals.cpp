#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
	// TODO: Add your implementation
}

void ctrlCHandler(int sig_num) {
    cout << "smash: got ctrl-C" << endl;
    int currPidInFg = sm.getJobsList().currJobInFg;
    if(currPidInFg != -1){
        kill(currPidInFg, SIGKILL);
        cout << "smash: process" << currPidInFg << "was killed" << endl;
        sm.getJobsList().currJobInFg = -1;
    }
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}

