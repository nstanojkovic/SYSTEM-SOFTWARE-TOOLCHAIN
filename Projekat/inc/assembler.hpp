
#include<iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <regex>
#include <iomanip>
#include<vector>
#include <algorithm>

#include <exception>
using namespace std;



    //status -0
    //handler -1
    //cause -2


struct ElemRelTable{

  int dection_id;
  string  offset;
  string relocationType;
  int number;
  string addend;

};

struct ElemData{
  int offset;
  string value;
  string description;
  
};

struct ElemSymTable
{

 int num;
 int value;
 int size;
 int location_counter;
 string type;
 string bind;
 string ndx;
 string name;


};

struct Literal{
  string flag;
  int size;//mozda nam je potrebno da znamo i velicinu
  string value;

};
struct ElemSecTable
{

  int section_id;


  int location_counter;

  int size;

  string name;

  vector<Literal>poolLiteral;//jednostavno ovde cemo da cuvamo za datu sekciju
  
  vector<ElemRelTable>RelocationTable;

  vector<ElemData>Data;///to ce da mi bude neki sadrzaj koji cu da prebacim u masinski kod
 
};


struct SymTable{
  vector<ElemSymTable>listSymTable;
};

struct SectionTable{
  vector<ElemSecTable>listSectionTable;

};

 class Assembler{


private:

  SectionTable sectionTable;
  SymTable symTable;
  ifstream inputFile;
  ofstream outputFile;


public:



  bool openFiles(string inputFileString, string outputFileString);

  bool openFileInput(string inputFileString);

  bool closeFileInput();
  
  static bool isOkInput(string options,string inputFile,string outputFile);

  int firstPass();
  int secondPass();

  void calculateSize();

  bool readDirective(); //ovo ce da sluzi da vidimo da li je u pitanju direktiva
  bool readLabel();//ovo ce da sluzi da procitamo labelu
  bool readCommand(string line); //ovo ce da sluzi da vidimo da li je u pitanju komanda
  bool readCommand2Pass(string line); //ovo ce da sluzi da vidimo da li je u pitanju komanda
  bool readComment();

  void addElementSymTab(int offset, string type, string bind, string ndx, string name);
  void addElementSecTab(string name);



  void printSymTable(string s);
  void printSecTable();
  void printMachineCod();
  void printRelTable();

  bool checkInSection();

  bool writeWord(string line);
   bool writeSkip(string line);


  void defineSymbol(string name);

  bool checkInSysTableDef(string name);


  bool checkInSysTableUnDef(string name);

  ElemSecTable currentSection;
  
  int numberOfSymbols=0;
  int numberOfSection=0;


  string AfterDollar(string input);


  
  string decimalToString(int pr);
   string  decimalToHexadecimal( string decimal);
unsigned int stringToHexaDecimal(string hexString);
string hexaDecimalToString(unsigned int v);


   string removeSpacesAndCommas( string input);
   string removeSpacesAndCommasAndR( string input);
  
   bool functionLiteral(string s);
   bool checkLiteralInPool(string s);

   void setLiteralValue();


   void write_two_two(string str);

   void addDispatchToLiteral(string& povratna,string s,bool brackets);
   void addDispatchToSymbol(string s,bool word);

   vector<string> registar_regex(string line);
   vector<string> csr_regex(string line);

   string hexToLittleEndian(string s);

   void changeNdx();

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




//string operand="^\\s*([a-zA-Z_$][a-zA-Z_$0-9]*|[0-9]*|0[xX][0-9a-fA-F]*|\\$\\+[0-9]*|\\$\\+0[xX][0-9a-fA-F]*)$";

string operand="([a-zA-Z_$][a-zA-Z_$0-9]*|[0-9]*|0[xX][0-9a-fA-F]*|\\$\\+[0-9]*|\\$\\+0[xX][0-9a-fA-F]*)";
string gpr="%(r10|r11|r12|r13|r14|sp|r15|pc|r0|r1|r2|r3|r4|r5|r6|r7|r8|r9)";


string operandWithBrackets = "\\[" + gpr + "\\s*\\+\\s*" + operand + "\\]";

//string operandWithBrackets = "\\[" + gpr + "(?:\\s*\\+\\s*" + operand1 + ")?\\]";
string bracketsString=("\\[" + gpr + "\\]");

regex bracketsRegex ("\\[" + gpr + "\\]");

/////bitno
string operandLdSt="("+operand+"|"+operandWithBrackets+"|"+bracketsString+")";


regex operandWithBracketsRegex(operandWithBrackets);


string csr="%(status|handler|cause)";

regex commentRegex("^\\s*#|\\s+#");

regex externRegex(R"(\.extern\s+([a-zA-Z_]\w*(?:\s*,\s*[a-zA-Z_]\w*)*))");

regex globalRegex(R"(\.global\s+([a-zA-Z_]\w*(?:\s*,\s*[a-zA-Z_]\w*)*))");


regex sectionRegex(R"(\.section\s+([a-zA-Z_]\w*))");

//regex wordRegex(R"(\.word\s+("+operand+"*(?:\s*,\s*"+operand+")*))");

//regex wordRegex(R"(\.word\s+(?:(0x[0-9A-Fa-f]+)|(\d+)|([a-zA-Z_]\w*))(\s*,\s*(?:(0x[0-9A-Fa-f]+)|(\d+)|([a-zA-Z_]\w*)))*\s*)");
regex wordRegex(R"(\.word\s+(?:(0x[0-9A-Fa-f]+)|(\d+)|([a-zA-Z_$]\w*))(\s*,\s*(?:(0x[0-9A-Fa-f]+)|(\d+)|([a-zA-Z_$]\w*)))*\s*)");




regex endRegex(R"(\.end)");//to je da zavrsim end to mi za to sluzi

regex symbolRegex(R"(\w+:)");


regex skipRegex("\\s*.skip\\s*"+operand+"+");
//////////////////////
//////////////////////////
regex haltRegex("\\s*halt\\s*");
regex intRegex("\\s*int\\s*");
regex iretRegex("\\s*iret\\s*");
regex retRegex("\\s*ret\\s*");


regex callRegex("\\s*call\\s*"+operand+"+");
regex jmpRegex("\\s*jmp\\s*"+operand+"+");


regex beqRegex("\\s*beq\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*"+operand+"\\s*");

regex bneRegex("\\s*bne\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*"+operand+"\\s*");

regex bgtRegex("\\s*bgt\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*"+operand+"\\s*");

regex pushRegex("\\s*push\\s*"+gpr);

regex popRegex("\\s*pop\\s*"+gpr);

regex xchgRegex("\\s*xchg\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex addRegex("\\s*add\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex subRegex("\\s*sub\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex mulRegex("\\s*mul\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex divRegex("\\s*div\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex notRegex("\\s*not\\s*"+gpr);

regex andRegex("\\s*and\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex orRegex("\\s*or\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex xorRegex("\\s*xor\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex shlRegex("\\s*shl\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex shrRegex("\\s*shr\\s*"+gpr+"\\s*,\\s*"+gpr+"\\s*");

regex ldRegex("\\s*ld\\s*"+operandLdSt+"\\s*,\\s*"+gpr+"\\s*");
regex stRegex("\\s*st\\s*"+gpr+"\\s*,\\s*"+operandLdSt+"\\s*");




regex csrrdRegex("\\s*csrrd\\s*"+csr+"\\s*,\\s*"+gpr+"\\s*");

regex csrwrRegex("\\s*csrwr\\s*"+gpr+"\\s*,\\s*"+csr+"\\s*");





regex literalRegex(R"((?:^|\s|,|$)(\$?(?:0x[0-9a-fA-F]+|\d+))(?:,|$|]))");
//regex symbolRegex1(R"((?:^|\s|,|$)(\$?[a-zA-Z]+)(?:,|$))");

regex symbolRegex1(R"((?:^|\s|,|$)(\$?[a-zA-Z_][a-zA-Z_\d]*)(?:,|$))");

regex hexRegex(R"(0x([0-9A-Fa-f]+))");
regex hex1Regex(R"(([0-9A-Fa-f]+))");


//regex registarRegex("%r(.+?)(?=%r|$)|%sp|%pc");

regex csrRegex("\\s*"+csr+"\\s*");

regex registarRegex("\\s*"+gpr+"\\s*");

regex emptyRegex("^\\s*$");




