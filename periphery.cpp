#include "periphery.h"

void RFSubblock::runTASE(string tempPath) {
    stringstream sst;
    cout << "perl " << tasePath << "/device/BIN/run.pl -i " << tempPath << endl;
    sst << "perl " << tasePath << "/device/BIN/run.pl -i " << tempPath <<" -noimg -nopdf";
    system(sst.str().c_str());
}

void RFSubblock::moveResults(string test) {
    stringstream mvcmd;
    mvcmd << "cp -fr " << tasePath << "/device/BIN/" << getenv("USER") << " ../results_v2/" << test;
    system(mvcmd.str().c_str());
}

string RFSubblock::constructTempelate(string testName, string techTemp, string testInfo) {
    stringstream templatePath;
    templatePath << tasePath << "/template/RVP_" << testName << tech << ".ini";
    ofstream ofile(templatePath.str().c_str());
    ofile << techTemp << testInfo << endl;
    ofile.close();
    return templatePath.str();
}


DFF::DFF() {
}

DFF::DFF(userInput& uInp) {
}

DFF::~DFF() {
}

void DFF::setInput(userInput& uImp) {
    input = &uImp;
    tasePath = input->TASEpath;
    tech = input->technology;
}

void DFF::simulate(string techTemp) {
    string testInfo = "<ocn>\n\
    RVP_DFF\n\
    </ocn>\n";
    string tempPath = constructTempelate("DFF",techTemp,testInfo);
    runTASE(tempPath);
    moveResults("DFF");
}

void DFF::extractOutput() {
        stringstream sst;
        ifstream fileHandle;
        fileHandle.open("../results_v2/DFF/RVP_DFF/data.txt");
        if(!fileHandle.is_open()) {
            cerr << "Error: Can't open output file \n ../results_v2/DFF/RVP_DFF/data.txt" << endl;
            exit(1);
        }
        sst << fileHandle.rdbuf();
        sst >> delay_DFF >> delay_DFF_w >> energy_DFF_r >> energy_DFF_w;
        fileHandle.close();
}

float DFF::getRdelay() {
    return delay_DFF;
}

float DFF::getRenergy() {
    return energy_DFF_r;
}

float DFF::getWdelay() {
    return delay_DFF_w;
}

float DFF::getWenergy() {
    return energy_DFF_w;
}

void DFF::print() {
    cout << "tasePath " << tasePath <<  endl;
    cout << "delay_DFF= " << delay_DFF << "\n energy_DFF_r= " << energy_DFF_r << endl;
    cout << "delay_DFF_w= " << delay_DFF_w << "\n energy_DFF_w= " << energy_DFF_w << endl;
}

senseAmp::senseAmp() {
}

senseAmp::senseAmp(userInput& uInp) {
}

senseAmp::~senseAmp() {
}

void senseAmp::getBCInfo() {
}

void senseAmp::simulate(string techTemp) {
    stringstream techTemplate;
    techTemplate << techTemp;
    techTemplate << "<ocn>\n\
    RVP_SA\n\
    </ocn>\n";
    stringstream newTempPath;
    newTempPath << tasePath << "/template/RVP_SA_" << tech << ".ini";
    ofstream ofile(newTempPath.str().c_str());
    ofile << techTemplate.str() << endl;
    ofile.close();
    runTASE(newTempPath.str());
    moveResults("SA");
}

void senseAmp::extractOutput() {
    stringstream sst;
    ifstream fileHandle;
    string dummy;

    fileHandle.open("../results_v2/SA/RVP_SA/datar.txt");
    if(!fileHandle.is_open()) {
        cerr << "Error: Can't open output file \n ../results_v2/SA/RVP_SA/datar.txt" << endl;
        exit(1);
    }
    sst << fileHandle.rdbuf();
    sst >> dummy >> dummy >> energy_SA >> delay_SA >> energy_inter >> delay_inter;
    fileHandle.close();
}

void senseAmp::setInput(userInput& uImp) {
        input = &uImp;
        tasePath = input->TASEpath;
        tech = input->technology;
}

float senseAmp::getSADelay() {
    return delay_SA;
}

float senseAmp::getSAEnergy() {
    return energy_SA;
}

float senseAmp::getIntDelay() {
    return delay_inter;
}

float senseAmp::getIntEnergy() {
    return energy_inter;
}

void senseAmp::print() {
   //cout << "tasePath " << tasePath <<  endl;
    cout << "energy_SA= " << energy_SA << "\n delay_SA= " << delay_SA << endl;
    cout << "energy_SA_inter= " << energy_inter << "\n delay_SA_inter= " << delay_inter << endl;
}

rowDecoder::rowDecoder() {
}

rowDecoder::rowDecoder(userInput& uInp) {
}

rowDecoder::~rowDecoder() {
}

void rowDecoder::simulate(string techTemp) {
    stringstream techTemplate;
    techTemplate << techTemp;

    // Check if WL Boosting is specified
    if(input->WLBoost) {
        techTemplate << "<ocn>\n\
        RVP_Decoder_WWL_Boost\n\
        </ocn>\n";
    }
    else
    {
        techTemplate << "<ocn>\n\
        RVP_Decoder\n\
        </ocn>\n";
    }
    stringstream newTempPath;
    newTempPath << tasePath << "/template/RVP_RD_" << tech << ".ini";
    ofstream ofile(newTempPath.str().c_str());
    ofile << techTemplate.str() << endl;
    ofile.close();
    runTASE(newTempPath.str());
    moveResults("RD");
}

void rowDecoder::extractOutput() {
        stringstream sst_path, sst;
        ifstream fileHandle;
        string dummy;

    //Revisit the hierarchy
    //Check WL Boosting
    if(input->WLBoost) {
        sst_path << "../results_v2/RD/RVP_Decoder_WWL_Boost/output_" << nRows << ".txt";
        fileHandle.open(sst_path.str().c_str());
        cout << " Rows= " << nRows << endl;
        if(!fileHandle.is_open()) {
            cerr << "Error: Can't open output file \n" << sst_path << endl;
            exit(1);
        }
        sst << fileHandle.rdbuf();

        float power;
        sst >> dummy >> dummy >> dummy >> delay_rowDecoder >>  power;
        energy_rowDecoder = power*32e-9;
    } else {
        sst_path << "../results_v2/RD/RVP_Decoder/output_" << nRows << ".txt";
        fileHandle.open(sst_path.str().c_str());
        cout << " Rows= " << nRows << endl;
        if(!fileHandle.is_open()) {
            cerr << "Error: Can't open output file \n" << sst_path << endl;
            exit(1);
        }
        sst << fileHandle.rdbuf();
        string str = sst.str();
        regex_t re;
        size_t     nmatch = 3;
        regmatch_t pmatch[3];

        char *pattern = "[^\n]*\n[^\n]*\n[^\n]*\n[ \t]*[^ \t]+[ \t]*[^ \t]+[ \t]*[^ \t]+[ \t]*([^ \t]+)[ \t]*([^ \t]+)";
        regcomp(&re, pattern, REG_EXTENDED);
        int status = regexec(&re, str.c_str(), nmatch, pmatch, 0);
        regfree(&re);
        if(status!=0) {
            cerr << "Error: Can't Parse Row Decoder file " << sst_path.str().c_str() << "\n" << str << endl;
            exit(0);
        }
        string delay_s = str.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        string energy_s = str.substr(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
        delay_rowDecoder = atof(delay_s.c_str());
        energy_rowDecoder = atof(energy_s.c_str())*32e-9;
    }
    fileHandle.close();
}

void rowDecoder::setInput(userInput& uImp) {
    input = &uImp;
    tasePath = input->TASEpath;
    tech = input->technology;
    nRows = (int)log2((int)input->n_rows);
}

float rowDecoder::getEnergy() {
    return energy_rowDecoder;
}

float rowDecoder::getDelay() {
    return delay_rowDecoder;
}

void rowDecoder::print() {
    cout << "tasePath " << tasePath <<  endl;
    cout << "delay_rowDecoder= " << delay_rowDecoder << "\n energy_rowDecoder= " << energy_rowDecoder << endl;
}

colMux::colMux() {
}

colMux::colMux(userInput& uInp) {
}

colMux::~colMux() {
}

void colMux::simulate(string techTemp) {
    stringstream techTemplate;
    techTemplate << techTemp;
    techTemplate << "<ocn>\n\
    RVP_Bank_Mux\n\
    </ocn>\n";
    stringstream newTempPath;
    newTempPath << tasePath << "/template/RVP_CM_" << tech << ".ini";
    ofstream ofile(newTempPath.str().c_str());
    ofile << techTemplate.str() << endl;
    ofile.close();
    runTASE(newTempPath.str());
    moveResults("CM");
}

void colMux::setInput(userInput& uImp) {
    input = &uImp;
    tasePath = input->TASEpath;
    tech = input->technology;
}

bitCell::bitCell() {
}

bitCell::bitCell(userInput& uInp) {
}

bitCell::~bitCell() {
}

void bitCell::simulate(string techTemp) {
    // Find ldef and wdef
    regex_t re;
    regmatch_t pmatch[2];

    char *pattern1 = "[ \t]*<ldef>[ \t]*([0-9]+)[ \t]*n";
    regcomp(&re, pattern1, REG_EXTENDED);
    regexec(&re, techTemp.c_str(), 2, pmatch, 0);
    int ldef = atoi(techTemp.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so).c_str());
    regfree(&re);

    char *pattern2 = "[ \t]*<wdef>[ \t]*([0-9]+)[ \t]*n";
    regcomp(&re, pattern2, REG_EXTENDED);
    regexec(&re, techTemp.c_str(), 2, pmatch, 0);
    int wdef = atoi(techTemp.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so).c_str());
    regfree(&re);

    stringstream techTemplate;
    techTemplate << techTemp;
    // Add WLBoosting offset if specified
    float offset = 0;
    if(input->WLBoost) {
        offset = input->WLOffset ;
    }
    techTemplate << "<WLOffset> " << offset << endl;
    // Add PCH parameters
    int pcRatio =  6;
    if(input->PCratio != -1) {
        pcRatio = input->PCratio;
    }
    techTemplate << "<wpc> " << pcRatio*wdef*1e-9 << "\n<lpc> " << ldef*1e-9 << endl;

    int wlRatio =  1;
    if(input->WLratio != -1) {
        wlRatio = input->WLratio;
    }
    techTemplate << "<WLratio> " << wlRatio << endl;

    // Add BC Tests
    techTemplate << "\n<scs>\n\
    RVP_Ileak_PU\n\
    RVP_Ileak_PD\n\
    RVP_Ileak_PG\n\
    </scs>\n\
    \n\
    <ocn>\n\
    RVP_CD\n\
    </ocn>\n";
    stringstream newTempPath;
    newTempPath << tasePath << "/template/RVP_BC_" << tech << ".ini";
    ofstream ofile(newTempPath.str().c_str());
    ofile << techTemplate.str() << endl;
    ofile.close();
    runTASE(newTempPath.str());
    moveResults("BC");
}

void bitCell::extractOutput() {
    stringstream sst;
    ifstream fileHandle;
    string dummy;
    fileHandle.open("../results_v2/BC/RVP_CD/datar.txt");
    if(!fileHandle.is_open()) {
        cerr << "Error: Can't open output file \n ../results_v2/BC/RVP_CD/datar.txt" << endl;
        exit(1);
    }
    sst << fileHandle.rdbuf();
    sst >> dummy >> dummy >> dummy >> delay_bitcell_r >> delay_pch_r >> energy_pch_r >> energy_bitcell_r >> energy_inter_read >> delay_inter_read;
    fileHandle.close();

    sst.clear();
    sst.str("");

    fileHandle.open("../results_v2/BC/RVP_CD/dataw.txt");
    if(!fileHandle.is_open()) {
        cerr << "Error: Can't open output file \n ../results_v2/BC/RVP_CD/dataw.txt" << endl;
        exit(1);
    }
    sst << fileHandle.rdbuf();
    sst >> dummy >> dummy >> dummy >> delay_pch_w >> delay_writeDriver >> delay_bitcell_w >> energy_pch_w >> energy_bitcell_w >> energy_writeDriver >> energy_inter_write >> delay_inter_write;
    fileHandle.close();

    // Take it from the input
    //int vdd = 1;
    //int memSize = 512;

    // Re-visit Leakage Calculations!!
    // Is it Vdd or Vth

    float iOffPg,iOffPu, iOffPd;

    sst.clear();
    sst.str("");

    // Pass-gate leakage
    fileHandle.open("../results_v2/BC/RVP_Ileak_PG/DAT/dc_N1_NX_d.dat");
    if(!fileHandle.is_open()) {
        cerr << "Error: Can't open output file \n ../results_v2/BC/RVP_Ileak_PG/DAT/dc_N1_NX_d.dat" << endl;
        exit(1);
    }
    sst << fileHandle.rdbuf();
    sst >> iOffPg;
    fileHandle.close();

    sst.clear();
    sst.str("");

    // Pull-up leakage
    fileHandle.open("../results_v2/BC/RVP_Ileak_PU/DAT/dc_P1_PX_d.dat");
    if(!fileHandle.is_open()) {
        cerr << "Error: Can't open output file \n ../results_v2/BC/RVP_Ileak_PU/DAT/dc_P1_PX_d.dat" << endl;
        exit(1);
    }
    sst << fileHandle.rdbuf();
    sst >> iOffPu;
    fileHandle.close();

    sst.clear();
    sst.str("");

    // Pull-down leakage
    fileHandle.open("../results_v2/BC/RVP_Ileak_PD/DAT/dc_N1_NX_d.dat");
    if(!fileHandle.is_open()) {
        cerr << "Error: Can't open output file \n ../results_v2/BC/RVP_Ileak_PD/DAT/dc_N1_NX_d.dat" << endl;
        exit(1);
    }
    sst << fileHandle.rdbuf();
    sst >> iOffPd;
    fileHandle.close();

    leakage_power=(iOffPu+iOffPg+iOffPd)*memSize*vdd;
}

void bitCell::print() {
    // Q.
    // Why do we have different variables pch_w and pch_r
    // Aren't they the same?
    cout << "tasePath " << tasePath <<  endl;
    cout << "delay_bitcell_r= "  << delay_bitcell_r << "\n energy_bitcell_r = " << energy_bitcell_r << endl;
    cout << "delay_inter_pch= " << delay_inter_read << "\n energy_inter_pch= " << energy_inter_read << endl;
    cout << "delay_pch_r= " << delay_pch_r << "\n energy_pch_r " << energy_pch_r << endl;
    cout << "delay_pch_w= " << delay_pch_w << "\n energy_pch_w= " << energy_pch_w << endl;
    cout << "delay_writeDriver= " << delay_writeDriver << "\n energy_writeDriver= " << energy_writeDriver << endl;
    cout << "delay_bitcell_w= " << delay_bitcell_w << "\n energy_bitcell_w= " << energy_bitcell_w << endl;
    cout << "leakage= " << leakage_power << endl;
}

void bitCell::setInput(userInput& uImp) {
    input = &uImp;
    tasePath = input->TASEpath;
    tech = input->technology;
    vdd = input->vdd;
    memSize = input->memory_size;
}

float bitCell::getBCRDelay() {
    return delay_bitcell_r;
}

float bitCell::getBCREnergy() {
    return energy_bitcell_r;
}

float bitCell::getBCWDelay() {
    return delay_bitcell_w;
}

float bitCell::getBCWEnergy() {
    return energy_bitcell_w;
}

float bitCell::getPCRDelay() {
    return delay_pch_r;
}

float bitCell::getPCREnergy() {
    return energy_pch_r;
}

float bitCell::getPCWDelay() {
    return delay_pch_w;
}

float bitCell::getPCWEnergy() {
    return energy_pch_w;
}

float bitCell::getWDDelay() {
    return delay_writeDriver;
}

float bitCell::getWDEnergy() {
    return energy_writeDriver;
}

float bitCell::getBCLeakage() {
    return leakage_power;
}

float bitCell::getIntREenergy() {
    return energy_inter_read;
}

float bitCell::getIntWEenergy() {
    return energy_inter_write;
}

float bitCell::getIntRDelay() {
    return delay_inter_read;
}

float bitCell::getIntWDelay() {
    return delay_inter_write;
}

timingBlock::timingBlock() {
}

timingBlock::timingBlock(userInput& uInp) {
}

timingBlock::~timingBlock() {
}

void timingBlock::simulate(string techTemp) {
    stringstream techTemplate;
    techTemplate << techTemp;
    techTemplate << "<ocn>\n\
    RVP_TIMING\n\
    </ocn>\n";
    stringstream newTempPath;
    newTempPath << tasePath << "/template/RVP_TB_" << tech << ".ini";
    ofstream ofile(newTempPath.str().c_str());
    ofile << techTemplate.str() << endl;
    ofile.close();
    runTASE(newTempPath.str());
    moveResults("TB");
}

void timingBlock::extractOutput() {
    stringstream sst;
    ifstream fileHandle;

    fileHandle.open("../results_v2/TB/RVP_TIMING/data.txt");
    if(!fileHandle.is_open()) {
        cerr << "Error: Can't open output file \n ../results_v2/TB/RVP_TIMING/data.txt" << endl;
        exit(1);
    }
    sst << fileHandle.rdbuf();
    sst >> energy_timing;
    fileHandle.close();
}

void timingBlock::setInput(userInput& uImp) {
    input = &uImp;
    tasePath = input->TASEpath;
    tech = input->technology;
}

void timingBlock::print() {
    cout << "tasePath= " << tasePath <<  endl;
    cout << "energy_timing= " << energy_timing << endl;
}

float timingBlock::getEnergy() {
    return energy_timing;
}

writeDriver::writeDriver() {
}

writeDriver::writeDriver(userInput& uInp) {
}

writeDriver::~writeDriver() {
}

void writeDriver::simulate(string techTemp) {
}

void writeDriver::setInput(userInput& uImp) {
    input = &uImp;
    tasePath = input->TASEpath;
    tech = input->technology;
}

bankMux::bankMux() {
}

bankMux::bankMux(userInput& uInp) {
}

bankMux::~bankMux() {
}

void bankMux::simulate(string techTemp) {
    stringstream techTemplate;
    techTemplate << techTemp;
    techTemplate << "<ocn>\n\
    RVP_Bank_Mux\n\
    </ocn>\n";
    stringstream newTempPath;
    newTempPath << tasePath << "/template/RVP_BM_" << tech << ".ini";
    ofstream ofile(newTempPath.str().c_str());
    ofile << techTemplate.str() << endl;
    ofile.close();
    runTASE(newTempPath.str());
    moveResults("BM");
}

void bankMux::extractOutput() {
    stringstream sst;
    ifstream fileHandle;
    string dummy;

    fileHandle.open("../results_v2/BM/RVP_Bank_Mux/data.txt");
    sst << fileHandle.rdbuf();
    sst >> dummy >> dummy >> energy_bankMux >> delay_bankMux >> energy_inter >> delay_inter;
    fileHandle.close();
}

void bankMux::setInput(userInput& uImp) {
    input = &uImp;
    tasePath = input->TASEpath;
    tech = input->technology;
}

void bankMux::print() {
    cout << "tasePath= " << tasePath <<  endl;
    cout << "delay_BM= " << delay_bankMux << "\n energy_BM= " << energy_bankMux << endl;
    cout << "delay_BM_inter= " << delay_inter << "\n energy_BM_inter= " << energy_inter << endl;
}

float bankMux::getBankMuxtDelay() {
    return delay_bankMux;
}

float bankMux::getBankMuxtEnergy() {
    return energy_bankMux;
}

float bankMux::getIntDelay() {
    return delay_inter;
}

float bankMux::getIntEnergy() {
    return energy_inter;
}
