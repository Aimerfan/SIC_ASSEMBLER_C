#ifndef _TABLIB_
#define _TABLIB_

#include <stdlib.h>
#include <string.h>

//-----struct define-----
typedef struct listnode{
	int  locctr;
	char opcode[7];
	char label[9];
	char ins[7];
	char operand[19];
	char* comment;
	int length;
	struct listnode* next;
}aline;

typedef struct symnode{
	char label[9];
	int locctr;
	struct symnode* left;
	struct symnode* right;
}alab;
typedef alab* labptr;

//-----symtable-----
labptr symfind(labptr symtab, const char* src){
	int compare;
	labptr result;
	if(symtab && src){
		compare = strcmp(src,symtab->label);
		if(compare == 0) result = symtab;
		else if(compare > 0) result = symfind(symtab->right,src);
		else if(compare < 0) result = symfind(symtab->left,src);
	}
	else{
		result = NULL;
	}
	return result;
}

labptr newlab(char* label, int locctr){
	labptr lab = (labptr)malloc(sizeof(alab));
	strcpy(lab->label,label);
	lab->locctr = locctr;
	lab->left = NULL;
	lab->right = NULL;
	return lab;
}

labptr syminsert(labptr symtab, char* label, int locctr){
	labptr insert = NULL;
	int compare;
	if(!label) return NULL;
	if(symtab){
		compare = strcmp(label,symtab->label);
		if(!compare) insert = NULL;
		else if(compare > 0){
			if(symtab->right) syminsert(symtab->right,label,locctr);
			else{
				insert = newlab(label,locctr);
				symtab->right = insert;
			}
		}
		else if(compare < 0){
			if(symtab->left) syminsert(symtab->left,label,locctr);
			else{
				insert = newlab(label,locctr);
				symtab->left = insert;
			}
		}
	}
	else{
		insert = newlab(label,locctr);
	}
	
	return insert;
}

void symlist(labptr root){
	if(root->left) symlist(root->left);
	printf("%s-%X\n",root->label,root->locctr);
	if(root->right) symlist(root->right);
	return;
}

//-----optable-----

int opfind(const char* str){
	int opcode = -1;
	if(!str){}
	else if(str[0] == 'A'){
		if(!strcmp("ADD   ",str)) opcode = 0x180000;
		if(!strcmp("AND   ",str)) opcode = 0x400000;
	}
	else if(str[0] == 'C'){
		if(!strcmp("COMP  ",str)) opcode = 0x280000;
	}
	else if(str[0] == 'D'){
		if(!strcmp("DIV   ",str)) opcode = 0x240000;
	}
	else if(str[0] == 'J'){
		if(!strcmp("J     ",str)) opcode = 0x3C0000;
		if(!strcmp("JEQ   ",str)) opcode = 0x300000;
		if(!strcmp("JGT   ",str)) opcode = 0x340000;
		if(!strcmp("JLT   ",str)) opcode = 0x380000;
		if(!strcmp("JSUB  ",str)) opcode = 0x480000;
	}
	else if(str[0] == 'L'){
		if(!strcmp("LDA   ",str)) opcode = 0x000000;
		if(!strcmp("LDCH  ",str)) opcode = 0x500000;
		if(!strcmp("LDL   ",str)) opcode = 0x080000;
		if(!strcmp("LDX   ",str)) opcode = 0x040000;
	}
	else if(str[0] == 'M'){
		if(!strcmp("MUL   ",str)) opcode = 0x200000;
	}
	else if(str[0] == 'O'){
		if(!strcmp("OR    ",str)) opcode = 0x440000;
	}
	else if(str[0] == 'R'){
		if(!strcmp("RD    ",str)) opcode = 0xD80000;
		if(!strcmp("RSUB  ",str)) opcode = 0x4C0000;
	}
	else if(str[0] == 'S'){
		if(!strcmp("STA   ",str)) opcode = 0x0C0000;
		if(!strcmp("STCH  ",str)) opcode = 0x540000;
		if(!strcmp("STL   ",str)) opcode = 0x140000;
		//if(!strcmp("STSW    ",str)) opcode = 0xE80000;
		if(!strcmp("STX   ",str)) opcode = 0x100000;
		if(!strcmp("SUB   ",str)) opcode = 0x1C0000;	
	}
	else if(str[0] == 'T'){
		if(!strcmp("TD    ",str)) opcode = 0xE00000;
		if(!strcmp("TIX   ",str)) opcode = 0x2C0000;
	}
	else if(str[0] == 'W'){
		if(!strcmp("WD    ",str)) opcode = 0xDC0000;
	}
	
	return opcode;
}


#endif
