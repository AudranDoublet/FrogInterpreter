#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
void myprint(void* s, int c){
	char *s1 = s;
	for(int i = 0 ;i<c ;i++)
		{write(1,s1,1);
		s1++;}
}
int search_w (char* to_s,char* w_s , char** res){ // to_search, where_i_have_to_search, list_of_pointer
//	size_t w = strlen(w_s);
	size_t t = strlen(to_s);
		if(to_s[t-1] != 32 ){
			strncat(to_s," ",1);}
	int i = 0;
	while(*w_s != 0){
		*res = strstr(w_s,to_s);
		if (*res != NULL){
		w_s = *res + 1;
		res++;
		i++;}
		else{
			break;}
	}
	return i;} // return nomber of occurance found	



int main(int argc, char** argv){
	if(argc < 3) return 0;

	char* s =argv[1];
	char* se = malloc(sizeof(*se)*256);
	se = argv[2];
	char **res = malloc(sizeof(char)*2048);
	search_w(se,s,res);
	myprint("voici le resultat 1:",21);
	myprint(res[0],8);
	myprint("\n",1);
	myprint("voici le resultat 2:",21);
	write(1,res[1],8);
	myprint("\n",1);
	return 0;
}
	
