#include <iostream>
#include "periphery.h"
#include "calculator.h"
#include "parser.h"
#include "userInput.h"
#include "RegFile.h"

using namespace std;

int main()
{
    /*
    string filename;
    string test2run = "SA";
    filename = "Subblock_" + test2run + ".log";
    ofstream ofile(filename.c_str());
    ofile << "haha";
    ofile.close();
    */
    calculator* CT = new calculator;
    CT->parseInputFile();
    CT->createRegFile();
    CT->rmPrevResults();
    //CT->printInputParam();
    CT->redefineKnobs();
    //CT->printInputParam();
    CT->runCharTests();
    CT->checkInputParam();
    //CT->simulate();
    //CT->sweep();
    //CT->execOptimize();
    CT->runBruteForce();

    float rE,rD, wE, wD;
    rE=88;
    rD=99;
    wE=188;
    wD=199;

    CT->getED(rE,rD,wE, wD);
    //CT->print();
    cout << "rEn= " << rE << " rDel= " << rD << "\nwEn= " << wE << " wDel= " << wD << endl;

    return 0;
}
