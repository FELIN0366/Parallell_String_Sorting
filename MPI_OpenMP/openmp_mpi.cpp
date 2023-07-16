#include <stdio.h>
#include <mpi.h>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include<chrono>
#include<ctime>
#include<vector>
#include<cstring>
#include<iostream>
#include<fstream>
#include <regex>
#include<map>
using namespace std;
using namespace std::chrono;

long verlen;
int datanum; // ������

int read_in_file(string file_name,vector<string>*data);
int partition(vector<string>* data, int start, int end);
void para_quickSort(vector<string>* data, int start, int end, int sum_id, int id, int now_id);
//openMP(8)���п�������
void QuickSort(vector<string>* data, int start, int end);
void quickSort_parallel(vector<string>* data, int lenArray, int numThreads);
void quickSort_parallel_internal(vector<string>* data, int start, int end);
int CompareString(string x, string y);
vector<string>* mergeList(vector<string>a, int sta1, int end1, vector<string>b, int sta2, int end2);
int partition(vector<string>* data, int start, int end);



int main(int argc, char* argv[])
{   
    vector<string>* data{};
    int now_id, sum_id;
    int whi_r;
    string file_name;
    auto start = system_clock::now();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &now_id);//��ǰ����
    MPI_Comm_size(MPI_COMM_WORLD, &sum_id);//��������
    whi_r = -1;

    if (now_id == 0) {
        if (numprocs != 4) {
            cout << "�ó����֧��4�����̣����������룡" << endl;
            return 0;
        }
        cout << "���������ݼ�·����" << endl;
        cin >> file_name;
        datanum = read_in_file(file_name,data);
    }
    MPI_Bcast(&datanum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    /*
        openmp+mpi��ϲ�������
        �ĸ����̣�
        ����0�㲥data1����ֿ��������ֵ�����1��2
        ����1��2���ö��߳̽��п�������
        �߳�1��2�������ź��������Send������3
        ����3���ù鲢���򣬺ϲ���������
        ��󸲸ǵ�����0
        �õ��ź��������
    */
    para_quickSort(data, 0, datanum - 1, sum_id, 0, now_id);

    //��������
    if (now_id == 0) {
        //��������
        MPI_Status status;
        vector<string>data;
        int size;
        MPI_Recv(&size, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, &status);
        int k = 0;
        for (int i = 0; i <size; ++i) {
            int dsize;
            k = k + 1;
            MPI_Recv(&dsize, 1, MPI_INT, 3, k, MPI_COMM_WORLD, &status);
            k = k + 1;
            char* buffer = new char[dsize];
            MPI_Recv(buffer,dsize,MPI_CHAR,3,k, MPI_COMM_WORLD, &status);
            data.push_back(buffer);
            delete[]buffer;
        }

        regex reg("[0-9]+");
        smatch re;
        bool a = regex_search(file_name, re, reg);
        string  result = re[0];
        //������
        string result_path = "../202164700523_hw3_openmp_mpi" + result + ".txt";
        fstream out;
        out.open(result_path, ios::out | ios::app);

        //����ͬ���ַ������
        int count = 1; //�鳤��
        map<int, int>counts;
        out << data[0] << " ";
        for (int i = 1; i < data.size(); i++) {
            if (CompareString(data[i], data[i - 1]) != 0) {

                map<int, int>::iterator it = counts.find(count);
                if (it != counts.end()) (it->second) += 1;
                else counts.insert(pair<int, int>(count, 1));

                out << endl;
                out << data[i] << " ";

                count = 1;
            }
            else {
                out << data[i] << " ";
                count++;
            }
        }
        out << endl;
        map<int, int>::iterator iter = counts.find(count);
        if (iter != counts.end()) (iter->second) += 1;
        else counts.insert(pair<int, int>(count, 1));

        //������ֳ��ȵ���ĸ���
        map<int, int>::iterator it;
        for (it = counts.begin(); it != counts.end(); it++) {
            out << "�ַ�����Ϊ��" << (it->first) << "  ����Ϊ��" << (it->second) << endl;
        }

        //���ʱ��
        auto end = system_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        double time = double(duration.count()) * microseconds::period::num / microseconds::period::den;
        printf("�����%s.txt ����ʱ��Ϊ�� %.4f seconds\n", result.c_str(), time);
    }
    MPI_Finalize();
}
//index:pivot length_r:�Ұ벿�ֳ���
void para_quickSort(vector<string>* data, int start, int end, int sum_id, int id, int now_id) {
    int index, whi_j, whi_i;
    index = whi_j = whi_i = -1;
    int length_r = -1;
    MPI_Status status;

    //������0����������룬һ�뵽����1��һ�뵽����2
    if (now_id == 0) {

        index = partition(data, start, end);
        length_r = end - index + 1;

        //��Ҫ��ɢ�����鳤�ȷ�ɢ��ָ������
        MPI_Send(&index, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);//0:����ܳ���
        MPI_Send(&length_r, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);//0���ұ��ܳ���
        MPI_Send(&index, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);//0:����ܳ���
        MPI_Send(&length_r, 1, MPI_INT, 3, 1, MPI_COMM_WORLD);//1:�ұ߳���

        //��Ҫ��ɢ������Ԫ�ط�ɢ��ָ������
        //MPI_Send(data, index, MPI_INT, 1, 0, MPI_COMM_WORLD);
        //MPI_Send(data + index + 1, length_r, MPI_INT, 2, 0, MPI_COMM_WORLD);
        int k = 0;//����0��1��ʼ�������ݸ�����1
        for (int i = 0; i < index; ++i) {
            int datasize = (*data)[i].size();
            k = k + 1;
            MPI_Send(&datasize, 1, MPI_INT, 1, k, MPI_COMM_WORLD);
            k = k + 1;
            MPI_Send((*data)[i].c_str(), datasize, MPI_CHAR, 1, k, MPI_COMM_WORLD);
        }
        int j = 0;//����0��1��ʼ�������ݸ�����2
        for (int i = index + 1; i < index + 1 + length_r; ++i) {
            int datasize = (*data)[i].size();
            j = j + 1;//��1������
            MPI_Send(&datasize, 1, MPI_INT, 2, j, MPI_COMM_WORLD);
            j = j + 1;
            MPI_Send((*data)[i].c_str(), datasize, MPI_CHAR, 2, j, MPI_COMM_WORLD);
        }
    }
    if (now_id == 2) {
        //�������Խ���0�����鳤��
        MPI_Recv(&length_r, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        //�������Խ���0������Ԫ��
        vector<string>* tempdata1{};
        int j = 0;
        for (int i = 0; i < length_r; ++i) {
            int temp_size;
            j = j + 1;
            MPI_Recv(&temp_size, 1, MPI_INT, 0, j, MPI_COMM_WORLD, &status);
            char* buffer = new char[temp_size];
            j = j + 1;
            MPI_Recv(buffer, temp_size, MPI_CHAR, 0, j, MPI_COMM_WORLD, &status);
            (*tempdata1).at(i) = buffer;
            delete[]buffer;
        }
        //tmp1 = (int*)malloc(sizeof(int) * length_r);
        //MPI_Recv(tmp1, length_r, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        //������ж��̵߳Ŀ�������(�Ұ벿�֣�
        QuickSort(tempdata1, 0, length_r - 1);
        //���ź���������鷢�͵�����3
        //MPI_Send(tmp1, length_r, MPI_INT, 3, 2, MPI_COMM_WORLD);
        int k = 0;//��1��ʼ���͸�����3
        for (int i = 0; i < length_r; ++i) {
            int tempsize = (*tempdata1)[i].size();
            k = k + 1;
            MPI_Send(&tempsize, 1, MPI_INT, 3, k, MPI_COMM_WORLD);
            k = k + 1;
            MPI_Send((*tempdata1)[i].c_str(), tempsize, MPI_CHAR, 3, k, MPI_COMM_WORLD);
        }
    }
    if (now_id == 1) {
        //��������0�����鳤��
        MPI_Recv(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        //�������Խ���0������Ԫ��
        int j = 0;
        vector<string>* tempdata2{};
        for (int i = 0; i < length_r; ++i) {
            int temp_size;
            j = j + 1;
            MPI_Recv(&temp_size, 1, MPI_INT, 0, j, MPI_COMM_WORLD, &status);
            char* buffer = new char[temp_size];
            j = j + 1;
            MPI_Recv(buffer, temp_size, MPI_CHAR, 0, j, MPI_COMM_WORLD, &status);
            (*tempdata2).at(i) = buffer;
            delete[]buffer;
        }
        //tmp2 = (int*)malloc(sizeof(int) * index);
        //MPI_Recv(tmp2, index, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        //������ж��̵߳Ŀ�������(��벿�֣�
        QuickSort(tempdata2, 0, index - 1);
        //���ź���������鷢������3
        int k = 0;//��1��ʼ����
        for (int i = 0; i < index; ++i) {
            int tempsize = (*tempdata2)[i].size();
            k = k + 1;
            MPI_Send(&tempsize, 1, MPI_INT, 3, k, MPI_COMM_WORLD);
            k = k + 1;
            MPI_Send((*tempdata2)[i].c_str(), tempsize, MPI_CHAR, 3, k, MPI_COMM_WORLD);
        }
        //MPI_Send(tmp2, index, MPI_INT, 3, 1, MPI_COMM_WORLD);
    }
    if (now_id == 3) {
        //�������Խ���0�Ļ��ֵ�������ĳ���
        MPI_Recv(&length_r, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&index, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        //�������Խ���1��2���ź����������Ԫ��
        vector<string>* tempdata3{};
        int j = 0;
        for (int i = 0; i < index; i++) {
            int tempsize;
            j = j + 1;
            MPI_Recv(&tempsize, 1, MPI_INT, 1, j, MPI_COMM_WORLD, &status);
            char* buf = new char[tempsize];
            MPI_Recv(buf, tempsize, MPI_CHAR, 1, j, MPI_COMM_WORLD, &status);
            (*tempdata3).at(i) = buf;
            delete[]buf;
        }
        vector<string>* tempdata4{};
        int k = 0;
        for (int i = 0; i < length_r; i++) {
            int tempsize;
            k = k + 1;
            MPI_Recv(&tempsize, 1, MPI_INT, 2, k, MPI_COMM_WORLD, &status);
            char* buf = new char[tempsize];
            MPI_Recv(buf, tempsize, MPI_CHAR, 2, k, MPI_COMM_WORLD, &status);
            (*tempdata4).at(i) = buf;
            delete[]buf;
        }
        //tmp3 = (int*)malloc(sizeof(int) * index);
        //tmp4 = (int*)malloc(sizeof(int) * length_r);
        //tmp5 = (int*)malloc(sizeof(int) * (length_r + index) + 1);
        //MPI_Recv(tmp3, index, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
        //MPI_Recv(tmp4, length_r, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
        //�ϲ�����������
        vector<string>* tempdata5{};
        tempdata5 = mergeList(*tempdata3, 0, index, *tempdata4, 0, length_r);
        //�ش����������0
        int size = (*tempdata5).size();
        MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        int c = 0;
        for (int i = 0; i < size; ++i) {
            int dsize = (*tempdata5)[i].size();
            c = c + 1;
            MPI_Send(&dsize, 1, MPI_INT, 0, c, MPI_COMM_WORLD);
            c = c + 1;
            MPI_Send((*tempdata5)[i].c_str(), dsize, MPI_CHAR, 0, c, MPI_COMM_WORLD);
        }
    }
}
int partition(vector<string>* data, int start, int end) {
    //ȷ����start��<end
    int base_index = end;
    int left = start, right = end - 1;
    while (true) {
        while ((CompareString((*data)[left], (*data)[base_index]) == -1) && (left < right))
            ++left;
        while ((CompareString((*data)[right], (*data)[base_index]) > -1) && (left < right))
            --right;
        if (left >= right)
            break;
        swap((*data)[left], (*data)[right]);
    }

    if (CompareString((*data)[left], (*data)[end]) > -1)
        swap((*data)[left], (*data)[end]);
    else
        ++left;

    return left;
}

//�߳�openmp����������в��п���
//tempdata:0-(length-1)
void QuickSort(vector<string>*data, int start, int end) {
    //�������Ұ벿��ʹ��8���߳̽�������
    quickSort_parallel(data, end - start + 1, 8);
}

//����openMP����
void quickSort_parallel(vector<string>*data, int lenArray, int numThreads) {
#pragma omp parallel num_threads(numThreads) //ָ���߳���������
    {
#pragma omp single //����ִ��
        {
            quickSort_parallel_internal(data, 0, lenArray - 1);
        }
    }
}
void quickSort_parallel_internal(vector<string>*data, int start, int end) {
        if (start >= end) return;
        int base_index = end;
        int left = start, right = end - 1;
        while (true) {
            while ((CompareString((*data)[left], (*data)[base_index]) == -1) && (left < right))
                ++left;
            while ((CompareString((*data)[right], (*data)[base_index]) > -1) && (left < right))
                --right;
            if (left >= right)
                break;
            swap((*data)[left], (*data)[right]);
        }

        if (CompareString((*data)[left], (*data)[end]) > -1)
            swap((*data)[left], (*data)[end]);
        else
            ++left;
#pragma omp parallel sections
        {
#pragma omp section
            {
                quickSort_parallel_internal(data,start, left - 1);
            }
#pragma omp section
            {
                quickSort_parallel_internal(data,left + 1, end);
            }
        }
}
int CompareString(string x, string y) {
    //x<y -1;
    //x>y 1;
    // x==y 0;
    //��ӣ����ȣ����;
    sort(x.begin(), x.end());
    sort(y.begin(), y.end());
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}
//�ϲ����������������
vector<string>* mergeList(vector<string>a, int sta1, int end1, vector<string>b, int sta2, int end2) {
    int a_index = sta1; // ��������a���±�
    int b_index = sta2; // ��������b���±�
    //int *c;
    //c = (int *)malloc(sizeof(int) * (end1 - sta1 + 1 + end2 - sta2 + 1));
    vector<string>c;
    while (a_index < end1 && b_index < end2) {
        if(CompareString(a[a_index], b[b_index])<=0){
            c.push_back( a[a_index]);
            a_index++;
        }
        else {
            c.push_back(b[b_index]);
            b_index++;
        }
    }
    while (a_index < end1) {
        c.push_back( a[a_index]);
        a_index++;
    }
    while (b_index < end2) {
        c.push_back( b[b_index]);
        b_index++;
    }
    return &c;
}
int read_in_file(string file_name,vector<string>*data) {
    ifstream fin(file_name);
    if (!fin.is_open()) {
        cerr << "Cannot open the file" << endl;
        MPI_Finalize();
        exit(1);
    }

    string line;
    int num = 0;
    while (getline(fin, line)) {
        (*data).push_back(line);
        num++;
    }
    fin.close();
    return num;
}
