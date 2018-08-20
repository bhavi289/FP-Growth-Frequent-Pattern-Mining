#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct HeaderTable{
	char item[10000];
	int frequency;
	// pointer to tree node
}HEADERTABLE;

void AddInHeaderTable(char transaction[], HEADERTABLE* headerTable, int row){
	/*int i, flag = 0, index = 0;
	// strcpy(headerTable[row].item, transaction);
	char insertString[50];
	for(i=0; i<strlen(transaction); i++){
		insertString[index] = transaction[i];
		if (transaction[i] == ',' && flag == 0){
			flag = 1; 
			index = 0;
			continue;	
		}
		if(transaction[i] == ','){
			insertString[index] = '\0';
			printf("%s\n", insertString);
			strcpy(headerTable[row], insertString);
			index = 0;
		}
		index = index + 1;
	}
	printf("\n");*/
}

void ReadFile(char fileName[], HEADERTABLE* headerTable){
	FILE *fp;
	char transaction[10000], allEntries[5000][30];
	fp = fopen(fileName, "r");
	int row = 0, i;
	int total = 0;
	do{
		char insertString[100];
		insertString[0] = '\0';

		int flag = 0;
		int index = 0;
        fscanf(fp," %[^\n]s",transaction);
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
				// printf("%s\n", insertString);
				strcpy(allEntries[total],insertString);
				// printf("%s\n", insertString);

				// strcpy(headerTable[total].item, insertString);
				index = 0;
			    // printf("%d\n", total );
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
    while(!feof(fp));
    for(i=0; i<total; i++)
			printf("%d %s\n",i, allEntries[i]);
    printf("----------------%d\n", total );
	fclose(fp);
}

int main(){
	HEADERTABLE* headerTable = (HEADERTABLE *)malloc(100000 * sizeof(HEADERTABLE));
	ReadFile("groceries_subset.csv", headerTable);
	printf("%s\n", headerTable[1].item );
}