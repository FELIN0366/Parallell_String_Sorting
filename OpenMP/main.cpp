
/*
* OpenMP 并行输出
* 递归实现多线程同时进行子块的快速排序
*/
#include<omp.h>
#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<map>
#include<chrono>
#include<ctime>
#include<algorithm>
#include<regex>

using namespace std;
using namespace std::chrono;

#define NUMS 2
long verlen; //每个线程的处理数量
int datanum;//数据量
vector<string> Data;

void parrallel();
int read_in_file(string file_name);
int CompareString(int x, int y);
int CompareResult(int x, int y);
void quickSort(int start, int end);

int main() {


	//读入指令 = "../../datasets/15611.txt"
	string file_name;
	printf("请输入数据集路径：\n");
	cin >> file_name;

	auto start = system_clock::now();

	datanum = read_in_file(file_name);
	verlen = datanum / NUMS;

	
	
	parrallel();


	//auto end = system_clock::now();
	//auto duration = duration_cast<microseconds>(end - start);
	//double time = double(duration.count()) * microseconds::period::num / microseconds::period::den;

	regex reg("[0-9]+");
	smatch re;
	bool a = regex_search(file_name, re, reg);
	string  result = re[0];

	//输出结果
	string result_path = "../202164700523_hw2_" + result + ".txt";
	fstream out;
	out.open(result_path, ios::out | ios::app);

	//计算同组字符串结果
	int count = 1; //组长度
	map<int, int>counts;
	out << Data[0] << " ";
	for (int i = 1; i < Data.size(); i++) {
		if (CompareResult(i, i - 1) != 0) {

			map<int, int>::iterator it = counts.find(count);
			if (it != counts.end()) (it->second) += 1;
			else counts.insert(pair<int, int>(count, 1));

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
	if (iter != counts.end()) (iter->second) += 1;
	else counts.insert(pair<int, int>(count, 1));

	//输出各种长度的组的个数
	map<int, int>::iterator it;
	for (it = counts.begin(); it != counts.end(); it++) {
		out << "字符长度为：" << (it->first) << "  数量为：" << (it->second) << endl;
	}

	//输出时间
	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start);
	double time = double(duration.count()) * microseconds::period::num / microseconds::period::den;
	printf("结果：%s.txt 运行时间为： %.4f seconds\n", result.c_str(), time);
}

void parrallel() {
#pragma omp parallel num_threads(NUMS) //指定线程数的数量
	{
#pragma omp single //串行执行
		{
			quickSort(0,datanum-1);
		}
	}
}


int CompareResult(int x, int y) {
	//x<y -1;
	//x>y 1;
	// x==y 0;
	//相加，不等，输出
	if (y == -1) return -1;
	string a = Data[x];
	string b = Data[y];
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

int CompareString(int x, int y) {
	//x<y -1;
	//x>y 1;
	// x==y 0;
	//相加，不等，输出
	if (y == -1) return -1;
	string a = Data[x];
	string b = Data[y];
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

//n:datanum num:NUMS
//lenArray:datanum numThread:verlen
//cutoff:verlen
void quickSort(int start, int end) {
	
	if (start >= end) return;
	int base_index = end;
	int left = start, right = end - 1;
	while (true) {
		while ((CompareString(left, base_index) == -1) && (left < right))
			++left;
		while ((CompareString(right, base_index) > -1) && (left < right))
			--right;
		if (left >= right)
			break;
		swap(Data[left], Data[right]);
	}
	
	if (CompareString(left, end) > -1)
		swap(Data[left], Data[end]);
	else
		++left;
#pragma omp parallel sections
	{
#pragma omp section
		{	
			quickSort(start, left - 1);
		}
#pragma omp section
		{	
			quickSort(left + 1, end);
		}
	}
}

int read_in_file(string file_name) {

	//读入文件
	ifstream fin;
	fin.open(file_name, ios::in);
	if (!fin.is_open()) {
		cerr << "Cannot open the file";
	}
	char line[2024] = { 0 };
	int num = 0;
	while (fin.getline(line, sizeof(line))) {
		Data.push_back(line);
		num++;
	}
	fin.close();
	return num;
}

