#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//Functions

void readReg(const string fileName, int* regContList);
void readMem(const string fileName, int* MemContList);
int countInst(const string fileName);
void readInst(const string fileName, char** instList, int count);
int decodeOp(const string instStr);
int decodeFunct(const string instStr);
int decodeRs(const string instStr);
int decodeRt(const string instStr);
int decodeRd(const string instStr);
int decodeImmediate(const string instStr);
string convertToString(char** a, int row, int col);
string signExtension(const string str);
string twoComplement(const string str, int* flag);
string addBinary(string A, string B);
void processInst(int count, char** instList, int* regContList, int* MemContList, const string fileName);
void timingOutput_R_type(int& cycCount, int& instNum, const string fileName, const int funct);
void timingOutput_I_type(int& cycCount, int& instNum, const string fileName, const int opCode);
void print_Reg_Mem_cont(int* regContList, int* MemContList, int regCount, int MemCount, const string fileName);
void checkFileExist(const string fileName);

//

int main() {

	char userResponse = 'y';
	do {
		int count, regContList[32] = { 0 }, MemContList[250] = { 0 };
		char** instList = { 0 };
		string userInput_inputFile, userInput_outputFile;

		cout << "PLease enter the name of the input file:" << endl;
		cin >> userInput_inputFile;

		cout << "PLease enter the name of the output file:" << endl;
		cin >> userInput_outputFile;

		checkFileExist(userInput_outputFile);
		
		readReg(userInput_inputFile, regContList);
		readMem(userInput_inputFile, MemContList);
		count = countInst(userInput_inputFile);

		instList = new char* [count];
		for (int i = 0; i < count; i++) {
			instList[i] = new char[32];
		}

		readInst(userInput_inputFile, instList, count);
		processInst(count, instList, regContList, MemContList, userInput_outputFile);
		print_Reg_Mem_cont(regContList, MemContList, 32, 250, userInput_outputFile);

		for (int i = 0; i < count; i++) {
			delete[] instList[i];
		}
		delete[] instList;

		cout << "Would you like to continue with another round of simulation? (type y/n)" << endl;
		cin >> userResponse;

	} while (userResponse == 'y');

	return 0;
}

void readReg(const string fileName, int* regContList) {

	int index, targetFound = 0, length;
	string str, regNum, regNum_no_R, regCont;
	ifstream inFile;
	inFile.open(fileName, ios::in);

	if (inFile.is_open()) {
		while (!inFile.eof() && targetFound != 1) {
			inFile >> str;
			if (str.compare("REGISTERS") == 0) {
				inFile >> regNum;
				while (regNum.compare("MEMORY") != 0 && !inFile.eof()) {
					length = regNum.length();
					regNum_no_R = regNum.substr(1, length - 1);
					index = stoi(regNum_no_R);
					inFile >> regCont;
					regContList[index] = stoi(regCont);
					inFile >> regNum;
				}
				targetFound = 1;
			}
		}
	}
	else {
		cout << "Could not open the input file. Terminating the Program." << endl;
		exit(EXIT_FAILURE);
	}
	inFile.close();
}

void readMem(const string fileName, int* MemContList) {

	int index, targetFound = 0;
	string str, MemNum, MemCont;
	ifstream inFile;
	inFile.open(fileName, ios::in);

	if (inFile.is_open()) {
		while (!inFile.eof() && targetFound != 1) {
		    inFile >> str;
			if (str.compare("MEMORY") == 0) {
				inFile >> MemNum;
				while (MemNum.compare("CODE") != 0 && !inFile.eof()) {
					index = stoi(MemNum) / 4;
					inFile >> MemCont;
					MemContList[index] = stoi(MemCont);

					inFile >> MemNum;
				}
				targetFound = 1;
			}
		}
	}
	else {
		cout << "Could not open the input file. Terminating the Program." << endl;
		exit(EXIT_FAILURE);
	}
	inFile.close();
}

int countInst(const string fileName) {

	int count = 0, targetFound = 0;
	string str;
	ifstream inFile;
	inFile.open(fileName, ios::in);

	if (inFile.is_open()) {
		while (!inFile.eof() && targetFound != 1) {
			inFile >> str;
			if (str.compare("CODE") == 0) {
				while (!inFile.eof()) {
					inFile >> str;
					count++;
				}
				targetFound = 1;
			}
		}
	}
	else {
		cout << "Could not open the input file. Terminating the Program." << endl;
		exit(EXIT_FAILURE);
	}
	inFile.close();
	return count;
}

void readInst(const string fileName, char** instList, int count) {

	int instNum = 0, targetFound = 0;
	string str;
	ifstream inFile;
	inFile.open(fileName, ios::in);

	if (inFile.is_open()) {
		while (!inFile.eof() && targetFound != 1) {
			inFile >> str;
			if (str.compare("CODE") == 0) {
				while (!inFile.eof() && instNum < count) {
					inFile >> str;
					for (int j = 0; j < 32; j++) {
						instList[instNum][j] = str[j];
					}
					instNum++;
				}
				targetFound = 1;
			}
		}
	}
	else {
		cout << "Could not open the input file. Terminating the Program." << endl;
		exit(EXIT_FAILURE);
	}

	inFile.close();
}

int decodeOp(const string instStr) {

	int op_dec;
	op_dec = stoi(instStr.substr(0, 6), 0, 2);
	return op_dec;
}

int decodeFunct(const string instStr) {

	int funct_dec;
	funct_dec = stoi(instStr.substr(26, 6), 0, 2);
	return funct_dec;
}

int decodeRs(const string instStr) {

	int rs_dec;
	rs_dec = stoi(instStr.substr(6, 5), 0, 2);
	return rs_dec;
}

int decodeRt(const string instStr) {

	int rt_dec;
	rt_dec = stoi(instStr.substr(11, 5), 0, 2);
	return rt_dec;
}

int decodeRd(const string instStr) {

	int rd_dec;
	rd_dec = stoi(instStr.substr(16, 5), 0, 2);
	return rd_dec;
}

int decodeImmediate(const string instStr) {

	int immediate_dec, flagNegative = 0;
	int* flag = &flagNegative;
	string immediate, extImmediate, signExtImmediate;
	immediate = instStr.substr(16, 16);
	extImmediate = signExtension(immediate);
	signExtImmediate = twoComplement(extImmediate, flag);
	immediate_dec = stoi(signExtImmediate, 0, 2);
	if (*flag == 1) {
		immediate_dec = stoi(("-" + to_string(immediate_dec)));
	}
	return immediate_dec;
}

string convertToString(char** a, int row, int col) {
	string s = "";
	for (int j = 0; j < col; j++) {
		s = s + a[row][j];
	}
	return s;
}

string signExtension(const string str) {

	string extension, extStr;
	if ((int)(str[0] - '0') == 0) {
		extension = "0000000000000000";
	}
	else if ((int)(str[0] - '0') == 1) {
		extension = "1111111111111111";
	}
	extStr = extension + str;
	return extStr;
}

string twoComplement(const string str, int* flag) {

	int diff;
	string zeros = "0000000000000000", ones = "1111111111111111", padding;
	if (zeros.compare(str.substr(0, 16)) == 0) {
		return str;
	}
	else if (ones.compare(str.substr(0, 16)) == 0) {
		*flag = 1;
		string rightHalf = str.substr(16, 16);
		string result;
		for (int i = 0; i < 16; i++) {
			if ((int)(rightHalf[i] - '0') == 0) {
				rightHalf[i] = '1';
			}
			else if ((int)(rightHalf[i] - '0') == 1) {
				rightHalf[i] = '0';
			}
		}
		result = addBinary(rightHalf, "1");
		diff = 32 - result.length();
		for (int i = 0; i < diff; i++) {
			padding.push_back('0');
		}
		return padding + result;
	}
}

string addBinary(string A, string B) {
	// If the length of string A is greater than the length
	// of B then just swap the the string by calling the
	// same function and make sure to return the function
	// otherwise recursion will occur which leads to
	// calling the same function twice
	if (A.length() > B.length())
		return addBinary(B, A);

	// Calculating the difference between the length of the
	// two strings.
	int diff = B.length() - A.length();

	// Initialise the padding string which is used to store
	// zeroes that should be added as prefix to the string
	// which has length smaller than the other string.
	string padding;
	for (int i = 0; i < diff; i++)
		padding.push_back('0');

	A = padding + A;
	string res;
	char carry = '0';

	for (int i = A.length() - 1; i >= 0; i--) {
		// This if condition solves 110 111 possible cases
		if (A[i] == '1' && B[i] == '1') {
			if (carry == '1')
				res.push_back('1'), carry = '1';
			else
				res.push_back('0'), carry = '1';
		}
		// This if condition solves 000 001 possible cases
		else if (A[i] == '0' && B[i] == '0') {
			if (carry == '1')
				res.push_back('1'), carry = '0';
			else
				res.push_back('0'), carry = '0';
		}
		// This if condition solves 100 101 010 011 possible
		// cases
		else if (A[i] != B[i]) {
			if (carry == '1')
				res.push_back('0'), carry = '1';
			else
				res.push_back('1'), carry = '0';
		}
	}

	// If at the end their is carry then just add it to the
	// result
	if (carry == '1')
		res.push_back(carry);
	// reverse the result
	reverse(res.begin(), res.end());

	// To remove leading zeroes
	int index = 0;
	while (index + 1 < res.length() && res[index] == '0')
		index++;
	return (res.substr(index));
}

void processInst(int count, char** instList, int* regContList, int* MemContList, const string fileName) {

	int opCode, funct, rs, rt, rd, immediate, PC, cycCount, instNum;
	string instStr;

	cycCount = 0;
	instNum = 0;
	for (PC = 0; PC < count; PC++) {
		instNum++;
		instStr = convertToString(instList, PC, 32);
		opCode = decodeOp(instStr);
		if (opCode == 0) {
			rs = decodeRs(instStr);
			rt = decodeRt(instStr);
			rd = decodeRd(instStr);
			funct = decodeFunct(instStr);
			if (funct == 32) { //add
				regContList[rd] = regContList[rs] + regContList[rt];
			}
			if (funct == 34) { //sub
				regContList[rd] = regContList[rs] - regContList[rt];
			}
			if (funct == 42) { //slt
				if (regContList[rs] < regContList[rt]) {
					regContList[rd] = 1;
				}
				else {
					regContList[rd] = 0;
				}
			}
			timingOutput_R_type(cycCount, instNum, fileName, funct);
		}
		if (opCode == 4 || opCode == 5 || opCode == 8 || opCode == 35 || opCode == 43) {
			rs = decodeRs(instStr);
			rt = decodeRt(instStr);
			immediate = decodeImmediate(instStr);
			if (opCode == 4) { //beq
				if (regContList[rs] == regContList[rt]) {
					PC += immediate;
				}
			}
			if (opCode == 5) { //bne
				if (regContList[rs] != regContList[rt]) {
					PC += immediate;
				}
			}
			if (opCode == 8) { //addi
				regContList[rt] = regContList[rs] + immediate;
			}
			if (opCode == 35) { //lw
				regContList[rt] = MemContList[(regContList[rs] / 4) + (immediate / 4)];
			}
			if (opCode == 43) { //sw
				MemContList[(regContList[rs] / 4) + (immediate / 4)] = regContList[rt];
			}
			timingOutput_I_type(cycCount, instNum, fileName, opCode);
		}
	}
}

void timingOutput_R_type(int& cycCount, int& instNum, const string fileName, const int funct) {

	ofstream outFile;
	outFile.open(fileName, ios::out | ios::app);

	if (outFile.is_open()) {
		
		cycCount++;
		outFile << "C#" << cycCount << " I" << instNum << "-IF" << endl;
		cycCount++;
		outFile << "C#" << cycCount << " I" << instNum << "-ID" << endl;

		if (funct == 32 || funct == 34 || funct == 42) { //add or sub or stl

			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-EX" << endl;
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-WB" << endl;
		}
	}
	else {
		cout << "Could not open the output file. Terminating the Program." << endl;
		exit(EXIT_FAILURE);
	}
	outFile.close();
}

void timingOutput_I_type(int& cycCount, int& instNum, const string fileName, const int opCode) {

	ofstream outFile;
	outFile.open(fileName, ios::out | ios::app);

	if (outFile.is_open()) {

		cycCount++;
		outFile << "C#" << cycCount << " I" << instNum << "-IF" << endl;
		cycCount++;
		outFile << "C#" << cycCount << " I" << instNum << "-ID" << endl;

		if (opCode == 4 || opCode == 5) { //beq or bne
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-EX" << endl;
		}
		if (opCode == 8) { //addi
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-EX" << endl;
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-WB" << endl;
		}
		if (opCode == 35) { //lw
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-EX" << endl;
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-MEM" << endl;
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-WB" << endl;
		}
		if (opCode == 43) { //sw
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-EX" << endl;
			cycCount++;
			outFile << "C#" << cycCount << " I" << instNum << "-MEM" << endl;
		}
	}
	else {
		cout << "Could not open the output file. Terminating the Program." << endl;
		exit(EXIT_FAILURE);
	}
	outFile.close();
}

void print_Reg_Mem_cont(int* regContList, int* MemContList, int regCount, int MemCount, const string fileName) {

	ofstream outFile;
	outFile.open(fileName, ios::out | ios::app);

	if (outFile.is_open()) {
		outFile << "REGISTERS" << endl;
		for (int i = 0; i < regCount; i++) {

			if (regContList[i] != 0) {
				outFile << "R" << i << " " << regContList[i] << endl;
			}
		}
		outFile << "MEMORY" << endl;
		for (int i = 0; i < MemCount; i++) {

			if (MemContList[i] != 0) {
				outFile << i * 4 << " " << MemContList[i] << endl;
			}
		}
	}
	else {
		cout << "Could not open the output file. Terminating the Program." << endl;
		exit(EXIT_FAILURE);
	}
	outFile.close();
}

void checkFileExist(const string fileName) {

	int fileAlreadyExists = 0;
	fstream file;

	file.open(fileName, ios::in);
	if (file.is_open()) {
		fileAlreadyExists = 1;
		file.close();
	}

	if (fileAlreadyExists == 1) {
		file.open(fileName, ios::out | ios::trunc);
		file.close();
	}
}