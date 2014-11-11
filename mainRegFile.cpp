#include <iostream>
#include "periphery.h"
#include "calculator.h"
#include "parser.h"
#include "userInput.h"
#include "RegFile.h"

using namespace std;

int main()
{
	calculator* CT = new calculator;
	CT->parseInputFile();
	CT->createRegFile();
	CT->rmPrevResults();
	//CT->printInputParam();
	CT->redefineKnobs();
    /*
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

	//Create parser handle
	//parser* parserHandle = new parser;
	//parserHandle->parseFile("user.m");

	//Get user input handle
	//userInput inputHandle = parserHandle->getInputHandle();
	//inputHandle.print();

	// Create SRAM obj
	//SRAM* sram = new SRAM;
	//sram->setInput(inputHandle);
	//sram->runPreSimulation();
	//sram->runSimulations();
	//sram->constructTemplate();

    */

	return 0;
}
