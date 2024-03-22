#include "../inc/linker.hpp"
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>


int main(int argc, char const *argv[])
{
 try {
  if(argc<4) {
    throw MyException("You have not entered all the elements in the terminal");
  }

  Linker linker;


if(linker.isOkInput(argc,argv))

  {
      for(int i=0;i<linker.inputFilesString.size();i++)
      {

          string s=linker.inputFilesString[i];

          if(linker.openInputFile(s)){

            int c=linker.readObjFile(s);

            
          }else{
            throw MyException("Bad input on the terminal");
          }

          if( !linker.closeInputFile())
            {throw MyException("You can not close input file");}


      }


      if(linker.getSizeSetU()!=0){
              throw MyException("Some symbol is not defined");
        }

       if(!linker.openOutputFile(false)){
                  throw MyException("I can not open output file");//nije  u r
       } 


      linker.newAddressSection();
      linker.tableSymbol(true);
      linker.machineCodeSections();

      linker.addSizeToPoolAndChangeAdress();
      linker.tableSymbol(false);
      linker.relocate();

      linker.machineCodeSections();
      


  }
  else{
          throw MyException("Bad input on the terminal");
    }


    linker.printSectionTable();
    linker.printSymbolTable();
    linker.printRelacationTables();
   linker.printMachineCode();

   linker.printMachineCodeFinal();
  
 }catch (const MyException& ex) {
        // Uhvatanje izuzetka i ispisivanje poruke
        std::cerr << "Exception caught " << ex.what() << std::endl;
    }

  return 0;


 }

 bool Linker::isOkInput(int argc, char const* argv[])
 {
   string s=argv[1];

    
  if(s!="-hex"){
    return false;
  }

  bool check=false;
  bool check1=false;

  for (int i = 1; i < argc; ++i) {
        string arg = argv[i];


          string string1="";
          string string2="";
          string string3="";


            size_t found_eq = arg.find("="); 
            size_t found_at = arg.find("@"); 

            if (found_eq != string::npos && found_at != string::npos) {
                size_t start_place = 0;
                size_t length_place = found_eq - start_place;
                string1 = arg.substr(start_place, length_place); 

                size_t start_data = found_eq + 1;
                size_t length_data = found_at - start_data;
                string2 = arg.substr(start_data, length_data); 

                size_t start_address = found_at + 1;
                string3 = arg.substr(start_address); 
                arg=string1;
            } else {
                string1 = arg;
            }

        

      if (arg == "-place" && i + 1 < argc) {
          Place p;
          p.name = string2;
          p.address = string3;
          places.push_back(p);
      } else if (arg.substr(arg.size() - 2) == ".o") {
          check1=true;
          inputFilesString.push_back(arg);
          
      } else if (arg == "-o" && i + 1 < argc) {
          outputFileString = argv[++i];
          check=true;
      }
  }

    if(check==false){
      return false;
    
    }
    if(check1==false){
      return false;
    }
    
  string ss="";
  for(int j=0;j<4;j++){
    ss.push_back(outputFileString[outputFileString.size()+j-4]);
  }
    if((ss!=".hex"))return false;


    return true;
 }


 bool Linker::openInputFile(string s)
 {
    this->inputFile.open(s, ios::in);

    if(!inputFile.is_open())return false;

    return true;
 }


 

 bool Linker::openOutputFile(bool p)
 {
      if(p)
      this->outputFile.open(outputFileString, ios::out | ios::trunc);

      else
      this->outputFile.open("helpLinker.hex", ios::out | ios::trunc);

      if( !this->outputFile.is_open())return false;

      return  true;
 }

 bool Linker::closeInputFile()
 {
    this->inputFile.close();

    if (!this->inputFile.is_open())
    {
          return true;
    }
    else
    {
          return false;
  }
 }


 int Linker::readObjFile(string file)
 {
    
      readSymbolTable(file);

      readMaschineCode(file);

      readRelocationTable(file);

  return 1;//to znaci da radi sasvim dobro valjda kod mene ce to da bude tako
 }

 void Linker::readSymbolTable(string file)
 {

     string line;
    smatch matches;

    bool check=false;


    while(getline(inputFile,line)){

      if(regex_search(line, matches, symTabRegex)){
          check=true;
          continue;
      }

      if(check){


          if(regex_search(line, matches, emptyRegex)){
                check=false;

                break;
          }

            vector<string>words;

              stringstream iss(line);
              string word;
              while(iss>>word){
                words.push_back(word);
              }
            if(words[0]!="0" && words[0]!="Num" && words[3]!="SCTN")
              {   Elem elem;

                      //elem.num=stoi(words[0]);
                      string tmp=decimalToHexadecimal(words[1]);
                      elem.value=stringToHexaDecimal(tmp);
                      elem.num=stoi(words[0]);
                      elem.size=words[2];
                      elem.type=words[3];
                      elem.bind=words[4];
                      elem.ndx=words[5];
                      elem.name=words[6];
                      elem.file=file;
                      words.clear();

                  
                    if(elem.ndx=="UND" && setU.find(elem.name)==setU.end()){

                        if(setD.find(elem.name)==setD.end())
                          this->setU.insert(elem.name);
                      }
                      else{
                          auto it=setD.find(elem.name);
                          if(it!=setD.end()){throw MyException("Some symbol is defined more than once");}//znaci da je neki simbol definisan vise puta}

                          setD.insert(elem.name);

                          auto it1=setU.find(elem.name);
                          if(it1!=setU.end()){setU.erase(it1);}


                          }
                      symTable.push_back(elem);
                }else if(words[3]=="SCTN"){
                  ElemSec elemSec;
                  elemSec.number=words[0];
                  elemSec.section=words[6];
                  elemSec.address=words[1];
                  elemSec.size=words[2];
                  elemSec.file=file;
                  elemSec.newAddress=-1;
                  

                  secTable.push_back(elemSec);

                }
          }
      }


 }

 void Linker::readMaschineCode(string file)
 {

   string line;
    smatch matches;
    bool check=false;
  int number=-1;
      MashineCode element;
      element.file=file;

    while(getline(inputFile,line)){
          if(regex_search(line, matches, machineCodeRegex)){
              check=true;
            
              continue;
          }
          else if(regex_search(line, matches, relTableRegex)){
            break;
          }

          if(check){

            if(regex_search(line, matches, emptyRegex)){
                check=false;
                allMashineCode.push_back(element);
                element.code="";
                element.section="";
                number=-1;
              
                
          }
          
          else
           { line=removeSpacesAndCommas(line);
            

              if(regex_search(line, matches, emptyRegex)){
                    check=false;
              }
              if(number==-1){
                element.section=line;
                number=0;
              }
              else{
              line=line.substr(5);

              element.code+=line;
              }
              
              }
              
        }

    }
    
 }

 void Linker::readRelocationTable(string file)
 {
  closeInputFile();
  openInputFile(file);

     string line;
    smatch matches;
    bool check=false;
    RelocationTable element;
    element.file=file;
    int number=-1;


    while(getline(inputFile,line)){
      if(regex_search(line, matches, relTableRegex)){
        string s = matches[1].str();
        element.section=s;
        check=true;
        continue;
        
      }

      if(check){
        if(regex_search(line, matches, emptyRegex)){
                    check=false;
                    number=-1;
                    
              }
        else{
          if(number==-1)number=0;
          else
     
          {    
            vector<string>words;

              stringstream iss(line);
              string word;
              while(iss>>word){
                words.push_back(word);
              }
              element.offset=words[0];
   
              element.RelocationType=words[1];
           
              element.number=stoi(words[2]);
              element.addend=words[3];
              for(int i=0;i<secTable.size();i++){
                      if(element.file==secTable[i].file && secTable[i].section==element.section){
                        secTable[i].allRelocationTable.push_back(element);
                      }
                    }

                  }
        }

      }
    }
   
   for(int i=0;i<secTable.size();i++){
    
        if(secTable[i].file==file){
            for(int j=0;j<secTable[i].allRelocationTable.size();j++){
              RelocationTable& element=secTable[i].allRelocationTable[j];
                if(element.addend!="0000" && element.RelocationType=="R_32"){
                 
                  
                  unsigned int pomeraj=stringToHexaDecimal(element.addend);

                  for(int k=0;k<symTable.size();k++){
                    if(symTable[k].bind=="LOC" && symTable[k].file==file && pomeraj==symTable[k].value
                    && symTable[k].type!="SCTN" && symTable[k].ndx==to_string(element.number)){
                         element.number=symTable[k].num;
                         element.addend="0000";
                          break;
                      
                    }
                  }
                }
            }

          
        }
   }


   //ovo mi je bitno za ndx
        for(int i=0;i<secTable.size();i++){
    for(int j=0;j<symTable.size();j++){
      if(secTable[i].number==symTable[j].ndx && secTable[i].file==symTable[j].file){
        symTable[j].ndx=secTable[i].section;
      }
    }
  }


 }

 int Linker::getSizeSetU()
 {

  return setU.size();

 }

 void Linker::newAddressSection()
 {

  //ovo radimo da jedna sekcija bude iza ddruge iste sekcije ako imaju isti naziv zbog toga to radimo
  //radimo da jedna sekcija bude iza drugee sekcije ako imaju isti naziv i to je to 
    for(int i=0;i<secTable.size()-1;i++){
      for(int j=i+1;j<secTable.size();j++){
        if(secTable[j].section==secTable[i].section){
          //treba da smestim odmah iza
          ElemSec elem=secTable[i+1];
          secTable[i+1]=secTable[j];
          secTable[j]=elem;
        }
      }
    }


    unsigned int max=0;
    

//ovo radimo da damo novu adresu i to ako je dato sa place 
//takodje izbacice gresku ako se javi neka greska
    if(places.size()==0){
      max=0;
     
      secTable[0].newAddress=0x0;
      for(int i=1;i<secTable.size();i++){
              string s1=decimalToHexadecimal(secTable[i-1].size);
              unsigned int size1=stringToHexaDecimal(s1);
              unsigned int adresaPrev=secTable[i-1].newAddress;
          
              secTable[i].newAddress=adresaPrev+size1;

          }
    }
    else{
      for(int i=0;i<places.size();i++){

          bool check=false;

            for(int j=0;j<secTable.size();j++){
              if(secTable[j].section==places[i].name){
                check=true;
                secTable[j].newAddress=stringToHexaDecimal(places[i].address.substr(2));

                unsigned int pr=secTable[j].newAddress;

                if(pr>max)max=pr;
                break;
              }
              
            }

           if(check==false)throw MyException("Command place is wrong");

         
      }      
       for(int i=0;i<secTable.size()-1;i++){
            for(int j=i+1;j<secTable.size();j++){
              if(secTable[i].newAddress!=-1 && secTable[j].newAddress!=-1){
                if(secTable[i].newAddress>secTable[j].newAddress){
                  ElemSec s=secTable[i];
                  secTable[i]=secTable[j];
                  secTable[j]=s;
                }

              }else if(secTable[i].newAddress==-1 && secTable[j].newAddress!=-1){
                ElemSec s=secTable[i];
                  secTable[i]=secTable[j];
                  secTable[j]=s;
              }
            }
          }


          if(overlapPlace())throw MyException("Error places overlap");


          for(int i=places.size();i<secTable.size();i++){
              string s1=decimalToHexadecimal(secTable[i-1].size);
              unsigned int size1=stringToHexaDecimal(s1);
              unsigned int adresaPrev=secTable[i-1].newAddress;

              secTable[i].newAddress=adresaPrev+size1;

          }

    }



    
 }

 void Linker::tableSymbol(bool cc)
 {
  
  //dodavanje sekcije na pocetak tabele simbola 
        vector<Elem>vektor=symTable;
        symTableNew.clear();
       // symTable.clear();

        for(int i=0;i<secTable.size();i++){
          ElemSec elem=secTable[i];
              bool check=false;
              for(int j=0;j<symTableNew.size();j++){
                if(symTableNew[j].name==elem.section && symTableNew[j].type=="SCTN"){
                  check=true;
          
                  int ss=stoi(symTableNew[j].size)+stoi(elem.size);
                  string s=to_string(ss);//VEC GOTOVO to_string pretvara int u string

                  symTableNew[j].size=s;
                  break;
                }
              }

          if(!check ){
            if(cc)
            secTableNew.push_back(elem);
            addElementSymTab(elem.newAddress,elem.size,"SCTN","GLOB","-1",elem.section,elem.file);
            
           }
      }

  //dodavanje ostalih simbola

    for(int i=0;i<vektor.size();i++){
      Elem elem=vektor[i];


     
      for(int j=0;j<secTable.size();j++){
          ElemSec elemSec=secTable[j];
          //resavanje adrese
          if(elemSec.section==elem.ndx &&elemSec.file==elem.file){
            //onda je to to nasao sam ga 
            elem.value=elemSec.newAddress+elem.value;
          }
       }
       
       for(int k=0;k<symTableNew.size();k++){
        if(symTableNew[k].name==elem.ndx){
          elem.ndx=symTableNew[k].ndx;
        }
       }
       if(elem.ndx!="UND" )
       addElementSymTab(elem.value,elem.size,elem.type,elem.bind,elem.ndx,elem.name,elem.file);
      
    }
        
      
 }

 void Linker::printSectionTable()
 {

  this->outputFile<< "#.section_tab" << std::endl;
    this->outputFile << left << setw(14) << "Section";
     this->outputFile<< left << setw(12) << "Size";
    this->outputFile<< left << setw(12) << "Addresss";
    this->outputFile<< left << setw(16) << "File" ;
    this->outputFile<< "NewAdsress:" << endl;
    
    
  
    for (int i = 0; i <secTable.size(); ++i)
    {
        const ElemSec& elem = secTable[i];
        this->outputFile<< left << setw(14) << elem.section;

        this->outputFile << left << setw(12) << elem.size;
    
        this->outputFile << left << setw(12) << elem.address;

        this->outputFile<< left << setw(16)<<elem.file;

        this->outputFile << left << setw(12) <<hexaDecimalToString( elem.newAddress);


       

        this->outputFile<< endl;


    }
        
      this->outputFile<<endl;
      this->outputFile<<endl;
        
  }

  void Linker::printSymbolTable()
  {
    

        this->outputFile<<"#.symtab" << std::endl;
        this->outputFile << left << setw(4) << "Num";
        this->outputFile<< left << setw(14) << "Value";
        this->outputFile << left << setw(9) << "Size";
        this->outputFile << left << setw(9) << "Type";
        this->outputFile<< left << setw(9) << "Bind";
        this->outputFile << left << setw(11) << "Ndx";
        this->outputFile << left << setw(15) << "File";
        this->outputFile<< "Name" << endl;



        for (int i = 0; i <symTableNew.size(); i++)
        {
            const Elem& elem = symTableNew[i];
            this->outputFile<< left << setw(4) <<to_string( elem.num);
            this->outputFile<< left << setw(14) << hexaDecimalToString(elem.value);
            this->outputFile << left << setw(9) << elem.size;
            this->outputFile << left << setw(9) << elem.type;
            this->outputFile << left << setw(9) << elem.bind;
            this->outputFile << left << setw(11) << elem.ndx;
             this->outputFile << left << setw(15) << elem.file;
            this->outputFile<< elem.name << endl;
        }

            this->outputFile<<endl;
            this->outputFile<<endl;

  }

  void Linker::printRelacationTables()
 /* {

    for(int i=0;i<secTable.size();i++){
        this->outputFile<<secTable[i].file<<" , "<<secTable[i].section<<endl;
        for(int j=0;j<secTable[i].allRelocationTable.size();j++){
          this->outputFile<<secTable[i].allRelocationTable[j].offset<<" "<<
          secTable[i].allRelocationTable[j].RelocationType<<" "<<
          to_string(secTable[i].allRelocationTable[j].number)<<" "<<
          secTable[i].allRelocationTable[j].addend<<" "<<endl;
        }
    }
  
  }*/

  {

  for (int i = 0; i <secTable.size(); ++i)
    {

        ElemSec& elem = secTable[i];

      
        this->outputFile<< "#.rel_table: ";
        this->outputFile<<"File:"<<secTable[i].file<<"  Section:"<<secTable[i].section<<endl;
        
    
        this->outputFile << setw(4)<< left<<"Offset ";
        this->outputFile<<setw(15)<< left<<"RelocationType ";
        this->outputFile    <<left<<"Number ";
        this->outputFile << "    Addend "<<endl;
         for(int j=0;j<elem.allRelocationTable.size();j++)
          {   
            RelocationTable& r=elem.allRelocationTable[j];
            
            
            this->outputFile<<right<<setw(4) <<r.offset<<"   ";
            this->outputFile<<setw(15)<<setfill(' ')<< left<<r.RelocationType<<"   ";
            this->outputFile <<setw(8)<<setfill(' ')<<left<<(float) r.number<< "   ";
            this->outputFile << right<< setw(4)<<setfill('0')<<r.addend<<endl;
      
           
        }
         this->outputFile<< endl;


    }
        
      this->outputFile<<endl;
      this->outputFile<<endl;
        
    }

    void Linker::printMachineCode()
    {

      for(int i=0;i<secTableNew.size();i++){

        ElemSec element=secTableNew[i];
        this->outputFile<< "#.machine_code: ";
        this->outputFile<<"  Section:"<<element.section<<endl;

        for(int j=0;j<allMashineCodeSections.size();j++){
          MashineCode m=allMashineCodeSections[j];
            if(m.section==element.section){
              unsigned int pr=element.newAddress;
            
              int size=m.code.size();
              string code1=m.code;

              int k=0;
                while(k<size){
                string code=code1;
                
                string s=code.substr(k,16);
                this->outputFile<<hex<<setw(8) << right<<std::setfill('0')<<pr<<": ";
                 write_two_two(s);
                
                 this->outputFile<<endl;
                 k+=16;
                 pr+=8;
              }
  

            }
        }
        this->outputFile<<endl;
        this->outputFile<<endl;
        
      }
    }

    void Linker::printMachineCodeFinal()
    {
      this->outputFile.close();
      openOutputFile(true);


      vector<MashineCode> tmp=allMashineCodeSections;
      allMashineCodeSections.clear();

      if(places.size()==0){
        int i=0;
        MashineCode m;
        m.file=tmp[0].file;
        m.section=tmp[0].section;
        m.code="";
        while(i<tmp.size()){
          m.code+=tmp[i].code;
          ++i;
          
        }
        allMashineCodeSections.push_back(m);
      }
      else{
        for(int j=0;j<places.size()-1;j++){
          allMashineCodeSections.push_back(tmp[j]);
        }
      MashineCode m;
        m=tmp[places.size()-1];

        
        for(int i=places.size();i<tmp.size();i++){
            m.code+=tmp[i].code;

        }
        allMashineCodeSections.push_back(m);

      }
int size=1;
  if(places.size()!=0)size=places.size();
    for(int i=0;i<size;i++){
        ElemSec& element=secTableNew[i];

            for(int j=0;j<allMashineCodeSections.size();j++){
              MashineCode m=allMashineCodeSections[j];
                if(m.section==element.section){
                  unsigned int pr=element.newAddress;
                
                  int size=m.code.size();
                  string code1=m.code;

                  int k=0;
                    while(k<size){
                    string code=code1;
                    
                    string s=code.substr(k,16);
                    this->outputFile<<hex<<setw(8) << right<<std::setfill('0')<<pr<<": ";
                    write_two_two(s);
                    
                    this->outputFile<<endl;
                    k+=16;
                    pr+=8;
                  }
      

                }
            }

          
    }

 this->outputFile.close();

      
    }

  unsigned int Linker::stringToHexaDecimal(string hexString)
  {
    unsigned int hexValue;
    stringstream ss;
    ss << hex << hexString;
    ss >> hexValue;


    return hexValue;
  }



string Linker::decimalToHexadecimal(string decimal)
{
     
          // Konverzija stringa u unsigned long long int
          stringstream iss(decimal);
          unsigned long long int decimalValue;
          iss >> decimalValue;

          // Konverzija u heksadecimalni format
          stringstream ss;
          ss << hex << decimalValue;
          
          string hexadecimal = ss.str();
          
          string t="";
          for (char& c : hexadecimal) {
              t.push_back(toupper(c));
          }


          return  t; 
      
}

string Linker::hexaDecimalToString(unsigned int v)
{
   
          stringstream stream;
          stream << hex <<std::setw(8) <<right<< std::setfill('0') << v; // Postavljamo format na heksadecimalni
          string hexString = stream.str();

          return hexString;
               
}

int Linker::hexDecimalToInt(string h)
{
   int i;
    stringstream ss;
    ss << std::hex << h;
    ss >> i;
    return i;
}

bool Linker::overlapPlace()
{
      for(int i=1;i<places.size();i++){
        if((secTable[i].newAddress)<=(secTable[i-1].newAddress+stringToHexaDecimal(secTable[i-1].size)))
          return true;
      }

          return false;
}

void Linker::addElementSymTab(unsigned int value, string size, string type, string bind, string ndx, string name, string file)
{

  Elem el;
  el.num=symTableNew.size()+1;
  el.value=value;
  el.size=size;
  el.type=type;
  el.bind=bind;
  if(ndx=="-1"){
    
   int  ss=symTableNew.size()+1;
   
   el.ndx=to_string(ss);
  }
  else
  el.ndx=ndx;


  el.name=name;
  el.file=file;
  symTableNew.push_back(el);//dodajemo na kraj nase tabele

}


string Linker::removeSpacesAndCommas(string input)
{
  string result;

    for (char ch : input) {
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != ',' ) {
            result += ch;
        }
    }

    return result;
}

void Linker:: write_two_two(string str) {
    for (size_t i = 0; i < str.length(); i += 2) {
        this->outputFile << str.substr(i, 2) << " ";
    }
}

void Linker::machineCodeSections()
{

  allMashineCodeSections.clear();//brisemo sve elemente na pocetku

  //prvo da sortiramo masinski kod po symbolTab
  vector<MashineCode>tmp;
  for(int i=0;i<secTable.size();i++){
    for(int j=0;j<allMashineCode.size();j++){
      if(allMashineCode[j].section==secTable[i].section && 
      allMashineCode[j].file==secTable[i].file){
        tmp.push_back(allMashineCode[j]);
      }
    }
  }

  allMashineCode=tmp;

  for(int i=0;i<secTableNew.size();i++){
    ElemSec element=secTableNew[i];


    MashineCode m;
    m.file="";
    m.section=element.section;
    m.code="";
    string poolLiteral="";
    for(int j=0;j<allMashineCode.size();j++){
      MashineCode m1=allMashineCode[j];

      if(m1.section==m.section){

        for(int p=0;p<secTable.size();p++){
          if(secTable[p].file==m1.file && secTable[p].section==m1.section){
            element=secTable[p];
          }
        }

        int tmp =stoi(element.size);

        poolLiteral+=m1.code.substr(tmp*2);
        m.code+=m1.code.substr(0,tmp*2);//bez bazena literala to dodajemo kasnije
      }
    }
    m.code+=poolLiteral;//dodajemo na kraju
    secTableNew[i].poolSize=poolLiteral.size()/2;
    allMashineCodeSections.push_back(m);
 
  }
}

void Linker::addSizeToPoolAndChangeAdress()
{

  //addSize to pool
  for(int i=0;i<secTableNew.size();i++){

     int br=0;

    for(int j=0;j<secTable.size();j++){
      ElemSec element=secTable[j];
      
      if(element.section==secTableNew[i].section){
       
          for(int s=0;s<element.allRelocationTable.size();s++){
            if(element.allRelocationTable[s].RelocationType=="R_32")br++;
          }
      }
    }
    secTableNew[i].poolSize+=br*4;//jer svaki simbol ce da mi zauzem 4 bajta
  }
  


  //change adress to sections
  for(int i=places.size()-1;i<secTableNew.size()-1;i++){
    string s=decimalToHexadecimal(to_string(secTableNew[i].poolSize));
    unsigned int brr=stringToHexaDecimal(s);

    
    int k=i+1;
    while(k<secTableNew.size()){
              secTableNew[k].newAddress+=brr;
              k++;
        }

    for(int j=0;j<secTable.size();j++){
      if(secTable[j].section==secTableNew[i+1].section
      && secTable[j].file==secTableNew[i+1].file){
        int k=j;
        while(k<secTable.size()){
              secTable[k].newAddress+=brr;
              k++;
        }
      }
    }

    for(int j=0;j<symTableNew.size();j++){
      if(symTableNew[j].name==secTableNew[i+1].section)
      symTableNew[j].value+=brr;
    }
  }
  
}

void Linker::relocate()
{
  //svaka sekcija secTable  sadrzi relocateTable ve vezane za tu sekciju i to maramo sada da menjamo 
  //prvo mogu da uradim apsolutno adresiranje sa simbolima tako sto vrednost simbola smestaamo na kraju svake sekcije
  for(int i=0;i<secTable.size();i++){
    ElemSec &elemSec=secTable[i];

    for(int j=0;j<elemSec.allRelocationTable.size();j++){
     int indexAllMashineCode=-1;
      string value="";
      int offset2;
      

        RelocationTable &rel=elemSec.allRelocationTable[j];


       int br=0;//bitno mi je za addend
        Elem elemSym;
        if(rel.RelocationType=="R_32" || rel.RelocationType=="R_WORD_32"){

           
              string symbol="";
              for(int k=0;k<symTable.size();k++){
                if(symTable[k].num==rel.number && symTable[k].file==elemSec.file)
                {
                  symbol=symTable[k].name;
                  break;
                }
              }

              for(int k=0;k<symTableNew.size();k++){
                if(symTableNew[k].name==symbol){
                  elemSym=symTableNew[k];
                  break;
                }
              }

              rel.number=elemSym.num;


              //znamo daa je  u pitanju simbol
              int symbolEnd=-1;
              for(int k=0;k<allMashineCode.size();k++){

                
                MashineCode &m=allMashineCode[k];
                if(m.file==elemSec.file && m.section==elemSec.section){
                  
                    if(rel.RelocationType!="R_WORD_32")
                            {    string hexString=hexaDecimalToString(elemSym.value);
                            
                            hexString=hexToLittleEndian(hexString);
                            br=m.code.size()/2;
                            
                            m.code+=hexString;
                            }
                    indexAllMashineCode=k;
                    break;

                }
              }


        }
        else if(rel.RelocationType=="R_PC_32"){

       
             for(int k=0;k<allMashineCode.size();k++){

                
                MashineCode &m=allMashineCode[k];
                if(m.file==elemSec.file && m.section==elemSec.section){
                  int h=0;

                  for(int f=0;f<symTableNew.size();f++){
                    if(symTableNew[f].name==elemSec.section){
                        h=symTableNew[f].num;
                        break;
                    }
                  }

                  rel.number=h;
                                     
                    indexAllMashineCode=k;
                    break;

                }
              }

        }


        

         //za addend potrebno je da ostale elemente SymTable idemo napred
              //i ako imamo nekog ispred samo povecamo za vrednost te sekcije koja mora da nosi isto ime kao i ova


            //azuriramo addend
            int addend=hexDecimalToInt(rel.addend);
            int offest1=hexDecimalToInt(rel.offset);
            offset2=offest1;

          

            offest1+=2;
            if(br>0)br=br-offest1;

                addend=br+addend;

                  for(int k=i+1;k<secTable.size();k++){
                    if(secTable[k].section==elemSec.section)
                      addend+=stoi(secTable[k].size);
                  }

                string s=decimalToHexadecimal(to_string(addend));
                string tmp="";
                for(int p=0;p<(4-s.size());p++){
                  tmp.push_back('0');
                }


              if(indexAllMashineCode!=-1){
                if(rel.RelocationType!="R_WORD_32")
                  changeDispatch(allMashineCode[indexAllMashineCode],offset2,s,false);
                else
                  changeDispatch(allMashineCode[indexAllMashineCode],offset2 ,hexaDecimalToString(elemSym.value),true);

              }
                


                  if(rel.RelocationType!="R_WORD_32")
                  rel.addend=tmp+s;

                  

            //azuriramo offset

                 int offset=hexDecimalToInt(rel.offset);
                    for(int k=i-1;k>=0;k--){
                      if(secTable[k].section==elemSec.section)
                        offset+=stoi(secTable[k].size);
                    }

                     s=decimalToHexadecimal(to_string(offset));
                      tmp="";
                      for(int p=0;p<(4-s.size());p++){
                        tmp.push_back('0');
                      }
                    rel.offset=tmp+s;

    }
  }
}
void Linker::changeDispatch(MashineCode& m,int offset,string value,bool word)
{

if(word){
   offset=offset*2;

  for(int i=0;i<8;i++){
    m.code[offset+i]=value[i];
  }


}

else
 { string tmp="";
  for(int i=0;i<(3-value.size());i++)
    {
      tmp.push_back('0');
    }
    value=tmp+value;

    offset=offset*2;

    m.code[offset+1]=value[0];
    m.code[offset+2]=value[1];
    m.code[offset+3]=value[2];}
}


string Linker::hexToLittleEndian(string s)
{
     string ss="";
     for(int i=7;i>=0;i-=2){
      
        for(int k=1;k>=0;k--){
          ss.push_back(s[i-k]);
        }
      
     }
     return ss;
}