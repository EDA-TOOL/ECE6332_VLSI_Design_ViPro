#include "RegFile.h"
#include "userInput.h"

RegFile::RegFile(userInput& uImp) {
    inp = uImp;
    cout << "Const2 Called" << endl;
    gateCap = 0;
    rbl = 0;
    cbl = 0;
    cwl = 0;
}

RegFile::RegFile() {
    cout << "Const Called" << endl;
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
    stringstream templ_path,new_templ_path;
    string tech = inp.technology;
    templ_path << tasePath << "/template/RVPtpl_" << tech << ".ini";
    new_templ_path << tasePath << "/template/RVPn_" << tech << ".ini";

    cout << templ_path.str().c_str() << endl;
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
    ifstream fileHandle ("../results_v2/GC/RVP_Gate_Capacitance/data.txt");
    if (fileHandle.is_open()) {
        stringstream st;
        st << fileHandle.rdbuf();
        gateCap = atof(st.str().c_str());
        //cout << "RVP_Gate opened" << endl;
        fileHandle.close();
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
    //cout << rbl << " " << cbl << " " << cwl << endl;
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

        } else if(line.find("<NR_sweep>") != string::npos) {
            techTemplate << "<NR_sweep>  " << inp.n_rows << endl;


        } else if(line.find("<addrCol>") != string::npos) {
            techTemplate << "<addrCol>  " << log2(inp.n_colMux) << endl;

        } else if(line.find("<numBanks>") != string::npos) {
            techTemplate << "<numBanks>  " << inp.n_banks << endl;

        } else if(line.find("<memsize>") != string::npos) {
            techTemplate << "<memsize>  " << inp.memory_size << endl;

        } else if(line.find("<ws>") != string::npos) {
            techTemplate << "<ws>  " << inp.word_size << endl;

        } else if(line.find("<NC_sweep>") != string::npos) {
            techTemplate << "<NC_sweep> " << inp.n_colMux * inp.word_size << endl;

        } else if(line.find("<colMux>") != string::npos) {
            techTemplate << "<colMux> " << inp.n_colMux << endl;

        } else if(line.find("<temp>") != string::npos) {
            techTemplate << "<temp> " << inp.temp << endl;

        } else if(line.find("<BL_DIFF>") != string::npos) {
            techTemplate << "<BL_DIFF> " << inp.SAoffset << endl;

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
    cout << "start simulation" << endl;
    // Run all tests
    if(tests.empty()) {
        SA.simulate(techTemplate.str());
        RD.simulate(techTemplate.str());
        CM.simulate(techTemplate.str());
        BC.simulate(techTemplate.str());
        TB.simulate(techTemplate.str());
        WD.simulate(techTemplate.str());
        ioDFF.simulate(techTemplate.str());
        BM.simulate(techTemplate.str());
        return;
    }
    // Run specified tests
    if(tests == "SA") {
        SA.simulate(techTemplate.str());
    }
    cout << "simulation done" << endl;
}

void RegFile::extractOutput() {
    cout << "start extracting output" << endl;
    SA.extractOutput();
    cout << "SA Output extracted" << endl;
    RD.extractOutput();
    cout << "RD Output extracted" << endl;
    //CM.extractOutput();
    BC.extractOutput();
    cout << "BC Output extracted" << endl;
    TB.extractOutput();
    cout << "TB Output extracted" << endl;
    //WD.extractOutput();
    ioDFF.extractOutput();
    cout << "ioDFF Output extracted" << endl;
    BM.extractOutput();
    cout << "BM Output extracted" << endl;
    cout << "output extraction done" << endl;
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
    read_delay=delay_DFF+max(delay_pch_r+delay_inter_bc_read,delay_rowDecoder+delay_bm_inter)+max(delay_bitcell_r+delay_SA+delay_sa_inter,delay_bm_inter)+delay_bankMux+delay_DFF;
    read_energy=leakage_power*read_delay+energy_timing+energy_DFF_r+energy_rowDecoder+energy_pch_r+energy_bitcell_r+energy_bankMux+energy_SA+energy_sa_inter+(2*energy_bm_inter)+energy_inter_bc_read;

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

    write_delay = delay_DFF+max(max(delay_inter_bc_read+delay_pch_w,delay_rowDecoder+delay_bm_inter),(max(delay_DFF_w-delay_DFF,delay_inter_bc_write)+delay_writeDriver))+delay_bitcell_w;
    write_energy = leakage_power*write_delay+energy_timing+energy_DFF_w+energy_rowDecoder+energy_writeDriver+energy_pch_w+energy_bitcell_w+energy_inter_bc_write+(2*energy_bm_inter)+energy_inter_bc_read;

    //cout << "write_delay = " << write_delay << " write_energy = " << write_energy << endl;
}

void RegFile::runTASE(string tempPath) {
    stringstream sst;
    cout << inp.TASEpath << endl;
    sst << "perl " << inp.TASEpath << "/device/BIN/run.pl -i " << tempPath;
    system(sst.str().c_str());
}
