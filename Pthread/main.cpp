/*
* P_Thread �������
* �����ַ���ascall���ܺ�����
* �����ļ�--�����У�������Զε���ĸ-->merge--��������ֳ��ȵĸ���
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

struct Section { //���ڴ洢���̵߳���ĩ��
	int left;
	int right;
	Section() {};
};

#define NUMTHRDS 7 //�߳�����
long verlen; //ÿ���̵߳Ĵ�������
vector<string> Data,SortedData; //�ַ�����
vector<Section*>Sections;	//�̷ָ߳��
pthread_t callThd[NUMTHRDS];	//�߳�
pthread_barrier_t barrier;//ָ��Ҫ�ȴ����߳���

int read_in_file(string file_name);
int CompareString(int x, int y);
int CompareResult(int x, int y);
void quickSort(int start, int end);
void merge(int num);
void* p_sort(void* arg);

int main() {
	

	//����ָ�� = "../../../datasets/15611.txt"
	string file_name;
	printf("���������ݼ�·����\n");
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

	//������
	string result_path = "../202164700523_hw1_" + result + ".txt";
	fstream out;
	out.open(result_path, ios::out | ios::app);

	//����ͬ���ַ������
	int count = 1; //�鳤��
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
	//��ӣ����ȣ����
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
	//��ӣ����ȣ����
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

	//�ȴ������߳�
	pthread_barrier_wait(&barrier);
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
