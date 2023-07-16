/*
* MPI�������
* �ݹ�ʵ�ֶ����ͬʱ�����ӿ�Ŀ�������
*/
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <regex>
#include<map>
#include<cmath>
#include<cstring>
#include<chrono>
#include<ctime>

using namespace std;
using namespace std::chrono;

long verlen;
int datanum; // ������
vector<string> Data;

int read_in_file(string file_name);
int CompareString(int x, int y);
int CompareResult(int x, int y);
void quickSort(vector<string>* data, int start, int end);
int partition(vector<string>* data, int start, int end);
void para_quickSort(vector<string>* data, int start, int end, int whi_m, int id, int now_id);

int main(int argc, char* argv[]) {

    //��ʼ���߳�
    int num_procs, now_id;
    string file_name;
    MPI_Init(&argc, &argv); // ��ʼ��MPI
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // ��ȡ��������
    MPI_Comm_rank(MPI_COMM_WORLD, &now_id); // ��ȡ��ǰ���̵�����

    if (now_id == 0) {
        auto start = system_clock::now();
        //��ȡ����
        cout << "���������ݼ�·����" << endl;
        //printf("���������ݼ�·����\n");
        cin >> file_name;
        datanum = read_in_file(file_name);
    }
    int whi_m = log2(num_procs);
    // ͬ��������
    MPI_Bcast(&datanum, 1, MPI_INT, 0, MPI_COMM_WORLD); // ���������㲥�����н���

    //��������
    para_quickSort(&Data, 0, datanum - 1, whi_m, 0, now_id);

    if (now_id == 0) {
        // ������
        regex reg("[0-9]+");
        smatch re;
        bool a = regex_search(file_name, re, reg);
        string result = re[0];

        string result_path = "../202164700523_hw3_" + result + ".txt";
        ofstream out(result_path);

        // ����ͬ���ַ������
        int count = 1; // �鳤��
        map<int, int> counts;
        out << Data[0] << " ";
        for (int i = 1; i < datanum; i++) {
            if (CompareResult(i, i - 1) != 0) {
                map<int, int>::iterator it = counts.find(count);
                if (it != counts.end())
                    (it->second) += 1;
                else
                    counts.insert(pair<int, int>(count, 1));

                out << endl;
                out << Data[i] << " ";

                count = 1;
            }
            else {
                out << Data[i] << " ";
                count++;
            }
        }
        out << endl;
        map<int, int>::iterator iter = counts.find(count);
        if (iter != counts.end())
            (iter->second) += 1;
        else
            counts.insert(pair<int, int>(count, 1));

        // ������ֳ��ȵ���ĸ���
        for (auto it = counts.begin(); it != counts.end(); it++) {
            out << "�ַ�����Ϊ��" << it->first << "  ����Ϊ��" << it->second << endl;
        }

        out.close();

        //���ʱ��
        auto end = system_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        double time = double(duration.count()) * microseconds::period::num / microseconds::period::den;
        printf("�����%s.txt ����ʱ��Ϊ�� %.4f seconds\n", result.c_str(), time);
    }

    MPI_Finalize(); // ����MPI
    return 0;
}

int CompareResult(int x, int y) {
    string a = Data[x];
    string b = Data[y];
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());
    if (a < b)
        return -1;
    if (a > b)
        return 1;
    return 0;
}

int CompareString(int x, int y) {
    string a = Data[x];
    string b = Data[y];
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());
    if (a < b)
        return -1;
    if (a > b)
        return 1;
    return 0;
}
//n:datanum num:NUMS
//lenArray:datanum numThread:verlen
//cutoff:verlen
void quickSort(vector<string>* data, int start, int end) {

    if (start < end) {
        int mid = partition(data, start, end);
        quickSort(data, start, mid - 1);
        quickSort(data, mid + 1, end);
    }
}
//int sta,int end,int whi_m,int id,int now_id
//start:0 end:datanum-1 whi_m:log2(num) id:0 now_id:id
void para_quickSort(vector<string>* data, int start, int end, int whi_m, int id, int now_id) {
    //index:whi_r length_l:whi_j length_r whi_i start:sta
    int index, length_r, length_l;
    index = length_r = length_l = -1;
    MPI_Status status;
    vector<string>* tempdata{};
    //ʣ�ദ����Ϊ0ʱ�����д��п�������
    if (whi_m == 0 && now_id == id) {
        quickSort(data, start, end);
        return;
    }
    //�ɵ�ǰ���������зֿ�
    if (now_id == id) {
        index = partition(data, start, end);
        length_r = end - index;//�Ұ벿��ȡֵ
        MPI_Send(&length_r, 1, MPI_INT, id + pow(2, whi_m - 1), now_id, MPI_COMM_WORLD);
        if (length_r != 0) {
            for (int i = index + 1; i < index + 1 + length_r; ++i) {
                int data_size = (*data)[i].size() + 1;
                MPI_Send(&data_size, 1, MPI_INT, id + pow(2, whi_m - 1), now_id, MPI_COMM_WORLD);
                MPI_Send((*data)[i].c_str(), data_size, MPI_CHAR, id + pow(2, whi_m - 1), now_id, MPI_COMM_WORLD);
            }
        }
    }
    //��Ϊ��������Ϣ��ģ��
    if (now_id == id + pow(2, whi_m - 1)) {
        MPI_Recv(&length_r, 1, MPI_INT, id, id, MPI_COMM_WORLD, &status);
        if (length_r != 0) {
            for (int i = 0; i < length_r; ++i) {
                int temp_size;
                MPI_Recv(&temp_size, 1, MPI_INT, id, id, MPI_COMM_WORLD, &status);
                char* buffer = new char[temp_size];
                MPI_Recv(buffer, temp_size, MPI_CHAR, id, id, MPI_COMM_WORLD, &status);
                (*tempdata).at(i) = buffer;
                delete[]buffer;
                //MPI_Recv((*tempdata)[i].c_str(), (*tempdata)[i].size(), MPI_CHAR, id, id, MPI_COMM_WORLD, &status);
            }
        }
    }
    //����벿�����ݽ���ݹ�����
    length_l = index - start - 1;//��벿��ȡֵ
    MPI_Bcast(&length_l, 1, MPI_INT, id, MPI_COMM_WORLD);//�㲥���ݳ���
    if (length_l > 0) {
        para_quickSort(data, start, index - 1, whi_m - 1, id, now_id);
    }
    //���Ұ벿�����ݽ���ݹ�����
    MPI_Bcast(&length_r, 1, MPI_INT, id, MPI_COMM_WORLD);
    if (length_r > 0) {
        para_quickSort(tempdata, 0, length_r - 1, whi_m - 1, id + pow(2, whi_m - 1), now_id);
    }
    // �����Ұ벿���ź���,�ش��ⲿ������
    if ((now_id == id + pow(2, whi_m - 1)) && (length_r != 0)) {
        for (int i = 0; i < length_r; ++i) {
            int temp_size = (*tempdata)[i].size() + 1;
            MPI_Send(&temp_size, 1, MPI_INT, id, id + pow(2, whi_m - 1), MPI_COMM_WORLD);
            MPI_Send((*tempdata)[i].c_str(), temp_size, MPI_CHAR, id, id + pow(2, whi_m - 1), MPI_COMM_WORLD);
            //MPI_Send(&((*tempdata)[i]), (*tempdata)[i].length(), MPI_CHAR, id, id + pow(2, whi_m - 1), MPI_COMM_WORLD);
        }
    }
    //���ջش�����
    if ((now_id == id) && (length_r != 0)) {
        for (int i = index + 1; i < index + 1 + length_r; ++i) {
            int data_size;
            MPI_Recv(&data_size, 1, MPI_INT, id + pow(2, whi_m - 1), id + pow(2, whi_m - 1), MPI_COMM_WORLD, &status);
            char* buffer = new char[data_size];
            MPI_Recv(buffer, data_size, MPI_CHAR, id + pow(2, whi_m - 1), id + pow(2, whi_m - 1), MPI_COMM_WORLD, &status);
            (*data).at(i) = buffer;
            delete[]buffer;
            // MPI_Recv(&((*data)[i]), (*data)[i].length(), MPI_CHAR, id + pow(2, whi_m - 1), id + pow(2, whi_m - 1), MPI_COMM_WORLD, &status);
        }
    }
}

int partition(vector<string>* data, int start, int end) {
    //ȷ����start��<end
    int base_index = end;
    int left = start, right = end - 1;
    while (true) {
        while ((CompareString(left, base_index) == -1) && (left < right))
            ++left;
        while ((CompareString(right, base_index) > -1) && (left < right))
            --right;
        if (left >= right)
            break;
        swap((*data)[left], (*data)[right]);
    }

    if (CompareString(left, end) > -1)
        swap((*data)[left], (*data)[end]);
    else
        ++left;

    return left;
}

int read_in_file(string file_name) {
    ifstream fin(file_name);
    if (!fin.is_open()) {
        cerr << "Cannot open the file" << endl;
        MPI_Finalize();
        exit(1);
    }

    string line;
    int num = 0;
    while (getline(fin, line)) {
        Data.push_back(line);
        num++;
    }
    fin.close();
    return num;
}
