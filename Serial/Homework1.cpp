//串型输出
//给字符串数组，将所有字符串分组，字符相同但顺序不同
//算法一：将每个字符按照从小到大排序，再按照相同的输出
//并行部分：每个线程对一定数量的字符进行排序，再总体进行字典排序
//要求：编译为.out程序,接收命令行参数,输出实验结果
//实验结果要求:分组结果，每组长度，生成文本文件
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
	//小的排前面，更新test内容
	sort((x->test).begin(), (x->test).end());
	sort((y->test).begin(), (y->test).end());
	return (x->test) < (y->test);
}

int main() {
	//"../../15611.txt"
	string file_name;
	printf("请输入数据集路径：\n");
	cin >> file_name;

	auto start = system_clock::now();

	regex reg("[0-9]+");
	smatch re;
	bool a = regex_search(file_name, re, reg);
	string  result = re[0];

	//读入文件
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

	//按照从小到大排序
	sort(data.begin(), data.end(), cmp_data);

	string path = "../../202164700523_hw0_" + result + ".txt";
	fstream out;
	out.open(path, ios::out | ios::app);

	//输出同组字符串结果
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

	//输出各种长度的组的个数
	map<int, int>::iterator it;
	for (it = nums.begin(); it != nums.end(); it++) {
		out << "字符长度为：" << (it->first) << "  数量为：" << (it->second)<<endl;
	}

	out.close();

	//输出时间
	auto end = system_clock::now();
	auto duration = duration_cast<microseconds>(end - start);
	double time = double(duration.count()) * microseconds::period::num / microseconds::period::den;
	printf("结果：%s.txt 运行时间为： %.4f seconds\n", result.c_str(), time);
	

		/*
	//char*仅需要分配地址空间即可，string或者char[]需要存数据空间
	char* line=new char();
	vector<char*> data;
	//data获得p的地址,但当p转换成其他值时，data中的值也会发生改变
	while (in.getline(line, sizeof(line))) {
		char* p = new char();
		strcpy((char*)p, (char*)line);
		data.push_back(p);
	}
	*/
}