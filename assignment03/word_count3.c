#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

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
	struct node	*link; // 단어순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 단어순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for update_dic function
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

// internal insert function
// inserts data into a new node
// for update_dic function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr);

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
void update_dic( LIST *list, char *word);

// internal function
// for connect_by_frequency function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc);

// 단어순 리스트를 순회하며 빈도순 리스트로 연결
void connect_by_frequency( LIST *list);

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( LIST *pList); // 단어순

void print_dic_by_freq( LIST *pList); // 빈도순

// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화
// for update_dic function
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord( char *word);

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode);

////////////////////////////////////////////////////////////////////////////////
// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)  //n1이 크면 1(n1이 사전순으로 뒤쪽이면, 자리를 바꿔야하면)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	return strcmp( p1->word, p2->word);
}
////////////////////////////////////////////////////////////////////////////////
// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2)  //빈도로 n2가 크면(바꿔야 하면) 1, 같으면, n1이 크면(바꿔야하면) 1
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	int ret = (int) p2->freq - p1->freq;
	
	if (ret != 0) return ret;
	
	return strcmp( p1->word, p2->word);
}


////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	char word[1000];
	

	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by word\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_WORD;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	// creates an empty list
	list = createList();
	
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	while(fscanf( fp, "%s", word) != EOF)
	{
		// 사전(단어순 리스트) 업데이트
		update_dic( list, word);
	}
	
	fclose( fp);

	if (option == SORT_BY_WORD) {
		
		// 단어순 리스트를 화면에 출력
		print_dic( list);
	}
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_dic_by_freq( list);
	}
	
	// 단어 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}




////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void)
{
	LIST *newl = (LIST *)malloc(sizeof(LIST));

	if(newl ==NULL)
		return NULL;
	
	newl->count = 0;
	newl->head = NULL;
	newl->head2 = NULL;

	return newl;
}

// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화
// for update_dic function
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord( char *word)
{
	tWord * nw = (tWord *)malloc(sizeof(tWord));

	if(nw ==NULL)
		return NULL;

	nw->word = strdup(word);
	nw->freq =1;

	
	return nw;

}







// internal insert function
// inserts data into a new node
// for update_dic function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr)
{
	NODE* n = (NODE *)malloc(sizeof(NODE));

	if(n == NULL)
		return 0;

	n->dataPtr = dataInPtr;

	n->link = pPre->link;
	pPre->link = n;

	pList->count+=1;

	return 1;

}


// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
void update_dic( LIST *list, char *word)   //_insert, creatWord, search
{
	NODE* pPre = NULL;
	NODE* pLoc = NULL;

	if(list->count==0)  //처음 넣을때
	{
		tWord* p = createWord(word);
        NODE * n = (NODE*)malloc(sizeof(NODE));

        n->dataPtr = p;
        list->head = n;
        n->link = NULL;
		
		list->count=1;
	}

    else if(list->count==1)
	{
		tWord *p = createWord(word);
		if(compare_by_word(list->head->dataPtr, p)==0)
			{
                list->head->dataPtr->freq+=1;
				free(p->word);
				free(p);
            }
		else if(compare_by_word(list->head->dataPtr, p) > 0)
		{
			NODE* n = (NODE *)malloc(sizeof(NODE));
			n->dataPtr = p;
			n->link = list->head;
			list->head = n;
			list->count+=1;
		}
		else
		{
			_insert(list, list->head, p);
		}
	}

	else{   //세 번째 이후
		tWord* p = createWord(word);  

		int c=_search(list, &pPre, &pLoc, p);

		if(c==1)  //p가 있을 때, 쓰지 않으니 할당 해제???
		{
		pLoc-> dataPtr->freq+=1;
			free(p->word);
			free(p);
		}
		else{  //P가 list에 없을 때 (c=0)
			
			if(pPre == NULL)
			{
				NODE* n = (NODE *)malloc(sizeof(NODE));
			n->dataPtr = p;
			n->link = list->head;
			list->head = n;
			list->count+=1;

		
			}

			else
				_insert(list, pPre, p);
		}
	}

}


// for update_dic function
// return	1 found
// 			0 not found
static int _search(LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu)
{

	NODE * nd = pList->head;

	if(compare_by_word(pArgu, pList->head->dataPtr)<0) //맨앞에 들어가야 하는 경우
	{
		*pPre = NULL;

		return 0;
	}

	while(nd->link != NULL)  //노드가 두개 이상 들어가 있을 경우
	{
		if(compare_by_word(pArgu, nd->dataPtr)==0)  //찾았다!
		{
			*pLoc = nd;
			*pPre = NULL;
			return 1;
		}

		if(compare_by_word(nd->link->dataPtr, pArgu)>0)  //nd 다음거 앞으로 가자 == nd 뒤로 가자!
		{
			*pPre = nd;
			*pLoc = NULL;
			return 0;
		}
		nd = nd->link;  // 다음 노드로!
	}

	if(compare_by_word(nd->dataPtr, pArgu)==0)
	{
		*pLoc = nd;
		*pPre = NULL;
		return 1;
	}
	*pPre = nd;
	*pLoc = NULL;
	return 0;

}



static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu)
{
	NODE* n = pList->head2;

	if(compare_by_freq(pList->head2->dataPtr, pArgu)>1) 	//맨 앞에 와야 하는 경우는 pPre = NULL
	{
		*pPre = NULL;
		return 0;
	}
	
	while(n->link2 != NULL)
	{
		if(compare_by_freq(n->link2->dataPtr, pArgu)>0)  //n뒤로 가야 할 경우
		{
			*pPre = n;
			return 0;
		}
		n = n->link2;
	}
	*pPre = n;
	return 0;
}


// 단어순 리스트를 순회하며 빈도순 리스트로 연결
void connect_by_frequency( LIST *list)
{
	NODE* pPre = NULL;
	NODE* pLoc = NULL;
	list->head2 = list->head; //하나는 미리 넣는다.

	NODE* n = list->head->link;

	while(n != NULL) //단어순 리스트 순회
	{
		_search_by_freq(list, &pPre, &pLoc, n->dataPtr);

		if(pPre ==NULL) //맨 앞
		{
			n->link2 = list->head2;
			list->head2 = n;
		}

		else{
			n->link2 = pPre->link2;
			pPre->link2 = n;
		}

		n = n->link;
	}


}

// internal function
// for connect_by_frequency function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc)
{



}


// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( LIST *pList) // 단어순
{

	NODE* p = pList->head;

	while(p != NULL)
	{
		printf("%s\t%d\n",p->dataPtr->word, p->dataPtr->freq);
		p = p->link;
	}

}

void print_dic_by_freq( LIST *pList) // 빈도순
{

	NODE* p = pList->head2;

	while(p != NULL)
	{
		printf("%s\t%d\n", p->dataPtr->word, p->dataPtr->freq);
		p = p->link2;
	}

}

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode)
{
	free(pNode->word);
	free(pNode);
}

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList)
{

	NODE* p = pList->head;

	while(p != NULL)
	{
		NODE* pn = p->link;

		destroyWord(p->dataPtr);
		free(p);

		p=pn;
	}

	free(pList);

}
