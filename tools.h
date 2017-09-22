#ifndef _TOOLS_
#define _TOOLS_

int strIsDigit(char* target){
	int i,result = 1;
	for(i = 0 ; target[i] != '\0' ; i++){
		if(target[i] < '0' || '9' < target[i]){
			result = 0;
			break;
		}
	}
	return result;
}

int strToInt(char* src){
	int i,result = 0;
	if(src){
		for(i=0;'0' <= src[i] && src[i] <= '9';i++) result = result*10 + src[i]-'0';
	}
	return result;
}

int strToHex(char* src){
	int i,result = 0;
	if(src){
		for(i=0 ;  ; i++){
			if('0' <= src[i] && src[i] <= '9') result = (result<<4) + src[i]-'0';
			else if('A' <= src[i] && src[i] <= 'F') result = (result<<4) + src[i]-'7';
			else if('a' <= src[i] && src[i] <= 'f') result = (result<<4) + src[i]-'W';
			else break;
		}
	}
	return result;
}

char* strscpy(char* dest, const char* source, int n){
	int size = 0;
	memset(dest,'\0',n+1);
	for(size = 0;size < n && source[size] != '\0';size++){
		if(source[size] == '\n') dest[size] = ' ';
		else dest[size] = source[size];
	}
	for(;size < n;size++){
		dest[size] = ' ';
	}
	return dest;
}

#endif
