#include <iostream>
#include <vector>

using namespace std;
int arr[] = { 21, 7, 8 ,19, 2, 9, 6, 10 };
vector<int> seg_tree(32);

int Construct_tree(int start, int end, int current);
int Get_query(int n, int q_s, int q_e);
int Query_sum(int start, int end, int q_s, int q_e, int current);
void Segtree_update(int start, int end, int i, int d_value, int current);
void Display(int n);

int main()
{
	Construct_tree(0, 7, 0);
	Display(32);
	printf("%d", Get_query(8, 1, 7));

	system("pause");
}
int Construct_tree(int start, int end, int current) //init
{
	if (start == end)
	{
		seg_tree[current] = arr[start];
		return arr[start];
	}
	int mid = start + (end - start) / 2;
	int child = 2 * current;
	seg_tree[current] = Construct_tree(start, mid, child + 1) + Construct_tree(mid + 1, end, child + 2);
	return seg_tree[current];
}
int Get_query(int n, int q_s, int q_e)
{
	int current = 0;
	if (q_s < 0 || q_e > n - 1 || q_e < q_s)
		return -1;
	int sum = Query_sum(0, n - 1, q_s, q_e, current);
	return sum;
}
int Query_sum(int start, int end, int q_s, int q_e, int current)
{
	if (q_s <= start && q_e >= end)
		return seg_tree[current];
	if (end < q_s || start > q_e)
		return 0;
	int mid = start + (end - start) / 2;
	int child = 2 * current;
	return (Query_sum(start, mid, q_s, q_e, child + 1) + Query_sum(mid + 1, end, q_s, q_e, child + 2));
}
void Segtree_update(int start, int end, int i, int d_value, int current)
{
	if (i < start || i > end)
		return;
	seg_tree[current] = seg_tree[current] + d_value;
	if (start != end)
	{
		int mid = start + (end - start) / 2;
		int child = 2 * current;
		Segtree_update(start, mid, i, d_value, child + 1);
		Segtree_update(mid + 1, end, i, d_value, child + 2);
	}
}
void Display(int n)
{
	printf("세그먼트 트리\n[");
	for (int i = 0; i < n; i++)
	{
		printf("%d ", seg_tree[i]);
	}
	printf("]\n");
}