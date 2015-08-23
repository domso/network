# ifndef msgLog_h
# define msgLog_h
# include <vector>
#include <iostream>
#include <fstream>

class msgLog {

    private:
        std::string file;
        bool useFile;
        std::ofstream openFile;
        std::vector<std::string>data;
    public:
        msgLog();
        msgLog(const std::string& file);
        ~msgLog();
        void msg(const std::string& message);
        void error(const std::string& message);
        void warning(const std::string& message);
        void out();
        void clear();
        void savefile(const std::string& file);
        void appendfile(const std::string& file);
};



# endif
