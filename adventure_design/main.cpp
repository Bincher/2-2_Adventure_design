#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int* Seg_tree_const(int* list, int n);
int Construct_tree(int* list, int start, int end, int* seg_tree, int current);
int Get_query(int* seg_tree, int n, int q_s, int q_e);
int Query_sum(int* seg_tree, int start, int end, int q_s, int q_e, int current);
int Segtree_update(int* seg_tree, int start, int end, int i, int d_value, int current);

int main()
{
	int *list = 0;
	list = Seg_tree_const(list, 15);
	printf("%d", Get_query(list, 15, 0, 14));
}

int* Seg_tree_const(int* list, int n)
{
	int height = (int)log(n) + 1;
	int t_size = (int)(2 * pow(2, height) - 1);
	int* seg_tree = (int*)(malloc(t_size * sizeof(int)));
	Construct_tree(list, 0, n - 1, seg_tree, 0);
	return seg_tree;
}
int Construct_tree(int* list, int start, int end, int* seg_tree, int current)
{
	if (start == end)
	{
		seg_tree[current] = list[start];
		return list[start];
	}
	int mid = start + (end - start) / 2;
	int child = 2 * current;
	seg_tree[current] = Construct_tree(list, start, mid, seg_tree, child + 1) + Construct_tree((int*)list, mid + 1, end, seg_tree, child + 2);
	return seg_tree[current];
}
int Get_query(int* seg_tree, int n, int q_s, int q_e)
{
	int current = 0;
	if (q_s < 0 || q_e > n - 1 || q_e > q_s)
		return -1;
	int sum = Query_sum((int*)seg_tree, 0, n - 1, q_s, q_e, current);
	return sum;
}
int Query_sum(int* seg_tree, int start, int end, int q_s, int q_e, int current)
{
	if (q_s <= start && q_e >= end)
		return seg_tree[current];
	if (end < q_s or start > q_e)
		return 0;
	int mid = start + (end - start) / 2;
	int child = 2 * current;
	return Query_sum((int*)seg_tree, start, mid, q_s, q_e, child + 1) + Query_sum((int*)seg_tree, mid + 1, end, q_s, q_e, child + 2);
}
int Segtree_update(int* seg_tree, int start, int end, int i, int d_value, int current)
{
	if (i < start || i > end)
		return NULL;
	seg_tree[current] = seg_tree[current] + d_value;
	if (start != end)
	{
		int mid = start + (end - start) / 2;
		int child = 2 * current;
		Segtree_update((int*)seg_tree, start, mid, i, d_value, child + 1);
		Segtree_update((int*)seg_tree, mid + 1, end, i, d_value, child + 2);
	}
}