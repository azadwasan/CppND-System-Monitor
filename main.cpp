#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "util.h"
#include "SysInfo.h"
#include "ProcessContainer.h"

using namespace std;

//#define DEBUGMODE

#ifdef DEBUGMODE
 
 void writeSysInfoToConsole(SysInfo sys){
    cout<< "OS: " << sys.getOSName()<<endl;
    cout<< "Kernel version: " << sys.getKernelVersion();
    cout<< "CPU: " << Util::getProgressBar(sys.getCpuPercent());
    cout<< "Other cores:";
    std::vector<std::string> val = sys.getCoresStats();
    for(int i=0;i<val.size();i++){
     cout<< val[i];
    }
    cout<<endl;
    cout<< "Memory: ";
    cout<< Util::getProgressBar(sys.getMemPercent());
    cout<< "Total Processes:" << sys.getTotalProc();
    cout<< "Running Processes:" << sys.getRunningProc();
    cout<< "Up Time: " << Util::convertToTime(sys.getUpTime());
}

void getProcessListToConsole(std::vector<string> processes){
    for(int i=0; i< processes.size();i++){
        cout<<"Process string = "<<processes[i]<<endl;
   }
}
void printMain(SysInfo sys,ProcessContainer procs){
    int counter = 0;
    while(1){
        procs.refreshList();
        std::vector<std::vector<std::string>> processes = procs.getList();
        writeSysInfoToConsole(sys);
        getProcessListToConsole(processes[counter]);
        sleep(1);
        if(counter ==  (processes.size() -1)){
            counter = 0;
        }
        else {
            counter ++;
        }
    }
}


#else
char* getCString(std::string str){
    char * cstr = new char [str.length()+1];
    std::strcpy (cstr, str.c_str());
    return cstr;
}
void writeSysInfoToConsole(SysInfo sys, WINDOW* sys_win){
    sys.setAttributes();

    mvwprintw(sys_win,2,2,getCString(( "OS: " + sys.getOSName())));
    mvwprintw(sys_win,3,2,getCString(( "Kernel version: " + sys.getKernelVersion())));
    mvwprintw(sys_win,4,2,getCString( "CPU: "));
    wattron(sys_win,COLOR_PAIR(1));
    wprintw(sys_win,getCString(Util::getProgressBar(sys.getCpuPercent())));
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,5,2,getCString(( "Other cores:")));
    wattron(sys_win,COLOR_PAIR(1));
    std::vector<std::string> val = sys.getCoresStats();
    for(int i=0;i<val.size();i++){
     mvwprintw(sys_win,(6+i),2,getCString(val[i]));
    }
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,10,2,getCString(( "Memory: ")));
    wattron(sys_win,COLOR_PAIR(1));
    wprintw(sys_win,getCString(Util::getProgressBar(sys.getMemPercent())));
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,11,2,getCString(( "Total Processes:" + sys.getTotalProc())));
    mvwprintw(sys_win,12,2,getCString(( "Running Processes:" + sys.getRunningProc())));
    mvwprintw(sys_win,13,2,getCString(( "Up Time: " + Util::convertToTime(sys.getUpTime()))));
    wrefresh(sys_win);
}

void getProcessListToConsole(std::vector<string> processes,WINDOW* win){

    wattron(win,COLOR_PAIR(2));
    mvwprintw(win,1,2,"PID:");
    mvwprintw(win,1,9,"User:");
    mvwprintw(win,1,16,"CPU[%%]:");
    mvwprintw(win,1,26,"RAM[MB]:");
    mvwprintw(win,1,35,"Uptime:");
    mvwprintw(win,1,44,"CMD:");
    wattroff(win, COLOR_PAIR(2));
    for(int i=0; i< processes.size();i++){
        mvwprintw(win,2+i,2,getCString(processes[i]));
   }
}
void printMain(SysInfo sys,ProcessContainer procs){
	initscr();			/* Start curses mode 		  */
    noecho(); // not printing input values
    cbreak(); // Terminating on classic ctrl + c
    start_color(); // Enabling color change of text
    int yMax,xMax;
    getmaxyx(stdscr,yMax,xMax); // getting size of window measured in lines and columns(column one char length)
	WINDOW *sys_win = newwin(17,xMax-1,0,0);
	WINDOW *proc_win = newwin(15,xMax-1,18,0);

    init_pair(1,COLOR_BLUE,COLOR_BLACK);
    init_pair(2,COLOR_GREEN,COLOR_BLACK);
    int counter = 0;
    while(1){
        box(sys_win,0,0);
        box (proc_win,0,0);
        procs.refreshList();
        std::vector<std::vector<std::string>> processes = procs.getList();
        writeSysInfoToConsole(sys,sys_win);
        getProcessListToConsole(processes[counter],proc_win);
        wrefresh(sys_win);
        wrefresh(proc_win);
        refresh();
        sleep(1);
        if(counter ==  (processes.size() -1)){
            counter = 0;
        }
        else {
            counter ++;
        }
    }
	endwin();
}
#endif

int main( int   argc, char *argv[] )
{
 //Object which contains list of current processes, Container for Process Class
    cout<<"Initiating process contaienr"<<endl;
    ProcessContainer procs;
// Object which containts relevant methods and attributes regarding system details
    cout<<"Initiating sysInfo"<<endl;
    SysInfo sys;
    //std::string s = writeToConsole(sys);
    cout<<"PrintMain()"<<endl;

    printMain(sys,procs);
    return 0;
}
