/*
* MPI并行输出
* 递归实现多进程同时进行子块的快速排序
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
int datanum; // 数据量
vector<string> Data;

int read_in_file(string file_name);
int CompareString(int x, int y);
int CompareResult(int x, int y);
void quickSort(vector<string>* data, int start, int end);
int partition(vector<string>* data, int start, int end);
void para_quickSort(vector<string>* data, int start, int end, int whi_m, int id, int now_id);

int main(int argc, char* argv[]) {

    //开始多线程
    int num_procs, now_id;
    string file_name;
    MPI_Init(&argc, &argv); // 初始化MPI
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs); // 获取进程总数
    MPI_Comm_rank(MPI_COMM_WORLD, &now_id); // 获取当前进程的排名

    if (now_id == 0) {
        auto start = system_clock::now();
        //读取数据
        cout << "请输入数据集路径：" << endl;
        //printf("请输入数据集路径：\n");
        cin >> file_name;
        datanum = read_in_file(file_name);
    }
    int whi_m = log2(num_procs);
    // 同步数据量
    MPI_Bcast(&datanum, 1, MPI_INT, 0, MPI_COMM_WORLD); // 将数据量广播给所有进程

    //并行排序
    para_quickSort(&Data, 0, datanum - 1, whi_m, 0, now_id);

    if (now_id == 0) {
        // 输出结果
        regex reg("[0-9]+");
        smatch re;
        bool a = regex_search(file_name, re, reg);
        string result = re[0];

        string result_path = "../202164700523_hw3_" + result + ".txt";
        ofstream out(result_path);

        // 计算同组字符串结果
        int count = 1; // 组长度
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

        // 输出各种长度的组的个数
        for (auto it = counts.begin(); it != counts.end(); it++) {
            out << "字符长度为：" << it->first << "  数量为：" << it->second << endl;
        }

        out.close();

        //输出时间
        auto end = system_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        double time = double(duration.count()) * microseconds::period::num / microseconds::period::den;
        printf("结果：%s.txt 运行时间为： %.4f seconds\n", result.c_str(), time);
    }

    MPI_Finalize(); // 结束MPI
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
    //剩余处理器为0时，进行串行快速排序
    if (whi_m == 0 && now_id == id) {
        quickSort(data, start, end);
        return;
    }
    //由当前处理器进行分块
    if (now_id == id) {
        index = partition(data, start, end);
        length_r = end - index;//右半部分取值
        MPI_Send(&length_r, 1, MPI_INT, id + pow(2, whi_m - 1), now_id, MPI_COMM_WORLD);
        if (length_r != 0) {
            for (int i = index + 1; i < index + 1 + length_r; ++i) {
                int data_size = (*data)[i].size() + 1;
                MPI_Send(&data_size, 1, MPI_INT, id + pow(2, whi_m - 1), now_id, MPI_COMM_WORLD);
                MPI_Send((*data)[i].c_str(), data_size, MPI_CHAR, id + pow(2, whi_m - 1), now_id, MPI_COMM_WORLD);
            }
        }
    }
    //若为被传递信息的模块
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
    //将左半部分内容进入递归排序
    length_l = index - start - 1;//左半部分取值
    MPI_Bcast(&length_l, 1, MPI_INT, id, MPI_COMM_WORLD);//广播数据长度
    if (length_l > 0) {
        para_quickSort(data, start, index - 1, whi_m - 1, id, now_id);
    }
    //将右半部分内容进入递归排序
    MPI_Bcast(&length_r, 1, MPI_INT, id, MPI_COMM_WORLD);
    if (length_r > 0) {
        para_quickSort(tempdata, 0, length_r - 1, whi_m - 1, id + pow(2, whi_m - 1), now_id);
    }
    // 已在右半部分排好序,回传这部分数据
    if ((now_id == id + pow(2, whi_m - 1)) && (length_r != 0)) {
        for (int i = 0; i < length_r; ++i) {
            int temp_size = (*tempdata)[i].size() + 1;
            MPI_Send(&temp_size, 1, MPI_INT, id, id + pow(2, whi_m - 1), MPI_COMM_WORLD);
            MPI_Send((*tempdata)[i].c_str(), temp_size, MPI_CHAR, id, id + pow(2, whi_m - 1), MPI_COMM_WORLD);
            //MPI_Send(&((*tempdata)[i]), (*tempdata)[i].length(), MPI_CHAR, id, id + pow(2, whi_m - 1), MPI_COMM_WORLD);
        }
    }
    //接收回传数据
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
    //确保了start必<end
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
