#include <iostream>
#include <time.h>
#include <random>
#include <math.h>
#include <vector>
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

	Chunk* before = head; //�ʱ�ȭ
	Chunk* current = head; //�ʱ�ȭ
	int temp = 0;
	int count = 0;
	int index = 0; //return��
	int diff = 100;
	
	// 1. ã�� ����
	while (current != NULL)
	{
		// �ΰ����� ���ÿ� ����
		// current�� ������ chunk�� ����� 
		// 1. ��û���� ��������� ���̰� ������
		// 2. ��û���� ������� ū��
		if (((current->size - request_size) < diff) && (current->size >= request_size))
		{
			temp = current->start; //temp�� current�� ������ chunk��
			diff = current->size - request_size; //diff�� ���� �� �ٸ� ����������
			index = count; //chunk�� ���° ������� Ȯ��
			current = current->link; //current�� ���� ��带 ����
		}
		else
		{
			// ���� ������ �������� ������ ��尡 ���ٸ�
			if (current->link == NULL && before->size < request_size) 
			{
				fail += 1;
				return -1;
				break;
			}
			current = current->link; // ������ ���� �ƴϸ� current�� ���� ���� �̵�
		}

		count += 1; // index Ȯ���� ����
	}

	
	before = head; // �ʱ�ȭ
	current = head; // �ʱ�ȭ
	
	// 2. �Ҵ� ����
	for (int i = 0; i < index; i++) // �ش�Ǵ� ������ �̵�
	{
		before = current; // �ش�Ǵ� ���
		current = current->link; // �ش�Ǵ� ����� ���� ���
	}

	if (diff == 0) // �ش� ������ ���ϴ� ������� ������
	{
		before->link = current->link; // �ش� ����� �������� ������带 ����
		current = current->link; 
		if (head->size == request_size) // ���� �װ� �� ù ����̸�
		{
			head = head->link; // ���� ���� ���ʿ��ϹǷ� ���� ��带 head�� ����
		}
	}
	else // ������ ������
	{
		temp = current->start; // temp�� start��ġ ����(return �뵵)
		current->start += request_size; // �� ����
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

	int case_num = 0; // ��ȯ����
	int case_num2 = 0; // �պ�����

	if (next == NULL) // ��尡 �ϳ�
	{
		case_num = 1;
		case_num2 = 1;
	}


	// ��ȯ ����
	switch (case_num)
	{
	case 0: // ��尡 2�� �̻�
		while (1)
		{
			// next�� �������� > ������ �������� ��������
			// �������� ���߱�
			if (next == NULL || (next->start > newchunk->start)) 
			{
				// temp�� next���̿� ������
				newchunk->link = temp->link; // newchunk �ڷ� temp�� ���� ���� ���� 
				temp->link = newchunk; // temp�� ���� ���� newchunk�� 
				break;
			}
			else  // next�� �������� < ������ �������� ��������
			{
				temp = next; // temp�� ��������
				next = next->link; // next�� ��������
			}
		}
		break;
	case 1: // ��尡 �Ѱ�
		temp->link = newchunk; // �� ��带 ����
		newchunk->link = NULL;
		break;
	}

	// �պ� ����
	switch (case_num2)
	{
	case 0: // ��尡 �ΰ� �̻�
		while (next2 != NULL)
		{
			// �պ��ؾ��� ������� Ȯ��
			if (temp2->start + temp2->size == next2->start)
			{
				// temp2�� ������� �������
				// next2�� �����
				temp2->link = next2->link; // �պ� ��� ����� �հ� �ڸ� ����
				temp2->size += next2->size; // ��������� size ����
				next2 = next2->link; // next2�� ���� ���� �̵�
			}
			else
			{
				temp2 = next2; // ��ĭ�� �̵�
				next2 = next2->link;
			}
		}
		break;
	case 1: // ��尡 �Ѱ� -> �� �ʿ� ����
		break;
	}
}

int display()
{
	Chunk* temp;
	temp = head;
	float success = total - fail;
	if (temp == NULL)
	{
		printf("�Ҵ� ������(���� ���� ����): %f %% \n", success / total * 100);
		printf("�޸𸮰� ���� á���ϴ�!\n\n");
		system("pause");
	}
	else
	{
		while (temp->link != NULL)
		{
			printf("start: %d size: %d \n", temp->start, temp->size);
			temp = temp->link;
		}
		printf("start: %d size: %d \n", temp->start, temp->size);
		printf("�Ҵ� ������(���� ���� ����): %f %% \n\n", success / total * 100);
	}
	return 0;
}

void main()
{
	srand((unsigned int)time(NULL));
	double start, end;
	start = (double)clock() / CLOCKS_PER_SEC;
	int i;
	int index = 0;
	Chunk available; // �Ҵ�� ������ �� ���Ḯ��Ʈ available
	Init(&available);

	vector <int> start_v;
	vector <int> size_v;
	vector <char> index_v;

	for (int i = 0; i < 10; i++)
	{
		int size_temp = 0;
		int start_temp = 0;
		int choice;
		choice = rand() % 4;
		if (start_v.size() < 1)
			choice = 0;
		switch (choice)
		{
		case 0:
		case 1:
		case 2:
			size_temp = (rand() % 99) + 1;
			printf("�Ҵ��� �뷮 : %d\n", size_temp);

			size_v.push_back(size_temp);
			start_v.push_back(myalloc(&available, size_temp));
			//start_v.push_back(myalloc_best(&available, size_temp));
			index_v.push_back(97 + index);
			printf("%c: %d \n", 97 + index++, start_v[start_v.size() - 1]);
			printf("%d\n", i);

			if (start_v[start_v.size() - 1] == -1)
			{
				size_v.erase(size_v.begin() + size_v.size() - 1);
				start_v.erase(start_v.begin() + start_v.size() - 1);
				index_v.erase(index_v.begin() + index_v.size() - 1);
				index--;
			}
			display();

			break;
		case 3:
			start_temp = (rand() % start_v.size());
			printf("free %c (���� start = %d, ���� size = %d)\n", index_v.at(start_temp), start_v.at(start_temp), size_v.at(start_temp));
			myfree(&available, start_v.at(start_temp), size_v.at(start_temp));
			display();
			start_v.erase(start_v.begin() + start_temp);
			size_v.erase(size_v.begin() + start_temp);
			index_v.erase(index_v.begin() + start_temp);

			break;
		default:
			break;
		}
	}

	end = (((double)clock()) / CLOCKS_PER_SEC);
	printf("���α׷� ���� �ð� :%lf ��\n", (end - start));
	system("PAUSE");
}
