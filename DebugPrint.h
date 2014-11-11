#ifndef DEBUGPRINT_H_
#define DEBUGPRINT_H_

#include <fstream>

class DebugPrint {
public:
    DebugPrint();
    ~DebugPrint();
    void write2file();
private:
    ofstream datlog;
};

#endif /* DEBUGPRINT_H_ */
