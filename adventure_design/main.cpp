#include <iostream>
#include <time.h>
#include <random>
#include <string.h>
#include <math.h>
#include <vector>
using namespace std;


// 참고 : https://velog.io/@stellakim1012/%EA%B0%84%EB%8B%A8%ED%95%9C-%EB%A9%94%EB%AA%A8%EB%A6%AC-%EA%B4%80%EB%A6%AC%EC%9E%90

typedef struct chunk {
	int start; // 메모리 덩어리의 시작 주소
	int size;  // 메모리 덩어리의 크기
	struct chunk* link; // 다음 빈 공간에 대한 링크
}Chunk;

Chunk* head; // 연결리스트의 헤더 포인터

int total = 0;
int fail = 0;

void Init(Chunk* available)
{
	available->start = 0;
	available->size = 100;
	available->link = NULL;
	head = available; // head의 start, size, link를 초기화하기 위한 초석

	printf("start = %d \n", available->start);
	printf("size = %d \n\n", available->size);
}

int myalloc(Chunk* available, int request_size) // first-fit
{
	total += 1; // 메모리 할당 횟수
	int temp = 0; // 시작 위치 저장
	Chunk* before = head; // before 초기화
	Chunk* current = head; // current 초기화

	while (1)
	{
		if (current->size >= request_size) // 현재 사이즈가 요청받은 사이즈보다 크거나 같다 == 할당이 가능하다
		{
			temp = current->start; // temp에 start지점을 임시로 저장
			current->start += request_size; // start지점에 요청한 사이즈만큼 더해주기 -> 다음 할당은 더해진 만큼의 start위치에서 시작
			if (current->size == request_size) // 만약 남은 사이즈가 요청받은 사이즈랑 같으면?
			{
				current->size -= request_size; // size는 요청받은 사이즈만큼 감소
				before = current->link; // before는 다음 chunk를 가르킴
				current = current->link; // current는 이제 그다음 chunk
			}
			else
			{
				current->size -= request_size; // size는 요청받은 사이즈만큼 감소
			}
			if (head->start == 100) // 만약 첫 청크의 start가 100이면?
			{
				head = head->link; // head를 다음 청크와 연결
			}
			break;
		}
		else // 들어갈 용량이 없음!
		{
			before = current; // before는 current를 가르킴
			current = current->link; // current를 다음 chunk로 이동
			if (current == NULL) // 만약 다음 chunk가 없으면->실패
			{
				fail += 1;
				return -1;
				break;
			}
		}
	}
	return temp;
}
int myalloc_best(Chunk* available, int request_size)
{
	total += 1;

	Chunk* before = head;
	Chunk* current = head;
	int temp = 0;
	int count = 0;
	int index = 0;
	int diff = 100;

	while (current != NULL)
	{
		if (((current->size - request_size) < diff) && (current->size >= request_size))
		{
			temp = current->start;
			diff = current->size - request_size;
			index = count;
			current = current->link;
		}
		else
		{
			if (current->link == NULL && before->size < request_size)
			{
				fail += 1;
				return -1;
				break;
			}
			current = current->link;
		}

		count += 1;
	}

	before = head;
	current = head;

	for (int i = 0; i < index; i++)
	{
		before = current;
		current = current->link;
	}

	if (diff == 0)
	{
		before->link = current->link;
		current = current->link;
		if (head->size == request_size)
		{
			head = head->link;
		}
	}
	else
	{
		temp = current->start;
		current->start += request_size;
		current->size -= request_size;
	}

	return temp;
}

void myfree(Chunk* available, int start_address, int return_size) // 그림 필수일듯
{
	Chunk* newchunk = (Chunk*)malloc(sizeof(Chunk)); // 삭제를 함으로서 생기는 chunk를 표현
	newchunk->start = start_address; // chunk의 start는 기존 데이터의 start랑 같다
	newchunk->size = return_size; // chunk의 size도 마찬가지
	Chunk* temp = available; // 두 임시변수엔 첫 번째 청크
	Chunk* temp2 = temp;
	Chunk* next = temp->link; // 두 next변수엔 첫 청크의 다음 것
	Chunk* next2 = next;

	int case_num = 0; 
	int case_num2 = 0;

	if (next == NULL) // 청크의 다음 것이 없다면?
	{
		case_num = 1;
		case_num2 = 1;
	}

	switch (case_num)
	{
	case 0: // chunk의 next가 있다
		while (1)
		{
			if (next == NULL || (next->start > newchunk->start)) // next의 시작지점 > 삭제할 데이터의 시작지점
			{
				newchunk->link = temp->link; // newchunk는 temp(원래의 청크) 다음에 위치
				temp->link = newchunk; // temp(원래의 청크)를 newchunk와 연결
				break;
			}
			else  // next의 시작지점 < 삭제할 데이터의 시작지점
			{
				temp = next; // 원래의 청크는 next가 가리키는걸로 바꾸고
				next = next->link; // next는 next의 다음것을 지목
			}
		}
		break;
	case 1: 
		temp->link = newchunk;
		newchunk->link = NULL;
		break;
	}

	switch (case_num2)
	{
	case 0:
		while (next2 != NULL)
		{
			if (temp2->start + temp2->size == next2->start)
			{
				temp2->link = next2->link;
				temp2->size += next2->size;
				next2 = next2->link;
			}
			else
			{
				temp2 = next2;
				next2 = next2->link;
			}
		}
		break;
	case 1:
		break;
	}
}

void display()
{
	Chunk* temp;
	temp = head;
	float success = total - fail;
	if (temp == NULL)
	{
		printf("할당 성공률(공간 부족 없음): %f %% \n", success / total * 100);
		printf("메모리가 가득 찼습니다!\n\n");
		return ;
	}
	while (temp->link != NULL)
	{
		printf("start: %d size: %d \n", temp->start, temp->size);
		temp = temp->link;
	}
	printf("start: %d size: %d \n", temp->start, temp->size);
	printf("할당 성공률(공간 부족 없음): %f %% \n\n", success / total * 100);
}

void main()
{
	int i;
	double start, end;
	start = (double)clock() / CLOCKS_PER_SEC;

	Chunk available; // 할당및 저장을 할 연결리스트 available
	Init(&available);
	srand(time(NULL));

	int index = 0;
	vector <int> start_v;
	vector <int> size_v;
	vector <char> index_v;
	for (int i = 0; i < 10; i++)
	{
		int size_temp = 0;
		int start_temp = 0;
		int choice = rand() % 2;
		if (start_v.size() == 0)
			choice = 0;
		switch (choice)
		{
		case 0:
			size_temp = (rand() % 5) * 10 + 10;
			printf("할당할 용량 : %d\n", size_temp);

			size_v.push_back(size_temp);
			start_v.push_back(myalloc(&available, size_temp));
			index_v.push_back(97 + index);
			printf("%c: %d \n", 97 + index++, start_v[start_v.size() - 1]);

			if (start_v[start_v.size() - 1] == -1)
			{
				size_v.erase(size_v.begin() + size_v.size() - 1);
				start_v.erase(start_v.begin() + start_v.size() - 1);
				index_v.erase(index_v.begin() + index_v.size() - 1);
				index--;
			}

			
			display();
			
			break;
		case 1:
			start_temp = (rand() % start_v.size());
			printf("free %c\n", index_v.at(start_temp));
			myfree(&available, start_v.at(start_temp), size_v.at(start_temp));
			display();
			start_v.erase(start_v.begin() + start_temp);
			size_v.erase(size_v.begin() + start_temp);
			index_v.erase(index_v.begin() + start_temp);
\
			break;
		default:
			break;
		}
	}

	end = (((double)clock()) / CLOCKS_PER_SEC);
	printf("프로그램 수행 시간 :%lf 초\n", (end - start));
	system("PAUSE");
}