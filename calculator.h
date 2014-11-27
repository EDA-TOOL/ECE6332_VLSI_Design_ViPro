#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#include "userInput.h"
#include "RegFile.h"
#include "parser.h"
#include <map>
#include <vector>

class calculator {
public:
    calculator();
    ~calculator();
    void parseInputFile();
    void printInputParam();
    void checkInputParam();
    void createRegFile();
    void rmPrevResults();
    void runCharTests();
    void simulate(string testname);
    void sweep();
    void constHash();
    float getR(int in);
    bool checkStatus(float r, float c, float b);
    void execOptimize();
    void redefineKnobs();
    void optimize();
    void runBruteForce();
    void getED(float& rE, float& rD, float& wE, float& wD);
    void print();
    //void createSRAM();

private:
    map<float, float> en;
    map<float, float> del;
    vector<float> hList;

    userInput inputHandle;
    RegFile* regfile;
    //RegFile sram;
};


#endif /* CALCULATOR_H_ */
