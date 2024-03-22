#include<iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <regex>
#include <iomanip>
#include<vector>
#include<set>
#include<list>

#include <exception>
using namespace std;


struct ElemMemory{
  string address;
  string value;  
};




class Emulator{

private:
  ifstream inputFile;

  vector<ElemMemory>memory;
  
  vector<string>registar;
  vector<string>registarCsr;
  int stackSize=0;


public :



bool isOkInput(string inputFileString);
bool openFileInput(string inputFileString);
bool closeFileInput();
void readMemory();

void implementCode();

bool readCommand(string s);


unsigned int stringToHexaDecimal(string hexString);
string hexaDecimalToString(unsigned int v);
string removeSpacesAndCommas( string input);

string operandInMemory(string s);
void storeInMemory(string a,string v);

void pop(int n);

void print();

string littleEndiantoString(string s);



};


class MyException : public std::exception {
public:
    MyException(const char* message) : msg(message) {}


    const char* what() const noexcept override {
        return msg;
    }

private:
    const char* msg;
};