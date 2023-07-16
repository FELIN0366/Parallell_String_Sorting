//�������
//���ַ������飬�������ַ������飬�ַ���ͬ��˳��ͬ
//�㷨һ����ÿ���ַ����մ�С���������ٰ�����ͬ�����
//���в��֣�ÿ���̶߳�һ���������ַ�������������������ֵ�����
//Ҫ�󣺱���Ϊ.out����,���������в���,���ʵ����
//ʵ����Ҫ��:��������ÿ�鳤�ȣ������ı��ļ�
#define _CRT_SECURE_NO_WARNINGS
#include<fstream>
#include <iostream>
#include<vector>
#include<algorithm>
#include<cstring>
#include<map>
#include<chrono>
#include<ctime>
#include<regex>
using namespace std::chrono;
using namespace std;

struct DATA {
	string alpha;
	string test;
	DATA(string d):alpha(d),test(d){}
};
bool cmp_data(DATA* x, DATA* y) {
	//С����ǰ�棬����test����
	sort((x->test).begin(), (x->test).end());
	sort((y->test).begin(), (y->test).end());
	return (x->test) < (y->test);
}

int main() {
	//"../../15611.txt"
	string file_name;
	printf("���������ݼ�·����\n");
	cin >> file_name;

	auto start = system_clock::now();

	regex reg("[0-9]+");
	smatch re;
	bool a = regex_search(file_name, re, reg);
	string  result = re[0];

	//�����ļ�
	ifstream in;
	in.open(file_name, ios::in);
	if (!in.is_open()) {
		cerr << "cannot open the file";
	}
	char line[1024] = { 0 };
	vector<DATA*> data;
	while (in.getline(line, sizeof(line))) {
		data.push_back(new DATA(line));
	}

	//���մ�С��������
	sort(data.begin(), data.end(), cmp_data);

	string path = "../../202164700523_hw0_" + result + ".txt";
	fstream out;
	out.open(path, ios::out | ios::app);

	//���ͬ���ַ������
	int num = 1;
	map<int, int> nums;
	//out << data[0]->alpha << " ";
	for (int i = 1; i < data.size(); i++) {
			if ((data[i]->test) != (data[i - 1]->test) ){

				map<int, int>::iterator it= nums.find(num);
				if (it!=nums.end()) (it->second) += 1;
				else nums.insert(pair<int, int>(num, 1));

				//out << endl;
				//out << data[i]->alpha << " ";

				num = 1;
			}
			else {
				//out << data[i]->alpha << " ";
				num++;
			}
	}
	//out << endl;

	//������ֳ��ȵ���ĸ���
	map<int, int>::iterator it;
	for (it = nums.begin(); it != nums.end(); it++) {
		out << "�ַ�����Ϊ��" << (it->first) << "  ����Ϊ��" << (it->second)<<endl;
	}

	out.close();

	//���ʱ��
	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start);
	double time = double(duration.count()) * microseconds::period::num / microseconds::period::den;
	printf("�����%s.txt ����ʱ��Ϊ�� %.4f seconds\n", result.c_str(), time);
	

		/*
	//char*����Ҫ�����ַ�ռ伴�ɣ�string����char[]��Ҫ�����ݿռ�
	char* line=new char();
	vector<char*> data;
	//data���p�ĵ�ַ,����pת��������ֵʱ��data�е�ֵҲ�ᷢ���ı�
	while (in.getline(line, sizeof(line))) {
		char* p = new char();
		strcpy((char*)p, (char*)line);
		data.push_back(p);
	}
	*/
}