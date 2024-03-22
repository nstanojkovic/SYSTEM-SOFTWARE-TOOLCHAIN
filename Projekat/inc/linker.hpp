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


//za .word bi trebalo da vodim racuna o relokacionom zapisu ali samo ako je simbol u drugom slucaju (za literal) nema potrebe 
////// to_string(int)   pretvaraa int u string 

////// to_string(int)   pretvaraa int u string 

string symbol(R"((?:^|\s|,|$)(\$?[a-zA-Z_][a-zA-Z_\d]*)(?:,|$))");
regex symTabRegex("\\s*SECOND PASS#.symtab\\s*");
regex machineCodeRegex("\\s*#.machice_code:\\s*");
regex relTableRegex("\\s*#.rel_table:\\s*"+symbol+"\\s*");
regex emptyRegex("^\\s*$");


struct Elem
{

 int num;
 unsigned int value;
 string size;
 string type;
 string bind;
 string ndx;
 string name;
 string file;


};

struct Place {
    string name;
    string address;
};

struct MashineCode{
    string file;
    string code;
    string section;
};

struct RelocationTable{
    
    string offset;
    string RelocationType;
    int number;
    string addend;
    string file;
    string section;
};

struct ElemSec{
string number;
string section;
string address;
string file;
unsigned int newAddress;
string size;
vector<RelocationTable>allRelocationTable;
int poolSize;

};




class Linker{


vector<Place> places;



ifstream inputFile;
ofstream outputFile;

string outputFileString;


set<string>setD;//skup simbola koji su definisani u prethodnim ulaznim fajlovima
set<string>setU;//nerazreseni simboli koji su jos bez definicije

vector<Elem>symTable;//ovo je stara tabela simbola sve zajedno
vector<Elem>symTableNew;//ovo je nova tabela simbola zajedno
vector<ElemSec>secTable;
vector<ElemSec>secTableNew;//ovo je bitno tu nastaju nakon spajanja
vector<MashineCode>allMashineCode;
vector<MashineCode>allMashineCodeSections;


public:

vector<string>inputFilesString;

bool isOkInput(int argc,char const* argv[]);
bool openInputFile(string s);
bool openOutputFile(bool p);


bool closeInputFile();

int readObjFile(string s);

void readSymbolTable(string s);

void readMaschineCode(string s);
void readRelocationTable(string s);

int getSizeSetU();

void newAddressSection();
void tableSymbol(bool cc);

void printSectionTable();
void printSymbolTable();
void printRelacationTables();
void printMachineCode();
void printMachineCodeFinal();
void write_two_two(string str);

void machineCodeSections();

void addSizeToPoolAndChangeAdress();

void relocate();

unsigned int stringToHexaDecimal(string hexString);
string  decimalToHexadecimal( string decimal);
string hexaDecimalToString(unsigned int v);
int hexDecimalToInt(string h);

bool overlapPlace();

void addElementSymTab(unsigned int value,string size, string type, string bind, string ndx, string name,string file);

void changeDispatch(MashineCode& s,int offset,string value,bool word);

 string removeSpacesAndCommas( string input);

 string hexToLittleEndian(string s);

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