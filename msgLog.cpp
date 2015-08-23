#include <iostream>
#include "msgLog.h"
#include <vector>
#include <fstream>

msgLog::msgLog() {
    useFile = false;
}
msgLog::msgLog(const std::string& file) : file(file) {
    useFile = true;
    openFile.open(file, std::ios::app);
}
msgLog::~msgLog() {
    if (useFile) {
        openFile.close();
    }
}
void msgLog::msg(const std::string& message) {
    data.push_back(message);

    if (useFile) {
        openFile << message << std::endl;
        openFile.close();
    }

}
void msgLog::error(const std::string& message) {
    msg("[ERROR] " + message);
}
void msgLog::warning(const std::string& message) {
    msg("[WARNING] " + message);
}

void msgLog::out() {
    for (std::string & s : data) {
        std::cout << s << std::endl;
    }
}

void msgLog::clear() {
    data.clear();
}
void msgLog::savefile(const std::string& file) {
    if (useFile) {return;}
    openFile.open(file);
    for (std::string & s : data) {
        openFile << s << std::endl;
    }
    openFile.close();
}
void msgLog::appendfile(const std::string& file) {
    if (useFile) {return;}
    openFile.open(file, std::ios::app);
    for (std::string & s : data) {
        openFile << s << std::endl;
    }
    openFile.close();
}
