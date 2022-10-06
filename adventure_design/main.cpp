#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
using namespace std;

// ���� : https://velog.io/@stellakim1012/%EA%B0%84%EB%8B%A8%ED%95%9C-%EB%A9%94%EB%AA%A8%EB%A6%AC-%EA%B4%80%EB%A6%AC%EC%9E%90

typedef struct chunk {
	int start; // �޸� ����� ���� �ּ�
	int size;  // �޸� ����� ũ��
	struct chunk* link; // ���� �� ������ ���� ��ũ
}Chunk;

Chunk* head; // ���Ḯ��Ʈ�� ��� ������

int total = 0;
int fail = 0;

void Init(Chunk* available)
{
	available->start = 0;
	available->size = 100;
	available->link = NULL;
	head = available; // head�� start, size, link�� �ʱ�ȭ�ϱ� ���� �ʼ�

	printf("start = %d \n", available->start);
	printf("size = %d \n\n", available->size);
}

int myalloc(Chunk* available, int request_size) // first-fit
{
	total += 1; // �޸� �Ҵ� Ƚ��
	int temp = 0; // ���� ��ġ ����
	Chunk* before = head; // before �ʱ�ȭ
	Chunk* current = head; // current �ʱ�ȭ

	while (1)
	{
		if (current->size >= request_size) // ���� ����� ��û���� ������� ũ�ų� ���� == �Ҵ��� �����ϴ�
		{
			temp = current->start; // temp�� start������ �ӽ÷� ����
			current->start += request_size; // start������ ��û�� �����ŭ �����ֱ� -> ���� �Ҵ��� ������ ��ŭ�� start��ġ���� ����
			if (current->size == request_size) // ���� ���� ����� ��û���� ������� ������?
			{
				current->size -= request_size; // size�� ��û���� �����ŭ ����
				before = current->link; // before�� ���� chunk�� ����Ŵ
				current = current->link; // current�� ���� �״��� chunk
			}
			else
			{
				current->size -= request_size; // size�� ��û���� �����ŭ ����
			}
			if (head->start == 100) // ���� ù ûũ�� start�� 100�̸�?
			{
				head = head->link; // head�� ���� ûũ�� ����
			}
			break;
		}
		else // �� �뷮�� ����!
		{
			before = current; // before�� current�� ����Ŵ
			current = current->link; // current�� ���� chunk�� �̵�
			if (current == NULL) // ���� ���� chunk�� ������->����
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

void myfree(Chunk* available, int start_address, int return_size) // �׸� �ʼ��ϵ�
{
	Chunk* newchunk = (Chunk*)malloc(sizeof(Chunk)); // ������ �����μ� ����� chunk�� ǥ��
	newchunk->start = start_address; // chunk�� start�� ���� �������� start�� ����
	newchunk->size = return_size; // chunk�� size�� ��������
	Chunk* temp = available; // �� �ӽú����� ù ��° ûũ
	Chunk* temp2 = temp;
	Chunk* next = temp->link; // �� next������ ù ûũ�� ���� ��
	Chunk* next2 = next;

	int case_num = 0; 
	int case_num2 = 0;

	if (next == NULL) // ûũ�� ���� ���� ���ٸ�?
	{
		case_num = 1;
		case_num2 = 1;
	}

	switch (case_num)
	{
	case 0: // chunk�� next�� �ִ�
		while (1)
		{
			if (next == NULL || (next->start > newchunk->start)) // next�� �������� > ������ �������� ��������
			{
				newchunk->link = temp->link; // newchunk�� temp(������ ûũ) ������ ��ġ
				temp->link = newchunk; // temp(������ ûũ)�� newchunk�� ����
				break;
			}
			else  // next�� �������� < ������ �������� ��������
			{
				temp = next; // ������ ûũ�� next�� ����Ű�°ɷ� �ٲٰ�
				next = next->link; // next�� next�� �������� ����
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
		printf("�Ҵ� ������(���� ���� ����): %f %% \n", success / total * 100);
		printf("�޸𸮰� ���� á���ϴ�!\n\n");
		return ;
	}
	while (temp->link != NULL)
	{
		printf("start: %d size: %d \n", temp->start, temp->size);
		temp = temp->link;
	}
	printf("start: %d size: %d \n", temp->start, temp->size);
	printf("�Ҵ� ������(���� ���� ����): %f %% \n\n", success / total * 100);
}

void main()
{
	int i;
	double start, end;
	start = (double)clock() / CLOCKS_PER_SEC;

	Chunk available; // �Ҵ�� ������ �� ���Ḯ��Ʈ available
	Init(&available);

	int a = myalloc_best(&available, 10);
	printf("a: %d \n", a);
	display();

	int b = myalloc_best(&available, 20);
	printf("b: %d \n", b);
	display();

	int c = myalloc_best(&available, 10);
	printf("c: %d \n", c);
	display();

	printf("free a\n");
	myfree(&available, a, 10);
	display();

	a = myalloc_best(&available, 30);
	printf("a: %d \n", a);
	display();

	printf("free c\n");
	myfree(&available, c, 10);
	display();;

	c = myalloc_best(&available, 10);
	printf("c: %d \n", c);
	display();

	int d = myalloc_best(&available, 40);
	printf("d: %d \n", d);
	display();

	printf("free b\n");
	myfree(&available, b, 20);
	display();

	end = (((double)clock()) / CLOCKS_PER_SEC);
	printf("���α׷� ���� �ð� :%lf ��\n", (end - start));
	system("PAUSE");
}