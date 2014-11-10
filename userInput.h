#ifndef USERINPUT_H_
#define USERINPUT_H_

#include <string>

using namespace std;

class userInput {
    public:
        userInput();
        ~userInput();
        void print();
        void setTech(string tech);
        void setMemSize(int memSize);
        void setNBanks(int nBanks);
        void setNColMux(int nColMux);
        void setNRows(int nRows);
        void setWordSize(int wordSize);
        void setSAOffset(float SAOffset);
        void setBCHeight(float BCHeight);
        void setBCWidth(float BCWidth);
        void setENConstraint(float ENConst);
        void setDLConstraint(float DLConst);
        void setWDWidth(int WDWidth);
        void setTASEPath(string path);
        void setTemp(float temperature);
        void setVdd(float Vdd);
        void setWLBoost(bool WLBoostFlag);
        void setWLOffset(float offset);
        string getTech();
        int getMemSize();
        int getNBanks();
        int getNColMux();
        int getNRows();
        int getWordSize();
        float getSAOffset();
        float getBCHeight();
        float getBCWidth();
        float getENConstraint();
        float getDLConstraint();
        float getWDWidth();
        string getTASEPath();
        float getTemp();
        float getVdd();
        bool getWLBoost();
        float getWLOffset();

//    private:
        string technology;
        int memory_size;
        int n_banks;
        int n_colMux;
        int n_rows;
        int word_size;
        float SAoffset;
        float BCheight;
        float BCwidth;
        float energy_constraint;
        float delay_constraint;
        int WDwidth;
        float temp;
        float vdd;
        string TASEpath;

        int knobCount;
        float knobMin[20];
        float knobMax[20];
        string knobName[20];

        bool calNumBanks;
        int minNumBanks;
        int maxNumBanks;

        bool calNumColMux;
        int minNumColMux;
        int maxNumColMux;

        bool WLBoost;
        float WLOffset;

        int PCratio;
        int WLratio;

        string sweepToken;
        float sweepBegin;
        float sweepEnd;
        float sweepStep;
        string sweepOutput;
};

#endif
