#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
# define SUPPORT 0.02

int sa = 0;
int total_entries = 0;
FILE *finalResult;


typedef struct Node{
	char item[10000];
	int frequency;
	struct Node* parent; // Pointer to parent Node in C
	struct Node* child;
	struct Node* next; // Not Using
	struct Node* nextSameItem; //Pointer To Node in Tree with Same Item Name
	int isEnd; // Not Using
}NODE;

typedef struct HeaderTable{
	char item[10000];
	int frequency;
	NODE* itemNode; // Pointer To Node in Tree with same Item Name
	// pointer to tree node

}HEADERTABLE;

typedef struct FrequentItemsInTransactions{
	
}FREQUENTITEMSINTRANSACTIONS;

typedef struct  Transaction{
	char item[10000];
	int frequency;
}TRANSACTION;

// Called in Recursion
void MineFrequentItemsets(HEADERTABLE* headerTable, int lengthOfTable);

/*
	Compare function to sort on basis of frequency using qsort function
*/
int cmpfunc (const void * a, const void * b) {
   return ( (*(HEADERTABLE*)b).frequency - (*(HEADERTABLE*)a).frequency );
}


/*
	Items are added in the Header Table
*/
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

/*
	CSV File is Read and based on Items and SUPPORT, Headertable is formed
*/
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

/*
	After forming Header Table we need to remove all items from all transactions which are
	not there in Header Table.
	This function checks the presence of item in Header Table and returns accordingly.
*/	
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

/*
	Link Header Table entries to nodes in the tree.
*/
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

/*
	Creating a new Node
*/
NODE* NewNode(char item[], int frequency){
	NODE *new_node = (NODE*)malloc(sizeof(NODE));
	strcpy(new_node->item, item);
	new_node->frequency = 1;
	return new_node;
}

/*
	Adding a child to the node. Since child already exists, the new node is appended at the end of linked list.
*/
NODE* AddSibling(NODE* node, char item[], int frequency){
	while(node->next) node = node->next;
	return node->next = NewNode(item, frequency);
}

/*
	Checking If given node has children
*/
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

/*
	Increasing Frequency of Node.
*/
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

/*
	This function has 2 modes.
	Mode 0 is for forming the tree initially from the CSV.
	Mode 1 is when a new Header table is formed when a node is expanded.

	AddSibling() detects branching in Tree and is used to decide whether tree is linear or not.
*/
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


/*
	Once a linear tree is formed permute all members of the tree with the expanded Node.
	All itemsets of all itemlengths are produced.
*/

void grayCode(TRANSACTION* transaction, int N, char item[], int frequency) {
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
    // printf("\nThe permutations are: \n");
    for (i = 0; i < pow(2, N); i++) {
    	char t[300]={};
          t[0]='\0';
        //printf("{ ");
        for (j = 0; j < N; j++) {
            if (binary[i] % 10 == 1){
            	if(strcmp(t, transaction->item)!=0)
            	strcat(t,transaction->item);
                //printf("%s", set[j]);
            }
            binary[i] /= 10;
        }
        //printf("\n");
        finalResult = fopen("all_results.txt","a");
        // fprintf(finalResult, "t is %s-%s\n",t, item );
        if(strcmp(t,item)!=0 && strcmp(t,"")!=0){
	        strcat(t, ",");
	        strcat(t,item);
	        fprintf(finalResult, "%d^ %s \n",frequency, t );
	    }	
		

        fclose(finalResult);
        // printf("(%s) %s - %d\n",transaction->item, t);
    }
}

/*
	This Function is called in 2 modes, 0 and 1
   -int mode = 0 is when we're forming FP tree for first time.
   -int mode = 1 is when we're forming a FP tree when we've expanded one of the nodes and formed
	a new header table.
*/
void MakeTree(char fileName[], HEADERTABLE* headerTable, int lengthOfTable, NODE* root, int mode, NODE* expandedNode){
	// printf("Making Tree <%s>\n", fileName);
	FILE *fp;
				// printf("HeaderTable %s$\n", headerTable[lengthOfTable-1].item);

	char transaction[20000];
	fp = fopen(fileName, "r");
	int row = 0, i;
	FILE *fp2 = fopen("initial_transactions_modified.txt", "a");

	while(fscanf(fp," %[^\n]s",transaction) == 1){
		int total = 0;
		TRANSACTION* singleTransaction = (TRANSACTION *)malloc(200000 * sizeof(TRANSACTION));
		char insertString[1000];
		insertString[0] = '\0';
		int flag = 0;
		if(mode) flag = 1; 
		int index = 0;
        strcat(transaction,",");
        // printf("transaction %s\n\n", transaction);
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
				/*
					After forming Header Table we need to remove all items from all transactions which are
					not there in Header Table
				*/	

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
	    // printf("mode - %d\n", mode );
	    if(mode && total){
			 // printf("\nTransaction Added To Tree:-\n");

		    for ( i=0; i<total; i++ ){
				// printf("(%s-%d) ", singleTransaction[i].item, singleTransaction[i].frequency);
			}
			// printf("\n"); 

		}
		else{
			for ( i=0; i<total; i++ ){
				// printf("(%s-%d) ", singleTransaction[i].item, singleTransaction[i].frequency);
				fprintf(fp2, "(%s-%d) ", singleTransaction[i].item, singleTransaction[i].frequency);
			}
			// printf("\n");
			fprintf(fp2,"\n");
		}
		// printf("%d %d ****\n", total, lengthOfTable);

		/*
			Once we've removed items from transactions which are not there in Header Table we form the FP tree
			from the transactions.
		*/	
		int check = AddInFPTree(root, singleTransaction, total, headerTable, lengthOfTable, mode);
		if(mode && total){
			if(check){
				// printf("Recursing\n");
				MineFrequentItemsets(headerTable, total);
			}
			else{
				if(headerTable[lengthOfTable-1].itemNode)
					grayCode(singleTransaction, total, expandedNode->item, expandedNode->frequency);
			}
		}

    }
			fclose(fp2);

}

/*
	Function to form header table when expanding node in dataset.
*/
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

/*
	Function used to convert integer to string.
*/
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

/*
	Header Table is traversed. FP tree node connected to entry is expanded till root and then 
	subsequent nextSameItem node is expanded.
	New Set of transactions is obtained.
	A new Header Table is formed.
	MakeTree function is called with mode = 1, and new header table is sent to it.
	In make Tree transactions are modified according to header table and a new FP tree is formed.
	Once FP tree is created, branching is checked.
	If no branches then - Permutations are done.
	Otherwise this function is called again, with new header table.
*/
int file_counter = 0;
void MineFrequentItemsets(HEADERTABLE* headerTable, int lengthOfTable){
	// index becomes length of new header table
	int i;
	for(i = 0; i<lengthOfTable-1; i++){
		char str[10];
		tostring(str, file_counter);
		file_counter++;

		/*
			Temporary Files are created to manage state of transaction in recursion.
			All states of transaction can be seen in the directory with files with prefix "temporary_set"
		*/
		char fileName[100] = "temporary_set";
		strcat(fileName,str);
		strcat(fileName,".txt");

		FILE *fp = fopen(fileName, "w");

		HEADERTABLE* newHeaderTable = (HEADERTABLE *)malloc(200000 * sizeof(HEADERTABLE));
		NODE* sameItemLinks = (NODE *)malloc(sizeof(NODE));
		sameItemLinks = headerTable[i].itemNode;

		int index=0;
		HEADERTABLE* temporaryTable = (HEADERTABLE *)malloc(200000 * sizeof(HEADERTABLE));
		// printf("\nStart\n");
		int supportThreshold = SUPPORT * total_entries;
		// if(sameItemLinks->frequency>=supportThreshold){
		// if(headerTable[i].itemNode->frequency>=supportThreshold){
		// 	finalResult = fopen("all_results.txt","a");
		// 	// printf("'%s %d'\n", sameItemLinks->item, sameItemLinks->frequency);
		// 	fprintf(finalResult, "%s %d\n", sameItemLinks->item, sameItemLinks->frequency);
		// 	fclose(finalResult);
		// }
		while(sameItemLinks){
			// printf("----------%s %d-------------\n",sameItemLinks->item, sameItemLinks->frequency);
			NODE* treeLinks = (NODE *)malloc(sizeof(NODE));

			treeLinks = sameItemLinks->parent;
			int l = 0;
			// printf("** %d #%s#\n", treeLinks->frequency, treeLinks->item);
			if(strcmp(treeLinks->item,"")==0 && sameItemLinks->frequency>supportThreshold){
				finalResult = fopen("all_results.txt","a");
				// printf("'%s %d'\n", sameItemLinks->item, sameItemLinks->frequency);
				// fprintf(finalResult, "%s %d\n", sameItemLinks->item, sameItemLinks->frequency);
				fclose(finalResult);
				// break;
			}
			else{
				while(strcmp(treeLinks->item,"")!=0){
					// printf("*** %s #%s# %d %d ***\n",treeLinks->item, treeLinks->parent->item, treeLinks->frequency, l);
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

		int j=0;
	    qsort(temporaryTable, index, sizeof(HEADERTABLE), cmpfunc);
	    for ( j=0; j<index; j++ ){
				// printf("--%s %d\n", temporaryTable[j].item, temporaryTable[j].frequency);
	    }
	    j=0;
	    int k=0;
	    for ( j=0; j<index; j++ ){
			if(temporaryTable[j].frequency >= supportThreshold){
				newHeaderTable[j] = temporaryTable[j];
				// printf("%s %d\n", newHeaderTable[j].item, newHeaderTable[j].frequency);
				k++;
			}
			else break;
	    }
	    if(k) printf("________________________\nNEW HEADER TABLE: - \n");
	    for(j = 0;j<k;j++){
			printf("%s %d\n", newHeaderTable[j].item, newHeaderTable[j].frequency);
	    }
	    if(k) printf("*************************\n\n");

	    free(temporaryTable);
		if(k){
			NODE* root = (NODE*)malloc(sizeof(NODE));
			root = NewNode("", 1);
		    MakeTree(fileName, newHeaderTable, k, root, 1, headerTable[i].itemNode);
		 }
	    // free(newHeaderTable);
	}
}	

int main(){
	char fileName[] = "groceries_subset.csv";
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	
	// Removing Content from Files
	FILE *finR = fopen("all_results.txt", "w");
	FILE *in = fopen("initial_transactions_modified.txt", "w");
	fclose(in);
	fclose(finR);

	HEADERTABLE* headerTable = (HEADERTABLE *)malloc(200000 * sizeof(HEADERTABLE));
	int lengthOfTable, i;

	lengthOfTable = MakeHeaderTable(fileName, headerTable);

	/*
		Printing Contents of initial Header Table
	*/
	for ( i=0; i<lengthOfTable; i++ ){
		finalResult = fopen("all_results.txt","a");
			// printf("'%s %d'\n", sameItemLinks->item, sameItemLinks->frequency);
		fprintf(finalResult, "%d^ %s\n", headerTable[i].frequency,headerTable[i].item);
		fclose(finalResult);
		printf("%s %d\n", headerTable[i].item, headerTable[i].frequency);
	}
	/*
		Create Root and make FP tree using the Created Header Table
	*/
	NODE* root = (NODE*)malloc(sizeof(NODE));
	root = NewNode("", 1);

	MakeTree(fileName, headerTable, lengthOfTable, root, 0, NULL);

	// printf("ROOT- %s %d #%s# $%d$\n", root->item, root->frequency, root->child->next->item, root->child->next->frequency);

	/*
		Start Mining Item by expanding nodes from the FP Tree.
		This is done by using links from the Header Table to THe FP tree.
	*/
	MineFrequentItemsets(headerTable, lengthOfTable);
	
	fclose(finalResult);

	/*
		FILE HANDLING to remove duplicate entries
	*/
	int supportThreshold = total_entries * SUPPORT;

	FILE* allResults = fopen("all_results.txt","r");
	FILE* frequentItemsets = fopen("FREQUENT_ITEMSETS.txt","w");
	char str[1000], str2[1000]="";
	int prevNum =0, number = 0, done=0 ;
	while(fscanf(allResults," %[^\n]s",str) == 1){
		int check = 0;
		if(strcmp(str2, str)!=0){
			int x =0, flag = 0;
			number = 0;
			for(x=0; x<strlen(str); x++){
				if(str[x]!='^' && !flag){
					number = number*10 + (str[x] - '0');
				}
				else{
					if(number>supportThreshold){
						fprintf(frequentItemsets, "%s\n", str);
					}
						break;

				} 
			}
		}
		if(strcmp(str2, str)==0 && number<supportThreshold){
			if(!done){
				prevNum += number;
				if(prevNum> supportThreshold){
					// printf("%d %s\n",prevNum, str);
					char tostr[20];
					tostring(tostr, prevNum);
					strcat(tostr,"^");
					int len = strlen(tostr);
					int x, check2 = 0;
					for(x = 0;x< strlen(str);x++){
						// printf("%c", str[x]);
						if(str[x]=='^'){
							check2 = 1;
						}
						else if(check2){
							tostr[len++] = str[x];
							// fprintf(frequentItemsets, "%s\n",tostr );
						}
					}
					tostr[len++] = '\0';
					fprintf(frequentItemsets, "%s\n", tostr );
					done = 1;
				}
			}
		}
		else{
			done = 0;
			int x =0, flag = 0;
			 number = 0;
			for(x=0; x<strlen(str); x++){
				if(str[x]!='^' && !flag){
					number = number*10 + (str[x] - '0');
				}
				else{
					prevNum = number;
					flag=1;

				} 
			}
			strcpy(str2, str);
		}

	}
	fclose(frequentItemsets);
	fclose(allResults);

	printf("\n\n\nInitial Transactions after validating with Header Table can be found in 'initial_transactions_modified.txt'\n");
	printf("\nFinal Itemsets can be found in 'FREQUENT_ITEMSETS.txt'\n");
}