#include <stdio.h>
#include <stdlib.h>
#include "tablib.h"
#include "tools.h"

#define lineLength 100

int programLength = 0;
labptr symtab = NULL;
void pass1();
void pass2();

int main(int argc, char *argv[]) {
	pass1();
	pass2();
	
	return 0;
}

void pass1(){
	int i;
	char* cptr = NULL;
	char line[lineLength];
	char label[9];
	char ins[7];
	int locctr = 0;
	FILE *srcfile = fopen("SRCFILE","r");
	
	while(fgets(line,lineLength,srcfile) != NULL){
		if(line[0] == '.'){
			//printf("%s",line);
			continue;
		}
		cptr = line;
		strscpy(label,cptr,8);
		//printf("%s ",label);
		cptr += 9;
		strscpy(ins,cptr,6);
		//printf("%s|\n",ins);
		
		if(label[0] != ' '){
			syminsert(symtab,label,locctr);
		}
		
		if(opfind(ins) != -1){
			locctr += 3;
		}
		else if(!strcmp(ins,"BYTE  ")){
			cptr += 8;
			if(*cptr == 'C'){
				cptr += 2;
				for(i = 0;*cptr != '\'' && *cptr != ' ' && *cptr != '\n';cptr++) i++;
			}
			else if(*cptr == 'X'){
				cptr += 2;
				for(i = 0;*cptr != '\'' && *cptr != ' ' && *cptr != '\n';cptr++) i++;
				i /= 2;
			}
			else{
				i = 1;
			}
			
			locctr += i;
		}
		else if(!strcmp(ins,"WORD  ")){
			locctr += 3;
		}
		else if(!strcmp(ins,"RESB  ")){
			cptr += 8;
			locctr += strToInt(cptr);
		}
		else if(!strcmp(ins,"RESW  ")){
			cptr += 8;
			locctr += strToInt(cptr)*3;
		}
		else if(!strcmp(ins,"START ")){
			cptr += 8;
			locctr = strToHex(cptr);
			symtab = syminsert(symtab,label,locctr);
		}
		else if(!strcmp(ins,"END   ")){
			programLength = locctr - symtab->locctr;
			break;
		}
	}
	fclose(srcfile);
	//symlist(symtab);
}
void pass2(){
	FILE *srcfile = fopen("SRCFILE","r");
	FILE *lisfile = fopen("LISFILE1","w");
	FILE *objfile = fopen("OBJFILE1","w");
	char line[lineLength];
	char label[9]={0};
	char ins[7]={0};
	char operand[19]={0};
	char soperand[9]={0};
	char comment[lineLength-33]={0};
	char* cptr = NULL;
	labptr lptr = NULL;
	int i,j,opcode,locctr = 0;
	
	int cardStart = -1;
	char Tcard[61]={0};
	memset(Tcard,'\0',61);
	
	fprintf(lisfile,"SIC Assembler Ver.C\n\n");
	fprintf(lisfile,"loc  opcode label    ins     operand           comment\n");
	fprintf(lisfile,"---- ------ -------- ------  ----------------- ---------------\n");
	
	while(fgets(line,lineLength,srcfile) != NULL){
		//printf("%s",line);
		if(line[0] == '.'){
			fprintf(lisfile,"            %s",line);
			continue;
		}
		cptr = line;
		strscpy(label,cptr,8);
		for(i = 0;i < 9 && *cptr != '\0';cptr++){i++;}
		strscpy(ins,cptr,6);
		for(i = 0;i < 8 && *cptr != '\0';cptr++){i++;}
		strscpy(operand,cptr,18);
		for(i = 0;i < 18 && *cptr != '\0';cptr++){i++;}
		strcpy(comment,cptr);
		
		if(opfind(ins) != -1){	//,X問題 
			opcode = 0;
			strscpy(soperand,operand,8);
			if(cptr = strstr(soperand,",X")){
				opcode += 0x8000;
				*cptr++ = ' ';
				*cptr = ' ';
			}
			if(symfind(symtab,soperand)){
				opcode += opfind(ins)+(symfind(symtab,soperand)->locctr);
			}
			else if(opfind(ins) == 0x4C0000){
				opcode += opfind(ins);
			}
			fprintf(lisfile,"%04X %06X %s",locctr,opcode,line);
			
			sprintf(soperand,"%06X",opcode);
			if(cardStart < 0) cardStart = locctr;
			if(strlen(Tcard)+strlen(soperand) > 60 && strlen(Tcard) > 0){
				fprintf(objfile,"T%06X%02X%s\n",cardStart,strlen(Tcard)/2,Tcard);
				cardStart = locctr;
				memset(Tcard,'\0',61);
			}
			strcat(Tcard,soperand);
			
			locctr += 3;
		}
		else if(!strcmp(ins,"BYTE  ")){
			if(operand[0] == 'C'){
				cptr = operand + 2;
				for(i = 0;*cptr != '\'' && *cptr != ' ' && *cptr != '\n';cptr++) i++;
				strscpy(soperand,cptr,i);
				fprintf(lisfile,"%04X",locctr);
				for(j = 0;j < i;j++)fprintf(lisfile,"%-02X",soperand[j]);
				fprintf(lisfile," %s",line);
				
				//跟老師一樣的BUG懶的修 
				if(cardStart < 0) cardStart = locctr;
				if(strlen(Tcard)+strlen(soperand) > 60 && strlen(Tcard) > 0){
					fprintf(objfile,"T%06X%02X%s\n",cardStart,strlen(Tcard)/2,Tcard);
					cardStart = locctr;
					memset(Tcard,'\0',61);
				}
				strcat(Tcard,soperand);
			}
			else if(operand[0] == 'X'){
				cptr = operand + 2;
				for(i = 0;*cptr != '\'' && *cptr != ' ' && *cptr != '\n';cptr++) i++;
				cptr = operand + 2;
				strscpy(soperand,cptr,i);
				opcode = strToHex(soperand);
				fprintf(lisfile,"%04X %-6X %s",locctr,opcode,line);
				
				if(cardStart < 0) cardStart = locctr;
				if(strlen(Tcard)+strlen(soperand) > 60 && strlen(Tcard) > 0){
					fprintf(objfile,"T%06X%02X%s\n",cardStart,strlen(Tcard)/2,Tcard);
					cardStart = locctr;
					memset(Tcard,'\0',61);
				}
				strcat(Tcard,soperand);
				
				i /= 2;
			}
			else if(strIsDigit(operand)){
				i = 1;
				opcode = strToInt(operand);
				fprintf(lisfile,"%04X %6d %s",locctr,opcode,line);
				
				sprintf(soperand,"%02X",opcode);
				if(cardStart < 0) cardStart = locctr;
				if(strlen(Tcard)+strlen(soperand) > 60 && strlen(Tcard) > 0){
					fprintf(objfile,"T%06X%02X%s\n",cardStart,strlen(Tcard)/2,Tcard);
					cardStart = locctr;
					memset(Tcard,'\0',61);
				}
				strcat(Tcard,soperand);
			}
			
			locctr += i;
		}
		else if(!strcmp(ins,"WORD  ")){
			opcode = strToInt(operand);
			fprintf(lisfile,"%04X %06X %s",locctr,opcode,line);
			
			sprintf(soperand,"%06X",opcode);
			if(cardStart < 0) cardStart = locctr;
			if(strlen(Tcard)+strlen(soperand) > 60 && strlen(Tcard) > 0){
				fprintf(objfile,"T%06X%02X%s\n",cardStart,strlen(Tcard)/2,Tcard);
				cardStart = locctr;
				memset(Tcard,'\0',61);
			}
			strcat(Tcard,soperand);
			
			locctr += 3;
		}
		else if(!strcmp(ins,"RESB  ")){
			fprintf(lisfile,"%04X        %s",locctr,line);
			
			if(strlen(Tcard) > 0){
				fprintf(objfile,"T%06X%02X%s\n",cardStart,strlen(Tcard)/2,Tcard);
				cardStart = -1;
				memset(Tcard,'\0',61);
			}
			
			locctr += strToInt(operand);
		}
		else if(!strcmp(ins,"RESW  ")){
			fprintf(lisfile,"%04X        %s",locctr,line);
			
			if(strlen(Tcard) > 0){
				fprintf(objfile,"T%06X%02X%s\n",cardStart,strlen(Tcard)/2,Tcard);
				cardStart = -1;
				memset(Tcard,'\0',61);
			}
			
			locctr += strToInt(operand)*3;
		}
		else if(!strcmp(ins,"START ")){
			locctr = strToHex(operand);
			cardStart = locctr;
			lptr = symfind(symtab,label);
			if(lptr){
				fprintf(lisfile,"%04X        %s",lptr->locctr,line);
				fprintf(objfile,"H%s%06X%06X\n",lptr->label,lptr->locctr,programLength);
			}
			else{
				fprintf(lisfile,"%04X        %s",symtab->locctr,line);
				fprintf(objfile,"H%s%06X%06X\n",symtab->label,symtab->locctr,programLength);
			}
		}
		else if(!strcmp(ins,"END   ")){
			if(strlen(Tcard) > 0) fprintf(objfile,"T%06X%02X%s\n",cardStart,strlen(Tcard)/2,Tcard);
			
			fprintf(lisfile,"%04X        %s",locctr,line);
			fprintf(objfile,"E%06X",symtab->locctr);
			break;
		}
	}
	fclose(srcfile);
	fclose(lisfile);
	fclose(objfile);
}
