# Explanation

- 완성 코드: word_count3.c
- 정답 코드: word_count3

input: words.txt (정렬되지 않은 input)


**구조체 정의**
tWord: 단어와 빈도를 저장하는 구조체
NODE: 단어순, 빈도순 연결 리스트를 위한 포인터를 포함하는 구조체
LIST: 단어순, 빈도순 연결 리스트의 HEAD를 포함하는 구조체

##주요 함수
- createList
  - 빈 연결 리스트를 생성하는 함수
- createWord
  - 입력받은 단어로 tWord 구조체를 생성하는 함
- _insert
  - 연결 리스트의 중간에 새로운 노드를 삽입하는 함수
- update_dic
  - NODE를 연결 리스트에 추가하는 함수
  - 리스트에 처음 넣을 때와, 두 번째, 세 번째 이후로 경우의 수를 나누어 설계
- _search
  - update_dic 함수를 위한 함수로, 새로운 노드가 들어가야 할 위치를 찾거나, 입력으로 받은 노드가 리스트에 있는지 확인하는 함수
- _search_by_freq
  - 빈도 순으로 정렬한 리스트에서 _search와 같은 역할을 하는 함
- connect_by_frequency
  - 빈도순 리스트를 순회하며 빈도순 리스트로 연결하는 함수
- print_dic
  - 리스트를 화면에 출력
  - "단어\t빈도" 형식
- print_dic_by_freq
  - 리스트를 빈도순으로 출력
  - "단어\t빈도" 형식
- destroyWord
  - 단어 구조체에 할당된 메모리를 해제하는 함수
- destroyList
  - 연결 리스트 구조체에 할당된 메모리를 해제하는 함수
- main 함수
  - 명령어를 인자 단위로 처리하여 출력 양식 결정
  - 리스트 생성 후, 구조체 제작, 리스트 제작, 출력


## 정답 코드 실행 방법

- 단어순 정렬: $ ./word_count3 -n words.s.txt > 출력파일명
- 빈도순 정렬: $ ./word_count3 -f words.s.txt > 출력파일명
