#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <filesystem>
#include "constants.h"


using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};

string ProcessParser::getCmd(string pid){
    return "";
}

vector<string> ProcessParser::getPidList(){
    std::string path = Path::basePath();
    cout<<"Getting PIDlist "<<Path::basePath()<<endl;
    for (const auto & entry : filesystem::directory_iterator(path)){
        std::cout << entry.path() << std::endl;
    }    
    cout<<"Returing"<<endl;
    return vector<string>{};
}

std::string ProcessParser::getVmSize(string pid){
    return "";    
}

std::string ProcessParser::getCpuPercent(string pid){
    return "";
}

long int ProcessParser::getSysUpTime(){
    return 0;
}

std::string ProcessParser::getProcUpTime(string pid){
    return "";
}

string ProcessParser::getProcUser(string pid){
    return "";    
}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber){
    return vector<string>{};
}

float ProcessParser::getSysRamPercent(){
    return 0.0;
}

string ProcessParser::getSysKernelVersion(){
    return "";    
}
int ProcessParser::getTotalThreads(){
   return 0;
}
int ProcessParser::getTotalNumberOfProcesses(){
    return 0;
}
int ProcessParser::getNumberOfRunningProcesses(){
    return 0;
}

string ProcessParser::getOSName(){
    return "";
}

std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2){
    return "";
}

bool ProcessParser::isPidExisting(string pid){
    return false;
}
