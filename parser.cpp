#include "parser.h"
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <regex.h>
#include <fstream>
#include <sstream>

parser::parser() {
}

parser::~parser() {
}

string parser::openFile(char* file) {
    ifstream handle(file);
    if (!handle.is_open()) {
        cerr << "Error: Can't open input file " << file << endl;
        exit(1);
    }
    stringstream sst;
    sst << handle.rdbuf();
    handle.close();
    return sst.str();
}

int parser::parseFile(char* file) {
    string str = openFile(file);
    // delete comments in user.m
    consumeComments(str);
    // delete empty lines in user.m
    consumeNewLines(str);
    int count = 0;
    ofstream han("output.txt");
    while(!str.empty()) {
        ++count;
        han << count << " - " << str.empty() << endl;
        han << str << "----\n" << endl;
        parseToken(str);
    }
    han.close();
    return 0;
}

void parser::consumeComments(string& str) {
        // find begin of comment till new line
        size_t index1 = str.find('%');
        if(index1 != string::npos) {
            size_t index2 = str.find('\n',index1);
            str.erase(index1,index2-index1+1);
            // recursive call
            consumeComments(str);
        }
}

void parser::consumeNewLines(string& str) {
    // find new line
    size_t index = str.find('\n');
    if(index != string::npos) {
        const char* ch = " ";
        str.replace(index,1,ch);
        consumeNewLines(str);
    }
}

int parser::parseToken(string& str) {
    string cmd = checkCmd(str);
    if (!strcmp(cmd.c_str(), "SWEEP")) {
        cout << "SWEEP: " << endl;
        parseSWEEPToken(str);
    }
    else if (!strcmp(cmd.c_str(), "OPTIMIZE")) {
        parseOPTIMIZEToken(str);
    }
    else {
        cout << "UserToken: " << str.c_str() << endl;
        parseUserToken(str);
    }
}

int parser::parseUserToken(string& str) {
    regex_t re;
    size_t     nmatch = 3;
    regmatch_t pmatch[3];

    char *pattern = "^[ \t]*([^ \t]+)[ \t]*=[ \t]*([^ \t]+)[ \t]*;[ \t]*";
    if(regcomp(&re, pattern, REG_EXTENDED) != 0) {
       cerr << "Error: Can't Parse Token at \n" << str << endl;
       exit(1);
    }
    int status = regexec(&re, str.c_str(), nmatch, pmatch, 0);
    regfree(&re);

    if(status!=0) {
        cerr << "Error: Can't Parse Token at \n" << str << endl;
        exit(1);
    }
    string token = str.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
    string value = str.substr(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
    int ivalue = atoi(value.c_str());
    float fvalue = atof(value.c_str());
    if(token == "technology") {
        input_handle.setTech(value);
    } else if (token == "memSize") {
        input_handle.setMemSize(ivalue);
    } else if (token ==  "rows") {
        input_handle.setNRows(ivalue);
    } else if (token ==  "banks" ) {
        input_handle.setNBanks(ivalue);
    } else if (token ==  "colMux") {
        input_handle.setNColMux(ivalue);
    } else if (token ==  "wordSize") {
        input_handle.setWordSize(ivalue);
    } else if (token ==  "SAoffset") {
        input_handle.setSAOffset(fvalue);
    } else if (token ==  "height") {
        input_handle.setBCHeight(fvalue);
    } else if (token ==  "width") {
        input_handle.setBCWidth(fvalue);
    } else if (token ==  "energyConstraint" ) {
        input_handle.setENConstraint(fvalue);
    } else if (token ==  "delayConstraint") {
        input_handle.setDLConstraint(fvalue);
    } else if (token ==  "wnio") {
        input_handle.setWDWidth(ivalue);
    } else if (token ==  "tasePath" ) {
        input_handle.setTASEPath(value);
    } else if (token ==  "temp" ) {
        input_handle.setTemp(fvalue);
    } else if (token ==  "vdd" ) {
        input_handle.setVdd(fvalue);
    } else if (token ==  "WLBoost" ) {
        input_handle.setWLBoost(fvalue);
    } else if (token ==  "WLoffset" ) {
        input_handle.setWLOffset(fvalue);
    } else {
        cerr << "Error: Can't Parse Token at \n" << str << endl;
        exit(1);
    }
    str.erase(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);
}

int parser::parseSWEEPToken(string& str) {
    cout << str << endl;
    regex_t re;
    size_t nmatch = 6;
    regmatch_t pmatch[6];
    // Modify to check for numbers
    char *pattern = "^[ \t]*SWEEP[ \t]*([^ \t]+)[ \t]*([^ \t]+)[ \t]*([^ \t]+)[ \t]*([^ \t]+)[ \t]*([^ \t]+)[ \t]*";
    regcomp(&re, pattern, REG_EXTENDED);
    int status = regexec(&re, str.c_str(), nmatch, pmatch, 0);
    regfree(&re);
    if(status != 0) {
        cerr << "Error: Can't Parse SWEEP Token" << endl;
        exit(1);
    }
    input_handle.sweepToken = str.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
    input_handle.sweepBegin = atof(str.substr(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so).c_str());
    input_handle.sweepEnd = atof(str.substr(pmatch[3].rm_so, pmatch[3].rm_eo - pmatch[3].rm_so).c_str());
    input_handle.sweepStep = atof(str.substr(pmatch[4].rm_so, pmatch[4].rm_eo - pmatch[4].rm_so).c_str());
    input_handle.sweepOutput = str.substr(pmatch[5].rm_so, pmatch[5].rm_eo - pmatch[5].rm_so);
    str.erase(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);
    // No need for returning int
    return 0;
}

int parser::parseOPTIMIZEToken(string& str) {
    regex_t re;
    size_t nmatch = 3;
    regmatch_t pmatch[3];
    // Modify to check for numbers
    char *pattern = "^[ \t]*OPTIMIZE[ \t]*OBJECTIVE(.*)[ \t]*KNOBS(.*)[ \t]*";
    regcomp(&re, pattern, REG_EXTENDED);
    int status = regexec(&re, str.c_str(), nmatch, pmatch, 0);
    regfree(&re);
    if(status != 0) {
        cerr << "Error: Can't Parse OPTIMIZE Token" << endl;
        exit(1);
    }
    string obj = str.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
    string knobs = str.substr(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
    parseKNOBS(knobs);

    str.erase(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);
    cout << "OBJ = " << obj << "KNOB = " << knobs << endl;
    // No need for returning int
    return 0;
}

int parser::parseKNOBS(string& str) {
    while(!str.empty()) {
        regex_t re;
        size_t nmatch = 6;
        regmatch_t pmatch[6];
        // Modify to check for numbers
        char *pattern = "^[ \t]*([^ \t]+)[ \t]*([0-9]+(\\.[0-9]+)?)?[ \t]*([0-9]+(\\.[0-9]+)?)?[ \t]*";
        regcomp(&re, pattern, REG_EXTENDED);
        int status = regexec(&re, str.c_str(), nmatch, pmatch, 0);
        regfree(&re);
        if(status != 0) {
            cerr << "Error: Can't Parse OPTIMIZE Token" << endl;
            exit(1);
        }
        if(
            (pmatch[2].rm_so == -1 && pmatch[4].rm_so != -1)
        ||  (pmatch[4].rm_so == -1 && pmatch[2].rm_so != -1)
          )
        {
            cerr << "Error: Can't Parse OPTIMIZE Token." << endl;
            cerr << "Format: OPTIMIZE KNOBS KONB1 [MIN MAX] KNOB2 [MIN MAX] .." << endl;
            exit(1);
        }
        // store min max
        int count = input_handle.knobCount++;
        input_handle.knobMin[count] = -1;
        input_handle.knobMax[count] = -1;
        input_handle.knobName[count] = str.substr(pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
        if(pmatch[2].rm_so != -1) {
            input_handle.knobMin[count] = atof((str.substr(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so)).c_str());
            input_handle.knobMax[count] = atof((str.substr(pmatch[4].rm_so, pmatch[4].rm_eo - pmatch[4].rm_so)).c_str());
            cout << "knob = " << input_handle.knobName[count] << " min = " << input_handle.knobMin[count] << " max = " << input_handle.knobMax[count] << endl;
        }
        str.erase(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so);

        // Error if Min > Max
        float min = input_handle.knobMin[count];
        float max = input_handle.knobMax[count];
        if(min > max)
          cerr << "Maximum value of " << input_handle.knobName[count] << " is greater than its minimum value." << endl;

        // Limit the value of bank, rows, col
        if(input_handle.knobName[count] == "NBANKS") {
            if(max > 32 || min < 1) {
              cerr << "Number of banks should be between 1 and 16" << endl;;
              exit(1);
            }
        } else if (input_handle.knobName[count] == "NCOLS") {
            if(max > 8 || min < 1) {
              cerr << "Number of columns should be between 1 and 8" << endl;;
              exit(1);
            }
        } else if (input_handle.knobName[count] == "NROWS") {
            if(max > 512 || min < 32) {
              cerr << "Number of rows should be between 32 and 512" << endl;;
              exit(1);
            }
        }
    }
    return 0;
}

string parser::checkCmd(string str) {
    char st[20];
    if(sscanf(str.c_str(),"SWEEP%s", st)) {
        return "SWEEP";
    }
    else if(sscanf(str.c_str(),"OPTIMIZE%s",st)) {
        return "OPTIMIZE";
    }
    else {
        return "";
    }
}

userInput parser::getInputHandle() {
    return input_handle;
}
