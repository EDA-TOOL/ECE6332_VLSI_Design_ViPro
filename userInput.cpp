#include "userInput.h"
#include <iostream>

userInput::userInput() {
	//Intialize partameters
	//Can u intialize with struct
	knobCount = 0;
	PCratio = -1;
	WLratio = -1;
	calNumBanks = false;
	calNumColMux = false;
	n_banks = -1;
	n_colMux = -1;
	n_rows = -1;
	WLBoost = 0;
	WLOffset = 0;
}

userInput::~userInput() {
}

void userInput::print() {
	cout <<  "\nPRINTING USER INPUT INFO" << "\n"
		 "technology: " << technology << "\n"
	     "memory_size: " << memory_size << "\n"
		 "n_banks: " << n_banks << "\n"
		 "n_colMux: " << n_colMux << "\n"
		 "n_rows: " << n_rows << "\n"
		 "word_size: " << word_size << "\n"
		 "SAoffset: " << SAoffset << "\n"
		 "BCheight: " << BCheight << "\n"
		 "BCwidth: " << BCwidth << "\n"
		 "energy_constraint: " << energy_constraint << "\n"
		 "delay_constraint: " << delay_constraint << "\n"
		 "WDwidth: "  << WDwidth << "\n"
		 "TASEpath: " << TASEpath << "\n"
		 "sweepToken: " <<  sweepToken << "\n"
		 "sweepBegin: " << sweepBegin << "\n"
		 "sweepEnd: "  << sweepEnd << "\n"
		 "sweepStep: " << sweepStep << "\n"
		 "sweepOutput: " << sweepOutput << "\n"
		 "calNumBanks: " << calNumBanks << "\n"
		 "calNumColMux: " << calNumColMux << endl;

		 for(int i = 0; i < knobCount; ++i) {
		 	cout << "knobName: " << knobName[i] << ", Min: " << knobMin[i] << ", Max: " << knobMax[i] << endl;
		 }
}

void userInput::setTech(string tech) {
	technology = tech;
}

void userInput::setMemSize(int memSize) {
	memory_size = memSize;
}

void userInput::setNBanks(int nBanks) {
	n_banks = nBanks;
}

void userInput::setNColMux(int nColMux) {
	n_colMux = nColMux;
}

void userInput::setNRows(int nRows) {
	n_rows = nRows;
}

void userInput::setWordSize(int wordSize) {
	word_size = wordSize;
}

void userInput::setSAOffset(float SAOffset) {
	SAoffset = SAOffset;
}

void userInput::setBCHeight(float BCHeight) {
	BCheight  = BCHeight;
}

void userInput::setBCWidth(float BCWidth) {
	BCwidth  = BCWidth;
}

void userInput::setENConstraint(float ENConst) {
	energy_constraint = ENConst;
}

void userInput::setDLConstraint(float DLConst) {
	delay_constraint = DLConst;
}

void userInput::setWDWidth(int WDWidth) {
	WDwidth = WDWidth;
}

void userInput::setTASEPath(string path) {
	TASEpath = path;
}

void userInput::setTemp(float temperature) {
	temp = temperature;
}

void userInput::setVdd(float Vdd) {
	vdd = Vdd;
}

void userInput::setWLBoost(bool WLBoostFlag) {
	WLBoost = WLBoostFlag;
}

void userInput::setWLOffset(float offset) {
	WLOffset = offset;
}

string userInput::getTech() {
	return technology;
}

int userInput::getMemSize() {
	return memory_size;
}

int userInput::getNBanks() {
	return n_banks;
}

int userInput::getNColMux() {
	return n_colMux;
}

int userInput::getNRows() {
	return n_rows;
}

int userInput::getWordSize() {
	return word_size;
}

float userInput::getSAOffset() {
	return SAoffset;
}

float userInput::getBCHeight() {
	return BCheight;
}

float userInput::getBCWidth() {
	return  BCwidth;
}

float userInput::getENConstraint() {
	return  energy_constraint;
}

float userInput::getDLConstraint() {
	return delay_constraint;
}

float userInput::getWDWidth() {
	return  WDwidth;
}

string userInput::getTASEPath() {
	return  TASEpath;
}

float userInput::getTemp() {
	return  temp;
}

float userInput::getVdd() {
	return vdd;
}

bool userInput::getWLBoost() {
	return  WLBoost;
}

float userInput::getWLOffset() {
	return WLOffset;
}
