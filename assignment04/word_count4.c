#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
// 단어 구조체
typedef struct {
	char	*word;		// 단어
	int		freq;		// 빈도
} tWord;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tWord		*dataPtr;
	struct node	*llink; // backward pointer
	struct node	*rlink; // forward pointer 원래의 포인터와 같음
} NODE;

typedef struct
{
	int		count;
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList);

// Inserts data into list
// return	0 if overflow
//			1 if successful
//			2 if duplicated key (이미 저장된 단어는 빈도 증가)
int addNode( LIST *pList, tWord *dataInPtr);

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tWord *keyPtr, tWord **dataOutPtr);

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, tWord *pArgu, tWord **dataOutPtr);

// returns number of nodes in list
int countList( LIST *pList);

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList);

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tWord *));

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tWord *));

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr);

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tWord **dataOutPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

////////////////////////////////////////////////////////////////////////////////
// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화// return	word structure pointer
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord( char *word);

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode);

////////////////////////////////////////////////////////////////////////////////
// gets user's input
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'P':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	return strcmp( p1->word, p2->word);
}

// prints contents of name structure
// for traverseList and traverseListR functions
void print_word(const tWord *dataPtr)
{
	printf( "%s\t%d\n", dataPtr->word, dataPtr->freq);
}

// gets user's input
void input_word(char *word)
{
	fprintf( stderr, "Input a word to find: ");
	fscanf( stdin, "%s", word);
}

int main( int argc, char **argv)
{
	LIST *list;
	
	char word[100];
	tWord *pWord;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%s", word) != EOF)
	{
		pWord = createWord( word);
		
		// 이미 저장된 단어는 빈도 증가
		ret = addNode( list, pWord);
		
		if (ret == 0 || ret == 2) // failure or duplicated
		{
			destroyWord( pWord);
		}
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	///////////////////////////////////////////////////////////////////////////////
	
	while (1)
	{
		tWord *ptr;
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:  
				traverseList( list, print_word);
				break;
			
			case BACKWARD_PRINT:
				traverseListR( list, print_word);
				break;
			
			case SEARCH:
				input_word(word);
				
				pWord = createWord( word);

				if (searchNode( list, pWord, &ptr)) print_word( ptr);
				else fprintf( stdout, "%s not found\n", word);
				
				destroyWord( pWord);
				break;
				
			case DELETE:
				input_word(word);
				
				pWord = createWord( word);

				if (removeNode( list, pWord, &ptr))
				{
					fprintf( stdout, "(%s, %d) deleted\n", ptr->word, ptr->freq);
					destroyWord( ptr);
				}
				else fprintf( stdout, "%s not found\n", word);
				
				destroyWord( pWord);
				break;
			
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
		}
		
		if (action) fprintf( stderr, "Select Q)uit, P)rint, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}


// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void)
{
	LIST* newl = (LIST*)malloc(sizeof(LIST));

	newl->count=0;
	newl->head = NULL;
	newl->rear = NULL;

	return newl;

}


// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr)
{
	NODE* n = (NODE*)malloc(sizeof(NODE));
	if(n==NULL)	return 0;
	n->dataPtr = dataInPtr;

	NODE* pn = pPre->rlink;

	n->rlink = pPre->rlink;
	n->llink = pPre;
	pn->llink = n;
	pPre->rlink = n;
	pList->count+=1;

	return 1;
}

// Inserts data into list
// return	0 if overflow
//			1 if successful
//			2 if duplicated key (이미 저장된 단어는 빈도 증가)
int addNode( LIST *pList, tWord *dataInPtr)
{
	int c=0;

	NODE* pPre = NULL;
	NODE* pLoc = NULL;

	NODE* p = pList->head;

	if(pList->count == 0) //빈 리스트일 경우
	{
		NODE* nn = (NODE*)malloc(sizeof(NODE));
		if(nn==NULL)
			return 0;
		nn->dataPtr = dataInPtr;
		nn->llink=NULL;
		nn->rlink=NULL;

		pList->head = nn;
		pList->rear = nn;
		pList->count=1;
		return 1;
	}
	if(pList->count ==1) //한 개 들어가 있을 경우
	{
		if(compare_by_word(p->dataPtr, dataInPtr)==0)
		{
			p->dataPtr->freq+=1;
			return 2;
		}
		if(compare_by_word(p->dataPtr, dataInPtr)<0) //p뒤에   head -- p--nn -- rear
		{
			NODE* nn = (NODE*)malloc(sizeof(NODE));
			if(nn==NULL)
				return 0;
			nn->dataPtr = dataInPtr;

			p->rlink = nn;
			nn->rlink = NULL;

			nn->llink = p;
			pList->rear = nn;
			pList->count +=1;
			return 1;
		}
		//head -- nn -- p  --rear
		NODE* nn = (NODE*)malloc(sizeof(NODE));
		if(nn==NULL)
			return 0;
		nn->dataPtr = dataInPtr;

		pList->head = nn;
		nn->rlink = p;

		p->llink = nn;
		nn->llink = NULL;
		pList->count+=1;
		return 1;
	}

	//두개 이상 들어가 있을 경우.

	c= _search(pList, &pPre, &pLoc, dataInPtr);

	if(c==1) //찾았다. ploc에 위치
	{
		pLoc->dataPtr->freq+=1;
		return 2;
	} 
	
	NODE* nn = (NODE*)malloc(sizeof(NODE));
		if(nn==NULL)
			return 0;
		nn->dataPtr = dataInPtr;
	
	if (pPre==NULL) //맨 처음에 삽입
	{
		nn->rlink = pList->head;
		nn->llink = NULL;
		p->llink = nn;
		pList->head = nn;
		pList->count+=1;
		return 1;
	}

	//pPre 다음에 삽입

    if(pPre->rlink != NULL)
    {
		free(nn);
        _insert(pList, pPre, dataInPtr);
		return 1;
    }
    //pPre가 마지막 노드일 경우
    nn->rlink = NULL;
    nn->llink = pPre;
    pPre->rlink = nn;
    pList->rear = nn;
	pList->count+=1;
	return 1;
}


// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tWord **dataOutPtr)
{

	*dataOutPtr = pLoc->dataPtr;

	pList->count -=1;

	//처음 노드 - pPre = NULL
	if(pLoc == pList->head)
	{
		NODE* t = pLoc->rlink; //ploc다음꺼
		pList->head = t;
		t->llink = NULL;

		free(pLoc);
	}
	
	//마지막 노드
	else if(pLoc->rlink == NULL)
	{
		pPre->rlink = NULL;
		pList->rear = pPre;

		free(pLoc);
		
	}
	
	else
	{
		NODE* t = pLoc->rlink;

		pPre->rlink = t;
		t->llink = pPre;

		free(pLoc);
	}

}
// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, tWord *keyPtr, tWord **dataOutPtr)
{
	NODE* p = pList->head;

	while(p!= NULL)
	{
		if(compare_by_word(p->dataPtr, keyPtr)==0)  // p를 제거
		{
			NODE* pPre = p->llink;


			_delete(pList, pPre, p, dataOutPtr);

			return 1;
		}
		p=p->rlink;
	}

	return 0;

}

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, tWord *pArgu, tWord **dataOutPtr)
{
	NODE* p = pList->head;

	while(p != NULL)
	{
		if(compare_by_word(p->dataPtr, pArgu)==0)
		{
			*dataOutPtr = p->dataPtr;
			return 1;
		}
		p = p->rlink;
	}

	return 0;
}

// returns number of nodes in list
int countList( LIST *pList)
{
	return pList->count;
}
// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const tWord *))
{
	NODE* p = pList->head;
	while(p!=NULL)
	{
		callback(p->dataPtr);
		p = p->rlink;
	}
}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const tWord *))
{
	NODE* p = pList->rear;
	while(p!= NULL)
	{
		callback(p->dataPtr);
		p=p->llink;
	}
}

// for addNode, removeNode, searchNode functions
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu) //compare_by_word 사용
{
	NODE* nd = pList->head;

	if(compare_by_word(pArgu, pList->head->dataPtr)<0) //맨 앞 추가
	{
		*pPre = NULL;
		return 0;
	}
	while(nd->rlink != NULL) //노드가 두개 이상
	{
		if(compare_by_word(pArgu, nd->dataPtr)==0)
		{
			*pLoc=nd;
			return 1;
		}
		if(compare_by_word(nd->rlink->dataPtr, pArgu)>0) // nd 뒤로
		{
			*pPre = nd;
			return 0;
		}
		nd=nd->rlink;
	}

	if(compare_by_word(nd->dataPtr, pArgu)==0)
	{
		*pLoc = nd;
		return 1;
	}
	*pPre = nd;
	return 0;
}


tWord *createWord( char *word)
{
	tWord* nw = (tWord*)malloc(sizeof(tWord));

	if(nw == NULL)
		return NULL;
	
	nw->word = strdup(word);
	nw->freq=1;

	return nw;
}

void destroyWord( tWord *pNode)
{
	free(pNode->word);
	free(pNode);
}

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList)  //destroyWord 사용
{
	NODE* p =pList->head;

	while(p!=NULL)
	{
		NODE* pn = p->rlink;

		destroyWord(p->dataPtr);
		free(p);

		p = pn;
	}

	free(pList);
}

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList)
{
	if(pList->count==0)
		return 1;
	return 0;
}