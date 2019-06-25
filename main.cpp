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

#define USE_PROCESS_LIST_OPTIMIZATION
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
    int listSize = 0;
    while(1){
      //  writeSysInfoToConsole(sys);
#ifdef  USE_PROCESS_LIST_OPTIMIZATION
        listSize = procs.refreshList();
        std::vector<std::string> processes = procs.getList(counter);
        getProcessListToConsole(processes);

        cout<<"Counter ="<<counter
            <<", listSize = "<<listSize
            <<", processes count = "<<processes.size()
            <<", check ="<< ceil(float(listSize)/float(PROCESS_LIST_SIZE))
            <<endl;

#else
        procs.refreshList();
        std::vector<std::vector<std::string>> processes = procs.getList();
        getProcessListToConsole(processes[counter]);

        cout<<"Counter ="<<counter
        <<", processes count = "<<processes.size()
        <<endl;

#endif
#ifdef  USE_PROCESS_LIST_OPTIMIZATION
        if(counter ==  (ceil(float(listSize)/float(PROCESS_LIST_SIZE)) -1)){
            counter = 0;
        }
#else
        if(counter ==  (processes.size() -1)){
            counter = 0;
        }
#endif
        else {
            counter ++;
        }
        sleep(1);
    }
}
#else

void writeSysInfoToConsole(SysInfo sys, WINDOW* sys_win){
    sys.setAttributes();

    mvwprintw(sys_win,2,2,( "OS: " + sys.getOSName()).c_str());
    mvwprintw(sys_win,3,2,(( "Kernel version: " + sys.getKernelVersion())).c_str());
    mvwprintw(sys_win,4,2,"CPU: ");
    wattron(sys_win,COLOR_PAIR(1));
    wprintw(sys_win,(Util::getProgressBar(sys.getCpuPercent())).c_str());
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,5,2, "Other cores:");
    wattron(sys_win,COLOR_PAIR(1));
    std::vector<std::string> val = sys.getCoresStats();
    for(int i=0;i<val.size();i++){
     mvwprintw(sys_win,(6+i),2,val[i].c_str());
    }

    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,10,2, "Memory: ");
    wattron(sys_win,COLOR_PAIR(1));
    wprintw(sys_win, Util::getProgressBar(sys.getMemPercent()).c_str());
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,11,2, ( "Total Processes:" + sys.getTotalProc()).c_str());
    mvwprintw(sys_win,12,2, ( "Running Processes:" + sys.getRunningProc()).c_str());
    mvwprintw(sys_win,13,2, ( "Up Time: " + Util::convertToTime(sys.getUpTime())).c_str());
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
    int i = 0;
    for(const auto& process: processes){
        mvwprintw(win,2+i++, 2,process.c_str());
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
    int listSize = 0;
    while(1){
        box(sys_win,0,0);
        box (proc_win,0,0);
        writeSysInfoToConsole(sys,sys_win);
#ifdef  USE_PROCESS_LIST_OPTIMIZATION
        listSize = procs.refreshList();
        std::vector<std::string> processes = procs.getList(counter);
        getProcessListToConsole(processes,proc_win);
#else
        procs.refreshList();
        std::vector<std::vector<std::string>> processes = procs.getList();
        getProcessListToConsole(processes[counter],proc_win);
#endif
        wrefresh(sys_win);
        wrefresh(proc_win);
        refresh();
        sleep(1);
#ifdef  USE_PROCESS_LIST_OPTIMIZATION
        if(counter ==  (ceil(float(listSize)/float(PROCESS_LIST_SIZE)) -1)){
            counter = 0;
        }
#else
        if(counter ==  (processes.size() -1)){
            counter = 0;
        }
#endif
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
    ProcessContainer procs;
// Object which containts relevant methods and attributes regarding system details
    cout<<"Initiating sysInfo"<<endl;
    try{
        SysInfo sys;
        printMain(sys,procs);
    }
    catch(exception& e){
        cout<<"Failed to fetch system info. Exiting!";
        return 0;
    }
    //std::string s = writeToConsole(sys);
    return 0;
}
