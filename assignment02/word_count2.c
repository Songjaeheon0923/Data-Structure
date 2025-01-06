#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free, qsort
#include <string.h> // strdup, strcmp, memmove

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 구조체 선언
// 단어 구조체
typedef struct {
	char	*word;		// 단어
	int		freq;		// 빈도
} tWord;

// 사전(dictionary) 구조체
typedef struct {
	int		len;		// 배열에 저장된 단어의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 단어의 수)
	tWord	*data;		// 단어 구조체 배열에 대한 포인터
} tWordDic;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
// capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
void word_count( FILE *fp, tWordDic *dic);

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( tWordDic *dic);

// 사전에 할당된 메모리를 해제
void destroy_dic(tWordDic *dic);

// qsort를 위한 비교 함수
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2);

// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2);

////////////////////////////////////////////////////////////////////////////////
// 이진탐색 함수
// found : key가 발견되는 경우 1, key가 발견되지 않는 경우 0
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
				int (*compare)(const void *, const void *), int *found);
				
////////////////////////////////////////////////////////////////////////////////
// 함수 정의 (definition)

// 사전을 초기화 (빈 사전을 생성, 메모리 할당)
// len를 0으로, capacity를 1000으로 초기화
// return : 구조체 포인터
tWordDic *create_dic(void)
{
	tWordDic *dic = (tWordDic *)malloc( sizeof(tWordDic));
	
	dic->len = 0;
	dic->capacity = 1000;
	dic->data = (tWord *)malloc(dic->capacity * sizeof(tWord));

	return dic;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tWordDic *dic;
	int option;
	FILE *fp;
	
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
	
	// 사전 초기화
	dic = create_dic();

	// 입력 파일 열기
	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}

	// 입력 파일로부터 단어와 빈도를 사전에 저장
	word_count( fp, dic);

	fclose( fp);

	// 정렬 (빈도 내림차순, 빈도가 같은 경우 단어순)
	if (option == SORT_BY_FREQ) {
		qsort( dic->data, dic->len, sizeof(tWord), compare_by_freq);
	}
		
	// 사전을 화면에 출력
	print_dic( dic);

	// 사전 메모리 해제
	destroy_dic( dic);
	
	return 0;
}


//시작
// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( tWordDic *dic)
{

	for(int i=0 ; i<dic->len ; i++)
	{
		printf("%s\t%d\n", dic->data[i].word, dic->data[i].freq);
	}

}




// 사전에 할당된 메모리를 해제
void destroy_dic(tWordDic *dic)
{

	for(int i=0 ; i<dic->len ;i++)
	{
		free(dic->data[i].word);
	}

	free(dic->data);
	free(dic);


}



// qsort를 위한 비교 함수
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{

	char *a = (char *)n1;
	char *b = (char *)n2;

	return strcmp(a, b); //a가 크면 1


	return 0;
}

// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2)
{

	tWord *a = (tWord *)n1;
	tWord *b = (tWord *)n2;

	
	if(a->freq == b->freq)
		return strcmp(a->word, b->word);   //a가 크면 1, 같으면 0  문자열이 크다는 것은 사전순으로 뒤쪽에 있는 것
	
	return (b->freq - a->freq);

}


// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
// capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
void word_count( FILE *fp, tWordDic *dic)
{
	int found;
	char new_line[50];

	int m;

	while(fscanf(fp, "%s", new_line) != EOF)
	{

		if(dic->len ==0){
			dic->data[0].word=strdup(new_line);
			dic->data[0].freq =1;
			dic->len =1;
			continue;
		}

		m = binary_search(new_line, dic, dic->len, sizeof(tWord), compare_by_word, &found);

		if (found ==1)
		{
			dic->data[m].freq+=1;
		}

		else{
			memmove(&(dic->data[m+1]), &(dic->data[m]), sizeof(tWord)*(dic->len-m));
			dic->data[m].word = strdup(new_line);
			dic->data[m].freq =1;
			dic->len+=1;

			if(dic->len >= dic->capacity){
				dic->capacity+=1000;
				dic->data = realloc(dic->data, dic->capacity*sizeof(tWord));
			}
		}


	}

}




////////////////////////////////////////////////////////////////////////////////
// 이진탐색 함수
// found : key가 발견되는 경우 1, key가 발견되지 않는 경우 0
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search(const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *), int *found)
{

	*found =0;
	int m;
	int l=0;
	int r = nmemb-1;

	char *ky = (char *)key;
	tWordDic *dic = (tWordDic *)base;


	while(l <= r )
	{
		m=(l+r)/2;
		int cmp_result = compare(dic->data[m].word, ky);
		if(cmp_result ==0)
		{
			*found=1;
			return m;
		}
		else if(cmp_result<0)   //ky가 더 큰 경우
		{
			l=m+1;
		}
		else
		{
			r=m-1;
		}
	}

	return l;
}
