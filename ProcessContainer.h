#include "Process.h"
#include <vector>
class ProcessContainer{

private:
    std::vector<Process>_list{};
public:
    ProcessContainer(){
        this->refreshList();
    }
    int refreshList();
    std::string printList();
    std::vector<std::vector<std::string>> getList();
    std::vector<std::string> getList(int pageNumber);
};

int ProcessContainer::refreshList(){
    std::vector<std::string> pidList = ProcessParser::getPidList();
    this->_list.clear();
    for(int i=0;i<pidList.size();i++){
        try{
            Process proc(pidList[i]);
            this->_list.push_back(proc);
        }
        catch(exception& e){
            pidList.erase(pidList.begin()+i);
        }
    }
    return _list.size();
}
std::string ProcessContainer::printList(){
    std::string result="";
    for(int i=0;i<this->_list.size();i++){
        result += this->_list[i].getProcess();
    }
    return result;
}
std::vector<std::vector<std::string>> ProcessContainer::getList(){
    std::vector<std::vector<std::string>> values;
    std::vector<std::string> stringifiedList;
    for(int i=0; i<ProcessContainer::_list.size(); i++){
        stringifiedList.push_back(ProcessContainer::_list[i].getProcess());
    }
    int lastIndex = 0;
    for (int i=0; i<stringifiedList.size();i++){
        if(i %10 == 0 && i > 0){
          std::vector<std::string>  sub(&stringifiedList[i-10], &stringifiedList[i]);
          values.push_back(sub);
          lastIndex = i;
        }
        if(i == (ProcessContainer::_list.size() - 1) && (i-lastIndex)<10){
            std::vector<std::string> sub(&stringifiedList[lastIndex],&stringifiedList[i+1]);
            values.push_back(sub);
        }
   }
   return values;
}

std::vector<std::string> ProcessContainer::getList(int pageNumber){
    std::vector<std::string> stringifiedList;
    const int start = pageNumber*PROCESS_LIST_SIZE;
    for(int i=start; i<ProcessContainer::_list.size() && i<start + PROCESS_LIST_SIZE; i++){
        stringifiedList.push_back(ProcessContainer::_list[i].getProcess());
    }
   return stringifiedList;

}

