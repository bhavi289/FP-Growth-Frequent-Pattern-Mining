#include <stdio.h>
#include <string.h>
#include <stdlib.h>
# define SUPPORT 0.05
typedef struct HeaderTable{
	char item[10000];
	int frequency;
	// pointer to tree node
}HEADERTABLE;

int cmpfunc (const void * a, const void * b) {
   return ( (*(HEADERTABLE*)b).frequency - (*(HEADERTABLE*)a).frequency );
}

int AddInHeaderTable(HEADERTABLE *temporaryTable, char insertString[]){
	static int index = 0;
	int i = 0, flag = 0;
	// printf("%s\n", insertString);
	for(int i = 0; i<index; i++){
		if (strcmp(temporaryTable[i].item,insertString) == 0){
			temporaryTable[i].frequency += 1;
			flag = 1;
			break;
		}
	}
	if (flag == 0){
		strcpy(temporaryTable[index].item, insertString);
		temporaryTable[index].frequency = 1;
		index = index + 1;
	}
	return index;
}

int ReadFile(char fileName[], HEADERTABLE* headerTable){
	FILE *fp;
	HEADERTABLE* temporaryTable = (HEADERTABLE *)malloc(100000 * sizeof(HEADERTABLE));
	char transaction[10000], allEntries[5000][30];
	fp = fopen(fileName, "r");
	int row = 0, i;
	int total = 0, lengthOfTable = 0;
	while(fscanf(fp," %[^\n]s",transaction) == 1){
		char insertString[100];
		insertString[0] = '\0';
		int flag = 0;
		int index = 0;
        strcat(transaction,",");
        for(i=0; i<strlen(transaction); i++){
			if (transaction[i] == ',' && flag == 0){
				flag = 1; 
			}
			else if (transaction[i] == ',' && transaction[i-1] == ','){
				char insertString[50];
				insertString[0] = '\0';
				break;
			}
			else if (transaction[i] == ',') {
				insertString[index] = '\0';
				lengthOfTable = AddInHeaderTable(temporaryTable, insertString);
				index = 0;
				total += 1;
				char insertString[50];
			}
			else if (flag == 1){
				insertString[index] = transaction[i];
				index = index + 1;
			}
		}
        row = row + 1;
        // if(row == 4) break;
    }

    printf("%d\n", lengthOfTable);
    qsort(temporaryTable, lengthOfTable, sizeof(HEADERTABLE), cmpfunc);
    int supportThreshold = SUPPORT * row;
    printf("supportThreshold %d\n", supportThreshold );
	for ( i=0; i<lengthOfTable; i++ ){
		if(temporaryTable[i].frequency > supportThreshold)
			headerTable[i] = temporaryTable[i];
		else break;
    }
	fclose(fp);
	free(temporaryTable);
	return i;
}

int main(){
	HEADERTABLE* headerTable = (HEADERTABLE *)malloc(100000 * sizeof(HEADERTABLE));
	int lengthOfTable, i;
	lengthOfTable = ReadFile("groceries_subset.csv", headerTable);
	for ( i=0; i<lengthOfTable; i++ ){
		printf("%s %d\n", headerTable[i].item, headerTable[i].frequency);
	}
}