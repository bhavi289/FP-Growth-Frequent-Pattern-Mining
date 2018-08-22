#include <stdio.h>
#include <string.h>
#include <stdlib.h>
# define SUPPORT 0.05

typedef struct Node{
	char item[10000];
	int frequency;
	struct Node* parent;
	struct Node* child;
	struct Node* next;
	struct Node* nextSameItem;
	int isEnd;
}NODE;

typedef struct HeaderTable{
	char item[10000];
	int frequency;
	NODE* itemNode;
	// pointer to tree node

}HEADERTABLE;

typedef struct FrequentItemsInTransactions{
	
}FREQUENTITEMSINTRANSACTIONS;

typedef struct  Transaction{
	char item[10000];
	int frequency;
}TRANSACTION;

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

int MakeHeaderTable(char fileName[], HEADERTABLE* headerTable){
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

int CheckInHeaderTable(HEADERTABLE *headerTable, char insertString[], int lengthOfTable){
	int i, flag = 0;
	for ( i = 0; i < lengthOfTable; ++i){
		if(strcmp(headerTable[i].item, insertString) == 0)
			flag = headerTable[i].frequency;	
	}
	// printf("flag - %s %d\n",insertString, flag);
	return flag;
}

void CreateHeaderTableLink(HEADERTABLE* headerTable, NODE* node, int lengthOfTable){
	int i;
	for(i=0; i<lengthOfTable; i++){
		if(strcmp(headerTable[i].item, node->item) == 0){
			NODE* temp = headerTable[i].itemNode;
			if(temp){
				while(temp->nextSameItem) temp = temp->nextSameItem;
				temp->nextSameItem = node; 
			}
			else headerTable[i].itemNode = node;
		}
	}
}

NODE* NewNode(char item[], int frequency){
	NODE *new_node = (NODE*)malloc(sizeof(NODE));
	strcpy(new_node->item, item);
	new_node->frequency = 1;
	return new_node;
}

NODE* AddSibling(NODE* node, char item[], int frequency){
	while(node->next) node = node->next;
	return node->next = NewNode(item, frequency);
}

int CheckChildren(NODE* node, char item[]){
	NODE* temp = node;
		// printf("%s %s\n", temp->item, item);

	while (temp){
		// printf("INSIDE\n");
		if(strcmp(temp->item, item) == 0) return 1;
		temp = temp->next;
	}
	return 0;
}

NODE* IncreaseFrequency(NODE *node, char item[]){
	NODE* temp = node;
	while (temp){
		if(strcmp(temp->item, item) == 0){
			temp->frequency += 1;
			return temp;
		}
		temp = temp->next;
	}
	// printf("PROBLEM CHILD\n");
	return node; // not really reqd
}

void AddInFPTree(NODE* node, TRANSACTION* singleTransaction, int lengthOfTransaction, HEADERTABLE* headerTable, int lengthOfTable){
	int i;
	NODE* temp;
	for (i = 0; i < lengthOfTransaction; i++){
		if (node->child){
			// printf("%s %s\n", node->child->item, singleTransaction[i].item );
			if(CheckChildren(node->child, singleTransaction[i].item)){
				temp = IncreaseFrequency(node->child, singleTransaction[i].item );
				temp->parent = node;
				node = temp;
				printf("IncreaseFrequency [[%s -> %s]]\n", node->parent->item, node->item);
			}
			else{
				temp = AddSibling(node->child, singleTransaction[i].item, singleTransaction[i].frequency);
				temp->parent = node;
				node = temp;
				printf("AddSibling [[%s -> %s]]\n", node->parent->item, node->item );
				node->nextSameItem = NULL;
				CreateHeaderTableLink(headerTable, node, lengthOfTable);
			}
		}
		else{
			node->child = NewNode(singleTransaction[i].item, singleTransaction[i].frequency);
			node->child->parent = node;
			node = node->child;
			printf("NewNode [[%s -> %s]]\n", node->parent->item, node->item);
			node->nextSameItem = NULL;
			CreateHeaderTableLink(headerTable, node, lengthOfTable);
		}
		
	}
}

void MakeTree(char fileName[], HEADERTABLE* headerTable, int lengthOfTable, NODE* root){
	FILE *fp;

	char transaction[10000], allEntries[5000][30];
	fp = fopen(fileName, "r");
	int row = 0, i;
	while(fscanf(fp," %[^\n]s",transaction) == 1){
		int total = 0;
		TRANSACTION* singleTransaction = (TRANSACTION *)malloc(100000 * sizeof(TRANSACTION));
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
				int freq = 0;
				freq = CheckInHeaderTable(headerTable, insertString, lengthOfTable);
				if (freq){
					// printf("%s %d\n", insertString, freq);
					strcpy(singleTransaction[total].item, insertString);
					singleTransaction[total].frequency = freq;
					total += 1;
				}
				index = 0;
				char insertString[50];
			}
			else if (flag == 1){
				insertString[index] = transaction[i];
				index = index + 1;
			}
		}
        row = row + 1;
	    qsort(singleTransaction, total, sizeof(TRANSACTION), cmpfunc);
	    // printf("%d\n", total );
		printf("\n");
	    
	    for ( i=0; i<total; i++ ){
			printf("(%s-%d) ", singleTransaction[i].item, singleTransaction[i].frequency);
		}
		printf("\n");
		AddInFPTree(root, singleTransaction, total, headerTable, lengthOfTable);
        // if(row == 9) break;
    }
}


int main(){
	char fileName[] = "groceries_subset.csv";
	HEADERTABLE* headerTable = (HEADERTABLE *)malloc(100000 * sizeof(HEADERTABLE));
	int lengthOfTable, i;
	lengthOfTable = MakeHeaderTable(fileName, headerTable);
	// for ( i=0; i<lengthOfTable; i++ ){
	// 	printf("%s %d\n", headerTable[i].item, headerTable[i].frequency);
	// }
	NODE *root = NewNode("", 1);
	MakeTree(fileName, headerTable, lengthOfTable, root);
	printf("ROOT- %s %d %s %d\n", root->item, root->frequency, root->child->item, root->child->frequency);



}