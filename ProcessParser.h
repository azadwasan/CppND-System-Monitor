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
#include <string_view>
#include <optional>
#include "constants.h"

using namespace std;

class ProcessParser{
    private:
    static string fetchValue(string path, int index);
    static vector<string> fetchValues(string path, string_view searchString);
    static string fetchValue(string path, int index, string_view searchString);

    public:
    ProcessParser() = delete;
    ProcessParser(ProcessParser&) = delete;
    ProcessParser(ProcessParser&&) = delete;
    ~ProcessParser() = delete;

    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static int getNumberOfCores();
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysActiveCpuTime(vector<string> values);
    static float getSysIdleCpuTime(vector<string> values);
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);

};

vector<string> ProcessParser::getPidList(){
    DIR* dir;
    // Basically, we are scanning /proc dir for all directories with numbers as their names
    // If we get valid check we store dir names in vector as list of machine pids
    vector<string> container;
    if(!(dir = opendir("/proc")))
        throw std::runtime_error(std::strerror(errno));

    while (dirent* dirp = readdir(dir)) {
        // is this a directory?
        if(dirp->d_type != DT_DIR)
            continue;
        // Is every character of the name a digit?
        if (all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](char c){ return std::isdigit(c); })) {
            container.push_back(dirp->d_name);
        }
    }
    //Validating process of directory closing
    if(closedir(dir))
        throw std::runtime_error(std::strerror(errno));
    return container;
}

string ProcessParser::getCmd(string pid){
    try{
        string line;
        ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));
        std::getline(stream, line);
        return line;
    }
    catch(std::runtime_error& e){
        throw e;
    }
    catch(exception& e){
        //Ignore this exception and return empty string
    }
    return string(70, ' ');
}

string ProcessParser::fetchValue(string path, int index){
    try{
        ifstream stream = Util::getStream(path);
        string line;
        getline(stream,line);
        istringstream buf(line);
        istream_iterator<string> beg(buf), end;
        vector<string> values(beg, end);
        return values[index];
    }
    catch(exception& e){
        throw e;
    }
}

vector<string> ProcessParser::fetchValues(string path, string_view searchString){
    try{
        string line;
        ifstream stream = Util::getStream(path);
        while (std::getline(stream, line)) {
            if (line.compare(0, searchString.size(),searchString) == 0) {
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                return values;
            }
        }
    }
    catch(exception& e){
        throw e;    
    }
    throw std::invalid_argument("Failed to find the search string.");
}


string ProcessParser::fetchValue(string path, int index, string_view searchString){
    try{
        //TODO: Check if the index is in bounds
         return fetchValues(path, searchString)[index];
    }
    catch(exception& e){
        throw e;    
    }
    return "";
}

long int ProcessParser::getSysUpTime(){
    try{
        return stoi(fetchValue(Path::basePath() + Path::upTimePath(), 0));
    }
    catch(std::invalid_argument& e){
        //Ignore this exception
    }
    catch(exception& e){
        throw e;
    }
    return 0;
}

std::string ProcessParser::getProcUpTime(string pid){
    try{
        return to_string(float(stof(fetchValue(Path::basePath() + pid + "/" +  Path::statPath(),13))/sysconf(_SC_CLK_TCK)));
    }
    catch(std::runtime_error& e){
        throw e;
    }
    catch(exception& e){
        //Ignore the rest of the exceptions
    }
    return "";
}

int ProcessParser::getNumberOfCores()
{
    // Get the number of host cpu cores
    try{
        string result = fetchValue(Path::basePath() + "cpuinfo", 3, "cpu cores");
        if(result.size()!=0){
            return stoi(result);
        }
    }
    catch(std::invalid_argument& e){
        return 0;
    }
    catch(exception& e){
        throw e;
    }
    return 0;
}

string ProcessParser::getSysKernelVersion(){
    try{
        return fetchValue(Path::basePath() + Path::versionPath(), 2, "Linux version ");
    }
    catch(exception& e){
        throw e;
    }
}

std::string ProcessParser::getVmSize(string pid){
    try{
        return to_string(stof(fetchValue(Path::basePath() + pid + Path::statusPath(), 1, "VmData"))/float(1024*1024));
    }
    catch(std::runtime_error& e){
        throw e;
    }
    catch(exception& e){
        //Ignore the error
    }
    return "0.0";
}

int ProcessParser::getTotalNumberOfProcesses(){
    try{
        return stoi(fetchValue(Path::basePath() + Path::statPath(), 1, "processes"));
    }
    catch(std::invalid_argument& e){
        return 0;
    }
    catch(exception& e){
        throw e;
    }
}

int ProcessParser::getNumberOfRunningProcesses(){
    try{
        return stoi(fetchValue(Path::basePath() + Path::statPath(), 1, "procs_running"));
    }
    catch(std::invalid_argument& e){
        return 0;
    }
    catch(exception& e){
        throw e;
    }
}

string ProcessParser::getOSName(){
    try{
        string line = fetchValue("/etc/os-release", 0, "PRETTY_NAME") + fetchValue("/etc/os-release", 1, "PRETTY_NAME");
        std::size_t found = line.find("=") + 1;
        string result = line.substr(found);
        result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
        return result;
    }
    catch(std::invalid_argument& e){
        return 0;
    }
    catch(exception& e){
        throw e;
    }
}

int ProcessParser::getTotalThreads()
{
    int result = 0;
    vector<string>_list = ProcessParser::getPidList();
    for (const auto& pid:_list) {
        //getting every process and reading their number of their threads
        try{
            result += stoi(fetchValue(Path::basePath() + pid + Path::statusPath(), 1, "Threads"));
        }
        catch(exception& e){
            continue;
        }
    }
    return result;
}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber){
    // It is possible to use this method for selection of data for overall cpu or every core.
    // when nothing is passed "cpu" line is read
    // when, for example "0" is passed  -> "cpu0" -> data for first core is read
    try{
        return fetchValues(Path::basePath() + Path::statPath(), "cpu" + coreNumber);
    }
    catch(std::invalid_argument& e){
        return vector<string>{};
    }
    catch(exception& e){
        throw e;
    }
}

string ProcessParser::getProcUser(string pid){
    string result ="";
    try{
        result = fetchValue(Path::basePath() + pid + Path::statusPath(), 1, "Uid");

        ifstream stream = Util::getStream("/etc/passwd");
        string name =("x:" + result);
        string line;
        // Searching for name of the user with selected UID
        while (std::getline(stream, line)) {
            if (line.find(name) != std::string::npos) {
                result = line.substr(0, line.find(":"));
                return result;
            }
        }
    }
    catch(std::runtime_error& e){
        throw e;
    }

    catch(exception& e){
     //  Don't forward the exception
    }
    return "";
}

std::string ProcessParser::getCpuPercent(string pid){
    string line;
    string value;
    float result;
    try{
        ifstream stream = Util::getStream((Path::basePath()+ pid + "/" + Path::statPath()));
        getline(stream, line);
        string str = line;
        istringstream buf(str);
        istream_iterator<string> beg(buf), end;
        vector<string> values(beg, end); // done!
        // acquiring relevant times for calculation of active occupation of CPU for selected process
        float utime = stof(ProcessParser::getProcUpTime(pid));
        float stime = stof(values[14]);
        float cutime = stof(values[15]);
        float cstime = stof(values[16]);
        float starttime = stof(values[21]);
        float uptime = ProcessParser::getSysUpTime();
        float freq = sysconf(_SC_CLK_TCK);
        float total_time = utime + stime + cutime + cstime;
        float seconds = uptime - (starttime/freq);
        result = 100.0*((total_time/freq)/seconds);
    }
    catch(std::runtime_error& e){
        throw e;
    }
    catch(exception& e){
        //Ignore rest of the errors
    }

    return to_string(result);
}

float ProcessParser::getSysActiveCpuTime(vector<string> values){
   return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float ProcessParser::getSysIdleCpuTime(vector<string> values){
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

float ProcessParser::getSysRamPercent(){
    string line;
    string name1 = "MemAvailable:";
    string name2 = "MemFree:";
    string name3 = "Buffers:";

    string value;
    int result;
    float total_mem = 0;
    float free_mem = 0;
    float buffers = 0;
    try{
        ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
        while (std::getline(stream, line)) {
            if (total_mem != 0 && free_mem != 0)
                break;
            if (line.compare(0, name1.size(), name1) == 0) {
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                total_mem = stof(values[1]);
            }
            if (line.compare(0, name2.size(), name2) == 0) {
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                free_mem = stof(values[1]);
            }
            if (line.compare(0, name3.size(), name3) == 0) {
                istringstream buf(line);
                istream_iterator<string> beg(buf), end;
                vector<string> values(beg, end);
                buffers = stof(values[1]);
            }
        }
    }
    catch(exception& e){
        //Don't propogate, ignore the error
    }
    //calculating usage:
    return float(100.0*(1-(free_mem/(total_mem-buffers))));
}


std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2){
/*
Because CPU stats can be calculated only if you take measures in two different time,
this function has two parameters: two vectors of relevant values.
We use a formula to calculate overall activity of processor.
*/
    float activeTime = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
    float idleTime = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result = 100.0*(activeTime / totalTime);
    return to_string(result);
}

bool ProcessParser::isPidExisting(string pid){
    for(auto& exist_pid : getPidList())
    {
        if(exist_pid.compare(pid) == 0)
            return true;
    }
    return false;
}
