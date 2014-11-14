#include "calculator.h"
#include "parser.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <math.h>


calculator::calculator() {
}

calculator::~calculator() {
}

void calculator::parseInputFile() {
    //Create parser handle
    parser* parserHandle = new parser;
    parserHandle->parseFile("user.m");

    //Get user input handle
    inputHandle = parserHandle->getInputHandle();
}

void calculator::printInputParam() {
    inputHandle.print();
}

void calculator::checkInputParam() {
    if (inputHandle.n_banks == -1 ||
        inputHandle.n_colMux == -1 ||
        inputHandle.n_rows == -1)
    {
        cerr << "Either banks, colMux or rows are missing in the input file" << endl;
        exit(0);
    }
    if(inputHandle.memory_size != ((inputHandle.n_banks * inputHandle.n_rows) * (inputHandle.n_colMux * inputHandle.word_size))) {
        cerr << "Memory size don't match system specification i.e banks, rows, colMux & wordsize" << endl;
        exit(0);
    }
}

/*
void calculator::createSRAM() {
    // Create SRAM obj
    sram = new SRAM;
    sram->setInput(inputHandle);
}
*/


void calculator::createRegFile() {
    // Create SRAM obj
    //sram = new RegFile;
    regfile = new RegFile;
    regfile->setInput(inputHandle);
    #ifdef DEBUG2
    std::cout << "tech = " << inputHandle.technology << std::endl;
    std::cout << "memory_size = " << inputHandle.memory_size << std::endl;
    std::cout << "n_banks = " << inputHandle.n_banks << std::endl;
    std::cout << "n_colMux = " << inputHandle.n_colMux << std::endl;
    std::cout << "n_rows = " << inputHandle.n_rows << std::endl;
    std::cout << "word_size = " << inputHandle.word_size << std::endl;
    std::cout << "SAoffset = " << inputHandle.SAoffset << std::endl;
    std::cout << "BCheight = " << inputHandle.BCheight << std::endl;
    std::cout << "BCwidth = " << inputHandle.BCwidth << std::endl;
    std::cout << "energy_constraint = " << inputHandle.energy_constraint << std::endl;
    std::cout << "delay_constraint = " << inputHandle.delay_constraint << std::endl;
    std::cout << "WDwidth = " << inputHandle.WDwidth << std::endl;
    std::cout << "temp = " << inputHandle.temp << std::endl;
    std::cout << "vdd = " << inputHandle.vdd << std::endl;
    std::cout << "TASEpath = " << inputHandle.TASEpath << std::endl;
    std::cout << "knobCount = " << inputHandle.knobCount << std::endl;
    for(int i=0; i<20; ++i)
    {
        std::cout << "knobMin = " << inputHandle.knobMin[i] << std::endl;
        std::cout << "knobMax = " << inputHandle.knobMax[i] << std::endl;
        std::cout << "knobName = " << inputHandle.knobName[i] << std::endl;
    }
    std::cout << "calNumBanks = " << inputHandle.calNumBanks << std::endl;
    std::cout << "minNumBanks = " << inputHandle.minNumBanks << std::endl;
    std::cout << "maxNumBanks = " << inputHandle.maxNumBanks << std::endl;
    std::cout << "calNumColMux = " << inputHandle.calNumColMux << std::endl;
    std::cout << "minNumColMux = " << inputHandle.minNumColMux << std::endl;
    std::cout << "maxNumColMux = " << inputHandle.maxNumColMux << std::endl;
    std::cout << "WLBoost = " << inputHandle.WLBoost << std::endl;
    std::cout << "WLOffset = " << inputHandle.WLOffset << std::endl;
    std::cout << "PCratio = " << inputHandle.PCratio << std::endl;
    std::cout << "WLratio = " << inputHandle.WLratio << std::endl;
    std::cout << "sweepToken = " << inputHandle.sweepToken << std::endl;
    std::cout << "sweepBegin = " << inputHandle.sweepBegin << std::endl;
    std::cout << "sweepEnd = " << inputHandle.sweepEnd << std::endl;
    std::cout << "sweepStep = " << inputHandle.sweepStep << std::endl;
    std::cout << "sweepOutput = " << inputHandle.sweepOutput << std::endl;
    #endif
}

void calculator::rmPrevResults() {
    //sram->rmPrevResults();
    regfile->rmPrevResults();
}

void calculator::runCharTests() {
    // Run Gate Capacitance Test
    regfile->charGateCap();
    regfile->calculateTechRC();
}

void calculator::simulate() {
    regfile->rmPrevResults();
    regfile->constructTemplate();
    regfile->simulate("SA");
    regfile->extractOutput();
}

void calculator::sweep() {
    float rE, rD, wE, wD;
    ofstream ofile(inputHandle.sweepOutput.c_str());
    // TODO - Make it generic
    for (float var = inputHandle.sweepBegin; var <= inputHandle.sweepEnd; var+=inputHandle.sweepStep) {
        // Modify the input
        // Determine the token
        if(!strcmp(inputHandle.sweepToken.c_str(), "SAoffset")) {
            inputHandle.SAoffset = var;
        }
        else if(!strcmp(inputHandle.sweepToken.c_str(), "PCratio")) {
            inputHandle.PCratio = (int)var;
        }
        else if(!strcmp(inputHandle.sweepToken.c_str(), "WLratio")) {
            inputHandle.WLratio = (int)var;
        }
        regfile->setInput(inputHandle);

        // Simulate
        simulate();

        // Get new ED
        getED(rE,rD,wE, wD);

        // Write to output file
        ofile << "\nSAOffset =" << var << endl;
        ofile << "readEnergy= " << rE << endl;
        ofile << "readDelay= " << rD << endl;
        ofile << "writeEnergy= " << wE << endl;
        ofile << "writeDelay= " << wD << endl;
    }
    ofile.close();
}

// Used for debugging
void calculator::constHash () {
    int b, r, c;
    float e, d, hash;
    ifstream file("/var/home/plb3qt/scratch/plb3qt/SRAM_OPT/cpp/SRAM8K.dat");
    cout << "Construct Hash" << endl;
    if(!file.is_open()) {
        cout << "CANT OPEN FILE" << endl;
    }
    while (!file.eof()) {
        string line;
        getline(file,line);
        //cout << " line = " << line << endl;
        sscanf(line.c_str(),"%d %d %d %f %f",&b, &r, &c, &e, &d);
        cout << b << " " << r << " " << c << " " << e << " " << d << endl;
        hash = c + r + b*1e3;
        e*=1e12;
        d*=1e9;
        en[hash] = e;
        del[hash] = d;
        cout << "hash = " << hash << endl;
        cout << "en[hash] = " << en[hash] << endl;
    }
    file.close();
}

float calculator::getR (int in) {
    float r;
        if (in == 0) {
        // return either 2 or 0.5
        r = int(rand() % 2);
        if (r == 0) {
           	    r = 0.5;
        } else if (r == 1){
            r = 2;
        }
    } else if (in == 1) {
        // return either 2,4 or 0.5,0.25
        r = int(rand() % 4);
        if (r == 0) {
               r = 0.5;
        } else if (r == 1){
           r = 2;
        } else if (r == 2) {
           r = 0.25;
        } else {
           r = 4;
        }
    } else {
        // return either 2,4,8 or 0.5,0.25,0.125
        r = int(rand() % 8);
        if (r == 0) {
               r = 0.5;
        } else if (r == 1){
           r = 2;
        } else if (r == 2) {
           r = 0.25;
        } else if (r == 3) {
           r = 4;
        } else if (r == 4) {
           r = 0.125;
        } else if (r == 5) {
           r = 8;
        } else if (r == 6) {
           r = 1.0/16;
        } else {
           r = 16;
        }
    }
    return r;
}

// Used for debugging
bool calculator::checkStatus (float r, float c, float b) {
    // search for the element
    float hash = r + c + b*1e3;
    cout << "hash = " << hash << endl;
    vector<float>::iterator it = find(hList.begin(), hList.end(), hash);
    if(it == hList.end())
        return 1;
    else
        return 0;
}

void calculator::execOptimize() {
    constHash();
    optimize();
}

// Eliminate the dependency between
// the number of banks, rows and col Mux
// if rows, colMux and banks are included
// Use only rows and colMux. number of banks
// will be calculated according to the defined
// values of rows and colMux.
void calculator::redefineKnobs() {
    // Check which system level knob is defined in the user.m file
    bool tune_rows = false;
    bool tune_colMux = false;
    bool tune_banks = false;
    for(int i=0; i < inputHandle.knobCount; ++i)
    {
        string name = inputHandle.knobName[i];
        if (!strcmp(name.c_str(),"NBANKS")) {
            tune_banks = true;
        } else if(!strcmp(name.c_str(),"NCOLS")) {
            tune_colMux = true;
        } else if(!strcmp(name.c_str(),"NROWS")) {
            tune_rows = true;
        }
    }
    // Determine which knobs will be used
    // others will be calculated
    bool add_rows = false;
    bool add_colMux = false;

    // Add rows & colM to the knobs
    if(tune_banks && tune_colMux && tune_rows) {
        add_rows = true;
        add_colMux = true;
        inputHandle.calNumBanks = true;
    }
    // Add only rows to the knobs
    else if (tune_rows)
    {
        add_rows = true;
        if(tune_banks)
            inputHandle.calNumBanks = true;
        else
            inputHandle.calNumColMux = true;
    }
    // Add only colMux to the knobs
    else if (tune_colMux)
    {
       add_colMux = true;
       inputHandle.calNumBanks = true;
    }

    // Redefine knobs
    int newknobCount = 0;
    string newknobName[20];
    float newknobMin[20];
    float newknobMax[20];
    for(int i=0; i < inputHandle.knobCount; ++i)
    {
        if ( (!strcmp(inputHandle.knobName[i].c_str(),"NCOLS") && !add_colMux) ||
             (!strcmp(inputHandle.knobName[i].c_str(),"NROWS") && !add_rows) ||
             (!strcmp(inputHandle.knobName[i].c_str(),"NBANKS")) )
        {
            // Don't add the knob - Save min/max values
            if(!strcmp(inputHandle.knobName[i].c_str(),"NBANKS") && inputHandle.calNumBanks) {
                inputHandle.minNumBanks = (int)inputHandle.knobMin[i];
                inputHandle.maxNumBanks = (int)inputHandle.knobMax[i];
                #ifdef DEBUG
                std::cout << "(redefineKnobs) i: " << i << std::endl;
                std::cout << "(redefineKnobs) NBANKS Min: " << inputHandle.minNumBanks << std::endl;
                std::cout << "(redefineKnobs) NBANKS Max: " << inputHandle.maxNumBanks << std::endl;
                #endif
            }
            else if(!strcmp(inputHandle.knobName[i].c_str(),"NCOLS") && inputHandle.calNumColMux) {
                inputHandle.minNumColMux = (int)inputHandle.knobMin[i];
                inputHandle.maxNumColMux = (int)inputHandle.knobMax[i];
                #ifdef DEBUG
                std::cout << "(redefineKnobs) i: " << i << std::endl;
                std::cout << "(redefineKnobs) NCOLS Min: " << inputHandle.minNumColMux << std::endl;
                std::cout << "(redefineKnobs) NCOLS Max: " << inputHandle.maxNumColMux << std::endl;
                #endif
            }
        }
        else
        {
            newknobName[newknobCount] = inputHandle.knobName[i];
            newknobMin[newknobCount] = inputHandle.knobMin[i];
            newknobMax[newknobCount] = inputHandle.knobMax[i];
            ++newknobCount;
            #ifdef DEBUG
            std::cout << "(redefineKnobs) i: " << i << std::endl;
            std::cout << "(redefineKnobs) newknobName[" << newknobCount-1 << "] = " << inputHandle.knobName[i] << std::endl;
            std::cout << "(redefineKnobs) newknobMin[" << newknobCount-1 << "] = " << inputHandle.knobMin[i] << std::endl;
            std::cout << "(redefineKnobs) newknobMax[" << newknobCount-1 << "] = " << inputHandle.knobMax[i] << std::endl;
            std::cout << "(redefineKnobs) newknobCount = " << newknobCount <<  std::endl;
            #endif
        }
    }

    // Overwrite Userinput object
    inputHandle.knobCount = newknobCount;
    for(int i=0; i < newknobCount; ++i)
    {
        inputHandle.knobName[i] = newknobName[i];
        inputHandle.knobMin[i] = newknobMin[i];
        inputHandle.knobMax[i] = newknobMax[i];
        #ifdef DEBUG
        std::cout << "(overwriting) inputHandle.knobName[" << i << "] = " << inputHandle.knobName[i] << std::endl;
        std::cout << "(overwriting) inputHandle.knobMin[" << i << "] = " << inputHandle.knobMin[i] << std::endl;
        std::cout << "(overwriting) inputHandle.knobMax[" << i << "] = " << inputHandle.knobMax[i] << std::endl;
        #endif
    }
}

void calculator::optimize () {
    //ofstream file("output.txt");
    // the probability
	    //alpha =0.999;
    float proba = 0.999;
    float alpha =0.3;
    float temperature = 400;
    float eps = 0.001;
    float iteration = 0;
    float curr_cost = 100000;
    float best_cost = 100000;
    float cur_var = 1;
    float rMoves = 0;

    float hash;
    float b, r, c, m, w, s, nc, nr, nb;

    // Initial Values
    w = inputHandle.word_size;
    m = inputHandle.memory_size;
    b = inputHandle.n_banks;
    c = inputHandle.n_colMux;
    r = inputHandle.n_rows;
    // Check
    //r = m / (c * w);

    // Determine the knobs
    bool tuneNBank = 0;
    bool tuneNRow = 0;
        bool tuneNCol = 0;
        for(int i = 0; i < inputHandle.knobCount; ++i) {
        if(!strcmp(inputHandle.knobName[i].c_str(), "NBANKS")) {
        tuneNBank = 1;
    }
        else if(!strcmp(inputHandle.knobName[i].c_str(), "NCOLS")) {
        tuneNCol = 1;
    }
        else if(!strcmp(inputHandle.knobName[i].c_str(), "NROWS")) {
        tuneNRow = 1;
    }
    }
        cout << "tuneNBank = " << tuneNBank << " tuneNRow = " << tuneNRow  << " tuneNCol = " << tuneNCol  << endl;
    // Error if one knob exist on the system level
    if ((tuneNBank + tuneNCol + tuneNRow) == 1) {
        cerr << "Error: Insufficient number of knobs at the system level <banks, cols, rows>" << endl;
    exit(1);
        }
    // while the temperature did not reach eps
    // while ($temperature > $eps && $iteration < 25 && $rMoves < 1e3) {
    while (iteration < 1e2 && rMoves < 5) {
        ++iteration;
        if(iteration == 2) {
    	exit(0);
    }

    // Get a new b,r,c
    int lock = 0;
    int itr = 0;
    while(1) {
        cout << "\n\n b " << b << " r = " << r << " c = " << c <<  "\n";
        ++itr;
        if(itr > 1e2) {
           lock = 1;
        }
        if(itr > 1e3) {
           lock = 2;
        }
        if(itr > 1e5) {
           cout << "Exceed Max Number Of Substitution" << endl;
           exit(0);
        }

        // Check tuneBank flag
        int modifyB = 0;
        if(tuneNBank && tuneNCol && tuneNRow) {
            modifyB = int(rand() % 2);

        } else if (tuneNBank) {
            modifyB = 0;
        }

        if(modifyB) {
            cout << "ChangeBank" << endl;
            //Change b
            float ch = getR(lock);
            if ( (b*ch <= 16) && (b*ch >= 1)) {
                // Change r or c
                int modifyR = 0;
                if(tuneNCol && tuneNRow) {
                    modifyR = int(rand() % 2);
                } else if (tuneNRow) {
                    modifyR = 1;
                }
                if (modifyR && (r/ch <= 512) && (r/ch >= 32) && checkStatus(r/ch, c, b*ch)) {
                    // Change r
                    cout << "ChooseR" << endl;
                    nb=b*ch;
                    nr=r/ch;
                    nc = c;
                    break;
                } else if ( (c/ch >= 1) && (c/ch <= 8) && checkStatus(r, c/ch, b*ch)) {
                    // Change c
                    cout << "ChooseC" << endl;
                    nb=b*ch;
                    nc=c/ch;
                    nr = r;
                    break;
                }
            }
        } else {
            // Don't Change b
            cout << "DontChangeBank" << endl;
            float ch = getR(lock);
            if ((r*ch <= 512) && (r*ch >= 32) && (c/ch >= 1) && (c/ch <= 8) && checkStatus(r*ch, c/ch, b)) {
                // Change r&c
                nr=r*ch;
                nc=c/ch;
                nb = b;
                break;
            }
        }
    }
    // Store hash to avoid re-iteration
    hash = nc + nr + nb*1e3;
    cout << "Calculated Hash = " << hash << endl;
    hList.push_back(hash);
    cout << "DONE nb = "  << nb << " nr = " <<  nr << " nc = " << nc << endl;

    // Change input
    // Call Simulate
    // Get Energy/Delay
    float energy, delay;
    //energy = en[hash];
    //delay = del[hash];

    inputHandle.n_banks = (int)nb;
    inputHandle.n_colMux = (int)nc;
    inputHandle.n_rows = (int)nr;
    regfile->setInput(inputHandle);

    // Simulate
    simulate();

    // Get new ED
    float rE,rD,wE, wD;
    getED(rE,rD,wE, wD);
        energy = rE + wE;
    delay = rD + wD;

    // Evaluate new cost
    float new_cost = energy;

    if(delay > 1.05) {
        new_cost+=100*delay;
    }
    cout << "NC: " << new_cost << endl;

        // Compute the distance of the new permuted configuration
        float delta = new_cost - curr_cost;

        // if the new cost is smaller accept it and assign it
        if (delta<0) {
        b=nb;
        r=nr;
        c=nc;
        curr_cost = new_cost;
        cout << "-Accept Move-\t" << endl;
        if (new_cost < best_cost) {
       	        best_cost = new_cost;
        }
       } else {
            // Decrease Pa for now.
        // proba = rand();
                proba = 0.15;

        // if the new cost is worse accept
            // it but with a probability level
            // if the probability is less than
            // E to the power -delta/temperature.
            // otherwise the old value is kept
                if (proba < exp(-delta/temperature)) {
       	b=nb;
       	r=nr;
       	c=nc;
       	curr_cost = new_cost;
       	float tmp = exp(-delta/temperature);
           	cout << "-Accept Move prob = " << proba << " tmp = " << tmp << endl;
           } else {
            ++rMoves;
           	cout  << "-Reject Move-" << endl;
           }
    }
        // Cooling process on every iteration
        temperature = temperature * alpha;
    cout << "\n\nit= " << iteration << " \t " << nb << " " << nr << " " << nc << " " << new_cost << "\t" << b << " " << r << " " << c << " " << " " << curr_cost << "\t" << best_cost << endl;
   }
}

void calculator::runBruteForce()
{
    // Output file
    ofstream OutFile("./bruteForceOutput.txt");

    // Get the total num of iterations
    float num_iterations = 1;
    for(int i=0; i < inputHandle.knobCount; ++i) {
        float count;
        // banks, col and rows are specified
        // in a multiple of 2 increments
        // count=2 when min=2, max=4.
        if(inputHandle.knobName[i] == "NBANKS" ||
           inputHandle.knobName[i] == "NCOLS" ||
           inputHandle.knobName[i] == "NROWS")
        {
            count = log2(inputHandle.knobMax[i] / inputHandle.knobMin[i]) + 1;
        }
        else
        {
            count = inputHandle.knobMax[i] - inputHandle.knobMin[i];
        }
        num_iterations *= count;
        #ifdef DEBUG
        std::cout << "(loop #1) i = " << i << std::endl;
        std::cout << "(loop #1) inputHandle.knobName[" << i << "] = " << inputHandle.knobName[i] << std::endl;
        std::cout << "(loop #1) count = " << count << std::endl;
        std::cout << "(loop #1) num_iterations = " << num_iterations << std::endl;
        #endif
    }
    // Double number of iterations if WLBoosting is used
    if(inputHandle.WLBoost) {
        num_iterations *= 2;
        #ifdef DEBUG
        std::cout << "(WLboost) num_iterations = " << num_iterations << std::endl;
        #endif
    }

    // Current value of knobs
    float knob_curr_val[32];

    // Loop over all the iterations
    for(int i=0; i < num_iterations; ++i)
    {
        #ifdef DEBUG
        cout << "(loop #2) iterations (i/all) = " << i+1 << "/" << num_iterations << endl;
        #endif

        // Set the WLBoost to "0" in the second
        // group of iteration if it exists
        // Need to send a copy of the input handle
        // instead of the originial object
        if(num_iterations / i == 2) {
            inputHandle.WLBoost = 0;
        }

        // Assign initial values for the knobs
        if(i == 0) {
            for(int i=0; i < inputHandle.knobCount; ++i) {
                knob_curr_val[i] = inputHandle.knobMin[i];
                #ifdef DEBUG
                cout << "(loop #2 init) knob_curr_val[" << i << "] = " << knob_curr_val[i] << endl;
                #endif
            }
        }
        else
        {
            // Update knobs values
            int count = 1;
            for(int j=0; j < inputHandle.knobCount; ++j)
            {
                if (!((i+1) % count))
                {
                    // Check if it is a system level knob
                    // Multiply banks, rows, col
                    // increment anything else
                    bool sysKnob = false;
                    if(inputHandle.knobName[j] == "NBANKS" ||
                       inputHandle.knobName[j] == "NCOLS" ||
                       inputHandle.knobName[j] == "NROWS")
                    {
                        sysKnob = true;
                        #ifdef DEBUG
                        std::cout << "(loop #3 sysknob) inputHandle.knobName[" << j << "] = " << inputHandle.knobName[j] << std::endl;
                        #endif
                    }
                    // Update the knobs values
                    if(knob_curr_val[j] >= inputHandle.knobMax[j]) {
                        knob_curr_val[j] = inputHandle.knobMin[j];
                        #ifdef DEBUG
                        cout << "(loop #3 update knobs if) knob_curr_val[" << j << "] = " << knob_curr_val[j] << endl;
                        #endif
                    } else {
                        if(sysKnob) {
                            knob_curr_val[j] *= 2;
                            #ifdef DEBUG
                            cout << "(loop #3 update knobs else) knob_curr_val[" << j << "] = " << knob_curr_val[j] << endl;
                            #endif
                        } else {
                            knob_curr_val[j] += 1;
                            #ifdef DEBUG
                            cout << "(loop #3 update non-sys knobs else) knob_curr_val[" << j << "] = " << knob_curr_val[j] << endl;
                            #endif
                        }
                    }
                    // Update count to determine which
                    // knob to update for this iteration
                    if(sysKnob) {
                        count *= (int)(log2(inputHandle.knobMax[j] / inputHandle.knobMin[j]) + 1);
                        #ifdef DEBUG
                        std::cout << "(sysknob evaluate) count  = " << count << std::endl;
                        #endif
                    } else {
                        count *= (int)(inputHandle.knobMax[j] - inputHandle.knobMin[j]);
                        #ifdef DEBUG
                        std::cout << "(non-sysknob evaluate) count  = " << count << std::endl;
                        #endif
                    }
                }
            } // End of update knobs looop
        } // End of if(it == 0)

        // Copy the new values of the knob to
        // the input object to be used in simulation
        for(int j=0; j < inputHandle.knobCount; ++j)
        {
               string name = inputHandle.knobName[j];
               if(!strcmp(name.c_str(),"NCOLS")) {
                    inputHandle.n_colMux = (int)knob_curr_val[j];
                    #ifdef DEBUG
                    std::cout << "(copy knob value) inputHandle.n_colMux = " << inputHandle.n_colMux << std::endl;
                    #endif
               } else if(!strcmp(name.c_str(),"NROWS")) {
                    inputHandle.n_rows = (int)knob_curr_val[j];
                    #ifdef DEBUG
                    std::cout << "(copy knob value) inputHandle.n_rows = " << inputHandle.n_rows << std::endl;
                    #endif
               }
        }


        // Evaluate numBanks and/or numColMux if needed
        bool invalid = false;
        if(inputHandle.calNumBanks) {
            inputHandle.n_banks =  inputHandle.memory_size / (inputHandle.n_colMux * inputHandle.n_rows * inputHandle.word_size);
            #ifdef DEBUG
            std::cout << "(evaluate n_banks) inputHandle.n_banks = " << inputHandle.n_banks << std::endl;
            #endif
            // Check range
            if(inputHandle.n_banks > inputHandle.maxNumBanks ||
               inputHandle.n_banks < inputHandle.minNumBanks) {
                invalid = true;
            }
        }
        if(inputHandle.calNumColMux) {
            inputHandle.n_colMux =  inputHandle.memory_size / (inputHandle.n_banks * inputHandle.n_rows * inputHandle.word_size);
            #ifdef DEBUG
            std::cout << "(evaluate n_colMux) inputHandle.n_colMux = " << inputHandle.n_colMux << std::endl;
            #endif
            // Check range
            if(inputHandle.n_colMux > inputHandle.maxNumColMux ||
               inputHandle.n_colMux < inputHandle.minNumColMux) {
                invalid = true;
            }
        }

        // Skip this iteration - Invalid design
        if(invalid)
        {
            #ifdef DEBUG
            std::cout << "invalid knob, skip current iteration" << std::endl;
            #endif
            continue;
        }

        // update tokens
        regfile->setInput(inputHandle);

        // Simulate
        simulate();

        // Get new ED
        float rE,rD,wE, wD, energy, delay;
        getED(rE,rD,wE, wD);
        energy = rE + wE;
        delay = rD + wD;

        // Print iteration info
        for(int j=0; j < inputHandle.knobCount; ++j)
        OutFile << inputHandle.knobName[j] << " = " << knob_curr_val[j] << endl;
        OutFile << "n_banks = " << inputHandle.n_banks << endl;
        OutFile << "Read Energy = " << rE << endl << "Read Delay = " << rD << endl;
        OutFile << "Write Energy = " << wE << endl << "Write Delay = " << wD << endl;
        OutFile << endl;
        #ifdef DEBUG
        print();
        #endif
    } // End of iterations loop
    OutFile.close();
}

void calculator::getED(float&rE, float&rD, float&wE, float&wD) {
    regfile->calculateReadED(rE,rD);
    regfile->calculateWriteED(wE,wD);
}

void calculator::print() {
    regfile->print();
}
