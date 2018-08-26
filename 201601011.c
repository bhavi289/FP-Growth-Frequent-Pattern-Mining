#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
# define SUPPORT 0.02

int sa = 0;
int total_entries = 0;


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

void MineFrequentItemsets(HEADERTABLE* headerTable, int lengthOfTable);


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
	char transaction[20000];
	fp = fopen(fileName, "r");
	int row = 0, i;
	int total = 0, lengthOfTable = 0;
	while(fscanf(fp," %[^\n]s",transaction) == 1){
		char insertString[1000];
		insertString[0] = '\0';
		int flag = 0;
		int index = 0;
        strcat(transaction,",");
        for(i=0; i<strlen(transaction); i++){
			if (transaction[i] == ',' && flag == 0){
				flag = 1; 
			}
			else if (transaction[i] == ',' && transaction[i-1] == ','){
				char insertString[500];
				insertString[0] = '\0';
				break;
			}
			else if (transaction[i] == ',') {
				insertString[index] = '\0';
				lengthOfTable = AddInHeaderTable(temporaryTable, insertString);
				index = 0;
				total += 1;
				char insertString[500];
			}
			else if (flag == 1){
				insertString[index] = transaction[i];
				index = index + 1;
			}
		}
        row = row + 1;
        // if(row == 4) break;
    }
    total_entries = row;
    // printf("%d\n", lengthOfTable);
    qsort(temporaryTable, lengthOfTable, sizeof(HEADERTABLE), cmpfunc);
    int supportThreshold = SUPPORT * row;
    // printf("supportThreshold %d\n", supportThreshold );
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
		// printf("^^^^^^^^^^^^%d %s^^^^^^^^^^^^\n", headerTable[i].frequency, headerTable[i].item );
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

int AddInFPTree(NODE* node, TRANSACTION* singleTransaction, int lengthOfTransaction, HEADERTABLE* headerTable, int lengthOfTable, int mode){
	int i, flag=0;
	NODE* temp;
	for (i = 0; i < lengthOfTransaction; i++){
		if(strcmp(singleTransaction[i].item, "")!=0){
			if (node->child){
				// printf("^^^^^^^^^^^^^%s %s\n", node->child->item, singleTransaction[i].item );
				if(CheckChildren(node->child, singleTransaction[i].item)){
					temp = IncreaseFrequency(node->child, singleTransaction[i].item );
					temp->parent = node;
					node = temp;
					// printf("IncreaseFreqxuency [[%s -> %s]]\n", node->parent->item, node->item);
				}
				else{
					flag = 1; //Detects Branching
					temp = AddSibling(node->child, singleTransaction[i].item, singleTransaction[i].frequency);
					temp->parent = node;
					node = temp;
					// printf("AddSibling [[%s -> %s]]\n", node->parent->item, node->item );
					node->nextSameItem = NULL;
					CreateHeaderTableLink(headerTable, node, lengthOfTable);
				}
			}
			else{
				node->child = NewNode(singleTransaction[i].item, singleTransaction[i].frequency);
				node->child->parent = node;
				node = node->child;
				// printf("NewNode [[%s -> %s]]\n", node->parent->item, node->item);
				node->nextSameItem = NULL;
				CreateHeaderTableLink(headerTable, node, lengthOfTable);
			}
		}
	}
	return flag;
}
char n;
void subset(int start, int index, int num_sub,char items[][200])
{
    int i, j;
    if (index - start + 1  ==  num_sub)
    {
        if (num_sub  ==  1)
        {
            for (i = 0;i < n;i++)
                printf("%s\n", items[i]);
        }
        else
        {
            for (j = index;j < n;j++)
            {
                for (i = start;i < index;i++)
                    printf("%s, ", items[i]);
                printf("%s\n", items[j]);
            }
            if (start != n - num_sub)
                subset(start + 1, start + 1, num_sub, items);
        }
    }
    else
    {
        subset(start, index + 1, num_sub, items);
    }
}

void grayCode(TRANSACTION* transaction, int N, char item[]) {
    int grayCode[(int) pow(2, N)];
    int binary[(int) pow(2, N)];
 	int i;
 	int j;
    for (i = 0; i < pow(2, N); i++)
        grayCode[i] = (i >> 1) ^ i;
 
    for (i = 0; i < pow(2, N); i++) {
        int b = 1;
        binary[i] = 0;
        while (grayCode[i] > 0) {
            binary[i] += (grayCode[i] % 2) * b;
            grayCode[i] /= 2;
            b = b * 10;
        }
    }
    printf("\nThe permutations are: \n");
    for (i = 0; i < pow(2, N); i++) {
    	char t[300]={};
          t[0]='\0';
        //printf("{ ");
        for (j = 0; j < N; j++) {
            if (binary[i] % 10 == 1){
            	strcat(t,transaction->item);
                //printf("%s", set[j]);
            }
            binary[i] /= 10;
        }
        //printf("\n");
        strcat(t,item);
        printf("(%s) %s\n",transaction->item, t);
    }
}

 void permute(NODE *h_node, TRANSACTION* singleTransaction, int total, NODE* expandedNode)
 {
 	printf("Expanded Node %s\n", expandedNode->item);
 	char items[200][200];
 	int i=0;
 	while(strcmp(h_node->item,"")!=0)
 	{
 		strcpy(items[i],h_node->item);
 		// printf("ins %s\n", items[i]);
 		i++;
 		h_node=h_node->parent;
 	}
 	int length = i;
 	printf("%d\n", length);
 	int j=0;
 	for(j=0;j<total;j++)
 	{

 		// printf("Inserted Strings-> %s\n", singleTransaction[j].item);
	 	grayCode(singleTransaction, total, expandedNode->item);
 	}

 }

void MakeTree(char fileName[], HEADERTABLE* headerTable, int lengthOfTable, NODE* root, int mode, NODE* expandedNode){
	printf("Making Tree <%s>\n", fileName);
	FILE *fp;
				// printf("HeaderTable %s$\n", headerTable[lengthOfTable-1].item);

	char transaction[20000];
	fp = fopen(fileName, "r");
	int row = 0, i;
	FILE *fp2 = fopen("initial_tree.txt", "a");

	while(fscanf(fp," %[^\n]s",transaction) == 1){
		int total = 0;
		TRANSACTION* singleTransaction = (TRANSACTION *)malloc(200000 * sizeof(TRANSACTION));
		char insertString[1000];
		insertString[0] = '\0';
		int flag = 0;
		if(mode) flag = 1; 
		int index = 0;
        strcat(transaction,",");
        printf("transaction %s\n\n", transaction);
        for(i=0; i<strlen(transaction); i++){
			if (transaction[i] == ',' && flag == 0){
				flag = 1; 
			}
			else if (transaction[i] == ',' && transaction[i-1] == ','){
				char insertString[500];
				insertString[0] = '\0';
				break;
			}
			else if (transaction[i] == ',') {
				insertString[index] = '\0';
				int freq = 0;
				freq = CheckInHeaderTable(headerTable, insertString, lengthOfTable);
				// printf("here %d %s\n", freq, insertString);

				if (freq){
					// printf("***************%s****************\n", insertString);
					strcpy(singleTransaction[total].item, insertString);
					singleTransaction[total].frequency = freq;
					total += 1;
				}
				index = 0;
				char insertString[500];
			}
			else if (flag == 1){
				insertString[index] = transaction[i];
				index = index + 1;
			}
		}
        row = row + 1;
	    qsort(singleTransaction, total, sizeof(TRANSACTION), cmpfunc);
	    printf("mode - %d\n", mode );
	    if(mode && total){
			 printf("\nTransaction Added To Tree:-\n");

		    for ( i=0; i<total; i++ ){
				// printf("(%s-%d) ", singleTransaction[i].item, singleTransaction[i].frequency);
			}
			// printf("\n"); 

		}
		else{
			for ( i=0; i<total; i++ ){
				// printf("(%s-%d) ", singleTransaction[i].item, singleTransaction[i].frequency);
				// fprintf(fp2, "(%s-%d) ", singleTransaction[i].item, singleTransaction[i].frequency);
			}
			// printf("\n");
			// fprintf(fp2,"\n");
		}
		printf("%d %d ****\n", total, lengthOfTable);

		int check = AddInFPTree(root, singleTransaction, total, headerTable, lengthOfTable, mode);
		// printf("lengthOfTable %d\n", lengthOfTable);
		if(mode && total){
			if(check){
				// printf("recurse\n");
				// MineFrequentItemsets(headerTable, lengthOfTable);
				// recurse
			}
			else{
				// printf("PERMUTE $%s$\n", headerTable[lengthOfTable-1].itemNode->item );
				if(headerTable[lengthOfTable-1].itemNode)
					permute(headerTable[lengthOfTable-1].itemNode, singleTransaction, total, expandedNode);
				// all permutations
			}
		}

        // if(row == 9) break;
    }
			fclose(fp2);

}

int NewHeaderTable(HEADERTABLE *temporaryTable, char insertString[], int index, int frequency, char expandedString[]){
	// static int index = 0;
	int i = 0, flag = 0;
	for(int i = 0; i<index; i++){
		if (strcmp(temporaryTable[i].item,insertString) == 0){
			temporaryTable[i].frequency += frequency;
			flag = 1;
			break;
		}
	}
	if (flag == 0){
		strcpy(temporaryTable[index].item, insertString);
		temporaryTable[index].frequency = frequency;
		index = index + 1;
	}
	return index;
}

void NewFPTree(HEADERTABLE* headerTable, HEADERTABLE* newHeaderTable, int rowNumber){

}


void MineFrequentItemsets(HEADERTABLE* headerTable, int lengthOfTable){
	// index becomes length of new header table
	int i;
	for(i = 0; i<lengthOfTable-1; i++){
		FILE *fp = fopen("temporary_set.txt", "w+");

		HEADERTABLE* newHeaderTable = (HEADERTABLE *)malloc(200000 * sizeof(HEADERTABLE));
		// printf("%s\n", headerTable[i].itemNode->item );
		NODE* sameItemLinks = (NODE *)malloc(sizeof(NODE));
		sameItemLinks = headerTable[i].itemNode;
		int index=0;
		HEADERTABLE* temporaryTable = (HEADERTABLE *)malloc(200000 * sizeof(HEADERTABLE));
		// printf("\nStart\n");
		

		while(sameItemLinks){
			printf("gug\n");
			printf("----------%s %d-------------\n",sameItemLinks->item, sameItemLinks->frequency);
			NODE* treeLinks = (NODE *)malloc(sizeof(NODE));

			treeLinks = sameItemLinks->parent;
			int l = 0;
			printf("** %d #%s#\n", treeLinks->frequency, treeLinks->item);
			if(strcmp(treeLinks->item,"")==0){
				FILE *finalResult = fopen("final_result.txt","a");

				printf("'%s %d'\n", sameItemLinks->item, sameItemLinks->frequency);
				fprintf(finalResult, "%s %d\n", sameItemLinks->item, sameItemLinks->frequency);
				printf("kkk\n");
				fclose(finalResult);

				
			}
			else{
				printf("l\n");
				while(strcmp(treeLinks->item,"")!=0){
					printf("*** %s #%s# %d %d ***\n",treeLinks->item, treeLinks->parent->item, treeLinks->frequency, l);
					if (strcmp(treeLinks->parent->item,"")!=0){
						fprintf(fp, "%s,", treeLinks->item);
						index = NewHeaderTable(temporaryTable , treeLinks->item, index, sameItemLinks->frequency, sameItemLinks->item);
						treeLinks = treeLinks->parent;
					}
					else{
						fprintf(fp, "%s\n", treeLinks->item);
						index = NewHeaderTable(temporaryTable , treeLinks->item, index, sameItemLinks->frequency, sameItemLinks->item);
						break;
					}
				}
			}
			sameItemLinks = sameItemLinks->nextSameItem;

		}
		fclose(fp);


		int supportThreshold = SUPPORT * total_entries, j=0;
	    // printf("supportThreshold- %d %d\n", supportThreshold, index );
	    qsort(temporaryTable, index, sizeof(HEADERTABLE), cmpfunc);
	    for ( j=0; j<index; j++ ){
				// printf("--%s %d\n", temporaryTable[j].item, temporaryTable[j].frequency);
	    }
	    j=0;
	    int k=0;
	    // printf("\nNew Header Table\n");
	    for ( j=0; j<index; j++ ){
			if(temporaryTable[j].frequency >= supportThreshold){
				newHeaderTable[j] = temporaryTable[j];
				// printf("%s\n", newHeaderTable[j].item);
				k++;
			}
			else break;
	    }

	    free(temporaryTable);
		NODE* root = (NODE*)malloc(sizeof(NODE));
		root = NewNode("", 1);
	    MakeTree("temporary_set.txt", newHeaderTable, k, root, 1, headerTable[i].itemNode);
	    // free(newHeaderTable);
	}
}	

int main(){
	char fileName[] = "groceries_subset.csv";
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	HEADERTABLE* headerTable = (HEADERTABLE *)malloc(200000 * sizeof(HEADERTABLE));
	int lengthOfTable, i;
	lengthOfTable = MakeHeaderTable(fileName, headerTable);
	for ( i=0; i<lengthOfTable; i++ ){
		printf("%s %d\n", headerTable[i].item, headerTable[i].frequency);
	}
	NODE* root = (NODE*)malloc(sizeof(NODE));
	root = NewNode("", 1);
	MakeTree(fileName, headerTable, lengthOfTable, root, 0, NULL);

	// printf("ROOT- %s %d #%s# $%d$\n", root->item, root->frequency, root->child->next->item, root->child->next->frequency);

	MineFrequentItemsets(headerTable, lengthOfTable);
	// fclose(finalResult);
}