#include <stdio.h>

void readFile(char fileName[]){
	FILE *fp;
	char name[10000];
	fp = fopen(fileName, "r");
	do{
        fscanf(fp," %[^\n]s",name);
        printf("%s\n", name);
    }
    while(!feof(fp));
	fclose(fp);
}
int main(){
	readFile("groceries_subset.csv");
}