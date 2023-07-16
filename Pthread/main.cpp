/*
* P_Thread 并行输出
* 排序：字符串ascall码总和排序
* 读入文件--》并行：排序各自段的字母-->merge--》输出各种长度的个数
*/
#include<pthread.h>
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

struct Section { //用于存储各线程的起末点
	int left;
	int right;
	Section() {};
};

#define NUMTHRDS 7 //线程数量
long verlen; //每个线程的处理数量
vector<string> Data,SortedData; //字符数据
vector<Section*>Sections;	//线程分割块
pthread_t callThd[NUMTHRDS];	//线程
pthread_barrier_t barrier;//指定要等待的线程数

int read_in_file(string file_name);
int CompareString(int x, int y);
int CompareResult(int x, int y);
void quickSort(int start, int end);
void merge(int num);
void* p_sort(void* arg);

int main() {
	

	//读入指令 = "../../../datasets/15611.txt"
	string file_name;
	printf("请输入数据集路径：\n");
	cin >> file_name;

	auto start = system_clock::now();

	regex reg("[0-9]+");
	smatch re;
	bool a= regex_search(file_name,re,reg);
	string  result= re[0];

	int num=read_in_file(file_name);
	verlen = num / NUMTHRDS;

	pthread_t tid;
	pthread_barrier_init(&barrier, NULL, NUMTHRDS + 1);
	

	for (int i = 0; i < NUMTHRDS; ++i) {
		Section* section = new Section;
		section->left = i * verlen;
		if (i == NUMTHRDS - 1) { section->right = num - 1; }
		else { section->right = (i + 1) * verlen - 1; }
		Sections.push_back(section);

		int status = pthread_create(&tid, NULL, p_sort, (void*)(section));
		if (status != 0) {
			cout << "Create Thread error." << endl;
			return -1;
		}
	}

	//main thread wait
	pthread_barrier_wait(&barrier);

	merge(num);

	//输出结果
	string result_path = "../202164700523_hw1_" + result + ".txt";
	fstream out;
	out.open(result_path, ios::out | ios::app);

	//计算同组字符串结果
	int count = 1; //组长度
	map<int, int>counts;
	out << SortedData[0] << " ";
	for (int i = 1; i < SortedData.size(); i++) {
		if (CompareResult(i,i-1)!=0) {

			map<int, int>::iterator it = counts.find(count);
			if (it != counts.end()) (it->second) += 1;
			else counts.insert(pair<int, int>(count, 1));

			out << endl;
			out << SortedData[i] << " ";

			count = 1;
		}
		else {
			out << SortedData[i] << " ";
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
void merge(int num) {
	//index:section[i].left
	//index_most:section[i].right+1
	//sortnumperthread:verlen
	for (int i = 0; i < num; ++i) {
		int min_thread=-1;
		int min_index = -1;
		for (int j = 0; j < NUMTHRDS; ++j) {
			int start = Sections[j]->left;
			int end = Sections[j]->right;
			if ((start <= end) && (start < num)) {
				if (CompareString(start, min_index) == -1) {
					min_thread = j;
					min_index = start;
				}	
			}
		}
		int min_start = Sections[min_thread]->left;
		SortedData.push_back(Data[min_start]);
		Sections[min_thread]->left = (Sections[min_thread]->left)+1;
	}
}
int CompareResult(int x, int y) {
	//x<y -1;
	//x>y 1;
	// x==y 0;
	//相加，不等，输出
	if (y == -1) return -1;
	string a = SortedData[x];
	string b = SortedData[y];
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
	if(y==-1) return -1;
	string a = Data[x];
	string b = Data[y];
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());
	if (a < b) return -1;
	if (a > b) return 1;
	return 0;
}

void quickSort(int start, int end) {
	if (start >= end) return;
	int base_index = end;
	int left = start, right = end - 1;
	while (true) {
		while ((CompareString(left, base_index) == -1)&&(left<right)) 
			++left;
		while ((CompareString(right, base_index) > -1)&&(left<right))
			--right;	
		if (left >= right)
			break;
		swap(Data[left], Data[right]);
	}
	if (CompareString(left, end) > -1)
		swap(Data[left], Data[end]);
	else
		++left;
	quickSort(start, left - 1);
	quickSort(left + 1, end);
}
void* p_sort(void* arg) {
	Section* section = (Section*)arg;
	quickSort(section->left, section->right);

	//等待其他线程
	pthread_barrier_wait(&barrier);
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
