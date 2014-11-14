#ifndef PERIPHERY_H_
#define PERIPHERY_H_


#include "userInput.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <regex.h>
#include <cstdlib>
#include <math.h>

class RFSubblock {
	public:
        userInput* input;
        string tasePath;
        string tech;

        void runTASE(string tempPath);
        void moveResults(string test);
        string constructTempelate(string testName, string techTemp, string testInfo);
};

class DFF: public RFSubblock {
        public:
        DFF();
        DFF(userInput& uInp);
        ~DFF();
        void setInput(userInput& uImp);
        void simulate(string techTemp);
        void extractOutput();
        float getRdelay();
        float getRenergy();
        float getWdelay();
        float getWenergy();
        void print();

    private:
        float delay_DFF;
        float delay_DFF_w;
        float energy_DFF_r;
        float energy_DFF_w;
};

class senseAmp : public RFSubblock {
    public:
        senseAmp();
        senseAmp(userInput& uInp);
        ~senseAmp();
        void getBCInfo();
        void simulate(string techTemp);
        void extractOutput();
        void setInput(userInput& uImp);
        float getSADelay();
        float getSAEnergy();
        float getIntDelay();
        float getIntEnergy();
        void print();

    private:
        float energy_SA;
        float delay_SA;
        float energy_inter;
        float delay_inter;

};

class rowDecoder : public RFSubblock {
	public:
        rowDecoder();
        rowDecoder(userInput& uInp);
        ~rowDecoder();
        void simulate(string techTemp);
        void extractOutput();
        void setInput(userInput& uImp);
        float getEnergy();
        float getDelay();
        void print();

    private:
        int nRows;
        float energy_rowDecoder;
        float delay_rowDecoder;

};

// should be included somehow
class colMux : public RFSubblock {
    public:
        colMux();
        colMux(userInput& uInp);
        ~colMux();
        void simulate(string techTemp);
        void setInput(userInput& uImp);
    private:
};

class bitCell : public RFSubblock {
    public:
        bitCell();
        bitCell(userInput& uInp);
        ~bitCell();
        void simulate(string techTemp);
        void extractOutput();
        void print();
        void setInput(userInput& uImp);
        float getBCRDelay();
        float getBCREnergy();
        float getBCWDelay();
        float getBCWEnergy();
        float getPCRDelay();
        float getPCREnergy();
        float getPCWDelay();
        float getPCWEnergy();
        float getWDDelay();
        float getWDEnergy();
        float getBCLeakage();
        float getIntREenergy();
        float getIntWEenergy();
        float getIntRDelay();
        float getIntWDelay();

    private:
        float vdd;
        float memSize;

        // Separate them & Add Precharge-Class
        float delay_bitcell_r;
        float delay_pch_r;
        float energy_bitcell_r;
        float energy_pch_r;

        float energy_inter_read;
        float delay_inter_read;

        // Not getting this one
        // Do you use pre-charge during write?
        // thought the write driver do it!
        float delay_pch_w;
        float delay_writeDriver;
        float delay_bitcell_w;
        float energy_pch_w;
        float energy_bitcell_w;
        float energy_writeDriver;

        float energy_inter_write;
        float delay_inter_write;

        float leakage_power;
};

class timingBlock : public RFSubblock {
    public:
        timingBlock();
        timingBlock(userInput& uInp);
        ~timingBlock();
        void simulate(string techTemp);
        void extractOutput();
        void setInput(userInput& uImp);
        void print();
        float getEnergy();

    private:
        float energy_timing;
};

// Needs to separate the driver
// from the SRAM bitcell test
class writeDriver : public RFSubblock {
    public:
        writeDriver();
        writeDriver(userInput& uInp);
        ~writeDriver();
        // Currently with the BC
        void simulate(string techTemp);
        void setInput(userInput& uImp);
    private:
};

// Q: Why colMux is ignored?
// Q: Why energy is ignored in bankmux?
class bankMux : public RFSubblock {
    public:
        bankMux();
        bankMux(userInput& uInp);
        ~bankMux();
        void simulate(string techTemp);
        void extractOutput();
        void setInput(userInput& uImp);
        void print();
        float getBankMuxtDelay();
        float getBankMuxtEnergy();
        float getIntDelay();
        float getIntEnergy();

    private:
        float delay_bankMux;
        float energy_bankMux;
        float delay_inter;
        float energy_inter;
};


#endif
