
/*
* OpenMP �������
* �ݹ�ʵ�ֶ��߳�ͬʱ�����ӿ�Ŀ�������
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
long verlen; //ÿ���̵߳Ĵ�������
int datanum;//������
vector<string> Data;

void parrallel();
int read_in_file(string file_name);
int CompareString(int x, int y);
int CompareResult(int x, int y);
void quickSort(int start, int end);

int main() {


	//����ָ�� = "../../datasets/15611.txt"
	string file_name;
	printf("���������ݼ�·����\n");
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

	//������
	string result_path = "../202164700523_hw2_" + result + ".txt";
	fstream out;
	out.open(result_path, ios::out | ios::app);

	//����ͬ���ַ������
	int count = 1; //�鳤��
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

void parrallel() {
#pragma omp parallel num_threads(NUMS) //ָ���߳���������
	{
#pragma omp single //����ִ��
		{
			quickSort(0,datanum-1);
		}
	}
}


int CompareResult(int x, int y) {
	//x<y -1;
	//x>y 1;
	// x==y 0;
	//��ӣ����ȣ����
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
	//��ӣ����ȣ����
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

	//�����ļ�
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

