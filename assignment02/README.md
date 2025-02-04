# Explanation

- 완성 코드: word_count2.c
- 정답 코드: word_count2

input: words.txt (정렬되지 않은 input)


**구조체 정의**
tWord: 단어와 빈도를 저장하는 구조체
tWordDic: tWord 구조체의 배열(data)과 현재 저장된 단어의 수(len), 배열의 용량(capacity)을 관리하는 구조체

##주요 함수
- create_dic
  - 빈 사전을 생성하는 함수
- word_count
  - 파일에서 단어를 읽어와 사전에 저장, 빈도를 증가시키는 함수
  - 모든 단어를 탐색하지 않고, 마지막 단어와 비교하여 처리(sorted input)
- print_dic
  - 사전에 저장된 모든 단어와 빈도를 출력
  - output 형식: 단어\t빈도
- destroy_dic
  - malloc과 realloc으로 할당한 메모리를 해제
- compare_by_word
  - qsort를 위한 비교 함수로, 두 단어를 사전 순으로 비교
- compare_by_freq
  - qsort를 위한 비교 함수로, 두 단어를 빈도 기준으로 내림차순 정렬
  - 빈도가 동일하다면 단어를 사전순으로 정렬
- binary_search
  - 이진 탐색을 사용하여 사전에서 특정 단어를 찾거나 삽입할 위치를 찾는다.
  - 찾으려는 단어가 있는 경우, 해당 인덱스를 반환하고, found = 1
  - else, 삽입할 위치의 인덱스를 반환하고, found = 0
- main 함수
  - 명령어를 인자 단위로 처리하여 출력 양식 결정
  - 사전 초기화 후, 구조체 제작, 출력


## 정답 코드 실행 방법

- 단어순 정렬: $ ./word_count2 -n words.s.txt > 출력파일명
- 빈도순 정렬: $ ./word_count2 -f words.s.txt > 출력파일명
