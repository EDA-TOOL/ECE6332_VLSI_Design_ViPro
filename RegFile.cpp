#include "RegFile.h"
#include "userInput.h"
#include <sys/stat.h>

RegFile::RegFile(userInput& uImp) {
    inp = uImp;
    cout << "Const2 Called" << endl;
    gateCap = 0;
    rbl = 0;
    cbl = 0;
    cwl = 0;
}

RegFile::RegFile() {
    gateCap = 0;
    rbl = 0;
    cbl = 0;
    cwl = 0;
}

RegFile::~RegFile() {
}

void RegFile::setInput(userInput& uImp) {
    inp = uImp;
    SA.setInput(uImp);
    RD.setInput(uImp);
    CM.setInput(uImp);
    BC.setInput(uImp);
    TB.setInput(uImp);
    WD.setInput(uImp);
    ioDFF.setInput(uImp);
    BM.setInput(uImp);
}

void RegFile::charGateCap() {
    // Need to check TASE flag
    // Need to check if SCOT needs pre-processor(s)
    string tasePath = inp.TASEpath;
    stringstream templ_path, new_templ_path;
    string tech = inp.technology;
    templ_path << tasePath << "/template/RVPtpl_" << tech << ".ini";
    new_templ_path << tasePath << "/template/RVPn_" << tech << ".ini";

    #ifdef DEBUG
    cout << "templ_path = " << templ_path.str().c_str() << endl;
    cout << "new_templ_path = " << new_templ_path.str().c_str() << endl;
    #endif
    ifstream tpl(templ_path.str().c_str());
    ofstream newtpl(new_templ_path.str().c_str());
    stringstream new_tpl;
    if (tpl.is_open()) {
        new_tpl << tpl.rdbuf();
        tpl.close();
    } else {
        cerr << "Error: Can't find input template.\n";
        exit(1);
    }
    new_tpl << "\n\
<ocn>\n\
RVP_Gate_Capacitance\n\
</ocn>\n";

    newtpl << new_tpl.str();
    newtpl.close();

    // Run TASE
    runTASE(new_templ_path.str());

    // Move Results
    stringstream mvCmd;
    mvCmd << "mv " << tasePath << "/device/BIN/" << getenv("USER") << " ../results_v2/GC";
    system(mvCmd.str().c_str());


    // Get the Gate Cap
    ifstream fileHandle("../results_v2/GC/RVP_Gate_Capacitance/data.txt");
    if (fileHandle.is_open()) {
        stringstream st;
        st << fileHandle.rdbuf();
        gateCap = atof(st.str().c_str());
        fileHandle.close();
        #ifdef DEBUG
        std::cout << "gateCap = " << gateCap << std::endl;
        #endif
    } else {
        cerr << "Error: Can't open RVP_Gate_Capacitance test output " <<  endl;
        exit(1);
    }
}

void RegFile::calculateTechRC() {
    // Calculate R,C of tech, then CWL,CBL RBL
    float param[] = {0, 0, 0, 0, 0};
    // Get tech node
    int techNode;
    sscanf(inp.technology.c_str(),"%*[^0-9]%d",&techNode);
    //cout << "techNode = " << techNode << endl;
    RC obj = InterconnectRC(techNode, "i", "cu", param);
    rbl = obj.res*inp.BCheight*1e6;
    cbl = obj.cap*inp.BCheight*1e6;
    cwl = obj.cap*inp.BCwidth*1e6;
    #ifdef DEBUG
    std::cout << "rbl = " << rbl << std::endl;
    std::cout << "cbl = " << cbl << std::endl;
    std::cout << "cwl = " << cwl << std::endl;
    #endif
    //exit(1);
}

// should be moved to calculator
// call it add cap to template
void RegFile::constructTemplate() {

    // Empty the template
    techTemplate.str("");
    techTemplate.clear();

    // Read the input template
    string tasePath = inp.TASEpath;
    string tech = inp.technology;
    stringstream templ_path;
    templ_path << tasePath << "/template/RVPtpl_" << tech << ".ini";

    ifstream tpl(templ_path.str().c_str());
    if (!tpl.is_open()) {
        cerr << "Error: Can't find input template.\n";
        exit(1);
    }
    // Read the template and replace
    // Bit-cell information by user input
    // TODO-Need to check input complete
    // Check if height and width are mandatory
    string line;
    while(!tpl.eof()) {
        getline(tpl,line);
        if(line.find("addrRow") != string::npos) {
            techTemplate << "<addrRow>  " << log2(inp.n_rows) << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <addrRow> log2(inp.n_rows) = " << log2(inp.n_rows) << std::endl;
            #endif

        } else if(line.find("<NR_sweep>") != string::npos) {
            techTemplate << "<NR_sweep>  " << inp.n_rows << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <NR_sweep> n_rows = " << inp.n_rows << std::endl;
            #endif

        } else if(line.find("<addrCol>") != string::npos) {
            techTemplate << "<addrCol>  " << log2(inp.n_colMux) << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <addrCol> log2(n_colMux) = " << log2(inp.n_colMux) << std::endl;
            #endif

        } else if(line.find("<numBanks>") != string::npos) {
            techTemplate << "<numBanks>  " << inp.n_banks << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <numBanks> n_banks = " << inp.n_banks << std::endl;
            #endif

        } else if(line.find("<memsize>") != string::npos) {
            techTemplate << "<memsize>  " << inp.memory_size << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <memsize> memory_size = " << inp.memory_size << std::endl;
            #endif

        } else if(line.find("<ws>") != string::npos) {
            techTemplate << "<ws>  " << inp.word_size << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <ws> word_size = " << inp.word_size << std::endl;
            #endif

        } else if(line.find("<NC_sweep>") != string::npos) {
            techTemplate << "<NC_sweep> " << inp.n_colMux * inp.word_size << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <NC_sweep> n_colMux * word_size  = " << inp.n_colMux * inp.word_size << std::endl;
            #endif

        } else if(line.find("<colMux>") != string::npos) {
            techTemplate << "<colMux> " << inp.n_colMux << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <colMux> n_colMux = " << inp.n_colMux << std::endl;
            #endif

        } else if(line.find("<temp>") != string::npos) {
            techTemplate << "<temp> " << inp.temp << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <temp> temp = " << inp.temp << std::endl;
            #endif

        } else if(line.find("<BL_DIFF>") != string::npos) {
            techTemplate << "<BL_DIFF> " << inp.SAoffset << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <BL_DIFF> SAoffset = " << inp.SAoffset << std::endl;
            #endif

        } else if(line.find("<pvdd>") != string::npos) {
            // if vdd=1.0, append a ".0"
            if(inp.vdd == 1)
                techTemplate << "<pvdd> " << inp.vdd << ".0" << endl;
            else
                techTemplate << "<pvdd> " << inp.vdd << endl;
            #ifdef DEBUG
            std::cout << "(simulate templ generate) <pvdd> pvdd = " << inp.vdd << std::endl;
            #endif

        } else {
            techTemplate << line << endl;
        }
    }
    tpl.close();

    // Add the Gate Cap, cbl, cwl, rbl to the template
    techTemplate << "\
<cg>\t" << gateCap << "\
\n\
\n####################\n\
  # Metal Parasitics\n\
  ####################\n\
<rbl>\t" << rbl << "\n\
<cbl>\t" << cbl << "\n\
<cwl>\t" << cwl << "\n\
<char>\t0\n";

    // Copy Bitcell dimensions
    // TODO - Needs to be generic
    // Check which test needs that info
    // Add BC info to the template
    ifstream bc("../configuration/bitcellSizes.m");
    techTemplate << "\
#############################\n\
# Bitcell device dimensions\n\
#############################\n";
    if(!bc.is_open()) {
        cerr << "Error: Can't open bitcellSizes.m file.\n";
        exit(1);
    }
    while(!bc.eof()) {
        string line;
        getline(bc,line);
        //skip empty lines
        if(line.empty()) {
            continue;
        }
        regex_t re;
        size_t     nmatch = 3;
        regmatch_t pmatch[3];
        char *pattern = "^[ \t]*([^ \t]+)[ \t]*=[ \t]*([^ \t]+)[ \t]*;";
        regcomp(&re, pattern, REG_EXTENDED);
        int status = regexec(&re, line.c_str(), nmatch, pmatch, 0);
        regfree(&re);
        if(status!=0) {
            cerr << "Error: Can't Parse bitcell file.\n" << endl;
            exit(1);
        }
        string token = line.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        string value = line.substr(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
        techTemplate << "<" << token << ">" << "\t" << value << endl;
    }
    bc.close();
    //cout << "TEMPLATE \n" << techTemplate.str().c_str() << endl;
}

void RegFile::rmPrevResults() {
    // Delete previous results
    system("rm -fr ../results_v2");
    // Create new folder if it doesn't exist
    system("mkdir ../results_v2");
    // delete TASE results
    stringstream rmPreResults;
    rmPreResults << "rm -fr " << inp.TASEpath << "/device/BIN/" << getenv("USER") << "*" << endl;
    system(rmPreResults.str().c_str());
}

void RegFile::simulate(string tests) {
    test2run = tests;
    // Run all tests
    if(test2run.empty()) {
        SA.simulate(techTemplate.str());
        RD.simulate(techTemplate.str());
        //CM.simulate(techTemplate.str());
        BC.simulate(techTemplate.str());
        TB.simulate(techTemplate.str());
        //WD.simulate(techTemplate.str());
        ioDFF.simulate(techTemplate.str());
        BM.simulate(techTemplate.str());
        return;
    }
    // Run specified test2run
    if(test2run == "SA") {
        SA.simulate(techTemplate.str());
    }
    else if(test2run == "RD") {
        RD.simulate(techTemplate.str());
    }
    else if(test2run == "CM") {
        CM.simulate(techTemplate.str());
    }
    else if(test2run == "BC") {
        BC.simulate(techTemplate.str());
    }
    else if(test2run == "TB") {
        TB.simulate(techTemplate.str());
    }
    else if(test2run == "WD") {
        WD.simulate(techTemplate.str());
    }
    else if(test2run == "ioDFF") {
        ioDFF.simulate(techTemplate.str());
    }
    else if(test2run == "BM") {
        BM.simulate(techTemplate.str());
    }
    else {
    }
}

void RegFile::extractOutput() {
    if(test2run.empty()) {
        SA.extractOutput();
        RD.extractOutput();
        //CM.extractOutput();
        BC.extractOutput();
        TB.extractOutput();
        //WD.extractOutput();
        ioDFF.extractOutput();
        BM.extractOutput();
    }
    else if(test2run == "SA") {
        string filename = "Subtest_" + test2run + ".log";
        //ofstream ofile(filename.c_str());
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        float SAenergy, SAdelay;
        SA.extractOutput();
        getSAED(SAenergy, SAdelay);
        ofile << "SAenergy: " << SAenergy << endl;
        ofile << "SAdelay: " << SAdelay << endl;
        ofile << endl;
        ofile.close();
    }
    else if(test2run == "RD") {
        string filename = "Subtest_" + test2run + ".log";
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        float RDenergy, RDdelay;
        getRDED(RDenergy, RDdelay);
        ofile << "RDenergy: " << RDenergy << endl;
        ofile << "RDdelay: " << RDdelay << endl;
        ofile.close();
    }
    else if(test2run == "BC") {
        string filename = "Subtest_" + test2run + ".log";
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        float BCenergy, BCdelay;
        getBCED(BCenergy, BCdelay);
        ofile << "BCenergy: " << BCenergy << endl;
        ofile << "BCdelay: " << BCdelay << endl;
        ofile.close();
    }
    else if(test2run == "TB") {
        string filename = "Subtest_" + test2run + ".log";
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        float TBenergy;
        getTBED(TBenergy);
        ofile << "TBenergy: " << TBenergy << endl;
        ofile.close();
    }
    else if(test2run == "ioDFF") {
        string filename = "Subtest_" + test2run + ".log";
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        float DFFenergy, DFFdelay;
        getDFFED(DFFenergy, DFFdelay);
        ofile << "DFFenergy: " << DFFenergy << endl;
        ofile << "DFFdelay: " << DFFdelay << endl;
        ofile.close();
    }
    else if(test2run == "BM") {
        string filename = "Subtest_" + test2run + ".log";
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        float BMenergy, BMdelay;
        getBMED(BMenergy, BMdelay);
        ofile << "BMenergy: " << BMenergy << endl;
        ofile << "BMdelay: " << BMdelay << endl;
        ofile.close();
    }
    else {
    }
}

void RegFile::print() {
    cout << "start printing" << endl;
    SA.print();
    RD.print();
    //CM.print();
    BC.print();
    TB.print();
    //WD.print();
    ioDFF.print();
    BM.print();
    cout << "printing done" << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////
// read delay calculation
// stage 1- input data latching in DFF
// stage 2- row decoding
// stage 3- max(WL pulse width required to write bitcell+SA delay, bank select signal delay)
// stage 4- precharging BL back to VDD (this can be done in parallel with stage 2)
// stage 5- data sent from bank select to DFF +output data latching in DFF
////////////////////////////////////////////////////////////////////////////////////////////
void RegFile::calculateReadED(float& read_energy, float& read_delay) {
    // pch_r - due to driving bitlines back to .95*VDD after read (current default)
    float energy_pch_r = BC.getPCREnergy();
    float delay_pch_r = BC.getPCRDelay();

    // read- ED to discharge bitline to VDD-<BL_DIFF> (set in user.m)
    float energy_bitcell_r = BC.getBCREnergy();
    float delay_bitcell_r = BC.getBCRDelay();

    // only for read- sense amp resolution time, bank muxing to output DFF
    float energy_SA = SA.getSAEnergy();
    float delay_SA = SA.getSADelay();
    float energy_sa_inter = SA.getIntEnergy();
    float delay_sa_inter = SA.getIntDelay();

    // delay bankSelect is the propagation delay on the bank select signal, this occurs in parallel of the WL pulse+SA resolution
    // delay_bankOutput is the progation delay through the bank mux to the output DFF
    float delay_bankMux = BM.getBankMuxtDelay();
    float delay_bm_inter = BM.getIntDelay();
    float energy_bankMux = BM.getBankMuxtEnergy();
    float energy_bm_inter = BM.getIntEnergy();

    // read- ED of address inputs latching + output data latching
    float energy_DFF_r = ioDFF.getRenergy();
    float delay_DFF = ioDFF.getRdelay();

    // ED of row decoder/wordline driver
    float energy_rowDecoder = RD.getEnergy();
    float delay_rowDecoder = RD.getDelay();

    // total leakage power from bitcells- converted later to leakage energy by multiplying by Tmin
    float leakage_power = BC.getBCLeakage();

    // ED for timing block
    float energy_timing = TB.getEnergy();

    // Bit-cell interconnect delay
    float energy_inter_bc_read = BC.getIntREenergy();
    float energy_inter_bc_write = BC.getIntWEenergy();
    float delay_inter_bc_read = BC.getIntRDelay();
    float delay_inter_bc_write = BC.getIntWDelay();



    // Move leakage to staticE
    // removed the last delay_DFF from read delay for the output data DFF delay should not be included.
    //read_delay=delay_DFF+max(delay_pch_r+delay_inter_bc_read,delay_rowDecoder+delay_bm_inter)+max(delay_bitcell_r+delay_SA+delay_sa_inter,delay_bm_inter)+delay_bankMux+delay_DFF;
    //Reduced the energy of bm buffer by a half
    read_delay=delay_DFF+max(delay_pch_r+delay_inter_bc_read,delay_rowDecoder+delay_bm_inter) +
               max(delay_bitcell_r+delay_SA+delay_sa_inter,delay_bm_inter)+delay_bankMux;
    read_energy=leakage_power*read_delay+energy_timing+energy_DFF_r+energy_rowDecoder+energy_pch_r+
                energy_bitcell_r+energy_bankMux+energy_SA+energy_sa_inter+(energy_bm_inter)+energy_inter_bc_read;

    struct stat buf;
    string filename = "readDelay.log";
    if (stat(filename.c_str(), &buf) != -1)
    {
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        ofile << delay_DFF << ";";
        ofile << delay_pch_r + delay_inter_bc_read << ";";
        ofile << delay_rowDecoder + delay_bm_inter << ";";
        ofile << delay_bitcell_r + delay_SA + delay_sa_inter << ";";
        ofile << delay_bm_inter << ";";
        ofile << delay_bankMux << endl;
        ofile.close();
    }
    else
    {
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        ofile << "delay_DFF;";
        ofile << "delay_pch_r + delay_inter_bc_read;";
        ofile << "delay_rowDecoder + delay_bm_inter;";
        ofile << "delay_bitcell_r + delay_SA + delay_sa_inter;";
        ofile << "delay_bm_inter;";
        ofile << "delay_bankMux;" << endl;
        ofile << delay_DFF << ";";
        ofile << delay_pch_r + delay_inter_bc_read << ";";
        ofile << delay_rowDecoder + delay_bm_inter << ";";
        ofile << delay_bitcell_r + delay_SA + delay_sa_inter << ";";
        ofile << delay_bm_inter << ";";
        ofile << delay_bankMux << endl;
        ofile.close();
    }

    struct stat buf2;
    string filename2 = "readEnergy.log";
    if (stat(filename2.c_str(), &buf2) != -1)
    {
        ofstream ofile(filename2.c_str(), ofstream::out | ofstream::app);
        ofile << leakage_power * read_delay << ";";
        ofile << energy_timing << ";";
        ofile << energy_DFF_r << ";";
        ofile << energy_rowDecoder << ";";
        ofile << energy_pch_r << ";";
        ofile << energy_bitcell_r << ";";
        ofile << energy_bankMux << ";";
        ofile << energy_SA << ";";
        ofile << energy_sa_inter  << ";";
        ofile << energy_bm_inter << ";";
        ofile << energy_inter_bc_read << endl;
        ofile.close();
    }
    else
    {
        ofstream ofile(filename2.c_str(), ofstream::out | ofstream::app);
        ofile << "leakage_power * read_delay;";
        ofile << "energy_timing;";
        ofile << "energy_DFF_r;";
        ofile << "energy_rowDecoder;";
        ofile << "energy_pch_r;";
        ofile << "energy_bitcell_r;";
        ofile << "energy_bankMux;";
        ofile << "energy_SA;";
        ofile << "energy_sa_inter;";
        ofile << "energy_bm_inter;";
        ofile << "energy_inter_bc_read;" << endl;
        ofile << leakage_power * read_delay << ";";
        ofile << energy_timing << ";";
        ofile << energy_DFF_r << ";";
        ofile << energy_rowDecoder << ";";
        ofile << energy_pch_r << ";";
        ofile << energy_bitcell_r << ";";
        ofile << energy_bankMux << ";";
        ofile << energy_SA << ";";
        ofile << energy_sa_inter  << ";";
        ofile << energy_bm_inter << ";";
        ofile << energy_inter_bc_read << endl;
        ofile.close();
    }

    #ifdef DEBUG
    cout << "RowDecoder " << energy_rowDecoder <<
          "DFF" <<  energy_DFF_r <<
          "PCH" << energy_pch_r <<
          "sa" << energy_SA <<
          "Bitcell" << energy_bitcell_r <<
          "Timing" << energy_timing <<
          "Leakage" << (leakage_power*read_delay) <<
          "BankMux" << energy_bankMux <<
          "inter" << (energy_sa_inter+(2*energy_bm_inter)+energy_inter_bc_read) << endl;

    cout << "read_delay = " << read_delay << "  read_energy = " << read_energy << endl;
    #endif
}

//////////////////////////////////////////////////////////////////////////////////////////
// write delay calculation
// stage 1- input data latching in DFF
// stage 2- row decoding, column/bank decoding, write BL droop (this calculation has 2 parts, the delay of the actual write driver pulling the BL low, and the propgation delay of the data from the DFF to the write driver)
// stage 3- WL pulse width required to write bitcell
// stage 4- precharging BL back to VDD (this can be done in parallel with stage 2)
//////////////////////////////////////////////////////////////////////////////////////////
void RegFile::calculateWriteED(float& write_energy, float& write_delay) {

    // pch_w - driving bitlines back to .95*VDD after write (current default)
    float energy_pch_w = BC.getPCWEnergy();
    float delay_pch_w = BC.getPCWDelay();

    // write- ED to flip bitcell
    float energy_bitcell_w = BC.getBCWEnergy();
    float delay_bitcell_w = BC.getBCWDelay();

    // only for write- delay measured driving bitline to VDD*.05
    float energy_writeDriver = BC.getWDEnergy();
    float delay_writeDriver = BC.getWDDelay();

    // write- ED of data inputs
    float delay_DFF = ioDFF.getRdelay();
    float energy_DFF_w = ioDFF.getWenergy();
    float delay_DFF_w = ioDFF.getWdelay();

    // ED of row decoder/wordline driver
    float energy_rowDecoder = RD.getEnergy();
    float delay_rowDecoder = RD.getDelay();

    // total leakage power from bitcells- converted later to leakage energy by multiplying by Tmin
    float leakage_power = BC.getBCLeakage();

    // ED for timing block
    float energy_timing = TB.getEnergy();

    // Bank Mux
    float delay_bankMux = BM.getBankMuxtDelay();
    float delay_bm_inter = BM.getIntDelay();
    float energy_bankMux = BM.getBankMuxtEnergy();
    float energy_bm_inter = BM.getIntEnergy();

    // Bit-cell interconnect delay
    float energy_inter_bc_read = BC.getIntREenergy();
    float energy_inter_bc_write = BC.getIntWEenergy();
    float delay_inter_bc_read = BC.getIntRDelay();
    float delay_inter_bc_write = BC.getIntWDelay();
//Reduced energy of BM buffer by a half
    write_delay = delay_DFF+max(max(delay_inter_bc_read+delay_pch_w,delay_rowDecoder+delay_bm_inter),(max(delay_DFF_w-delay_DFF,delay_inter_bc_write)+delay_writeDriver))
                  +delay_bitcell_w;
    write_energy = leakage_power*write_delay+energy_timing+energy_DFF_w+energy_rowDecoder+
                   energy_writeDriver+energy_pch_w+energy_bitcell_w+energy_inter_bc_write+energy_bm_inter+energy_inter_bc_read;

    struct stat buf;
    string filename = "writeDelay.log";
    if (stat(filename.c_str(), &buf) != -1)
    {
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        ofile << delay_DFF << ";";
        ofile << delay_inter_bc_read + delay_pch_w << ";";
        ofile << delay_rowDecoder + delay_bm_inter << ";";
        ofile << delay_DFF_w - delay_DFF << ";";
        ofile << delay_inter_bc_write << ";";
        ofile << delay_writeDriver << ";";
        ofile << delay_bitcell_w << endl;
        ofile.close();
    }
    else
    {
        ofstream ofile(filename.c_str(), ofstream::out | ofstream::app);
        ofile << "delay_DFF;";
        ofile << "delay_inter_bc_read + delay_pch_w;";
        ofile << "delay_rowDecoder + delay_bm_inter;";
        ofile << "delay_DFF_w - delay_DFF;";
        ofile << "delay_inter_bc_write;";
        ofile << "delay_writeDriver;";
        ofile << "delay_bitcell_w;" << endl;
        ofile << delay_DFF << ";";
        ofile << delay_inter_bc_read + delay_pch_w << ";";
        ofile << delay_rowDecoder + delay_bm_inter << ";";
        ofile << delay_DFF_w - delay_DFF << ";";
        ofile << delay_inter_bc_write << ";";
        ofile << delay_writeDriver << ";";
        ofile << delay_bitcell_w << endl;
        ofile.close();
    }

    struct stat buf2;
    string filename2 = "writeEnergy.log";
    if (stat(filename2.c_str(), &buf2) != -1)
    {
        ofstream ofile(filename2.c_str(), ofstream::out | ofstream::app);
        ofile << leakage_power * write_delay << ";";
        ofile << energy_timing << ";";
        ofile << energy_DFF_w << ";";
        ofile << energy_rowDecoder << ";";
        ofile << energy_writeDriver << ";";
        ofile << energy_pch_w << ";";
        ofile << energy_bitcell_w << ";";
        ofile << energy_inter_bc_write << ";";
        ofile << energy_bm_inter << ";";
        ofile << energy_inter_bc_read << endl;
        ofile.close();
    }
    else
    {
        ofstream ofile(filename2.c_str(), ofstream::out | ofstream::app);
        ofile << "leakage_power * read_delay;";
        ofile << "energy_timing;";
        ofile << "energy_DFF_w;";
        ofile << "energy_rowDecoder;";
        ofile << "energy_writeDriver;";
        ofile << "energy_pch_w;";
        ofile << "energy_bitcell_w;";
        ofile << "energy_inter_bc_write;";
        ofile << "energy_bm_inter;";
        ofile << "energy_inter_bc_read;" << endl;
        ofile << leakage_power * write_delay << ";";
        ofile << energy_timing << ";";
        ofile << energy_DFF_w << ";";
        ofile << energy_rowDecoder << ";";
        ofile << energy_writeDriver << ";";
        ofile << energy_pch_w << ";";
        ofile << energy_bitcell_w << ";";
        ofile << energy_inter_bc_write << ";";
        ofile << energy_bm_inter << ";";
        ofile << energy_inter_bc_read << endl;
        ofile.close();
    }
}


void RegFile::getSAED(float& SAenergy, float& SAdelay) {
    float SA_E = SA.getSAEnergy();
    float SA_D = SA.getSADelay();

    float INT_E = SA.getIntEnergy();
    float INT_D = SA.getIntDelay();

    SAenergy = SA_E + INT_E;
    SAdelay = SA_D + INT_D;
}

void RegFile::getRDED(float& RDenergy, float& RDdelay) {
    RDenergy = RD.getEnergy();
    RDdelay = RD.getDelay();
}

void RegFile::getBCED(float& BCenergy, float& BCdelay) {
    // pch_w - driving bitlines back to .95*VDD after write (current default)
    float energy_pch_w = BC.getPCWEnergy();
    float delay_pch_w = BC.getPCWDelay();

    // write- ED to flip bitcell
    float energy_bitcell_w = BC.getBCWEnergy();
    float delay_bitcell_w = BC.getBCWDelay();

    // only for write- delay measured driving bitline to VDD*.05
    float energy_writeDriver = BC.getWDEnergy();
    float delay_writeDriver = BC.getWDDelay();

    // total leakage power from bitcells- converted later to leakage energy by multiplying by Tmin
    float leakage_power = BC.getBCLeakage();

    // Bit-cell interconnect delay
    float energy_inter_bc_read = BC.getIntREenergy();
    float energy_inter_bc_write = BC.getIntWEenergy();
    float delay_inter_bc_read = BC.getIntRDelay();
    float delay_inter_bc_write = BC.getIntWDelay();

    // need to figure out a new equation
    BCenergy = -1;
    BCdelay = -1;
}

void RegFile::getTBED(float& TBenergy) {
    TBenergy = TB.getEnergy();
}

void RegFile::getDFFED(float& DFFenergy, float& DFFdelay) {
    float Rdelay = ioDFF.getRdelay();
    float Renergy = ioDFF.getRenergy();

    float Wdelay = ioDFF.getWdelay();
    float Wenergy = ioDFF.getWenergy();

    DFFdelay = Rdelay + Wdelay;
    DFFenergy = Renergy + Wenergy;
}

void RegFile::getBMED(float& BMenergy, float& BMdelay) {
    float BM_D = BM.getBankMuxtDelay();
    float BM_E = BM.getBankMuxtEnergy();

    float INT_D = BM.getIntDelay();
    float INT_E = BM.getIntEnergy();

    BMdelay = BM_D + INT_D ;
    BMenergy = BM_E + INT_E;
}

void RegFile::runTASE(string tempPath) {
    stringstream sst;
    cout << inp.TASEpath << endl;
    //sst << "perl " << inp.TASEpath << "/device/BIN/run.pl -i " << tempPath;
    sst << "perl " << inp.TASEpath << "/device/BIN/run.pl -i " << tempPath << " -nopdf";
    system(sst.str().c_str());
}
