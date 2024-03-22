#include "../inc/assembler.hpp"
#include <sstream>
#include <vector>
#include <iostream>
#include <cmath>

string Assembler:: removeSpacesAndCommas(string input) {
    string result;

    for (char ch : input) {
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != ',' ) {
            result += ch;
        }
    }

    return result;
}

string Assembler::removeSpacesAndCommasAndR(string input)
{
    string result;

    for (char ch : input) {
        if (ch != ' ' && ch != '\t' && ch != '\n' && ch != ',' && ch != 'r' && ch!= '%'){
            result += ch;
        }
    }

    return result;
}

bool Assembler::functionLiteral(string s)
{      

      for(int i=0;i<s.size();i++)
      if(s[i]==']' || s[i]=='[')return true;//dodao sam oco za zagrade
      bool provera=true;
      
       s=removeSpacesAndCommas(s);
       if(s[0]=='$')provera=false;
      
       string ss=AfterDollar(s);    


       string hex="";
        int count=-1;
    
        if(ss==""){
          ss=s;

        }

              if(ss[0]=='0' && (ss[1]=='x'|| ss[1]=='X')){

                 smatch matches;

                if (!regex_search(ss, matches, hexRegex)) 
                        return false;
               
                hex=ss.substr(2);
            
              }
            
              else{
                 
                 hex=decimalToHexadecimal(ss);
            
              }

              bool check=checkLiteralInPool(ss);
              if(check==true)return true;
              
              bool check1=false;
              for(int j=0;j<currentSection.poolLiteral.size();j++){
                string t=currentSection.poolLiteral[j].flag;
                if(currentSection.poolLiteral[j].flag==hex){
                  check1=true;
                  break;
                }
              }

              if(check1==false )
              { Literal l;

                l.flag=hex;
              
                currentSection.poolLiteral.push_back(l);}
              
            
            return true;
          
}

bool Assembler::checkLiteralInPool(string s)
{

    //ovde bi trebalo da vidimo da li se literal nalazi ili se ne nalazi 

    for(int i=0;i<currentSection.poolLiteral.size();i++){
      if(currentSection.poolLiteral[i].flag==s)return true;
    }
              return false;
}

void Assembler::setLiteralValue()
{

  //ovde cemo da dodamo vrednosti labele
  for(int i=0;i<sectionTable.listSectionTable.size();i++){
    
    int pr=sectionTable.listSectionTable[i].size;

    for(int j=0;j<sectionTable.listSectionTable[i].poolLiteral.size();j++){

      
          string t=decimalToString(pr);
            
          sectionTable.listSectionTable[i].poolLiteral[j].value=decimalToHexadecimal(t);

          int br=sectionTable.listSectionTable[i].poolLiteral[j].flag.size();
          pr+=4;
    }

  }
}

int Assembler ::firstPass(){

  string line;
  smatch matches;

  this->currentSection.name="";//ovde krecemo sa ovim radom to je najbolje


  while(getline(inputFile,line)){
   if (regex_search(line, commentRegex)) {
   

        size_t pos = line.find('#');
        if (pos != std::string::npos) {
        line= line.substr(0, pos);
    }
    } 
     if (regex_search(line, matches, externRegex)) {
          string lista_simbola = matches[1].str();
          vector<string> tokens;
          stringstream ss(lista_simbola);

          string element;
 
          while (getline(ss, element, ',')) {
        //Uklanjamo   prostore sa početka i kraja svakog elementa
          size_t prvi_karakter = element.find_first_not_of(' ');
          size_t poslednji_karakter = element.find_last_not_of(' ');
          tokens.push_back(element.substr(prvi_karakter, poslednji_karakter - prvi_karakter + 1));
    }

          
          for(int i=0;i<tokens.size();i++){
           this->addElementSymTab(0,"NOTYP","GLOB","UND",tokens[i]);
          }
      }
    else   if (regex_search(line, matches, globalRegex)) {
         string lista_simbola = matches[1].str();
          vector<string> tokens;
          stringstream ss(lista_simbola);

          string element;

          while (getline(ss, element, ',')) {
            size_t prvi_karakter = element.find_first_not_of(' ');
            size_t poslednji_karakter = element.find_last_not_of(' ');
            tokens.push_back(element.substr(prvi_karakter, poslednji_karakter - prvi_karakter + 1));

      }

       for(int i=0;i<tokens.size();i++){
           this->addElementSymTab(0,"NOTYP","GLOB","UND",tokens[i]);
          }
    }

    

    else   if (regex_search(line, matches, sectionRegex)) {
        string sekcija = matches[1].str();
        sekcija=removeSpacesAndCommas(sekcija);
      
            bool flag=false;
            for(int i=0;i<sectionTable.listSectionTable.size();i++){
                if(sectionTable.listSectionTable[i].name==sekcija ){
                  if(this->currentSection.name==sectionTable.listSectionTable[i].name) return -2;//znaci vec sam u jednoj sekciji i on mi se javi opet
                  this->currentSection=sectionTable.listSectionTable[i];
                  sectionTable.listSectionTable[i].location_counter=currentSection.location_counter;
                 sectionTable.listSectionTable[i].poolLiteral=currentSection.poolLiteral;
                  flag=true;
                  break;
                }
            }

            
            if(!flag){
              this->addElementSecTab(sekcija);
              this->addElementSymTab(0,"SCTN","LOC",sekcija,sekcija);
            }
            
      }
      else if(regex_search(line, matches, wordRegex)) {

         if(!this->checkInSection())return -3;

          string lista = matches[0].str();

          stringstream ss(lista);

          string element;

            while (getline(ss, element, ',')) {
         
              currentSection.location_counter+=4;

                if (regex_search(element, matches, literalRegex)
                ) {
                      string s = matches[0].str();
                      s=removeSpacesAndCommas(s);
          
                      if(s[0]=='$')return -5;

                }
                 
              }
         
      }

  
      else if(regex_search(line, matches, endRegex)) {

            this->calculateSize();

            return 1;
          
      }

      else if(regex_search(line, matches, symbolRegex)) {

        if(!this->checkInSection())return -3;
        string symbol = matches[0].str();
      

        symbol.pop_back();
        
        if(checkInSysTableDef(symbol)){
          return -4;
        }

        if(checkInSysTableUnDef(symbol)){
          
          this->defineSymbol(symbol);
        }
        else{
          this->addElementSymTab(currentSection.location_counter,"NOTYP","LOC",currentSection.name,symbol);
        }
            
      }

     else if(regex_search(line, matches, skipRegex)) {
         if(!this->checkInSection())return -3;
        string s = matches[0].str();
        s=removeSpacesAndCommas(s);
        s=s.substr(5);

    

        if (!regex_search(s, matches, literalRegex)
                ) {
                  cout<<line<<endl;
               throw MyException("Skip is wrong , you don't have correct");       
          }
          else{

            int d=0;
              if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
                d = stoi(s, nullptr, 16);
            }
            else{
              d=stoi(s);
            }

            this->currentSection.location_counter+=4*d;
          }

     }

      else if(readCommand(line)){
        if(!this->checkInSection())return -3;

       // currentSection.location_counter+=4;//povecavamo za 4 ako naidjemo na neku naredbu
             
            
            currentSection.location_counter+=4;//povecavamo za 4 ako naidjemo na neku naredbu
              if( regex_search(line, matches, iretRegex)){
                this->currentSection.location_counter+=4;
              
              }
              else if(regex_search(line, matches, ldRegex)){
                string s = matches[0].str();
                s=removeSpacesAndCommas(s); 
                s=s.substr(2);   
                
                if(s[0]!='[' && s[0]!='$'){
                   this->currentSection.location_counter+=4;
                }
              }
        
      }

      else if(regex_search(line, matches, emptyRegex)){}
    
    else
    { 
    return -11;
    }
      for(int i=0;i<sectionTable.listSectionTable.size();i++){
                if(sectionTable.listSectionTable[i].name==this->currentSection.name){
                  sectionTable.listSectionTable[i].location_counter=currentSection.location_counter;
                  sectionTable.listSectionTable[i].poolLiteral=currentSection.poolLiteral;
                  sectionTable.listSectionTable[i].Data=currentSection.Data;
                  
                }
              }
    
  }

  
return -11;
}


int Assembler::secondPass()
{

  

  this->setLiteralValue();//tek u drugom prolazu postavljamo ove vrednosti

   string line;
  smatch matches;

  this->currentSection.name="";//ovde krecemo sa ovim radom to je najbolje

  while(getline(inputFile,line)){
   if (regex_search(line, commentRegex)) {

        size_t pos = line.find('#');
        if (pos != std::string::npos) {
        line= line.substr(0, pos);
    }
    }

     if (regex_search(line, matches, externRegex)) {
    
        //ovo preskacemo nema potrebe da ga obradjujemo 
      }
    else   if (regex_search(line, matches, globalRegex)) {
        
        //ovo preskacemo nema potrebe da ga obradjujemo
    }

    else   if (regex_search(line, matches, sectionRegex)) {
        string sekcija = matches[1].str();
        
            bool flag=false;
            for(int i=0;i<sectionTable.listSectionTable.size();i++){
                if(sectionTable.listSectionTable[i].name==sekcija ){
                  if(this->currentSection.name==sectionTable.listSectionTable[i].name) return -2;//znaci vec sam u jednoj sekciji i on mi se javi opet
                  this->currentSection=sectionTable.listSectionTable[i];
                  flag=true;
                  break;
                }
            }

            if(currentSection.name!=""){
              for(int i=0;i<sectionTable.listSectionTable.size();i++){
                if(sectionTable.listSectionTable[i].name==this->currentSection.name){
                  sectionTable.listSectionTable[i].location_counter=currentSection.location_counter;
                 sectionTable.listSectionTable[i].poolLiteral=currentSection.poolLiteral;
                }
              }
            }

            if(!flag){
              this->addElementSecTab(sekcija);
            
            }
               
      }
      else if(regex_search(line, matches, wordRegex)) {
        

         if(!this->checkInSection()){cout<<line<<endl;return -3;}

          //potrebno je da imam po , i onda aloziram prostor za simbol ili literal
          string lista = matches[0].str();

          regex pattern("\\.word");

          lista = regex_replace(lista, pattern, "");
        
          stringstream ss(lista);

          string element;

            while (getline(ss, element, ',')) {

              element=removeSpacesAndCommas(element);
              writeWord(element);
           
               currentSection.location_counter+=4;

          }
          
          if(currentSection.name!=""){
              for(int i=0;i<sectionTable.listSectionTable.size();i++){
                if(sectionTable.listSectionTable[i].name==this->currentSection.name){
                 sectionTable.listSectionTable[i].location_counter=currentSection.location_counter;
                  sectionTable.listSectionTable[i].Data=currentSection.Data;
                  sectionTable.listSectionTable[i].RelocationTable=currentSection.RelocationTable;
               
                }
              }
            }

      }

  
      else if(regex_search(line, matches, endRegex)) {
     
         return 1;
         break;//dobro tu se zavrsava valjda 

      }

      else if(regex_search(line, matches, symbolRegex)) {
          
      }

     else if(regex_search(line, matches, skipRegex)) {
        

         if(!this->checkInSection())return -3;

         
          string element = matches[0].str();


              element=removeSpacesAndCommas(element);
               regex pattern("\\.skip");

              element = regex_replace(element, pattern, "");
              writeSkip(element);

        
          if(currentSection.name!=""){
              for(int i=0;i<sectionTable.listSectionTable.size();i++){
                if(sectionTable.listSectionTable[i].name==this->currentSection.name){
                 sectionTable.listSectionTable[i].location_counter=currentSection.location_counter;
                  sectionTable.listSectionTable[i].Data=currentSection.Data;
                  sectionTable.listSectionTable[i].RelocationTable=currentSection.RelocationTable;
               
                }
              }
            }

      }

      else if(readCommand2Pass(line)){
        if(!this->checkInSection())return -3;//mora da se nalazi u nekoj sekciji

  
          if(currentSection.name!=""){
              for(int i=0;i<sectionTable.listSectionTable.size();i++){
                if(sectionTable.listSectionTable[i].name==this->currentSection.name){
                  sectionTable.listSectionTable[i].location_counter=currentSection.location_counter;
                  //sectionTable.listSectionTable[i].poolLiteral=currentSection.poolLiteral;
                  sectionTable.listSectionTable[i].Data=currentSection.Data;
                  sectionTable.listSectionTable[i].RelocationTable=currentSection.RelocationTable;
                  
                }
              }
            }
            currentSection.location_counter+=4;//povecavamo za 4 ako naidjemo na neku naredbu
        
      }

    else if(regex_search(line, matches, emptyRegex)){}
  
    else
    { 
    return -11;
    }

        
     
  }

return -11;//ovo  pokazuje da je neka velika greska u pitanju i da nismo mogli da procitamo odredjenu liniju

}


void Assembler::calculateSize()
{
    for(int i=0;i<sectionTable.listSectionTable.size();i++){
      sectionTable.listSectionTable[i].size=sectionTable.listSectionTable[i].location_counter;

          for(int j=0;j<symTable.listSymTable.size();j++){
            if(symTable.listSymTable[j].name==sectionTable.listSectionTable[i].name){
              symTable.listSymTable[j].size=sectionTable.listSectionTable[i].size;
            }
          }
      sectionTable.listSectionTable[i].location_counter=0;//spremam ga za drugi prolaz
    }    
}


bool Assembler::readCommand(string line)

{
  smatch matches;

  if(regex_search(line, matches, ldRegex)
  || regex_search(line, matches, callRegex)
  || regex_search(line, matches, stRegex)
  || regex_search(line, matches, bgtRegex)
  || regex_search(line, matches, bneRegex)
  || regex_search(line, matches, beqRegex)
  || regex_search(line, matches, jmpRegex)

  ) {

    if (regex_search(line, matches, literalRegex)) {
          string s = matches[0].str();

       bool check=functionLiteral(s);
       if(check==false)return false;

    }

    return true;
  }

  else if(regex_search(line, matches, csrwrRegex)
    || regex_search(line, matches, csrrdRegex)
    || regex_search(line, matches, shrRegex)
    || regex_search(line, matches, shlRegex)
    || regex_search(line, matches, xorRegex)
    || regex_search(line, matches, orRegex)
    || regex_search(line, matches, andRegex)
    || regex_search(line, matches, notRegex)
    || regex_search(line, matches, divRegex)
    || regex_search(line, matches, mulRegex)
    || regex_search(line, matches, subRegex)
    || regex_search(line, matches, addRegex)
    || regex_search(line, matches, xchgRegex)
    || regex_search(line, matches, popRegex)
    || regex_search(line, matches, pushRegex)
    || regex_search(line, matches, haltRegex)
    || regex_search(line, matches, intRegex)
    || regex_search(line, matches, retRegex)
    
  ) {
    return true;
  }
  else if( regex_search(line, matches, iretRegex)){
    return true;
  }
  

return false;
}


bool Assembler::writeWord(string line){
  smatch matches;
  string povratna="";
  


  if (regex_search(line, matches, literalRegex)) {
    

          string s = matches[0].str();
          s=removeSpacesAndCommas(s);
      

          if(s[0]=='$')
          {
           return false;
          }
          else{
             

            if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
            }
            else{
              s=decimalToHexadecimal(s);
            }
            povratna+=s;
            int br=8-povratna.size();

            string t="";

            for(int i=0;i<br;i++){
                t+="0";
            }
            povratna=t+povratna;          

          }
             ElemData d;
            d.value=povratna;
            d.offset=this->currentSection.location_counter;
            this->currentSection.Data.push_back(d);
            return true;
    }
  else  if (regex_search(line, matches, symbolRegex1)) {
    string s = matches[0].str();
    s=removeSpacesAndCommas(s);

    povratna+="00000000";
    addDispatchToSymbol(s,true);

    ElemData d;
    d.value=povratna;
    d.offset=this->currentSection.location_counter;
    this->currentSection.Data.push_back(d);
    return true;
  
  }

 return true;

}

bool Assembler::writeSkip(string line)
{
  smatch matches;
  string povratna="00000000";
  int d=-1;

//potrebno je da podatke u memoriji smestam u litltle endian poretku kako da to uradim
//kako da podatke smestam u little enian poretku ko ce ga nzati

  if (regex_search(line, matches, literalRegex)) {
    

          string s = matches[0].str();
          s=removeSpacesAndCommas(s);
      

          if(s[0]=='$')
          {
           return false;
          }
          else{
             

          if (regex_search(s, matches, hexRegex)) {
              s=s.substr(2);
               d = stoi(s, nullptr, 16);
          }
          else{
            
            d=stoi(s);

          }

          for(int i=0;i<d;i++){
             ElemData d;
            d.value=povratna;
            d.offset=this->currentSection.location_counter;
            this->currentSection.location_counter+=4;
            this->currentSection.Data.push_back(d);
            
          }
                   
          }
          return true;

  }
    
return false;


}

bool Assembler::readCommand2Pass(string line)
{

  smatch matches;
  string povratna="";
  bool check=false;

 

  if(regex_search(line, matches, ldRegex)) {
    
  bool provera=false;
  bool provera1=false;

   if (regex_search(line, matches, operandWithBracketsRegex)) {
    provera=true;
   }else if(regex_search(line, matches, bracketsRegex)) {
    provera1=true;

   }

     vector<string>tmp=registar_regex(line);
    if(tmp.size()!=1 && !provera && !provera1){
        return false;
      }

      if(provera1){
        if(tmp.size()!=2)return false;
        povratna+="92";
        povratna+=tmp[1];
        povratna+="0";
        povratna+=tmp[0];
        povratna+="000";

      }

       else  if (regex_search(line, matches, literalRegex)) {

        
          povratna+="9";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {

            if(provera)return false;

            povratna+="1";
            s=AfterDollar(s);

          }
          else{
            povratna+="2";
          }
   
        

          if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
            }
          else{
            s=decimalToHexadecimal(s);
          }
          if(provera){
          
          if(s.size()>3)return false;

          povratna+=tmp[1];
          povratna+="0";
          povratna+=tmp[0];

          int t=3-s.size();
          string tt="";
          for(int i=0;i<t;i++){
              tt+="0";
          }
          s=tt+s;
          povratna+=s;

          }
          else{
          povratna+=tmp[0];
          povratna+="f";
          povratna+="0";
          addDispatchToLiteral(povratna,s,false);
        }

          
    }
    else  if (regex_search(line, matches, symbolRegex1)) {

      if(provera)return false;

      povratna+="9";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
            povratna+="1";
            s=AfterDollar(s);

            povratna+=tmp[0];
          povratna+="f";
          povratna+="0";
       

          addDispatchToSymbol(s,false);
          povratna+="000";//znaci da treba da ga redefinisemo

          }
          else{
            povratna+="2";
          

          povratna+=tmp[0];
          povratna+="f";
          povratna+="0";
       

          addDispatchToSymbol(s,false);
          povratna+="000";//znaci da treba da ga redefinisemo

           ElemData d;
        d.value=povratna;
        d.offset=this->currentSection.location_counter;
        this->currentSection.Data.push_back(d);


          this->currentSection.location_counter+=4;


          povratna="92";
          povratna+=tmp[0];
          povratna+=tmp[0];
          povratna+="0000";

          }

    }
    else{
      return false;
    }


    check= true;
  }
else if(regex_search(line, matches, callRegex)) {

 
     if (regex_search(line, matches, literalRegex)) {

          povratna+="2";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
            return false;

          }
          else{
            povratna+="0";
          }

          if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
            }
          else{
            s=decimalToHexadecimal(s);
          }

            povratna+="f";
            povratna+="00";
            addDispatchToLiteral(povratna,s,false);

      }

      else  if (regex_search(line, matches, symbolRegex1)) {
       
  
      povratna+="2";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
           return false;
          }
          else{
            povratna+="0";
          }

           povratna+="f";
          povratna+="00";
       
          addDispatchToSymbol(s,false);
          povratna+="000";

    }
    else{
      return false;
    }

   check= true;
  }
  else if(regex_search(line, matches, csrwrRegex)) {

    vector<string>tmp=registar_regex(line);
   vector<string>tmp1=csr_regex(line);


    if( tmp1.size()!=1 || tmp.size()!=1){
        return false;
      }

      povratna+="94";
      povratna+=tmp1[0];
      povratna+=tmp[0];
      povratna+="0000";

      check= true;

  }
  else if(regex_search(line, matches, csrrdRegex)) {

   vector<string>tmp=registar_regex(line);
   vector<string>tmp1=csr_regex(line);

    if( tmp1.size()!=1 || tmp.size()!=1){
        return false;
      }

      povratna+="90";
      povratna+=tmp[0];
      povratna+=tmp1[0];
      povratna+="0000";

      check= true;
  }
  else if(regex_search(line, matches, stRegex)) {

    bool provera=false;
    bool provera1=false;
    

    if (regex_search(line, matches, operandWithBracketsRegex)) {
     provera=true;
   
     }else if (regex_search(line, matches, bracketsRegex)) {
        provera1=true;
     }


    vector<string>tmp=registar_regex(line);     
    if(tmp.size()!=1 && !provera && !provera1){
        return false;
      }


    if(provera1){
        if(tmp.size()!=2)return false;
        povratna+="80";
        povratna+=tmp[1];
        povratna+="0";
        povratna+=tmp[0];
        povratna+="000";

      }
     else if (regex_search(line, matches, literalRegex)) {

          povratna+="8";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {

            if(provera)return false;
            povratna+="2";
            s=AfterDollar(s);

          }
          else{
            povratna+="0";
          }
  
          

          if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
            }
          else{
            s=decimalToHexadecimal(s);
          }

          if(provera){
            if(s.size()>3)return false;

            povratna+=tmp[1];
            povratna+="0";
            povratna+=tmp[0];

            int t=3-s.size();
            string tt="";
            for(int i=0;i<t;i++){
                tt+="0";
            }
            s=tt+s;
            povratna+=s;

          }
          else{
            
            povratna+="00";
            povratna+=tmp[0];
            addDispatchToLiteral(povratna,s,false);

          }
          
    }
    else  if (regex_search(line, matches, symbolRegex1)) {
   
          povratna+="8";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
            povratna+="2";
            s=AfterDollar(s);

          }
          else{
            povratna+="0";
          }
          povratna+="0";
           povratna+="f";
          povratna+=tmp[0];
         
       

          addDispatchToSymbol(s,false);
          povratna+="000";//znaci da treba da ga redefinisemo

    }
    else{
      return false;
    }

   check= true;
  }
  else if(regex_search(line, matches, shrRegex)) {


    vector<string>tmp=registar_regex(line);
    if(tmp.size()!=2){
        return false;
      }

      povratna+="71";
      povratna+=tmp[1];
      povratna+=tmp[0];
      povratna+=tmp[1];
      povratna+="000";

      check= true;

  }
  else if(regex_search(line, matches, shlRegex)) {

    vector<string>tmp=registar_regex(line);
    if(tmp.size()!=2){
      return false;
     }

     povratna+="70";
     povratna+=tmp[1];
     povratna+=tmp[0];
     povratna+=tmp[1];
     povratna+="000";

    check= true;
  }
  else if(regex_search(line, matches, xorRegex)) {

   vector<string>tmp=registar_regex(line);

     if(tmp.size()!=2){
      return false;
     }

     povratna+="63";
     povratna+=tmp[1];
     povratna+=tmp[0];
     povratna+=tmp[1];
     povratna+="000";

    check= true;
  }
  else if(regex_search(line, matches, orRegex)) {

    vector<string>tmp=registar_regex(line);

     if(tmp.size()!=2){
      return false;
     }

     povratna+="62";
     povratna+=tmp[1];
     povratna+=tmp[0];
     povratna+=tmp[1];
     povratna+="000";

    check= true;
  }
  else if(regex_search(line, matches, andRegex)) {

     vector<string>tmp=registar_regex(line);

     if(tmp.size()!=2){
      return false;
     }

     povratna+="61";
     povratna+=tmp[1];
     povratna+=tmp[0];
     povratna+=tmp[1];
     povratna+="000";

    check= true;

  }
  else if(regex_search(line, matches, notRegex)) {

     vector<string>tmp=registar_regex(line);

     if(tmp.size()!=1){
      return false;
     }

     povratna+="60";
     povratna+=tmp[0];
     povratna+=tmp[0];
     povratna+="0000";

    check= true;
  }

  else if(regex_search(line, matches, divRegex)) {

    vector<string>tmp=registar_regex(line);
    

    if(tmp.size()!=2){
      return false;//znaci da nesto nije u redu
    }
    
    povratna+="53";
    povratna+=tmp[1];
    povratna+=tmp[1];
    povratna+=tmp[0];
    povratna+="000";

   
    check= true;
  }

  else if(regex_search(line, matches, mulRegex)) {

    vector<string>tmp=registar_regex(line);

    if(tmp.size()!=2){
      return false;//znaci da nesto nije u redu
    }
    
    povratna+="52";
    povratna+=tmp[1];
    povratna+=tmp[1];
    povratna+=tmp[0];
    povratna+="000";

   
    check= true;
  }
  else if(regex_search(line, matches, subRegex)) {
   
    vector<string>tmp=registar_regex(line);

    if(tmp.size()!=2){
      return false;//znaci da nesto nije u redu
    }
    
    povratna+="51";
    povratna+=tmp[1];
    povratna+=tmp[1];
    povratna+=tmp[0];
    povratna+="000";

   
    check= true;
  }
  else if(regex_search(line, matches, addRegex)) {
    

   vector<string>tmp=registar_regex(line);

    if(tmp.size()!=2){
      return false;//znaci da nesto nije u redu
    }
    
    povratna+="50";
    povratna+=tmp[1];
    povratna+=tmp[1];
    povratna+=tmp[0];
    povratna+="000";

   
    check= true;
  }
  else if(regex_search(line, matches, xchgRegex)) {
    //instrukcija atomicne zamene vrednosti 
    vector<string>tmp=registar_regex(line);

    if(tmp.size()!=2){
      return false;
    }
    povratna+="400";
    povratna+=tmp[0];
    povratna+=tmp[1];
    povratna+="000";

    check= true;
  }
  else if(regex_search(line, matches, popRegex)) {

    vector<string>tmp=registar_regex(line);

    if(tmp.size()!=1){
      return false;
    }
    povratna+="93";
    povratna+=tmp[0];
    povratna+="e0";
    povratna+="004";
    check= true;
  }
  else if(regex_search(line, matches, pushRegex)) {

    vector<string>tmp=registar_regex(line);

    if(tmp.size()!=1){
      return false;
    }
    povratna+="81";
    povratna+="e0";
    povratna+=tmp[0];
    povratna+="ffc";


    
    check= true;
  }
  else if(regex_search(line, matches, bgtRegex)) {
  
    vector<string>tmp=registar_regex(line);

    if(tmp.size()!=2 ){
      return false;
    }

  
    if (regex_search(line, matches, literalRegex)) {
     
          povratna+="3";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
           return false;
            
          }
          else{
            povratna+="3";
          }
      

          if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
            }
          else{
            s=decimalToHexadecimal(s);
          }

        
          povratna+="f";
          povratna+=tmp[0];
          povratna+=tmp[1];
          addDispatchToLiteral(povratna,s,false);

    }
  else  if (regex_search(line, matches, symbolRegex1)) {

          povratna+="3";

          string s = matches[0].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
            return false;
          }
          else{
            povratna+="3";
          }

            povratna+="f";
            povratna+=tmp[0];
            povratna+=tmp[1];
       

          addDispatchToSymbol(s,false);
          povratna+="000";//znaci da treba da ga redefinisemo

    }
    else{
      
      return false;
    }
   check= true;
  }
  else if(regex_search(line,matches,bneRegex)){
     vector<string>tmp=registar_regex(line);

    if(tmp.size()!=2 ){
      return false;
    }

  
    if (regex_search(line, matches, literalRegex)) {
     
          povratna+="3";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
           return false;
            
          }
          else{
            povratna+="2";
          }
      

          if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
            }
          else{
            s=decimalToHexadecimal(s);
          }

        
          povratna+="f";
          povratna+=tmp[0];
          povratna+=tmp[1];
          addDispatchToLiteral(povratna,s,false);

    }
  else  if (regex_search(line, matches, symbolRegex1)) {

          povratna+="3";

          string s = matches[0].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
            return false;
          }
          else{
            povratna+="2";
          }

            povratna+="f";
            povratna+=tmp[0];
            povratna+=tmp[1];
       

          addDispatchToSymbol(s,false);
          povratna+="000";//znaci da treba da ga redefinisemo

    }
    else{
      
      return false;
    }
   check= true;
  }
  else if(regex_search(line, matches, beqRegex)) {

   vector<string>tmp=registar_regex(line);

    if(tmp.size()!=2 ){
      return false;
    }

  
    if (regex_search(line, matches, literalRegex)) {
     
          povratna+="3";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
           return false;
            
          }
          else{
            povratna+="1";
          }
      

          if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
            }
          else{
            s=decimalToHexadecimal(s);
          }

        
          povratna+="f";
          povratna+=tmp[0];
          povratna+=tmp[1];
          addDispatchToLiteral(povratna,s,false);

    }
  else  if (regex_search(line, matches, symbolRegex1)) {

          povratna+="3";

          string s = matches[0].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
            return false;
          }
          else{
            povratna+="1";
          }

            povratna+="f";
            povratna+=tmp[0];
            povratna+=tmp[1];
       

          addDispatchToSymbol(s,false);
          povratna+="000";//znaci da treba da ga redefinisemo

    }
    else{
      
      return false;
    }
   check= true;
  }
  else if(regex_search(line, matches, jmpRegex)) {
  

     if (regex_search(line, matches, literalRegex)) {

          povratna+="3";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
            return false;

          }
          else{
            povratna+="0";
          }

          if (regex_search(s, matches, hexRegex)) {
                s=s.substr(2);
            }
          else{
            s=decimalToHexadecimal(s);
          }
          povratna+="f";
            povratna+="00";
            addDispatchToLiteral(povratna,s,false);

    }
      else  if (regex_search(line, matches, symbolRegex1)) {
       
  
      povratna+="3";

          string s = matches[1].str();

          s=removeSpacesAndCommas(s);
          if(s[0]=='$')
          {
            return false;
          }
          else{
            povratna+="0";
          }

            povratna+="f";
            povratna+="00";
       
          addDispatchToSymbol(s,false);
          povratna+="000";//znaci da treba da ga redefinisemo

    }
    else{
      return false;
    }

   check= true;
  }
  else if(regex_search(line, matches, haltRegex)) {
    
    povratna="00000000";
 
    check= true;
  }
  else if(regex_search(line, matches, intRegex)) {

    
    povratna="10000000";
    

   check= true;
  }
  else if(regex_search(line, matches, iretRegex)) {

    //MMMM==0b0110: csr[A]<=mem32[gpr[B]+gpr[C]+D];
    //status=mem[sp+4];

    povratna+="96";
    povratna+="0E0004";

        ElemData d;
        d.value=povratna;
        d.offset=this->currentSection.location_counter;
        this->currentSection.Data.push_back(d);

    this->currentSection.location_counter+=4;

    //pc=mem[sp]
    //sp=sp+8
    ///pc r15
    //sp r14

//MMMM==0b0011: gpr[A]<=mem32[gpr[B]]; gpr[B]<=gpr[B]+D;
    povratna="93";
    povratna+="f";
    povratna+="e0";
    povratna+="008";//za 8 ga povecavamo ovde

  
    check= true;
  }
  else if(regex_search(line, matches, retRegex)) {

    //pop pc
    //povratak iz funkcije

    povratna+="93";
    povratna+="f";
    povratna+="e0";
    povratna+="004";
    check= true;
   
    check= true;
  }
  ///////////

  if(check==true)
      { 
        ElemData d;
        d.value=povratna;
        d.offset=this->currentSection.location_counter;
        this->currentSection.Data.push_back(d);
        return true;
    }
  else{
    return false;
  }


}

void Assembler::addElementSymTab(int offset, string type, string bind, string ndx, string name)
{

  if(this->numberOfSymbols==0){
    ElemSymTable el;
    el.num=0;
    el.value=0;
    el.size=0;
    el.type="NOTYP";
    el.bind="LOC";
    el.ndx="UND";
    el.name="";

    //ovde sam dodao ali sam mogaao i na neko drugo mesto
    
    this->symTable.listSymTable.push_back(el);

  }
  
    ElemSymTable el;
    el.num=++numberOfSymbols;

    if(this->currentSection.name=="")
    el.value=0;
    else
    el.value=this->currentSection.location_counter;

    el.size=0;
    el.type=type;
    el.bind=bind;
    el.ndx=ndx;
    el.name=name;
    

    this->symTable.listSymTable.push_back(el);




}

void Assembler::addElementSecTab( string name)
{
  ElemSecTable el;
  if(this->numberOfSection==0){
    
    el.section_id=1;
    numberOfSection++;
   
  }else{
    el.section_id=numberOfSection;

  }

   el.location_counter=0;
    el.name=name;

    this->currentSection=el;
   

    sectionTable.listSectionTable.push_back(el);
    numberOfSection++;
}


void Assembler::printSymTable(string s)
{
 // this->outputFile<<this->symTable.listSymTable<<endl;


 this->outputFile<<s+"#.symtab" << std::endl;
    this->outputFile << left << setw(4) << "Num";
    this->outputFile<< left << setw(8) << "Value";
    this->outputFile << left << setw(9) << "Size";
    this->outputFile << left << setw(9) << "Type";
    this->outputFile<< left << setw(9) << "Bind";
    this->outputFile << left << setw(11) << "Ndx";
    this->outputFile<< "Name" << endl;



    for (int i = 0; i <symTable.listSymTable.size(); i++)
    {
        const ElemSymTable& elem = symTable.listSymTable[i];
        this->outputFile<< left << setw(4) << elem.num;
        this->outputFile<< left << setw(8) << elem.value;
        this->outputFile << left << setw(9) << elem.size;
        this->outputFile << left << setw(9) << elem.type;
        this->outputFile << left << setw(9) << elem.bind;
        this->outputFile << left << setw(11) << elem.ndx;
        this->outputFile<< elem.name << endl;
    }

        this->outputFile<<endl;
        this->outputFile<<endl;
}


void Assembler::printSecTable()
{

  this->outputFile<< "#.section_tab" << std::endl;
    this->outputFile << left << setw(4) << "ID";
    this->outputFile<< left << setw(12) << "SIZE";
    this->outputFile<< left << setw(12) << "NAME" ;
     this->outputFile<< "Literals:" << endl;
    
    
  
    for (int i = 0; i <sectionTable.listSectionTable.size(); ++i)
    {
        const ElemSecTable& elem = sectionTable.listSectionTable[i];
        this->outputFile<< left << setw(4) << elem.section_id;
    
          this->outputFile << left << setw(12) << elem.size;

          this->outputFile<< left << setw(12)<<elem.name;


        for(int j=0;j<elem.poolLiteral.size();j++){
          const Literal& l = elem.poolLiteral[j];
          this->outputFile<< l.flag<<", ";
        }

        this->outputFile<< endl;

    }
        
      this->outputFile<<endl;
      this->outputFile<<endl;
        
    }

  void Assembler:: write_two_two(string str) {

    string t="";
    for (char& c : str) {
        t.push_back(tolower(c));
    }
    str=t;
  for (size_t i = 0; i < str.length(); i += 2) {
      this->outputFile << str.substr(i, 2) << " ";
  }

}

void Assembler::addDispatchToLiteral(string& povratna, string s,bool brackets)
{
   //sada ide pretraga da vidimo da li imamo literal
          Literal l;
          bool c=false;
          for(int i=0;i<currentSection.poolLiteral.size();i++){
            if(currentSection.poolLiteral[i].flag==s){
              c=true;
              l=currentSection.poolLiteral[i];
            }
          }
          
          if(c==false || brackets){

            string tmp="";
              if(s.size()==3)
             { povratna+=s; tmp+=s;}
              else if(s.size()==2){
                povratna+="0";tmp+="0";
                povratna+=s;tmp+=s;
              }
              else if(s.size()==1){
                povratna+="00";tmp+="00";
                povratna+=s;tmp+="s";
              }
          }
          else{
         
             povratna+="000";

               ElemRelTable r;

              int h=currentSection.location_counter+2;
              string pom=decimalToString(h);
              r.offset=decimalToHexadecimal(pom);

              int target=stoi(l.value,nullptr,16);
         
              int next=currentSection.location_counter+4;
        
              int res=target-next;

              string t=decimalToString(res);
              r.addend=decimalToHexadecimal(t);
              
              r.relocationType="R_PC_32";

              int br=0;

              for(int k=0;k<symTable.listSymTable.size();k++){
                if(symTable.listSymTable[k].name==currentSection.name)
                br=symTable.listSymTable[k].num;
              }
              r.number=br;
              currentSection.RelocationTable.push_back(r);
          }
}


void Assembler::addDispatchToSymbol( string s,bool word)
{

   ElemRelTable r;
   bool check=false;
   bool local=false;
   int index=-1;
  for(int i=0;i<symTable.listSymTable.size();i++){
    ElemSymTable& elem=symTable.listSymTable[i];

    if(elem.name==s){
        check=true;
        r.number=elem.num;
        if(elem.bind=="LOC" && elem.type!="SCTN"){
          local=true;
          index=i;
          int s=stoi(elem.ndx);
          r.number=s;
          r.addend=decimalToHexadecimal(to_string(elem.value));
        }
      
        break;

    }
  }

  if(check==false){

    this->addElementSymTab(0,"NOTYP","GLOB","UND",s);
    r.number=this->numberOfSymbols;
  }
    if(!local)
         r.addend="0";
     int h=-1;

      if(word){
           r.relocationType="R_WORD_32";
            h=currentSection.location_counter;
      }
          
      else{

         r.relocationType="R_32";
           h=currentSection.location_counter+2;
      
        

      }

      string pom=decimalToString(h);
      r.offset=decimalToHexadecimal(pom);
      
       currentSection.RelocationTable.push_back(r);
  
  
  
}

vector<string> Assembler::registar_regex(string line)
{
  smatch matches;
  vector<string>tmp;
   while (regex_search(line, matches, registarRegex)) {
        string s=matches[0] ;


      
        s=this->removeSpacesAndCommasAndR(s);
        if(s=="sp"){
          s="14";
        }else if(s=="pc"){
          s="15";
        }
         
        
        string hexadecimal=decimalToHexadecimal(s);

        
        tmp.push_back(hexadecimal);
        line = matches.suffix();
        
    }
    return tmp;
}

vector<string> Assembler::csr_regex(string line)
{
  smatch matches;
  vector<string>tmp;
   while (regex_search(line, matches, csrRegex)) {
        string s=matches[1] ;

      
        s=this->removeSpacesAndCommas(s);


       if(s=="status"){
        tmp.push_back("0");
       }
       else if(s=="handler"){
        tmp.push_back("1");
       }
       else if(s=="cause"){
        tmp.push_back("2");
       }

        line = matches.suffix();
        
    }
    
    return tmp;
}

string Assembler::hexToLittleEndian(string s)
{
     string ss="";
     for(int i=7;i>=0;i-=2){
      
        for(int k=1;k>=0;k--){
          ss.push_back(s[i-k]);
        }
      
     }
     return ss;
}

void Assembler::changeNdx()
{

  for(int i=0;i<sectionTable.listSectionTable.size();i++){
    ElemSecTable& elem=sectionTable.listSectionTable[i];

    string s="";
    for(int k=0;k<symTable.listSymTable.size();k++){
      if(symTable.listSymTable[k].name==elem.name){
          s=to_string(symTable.listSymTable[k].num);
          break;
      }
    }
    for(int k=0;k<symTable.listSymTable.size();k++){
      if(symTable.listSymTable[k].ndx==elem.name){
          symTable.listSymTable[k].ndx=s;
      }
    }

  }
}

    void Assembler::printMachineCod()
    {


        for (int i = 0; i <sectionTable.listSectionTable.size(); ++i)
    {

         ElemSecTable& elem = sectionTable.listSectionTable[i];

          for(int j=0;j<elem.poolLiteral.size();j++){
                ElemData d;
                d.value=elem.poolLiteral[j].flag;
              
                  string f="";
                  for(int k=0;k<(8-d.value.size());k++){
                    f.push_back('0');
                  }
                  d.value=f+d.value;
                  d.value=hexToLittleEndian(d.value);
                d.offset=stoi(elem.poolLiteral[j].value,nullptr,16);
                elem.Data.push_back(d);
          }

        this->outputFile<< "#.machice_code: "<<endl;
        this->outputFile<< left << setw(4) << elem.name<<endl;
    
         
         for(int j=0;j<elem.Data.size();j++){
          if(j%2==0)
          this->outputFile <<setfill('0') << setw(4)<<right<<hex<<elem.Data[j].offset<<": ";

          this->write_two_two( elem.Data[j].value);

          if(j%2==1)this->outputFile<<endl;

  
         }

        this->outputFile<< endl;
        this->outputFile<< endl;

    }
        
      this->outputFile<<endl;
      this->outputFile<<endl;
        
    }

       void Assembler::printRelTable()
    {


      for (int i = 0; i <sectionTable.listSectionTable.size(); ++i)
    {

        ElemSecTable& elem = sectionTable.listSectionTable[i];

      
        this->outputFile<< "#.rel_table: ";
        this->outputFile<< elem.name<<endl;
    
        this->outputFile << setw(4)<< left<<"Offset ";
        this->outputFile<<setw(15)<< left<<"RelocationType ";
        this->outputFile    <<left<<"Number ";
        this->outputFile << "    Addend "<<endl;
         for(int j=0;j<elem.RelocationTable.size();j++)
          {   
            ElemRelTable& r=elem.RelocationTable[j];
            
            
            this->outputFile<<right<<setw(4) <<r.offset<<"   ";
            this->outputFile<<setw(15)<<setfill(' ')<< left<<r.relocationType<<"   ";
            this->outputFile <<setw(8)<<setfill(' ')<<left<<(float) r.number<< "   ";
            this->outputFile << right<< setw(4)<<setfill('0')<<r.addend<<endl;
      
           
        }
         this->outputFile<< endl;


    }
        
      this->outputFile<<endl;
      this->outputFile<<endl;
        
    }

    

bool Assembler::checkInSection()
{
    if (this->currentSection.name==""){
      return false;
    }
    return true;
}

void Assembler::defineSymbol(string name)
{

  for(int i=0;i<symTable.listSymTable.size();++i){
        if(symTable.listSymTable[i].name==name && symTable.listSymTable[i].ndx=="UND"){
          symTable.listSymTable[i].ndx=this->currentSection.name;
       
          symTable.listSymTable[i].value=this->currentSection.location_counter;
         // symTable.listSymTable[i].bind="LOC"; //ne moze da bude lokalni  element
          break;
        }
      }

    
}

bool Assembler::checkInSysTableDef(string name)
{
      for(int i=0;i<symTable.listSymTable.size();++i){
        if(symTable.listSymTable[i].name==name && symTable.listSymTable[i].ndx!="UND"){
          return true;
        }
      }

      return false;
}

bool Assembler::checkInSysTableUnDef(string name)
{
        for(int i=0;i<symTable.listSymTable.size();i++){
        if(symTable.listSymTable[i].name==name && symTable.listSymTable[i].ndx=="UND" && symTable.listSymTable[i].bind=="GLOB"){
          return true;
        }
      }

      return false;
}


string Assembler::AfterDollar(string input)
{
        size_t dollarPos = input.find('$');
      if (dollarPos != string::npos && dollarPos + 1 < input.length()) {

        string s=input.substr(dollarPos + 1);

        return s;
      
  }
  return "";

}

bool Assembler:: isOkInput(string options,string inputFile,string outputFile){

  char lc = inputFile[inputFile.length() - 1];
  char secondlc = inputFile[inputFile.length() - 2];

  char lc1 = outputFile[outputFile.length() - 1];
  char secondlc1 = outputFile[outputFile.length() - 2];

    if(options=="-o" && lc=='s' && secondlc=='.' 
    && lc1=='o' && secondlc1=='.'){
      return true;
    }else{
      return false;
}
}


string Assembler::decimalToString(int pr)
{
            stringstream ss;
            ss << pr;
            string t = ss.str();
            return t;
}

string Assembler::decimalToHexadecimal(string decimal)
{
     
          stringstream iss(decimal);
          unsigned long long int decimalValue;
          iss >> decimalValue;

          stringstream ss;
          ss << hex << decimalValue;
          
          string hexadecimal = ss.str();
          
          
          return  hexadecimal; 
      
}

bool Assembler::openFiles(string inputFileString, string outputFileString)
{
    this->inputFile.open(inputFileString, ios::in);
    this->outputFile.open(outputFileString, ios::out | ios::trunc);

    if (!this->inputFile.is_open() || !this->outputFile.is_open())
    {
          return false;
    }
    else
    {
          return true;
    }
}

bool Assembler::openFileInput(string inputFileString)
{
  this->inputFile.open(inputFileString, ios::in);
  

  if (!this->inputFile.is_open())
  {
        return false;
  }
  else
  {
        return true;
  }
}

bool Assembler::closeFileInput()
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
///////////////////////////////////////////////
/////////////////////////////////////////////////
////////////////////////////////////////////////
//////////////////////////////////////////////////
///////////////////////////////////////////
/////////////////////////////////////////////////
///koliko sa ukapirao place je options i to mozemo ali ne moramo da imamo 
//to mozemoali ne moramo da imamo i to je onda super

int main(int argc, char const* argv[]) {
 try {
  if(!argv[1] || !argv[2] || !argv[3] ) {
    throw MyException("You have not entered all the elements in the terminal");
  }


    string options = argv[1];
    string outputFile = argv[2];
    string inputFile = argv[3];

    
    if(!Assembler::isOkInput(options,inputFile,outputFile)){
      
      throw MyException("Bad input on the terminal");
    }

    Assembler assembler;
    
   if( assembler.openFiles(inputFile,outputFile))
{
 int i= assembler.firstPass();

  if(i==1){
    cout<<"It is ok"<<endl;
  }
  else if(i==-11){
    throw MyException("Bad line of code");
  }
  else if(i==-2){
    throw MyException("You are already in the given section");
  }
  else if(i==-3){
    throw MyException("You are not in section.");
  }
  else if(i==-4){
    throw MyException("You have already defined symbol");
  }
  else if(i==-5){
    throw MyException("Bad literal, the code isn't ok");
  }

 
assembler.changeNdx();
assembler.printSymTable("FIRST PASS");
assembler.printSecTable();

if(!assembler.closeFileInput()){
   throw MyException("Error, you cannot close the file");
}

if( assembler.openFileInput(inputFile))

{ 
  int j= assembler.secondPass();

    if(j==1){
      cout<<"It is ok"<<endl;
    }
    else if(j==-11){
    throw MyException("Bad line of code");
  }
    else if(j==-2){
      throw MyException("You are already in the given section");
    }
    else if(j==-3){
      throw MyException("You are not in section.");
    }
    else if(j==-5){
    throw MyException("Bad literal, the code isn't ok");
  }
    
  }

assembler.printSymTable("SECOND PASS");
assembler.printSecTable();

assembler.printMachineCod();
assembler.printRelTable();


}
else{
  throw MyException("You cannot open the file");
}


 }catch (const MyException& ex) {
        // Uhvatanje izuzetka i ispisivanje poruke
        std::cerr << "Exception caught " << ex.what() << std::endl;
    }

    return 0;
}

 unsigned int Assembler::stringToHexaDecimal(string hexString)
  {
    unsigned int hexValue;
    stringstream ss;
    ss << hex << hexString;
    ss >> hexValue;


    return hexValue;
  }

  string Assembler::hexaDecimalToString(unsigned int v)
{
   
          stringstream stream;
          stream << hex <<std::setw(8) <<right<< setfill('0') << v; 
          string hexString = stream.str();

          return hexString;
               
}