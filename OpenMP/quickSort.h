#include "omp.h"
//�����������
void rands(int* data, int sum);
//��������
void sw(int* a, int* b);
//��2��n����
int exp2(int wht_num);
//��log2(n)
int log2(int wht_num);
//�ϲ��������������
void mergeList(int* c, int* a, int sta1, int end1, int* b, int sta2, int end2);
//���п�������
int partition(int* a, int sta, int end);
void quickSort(int* a, int sta, int end);
//openMP(8)���п�������
void quickSort_parallel(int* array, int lenArray, int numThreads);
void quickSort_parallel_internal(int* array, int left, int right, int cutoff);

void rands(int* data, int sum) {
    int i;
    for (i = 0; i < sum; i++)
    {
        data[i] = rand() % 100000000;
    }
}

void sw(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}
int exp2(int wht_num) {
    int wht_i;
    wht_i = 1;
    while (wht_num > 0)
    {
        wht_num--;
        wht_i = wht_i * 2;
    }
    return wht_i;
}
int log2(int wht_num) {
    int wht_i, wht_j;
    wht_i = 1;
    wht_j = 2;
    while (wht_j < wht_num)
    {
        wht_j = wht_j * 2;
        wht_i++;
    }
    if (wht_j > wht_num)
        wht_i--;
    return wht_i;
}
int partition(int* a, int sta, int end) {
    int i = sta, j = end + 1;
    int x = a[sta];
    while (1)
    {
        while (a[++i] < x && i < end);
        while (a[--j] > x);
        if (i >= j)
            break;
        sw(&a[i], &a[j]);
    }
    a[sta] = a[j];
    a[j] = x;
    return j;
}
//����openMP����
void quickSort_parallel(int* a, int lenArray, int numThreads) {
    int cutoff = 1000;
#pragma omp parallel num_threads(numThreads) //ָ���߳���������
    {
#pragma omp single //����ִ��
        {
            quickSort_parallel_internal(a, 0, lenArray - 1, cutoff);
        }
    }
}
void quickSort_parallel_internal(int* a, int left, int right, int cutoff) {
    int i = left, j = right;
    int tmp;
    int pivot = a[(left + right) / 2];
    //��������ָ�ֳ������֣�������С�Ҵ�
    while (i <= j)
    {
        while (a[i] < pivot)
            i++;
        while (a[j] > pivot)
            j--;
        if (i <= j) {
            tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
            i++;
            j--;
        }
    }
    //int j = partition(a,left,right);
    if (((right - left) < cutoff)) {
        if (left < j) {
            quickSort_parallel_internal(a, left, j, cutoff);
        }
        if (i < right) {
            quickSort_parallel_internal(a, i, right, cutoff);
        }
    }
    else {
#pragma omp parallel sections
        {
#pragma omp section
            {
                quickSort_parallel_internal(a, left, j, cutoff);
            }
#pragma omp section
            {
                quickSort_parallel_internal(a, i, right, cutoff);
            }
            /*  task�ǡ���̬����������ģ������й����У�
                ֻ��Ҫʹ��task�ͻᶨ��һ������
                ����ͻ���һ���߳���ȥִ�У���ô����������Ϳ��Բ��е�ִ�С�
                ����ĳһ������ִ����һ���ʱ�򣬻�������Ҫִ�����ʱ��
                �������ȥ�����ڶ�������������һ���߳���ȥִ�У�һ����̬�Ĺ���
            */
            //���������ٽ��в��е��߳�����
        }
    }
    
}
//�ϲ����������������
void mergeList(int* c, int* a, int sta1, int end1, int* b, int sta2, int end2) {
    int a_index = sta1; // ��������a���±�
    int b_index = sta2; // ��������b���±�
    int i = 0;          // ��¼��ǰ�洢λ��
    //int *c;
    //c = (int *)malloc(sizeof(int) * (end1 - sta1 + 1 + end2 - sta2 + 1));
    while (a_index < end1 && b_index < end2) {
        if (a[a_index] <= b[b_index]) {
            c[i] = a[a_index];
            a_index++;
        }
        else {
            c[i] = b[b_index];
            b_index++;
        }
        i++;
    }
    while (a_index < end1) {
        c[i] = a[a_index];
        i++;
        a_index++;
    }
    while (b_index < end2) {
        c[i] = b[b_index];
        i++;
        b_index++;
    }
}
//���п�������
void quickSort(int* a, int sta, int end)
{
    if (sta < end) {
        //printf("3\n");
        int mid = partition(a, sta, end);
        quickSort(a, sta, mid - 1);
        quickSort(a, mid + 1, end);
    }
}
