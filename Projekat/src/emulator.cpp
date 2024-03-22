#include "../inc/emulator.hpp"
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>




int main(int argc, char const *argv[])
{

try{
    if(!argv[1]) {
        throw MyException("You have not entered all the elements in the terminal");
      }

      string inputFile = argv[1];
      Emulator emulator;
        
        if(!emulator.isOkInput(inputFile)){
          
          throw MyException("Bad input on the terminal");
        }
        if(!emulator.openFileInput(inputFile)){
          throw MyException("You can not open the input file");
        }

        emulator.readMemory();
        emulator.implementCode();




    if(!emulator.closeFileInput()){
          throw MyException("You can not close the input file");
        }

}catch (const MyException& ex) {
        // Uhvatanje izuzetka i ispisivanje poruke
        std::cerr << "Exception caught " << ex.what() << std::endl;
    }
  return 0;
}



bool Emulator::isOkInput(string inputFile)
{
     
     string s="";
     int i=3;
     while(i>=1){
        s.push_back(inputFile[inputFile.length() - i]);
        i--;
     }
  char secondLastCharacter = inputFile[inputFile.length() - 4];

   if(s=="hex" && secondLastCharacter=='.' ){
      return true;
    }else{
      return false;
    }
  return false;
}

bool Emulator::openFileInput(string inputFileString)
{
  this->inputFile.open(inputFileString, ios::in);
  

  if (!this->inputFile.is_open())
  {
     
        return false;
  }
  else
  {
     string s="00000000";
      for(int i=0;i<16;i++){
        registar.push_back(s);
      }

      for(int i=0;i<3;i++){
        registarCsr.push_back(s);
      }
        return true;
  }

  stackSize=0;
}

bool Emulator::closeFileInput()
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

void Emulator::readMemory()
{

  string line;
  while(getline(inputFile,line)){

 
    string adresa="";
    int i=0;
    while(line[i]!=':'){
      adresa.push_back(line[i]);
      i++;
    }

    i+=1;

    line=removeSpacesAndCommas(line);
    

     while(i<line.size()){
 
      ElemMemory mem;
      mem.value="";
      mem.address="";
      
    
      mem.value.push_back(line[i++]);
      
      mem.value.push_back(line[i++]);

    
      mem.address=adresa;

      unsigned int tmp=stringToHexaDecimal(adresa);
      tmp+=1;
      adresa=hexaDecimalToString(tmp);

      memory.push_back(mem);
      

    }

  }

 
}

void Emulator::implementCode()
{

  bool check=true;
  
  registar[15]=memory[0].address;


  while(check){
    string pc=registar[15];
    

    int index=-1;

    for(int i=0;i<memory.size();i++){
      if(pc==memory[i].address){
    
        index=i;
        break;

      }
    }
   //cout<<pc<<endl;  //ako zelimo da ipsiemo kako idu adrese
 
    
    if(index==-1){cout<<registar[15];throw MyException("PC address is not corect");}

    unsigned int tmp=stringToHexaDecimal(pc);
    tmp+=4;
    registar[15]=hexaDecimalToString(tmp);

    string instruction="";
    int k=0;
    while(k<=3){
      if((index+k)>=memory.size())throw MyException("Memory overflow occurred");
      instruction+=memory[index+k].value;
      ++k;
    }
   
   

    if(!readCommand(instruction))check=false;//halt


  }
}

bool Emulator::readCommand(string instruction)
{
  if(instruction=="00000000"){
    
   print();
    
    return false;}

  
    
  string tmp=instruction.substr(0,2);

  string dispatch="";
  dispatch.push_back(instruction[5]);
  dispatch.push_back(instruction[6]);
  dispatch.push_back(instruction[7]);

   string A="";
  A.push_back(instruction[2]);
  int indexA=stringToHexaDecimal(A);

  string B="";
  B.push_back(instruction[3]);
  int indexB=stringToHexaDecimal(B);

  string C="";
  C.push_back(instruction[4]);
  int indexC=stringToHexaDecimal(C);



//LD
  if (tmp=="91"){
    //MMMM==0b0001: gpr[A]<=gpr[B]+D;

          //znaci da nije slucaj [<reg>+ operand]
  
              { 

                unsigned int pc=stringToHexaDecimal(registar[indexB]);
                unsigned int pomeraj=stringToHexaDecimal(dispatch);
                pc=pc+pomeraj;


              string operand=this->operandInMemory(hexaDecimalToString(pc));

                registar[indexA]=operand;

                }

        


  }
  //LD
  else if(tmp=="92"){
        

        if(dispatch=="000" && indexB==0){
          //znaci da imamo [%r1]
                  

                  
                  string adresa=registar[indexC];

    
                  string operand=this->operandInMemory(adresa);

                  registar[indexA]=operand;
      

        }
        else if( indexC!=0){
        
                //znaci da imamo [%r1+operand] vrednost u memoriji reg +literal
                  
                  string adresa=registar[indexC];

                
                  unsigned int pr=stringToHexaDecimal(registar[indexC]);
                  unsigned int pr1=stringToHexaDecimal(dispatch);
                  unsigned int pr2=pr+pr1;


                  string operand=this->operandInMemory(hexaDecimalToString(pr2));

                  registar[indexA]=operand;
                
                
              }
        else{
                //onda je u pitanju adresa na kojoj se nalazi ili simbol ili literal
                //MMMM==0b0010: gpr[A]<=mem32[gpr[B]+gpr[C]+D];
                

                  unsigned int pc=stringToHexaDecimal(registar[indexB]);
                  unsigned int pomeraj=stringToHexaDecimal(dispatch);
                  pc=pc+pomeraj;


                  string operand=this->operandInMemory(hexaDecimalToString(pc));
                 // operand=this->operandInMemory(operand);

                  registar[indexA]=operand;
                 
            }

  }
  //ST
  else if(tmp=="80"){

       
        //MMMM==0b0000: mem32[gpr[A]+gpr[B]+D]<=gpr[C];

          //znaci da nije slucaj [<reg>+ operand] ili samo [reg]

          if(dispatch=="000"){
            //onda je u pitanju [%reg]
            string registarA=registar[indexA];
            string registarC=registar[indexC];

            storeInMemory(registarA,registarC);

          }
          else if (instruction[2]!='0'){
            //onda je u pitanju [%reg+literal]


            unsigned int d=stringToHexaDecimal(dispatch);
            unsigned int a=stringToHexaDecimal(registar[indexA]);
            d=d+a;

            string operand=this->operandInMemory(hexaDecimalToString(d));

            this->storeInMemory(operand,registar[indexC]);

          }
          else{
            //onda je st %r1, value5

            unsigned int pc=stringToHexaDecimal(registar[indexB]);
            unsigned int pomeraj=stringToHexaDecimal(dispatch);
            pc=pc+pomeraj;


            string operand=this->operandInMemory(hexaDecimalToString(pc));
       

            this->storeInMemory(operand,registar[indexC]);

          }
  
              
               

                
  }
  //PUSH
  else if(tmp=="81"){
    unsigned int pr=stringToHexaDecimal(dispatch);
    
    unsigned int registarA=stringToHexaDecimal(registar[indexA]);

    registarA=registarA-4;

    registar[indexA]=hexaDecimalToString(registarA);

    this->storeInMemory(registar[indexA],registar[indexC]);
    stackSize+=4;


  }
  //POP tu je i ret
  else if(tmp=="93"){


      string s=this->operandInMemory(registar[indexB]);


      registar[indexA]=s;
      

       unsigned int pr=stringToHexaDecimal(dispatch);
    
      unsigned int registarB=stringToHexaDecimal(registar[indexB]);
  
      

      registarB=registarB+pr;
      
      registar[indexB]=hexaDecimalToString(registarB);
      
      this->pop(pr);
      

  }
  //CALL
  else if(tmp=="20"){
    //MMMM==0b0000: push pc; pc<=gpr[A]+gpr[B]+D;

    unsigned int registarSP=stringToHexaDecimal(registar[14]);//ovo je sp

    registarSP=registarSP-4;

    registar[14]=hexaDecimalToString(registarSP);

    this->storeInMemory(registar[14],registar[15]);//registar 15 je pc

    unsigned int pc=stringToHexaDecimal(registar[indexA]);
    unsigned int pomeraj=stringToHexaDecimal(dispatch);
    pc=pc+pomeraj;


    string operand=this->operandInMemory(hexaDecimalToString(pc));

    registar[15]=operand;

    stackSize+=4;


  }
  //JMP
else if(tmp=="30"){
  //MMMM==0b0000: pc<=gpr[A]+D;

  unsigned int pc=stringToHexaDecimal(registar[indexA]);
  unsigned int pomeraj=stringToHexaDecimal(dispatch);
  pc=pc+pomeraj;


  string operand=this->operandInMemory(hexaDecimalToString(pc));

  registar[15]=operand;

}
//BEQ
else if (tmp=="31"){
  //MMMM==0b0001: if (gpr[B] == gpr[C]) pc<=gpr[A]+D;
  if(registar[indexB]==registar[indexC])
  {
    unsigned int pc=stringToHexaDecimal(registar[indexA]);
    unsigned int pomeraj=stringToHexaDecimal(dispatch);
    pc=pc+pomeraj;


    string operand=this->operandInMemory(hexaDecimalToString(pc));

    registar[15]=operand;
    }

}
//BNE
else if (tmp=="32"){
  //MMMM==0b0001: if (gpr[B] != gpr[C]) pc<=gpr[A]+D;
  if(registar[indexB]!=registar[indexC])
  {
    unsigned int pc=stringToHexaDecimal(registar[indexA]);
    unsigned int pomeraj=stringToHexaDecimal(dispatch);
    pc=pc+pomeraj;


    string operand=this->operandInMemory(hexaDecimalToString(pc));

    registar[15]=operand;
    }

}
//BGT
else if (tmp=="33"){
  //MMMM==0b0001: if (gpr[B] > gpr[C]) pc<=gpr[A]+D;

  if(stringToHexaDecimal(registar[indexB])>stringToHexaDecimal(registar[indexC]))
  {
    unsigned int pc=stringToHexaDecimal(registar[indexA]);
    unsigned int pomeraj=stringToHexaDecimal(dispatch);
    pc=pc+pomeraj;


    string operand=this->operandInMemory(hexaDecimalToString(pc));

    registar[15]=operand;
    }

}
//ADD
else if(tmp=="50"){
  //MMMM==0b0000: gpr[A]<=gpr[B] + gpr[C];
  unsigned int registarA=stringToHexaDecimal(registar[indexA]);
  unsigned int registarB=stringToHexaDecimal(registar[indexB]);
  unsigned int registarC=stringToHexaDecimal(registar[indexC]);

  registarA=registarB+registarC;
  registar[indexA]=hexaDecimalToString(registarA);

}
//SUB
else if(tmp=="51"){
  //MMMM==0b0000: gpr[A]<=gpr[B] - gpr[C];
  unsigned int registarA=stringToHexaDecimal(registar[indexA]);
  unsigned int registarB=stringToHexaDecimal(registar[indexB]);
  unsigned int registarC=stringToHexaDecimal(registar[indexC]);

  registarA=registarB-registarC;
  registar[indexA]=hexaDecimalToString(registarA);

}
//MUL
else if(tmp=="52"){
  //MMMM==0b0000: gpr[A]<=gpr[B] * gpr[C];
  unsigned int registarA=stringToHexaDecimal(registar[indexA]);
  unsigned int registarB=stringToHexaDecimal(registar[indexB]);
  unsigned int registarC=stringToHexaDecimal(registar[indexC]);

  registarA=registarB*registarC;
  registar[indexA]=hexaDecimalToString(registarA);

}
//DIV
else if(tmp=="53"){
  //MMMM==0b0000: gpr[A]<=gpr[B] / gpr[C];
  unsigned int registarA=stringToHexaDecimal(registar[indexA]);
  unsigned int registarB=stringToHexaDecimal(registar[indexB]);
  unsigned int registarC=stringToHexaDecimal(registar[indexC]);

  registarA=registarB/registarC;
  registar[indexA]=hexaDecimalToString(registarA);

}

//CSRWR
else if(tmp=="94"){
  //MMMM==0b0100: csr[A]<=gpr[B];
  registarCsr[indexA]=registar[indexB];//vrlo jednostavno

}

//CSRRD
else if(tmp=="90"){
  //MMMM==0b0000: gpr[A]<=csr[B];
  registar[indexA]=registarCsr[indexB];
}
else if(instruction=="10000000"){
    //INT softverski prekid

    //push status

          unsigned int registarA=stringToHexaDecimal(registar[14]);

          registarA=registarA-4;

          registar[14]=hexaDecimalToString(registarA);

          this->storeInMemory(registar[14],registarCsr[0]);

          stackSize+=4;

    //push pc
            
          registarA=stringToHexaDecimal(registar[14]);

          registarA=registarA-4;

          registar[14]=hexaDecimalToString(registarA);

          this->storeInMemory(registar[14],registar[15]);

          stackSize+=4;
    
    //cause<=4

          registarCsr[2]="00000004";

      
    //stattus<=status &(~0x1)
          unsigned int status=stringToHexaDecimal(registarCsr[0]);
          status=status &(~0x1);
          registarCsr[0]=hexaDecimalToString(status);

  //pc<=handle
          registar[15]=registarCsr[1];
    
  }
  //IRET ali prva instrukcijaa
  else if (tmp=="96"){
    //MMMM==0b0110: csr[A]<=mem32[gpr[B]+gpr[C]+D];
    //status=mem[sp+4];
    unsigned int sp=stringToHexaDecimal(registar[indexB]);//E indexB je 14
    sp=sp+4;
    registarCsr[0]=hexaDecimalToString(sp);
   


  }

//NOT
  else if (tmp=="60"){
    //MMMM==0b0000: gpr[A]<=~gpr[B];

    unsigned int registarA=stringToHexaDecimal(registar[indexA]);
    unsigned int registarB=stringToHexaDecimal(registar[indexB]);

    registarA=~registarB;
    registar[indexA]=hexaDecimalToString(registarA);


  }

//AND
  else if (tmp=="61"){
    //MMMM==0b0000: gpr[A]<=~gpr[B];

    unsigned int registarA=stringToHexaDecimal(registar[indexA]);
    unsigned int registarB=stringToHexaDecimal(registar[indexB]);
    unsigned int registarC=stringToHexaDecimal(registar[indexC]);

    registarA=registarB & registarC;
    registar[indexA]=hexaDecimalToString(registarA);


  }

  //OR
  else if (tmp=="62"){
    //MMMM==0b0000: gpr[A]<=~gpr[B];

    unsigned int registarA=stringToHexaDecimal(registar[indexA]);
    unsigned int registarB=stringToHexaDecimal(registar[indexB]);
    unsigned int registarC=stringToHexaDecimal(registar[indexC]);

    registarA=registarB | registarC;
    registar[indexA]=hexaDecimalToString(registarA);


  }

  //XOR
  else if (tmp=="63"){
    //MMMM==0b0000: gpr[A]<=~gpr[B];

    unsigned int registarA=stringToHexaDecimal(registar[indexA]);
    unsigned int registarB=stringToHexaDecimal(registar[indexB]);
    unsigned int registarC=stringToHexaDecimal(registar[indexC]);

    registarA=registarB ^ registarC;
    registar[indexA]=hexaDecimalToString(registarA);


  }

  //XCHG
  else if(tmp=="40"){
    string tmp=registar[indexB];
    registar[indexB]=registar[indexC];
    registar[indexC]=tmp;

  }

  //SHL
  else if(tmp=="70"){

    unsigned int registarA=stringToHexaDecimal(registar[indexA]);
    unsigned int registarB=stringToHexaDecimal(registar[indexB]);
    unsigned int registarC=stringToHexaDecimal(registar[indexC]);

    registarA=registarB << registarC;
    registar[indexA]=hexaDecimalToString(registarA);


  }
  //SHR
  else if(tmp=="71"){

    unsigned int registarA=stringToHexaDecimal(registar[indexA]);
    unsigned int registarB=stringToHexaDecimal(registar[indexB]);
    unsigned int registarC=stringToHexaDecimal(registar[indexC]);

    registarA=registarB >> registarC;
    registar[indexA]=hexaDecimalToString(registarA);

  }


  else throw MyException("Bad code for instruction");
  








  return true;
}

  unsigned int Emulator::stringToHexaDecimal(string hexString)
  {
    unsigned int hexValue;
    stringstream ss;
    ss << hex << hexString;
    ss >> hexValue;

    return hexValue;
  }

  string Emulator::hexaDecimalToString(unsigned int v)
{
   
          stringstream stream;
          stream << hex <<std::setw(8) <<right<< std::setfill('0') << v; // Postavljamo format na heksadecimalni
          string hexString = stream.str();

          return hexString;
               
}

string Emulator:: removeSpacesAndCommas(string input) {

    string result;

    for (char ch : input) {
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != ',' ) {
            result += ch;
        }
    }

    return result;
}

string Emulator::operandInMemory(string s)
{

  

   int ind=-1;
              for(int j=0;j<memory.size();j++){
                if(memory[j].address==s){
                  ind=j;
                  break;
                }
              }
              if(ind==-1)throw MyException("Can not find dispatch for address");

              int k=0;
      

              string operand="";
              while(k<=3){
                if((ind+k)>=memory.size())throw MyException("Memory overflow occurred");
                operand+=memory[ind+k].value;
                ++k;
              }

              operand=littleEndiantoString(operand);

              
      return operand;

}

void Emulator::storeInMemory(string a,string v)
{

  v=littleEndiantoString(v);

  int ind=-1;

  for(int i=0;i<memory.size();i++){
    if(memory[i].address==a){
      ind=i;
      break;
    }
  }

if(ind==-1){
    int k=0;

    while(k<=7){
      ElemMemory m;
      m.address=a;
      m.value="";
      m.value.push_back(v[k++]);
      m.value.push_back(v[k++]);
      memory.push_back(m);
      unsigned int pr=stringToHexaDecimal(a);
        pr=pr+1;
        a=hexaDecimalToString(pr);
        
    }

  }
  else{
    int k=0;
      for(int i=0;i<4;i++){
        memory[ind+i].value="";
        memory[ind+i].value.push_back(v[k++]);
        memory[ind+i].value.push_back(v[k++]);
      }
    }

}

void Emulator::pop(int n)
{

    if(stackSize>=n){stackSize-=n;}
    else{ stackSize-=n; cout<<stackSize<<": "<<n<<endl;
    throw MyException("Sory stack overflow");}

    int k=0;
 
    while(k<n){
              
                memory.pop_back();
                k++;
                
         }
  
}

void Emulator::print()
{

   /*  for(int i=0;i<memory.size();i++){
      cout<<memory[i].address<<": "<<memory[i].value<<endl;
     
    }
    */

    cout<<endl;
  
    cout<<"Emulated processor executed halt instruction"<<endl;
    cout<<"Emulated processor state:"<<endl;

    for(int j=0;j<16;j++){
       if(j%4==0 && j!=0)
      cout<<endl;
      cout<<"r"<<j<<": 0x"<<registar[j]<<"  ";
     
    }

    cout<<endl;
}



string Emulator::littleEndiantoString(string s)
{
     string ss="";
     for(int i=7;i>=0;i-=2){
      
        for(int k=1;k>=0;k--){
          ss.push_back(s[i-k]);
        }
      
     }
     return ss;
}


