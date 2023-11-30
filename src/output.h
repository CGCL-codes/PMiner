#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
using namespace std;

//同步文件管理类，只有一个，管理所有输出类
class SynchronizedFile {
public:
	SynchronizedFile(const string& path) : _path(path) {
		//打开文件
		outfile.open(_path);
	}

	void write(const string& dataToWrite) {
		//使用lock_guard加锁确保同一时间只有一个线程在写入，且写入结束后自动解锁
		std::lock_guard<std::mutex> lock(_writerMutex);

		//开始写入文件
		outfile << dataToWrite;
		outfile << endl;
	}

private:
	string _path;
	ofstream outfile;
	std::mutex _writerMutex;
};

//输出类，每个线程中包含一个，负责数据处理和调用管理类方法进行写入
class Writer {
public:
    //这里使用智能指针来管理，使得所有输出类指向管理类
	Writer(std::shared_ptr<SynchronizedFile> sf) : _sf(sf) {}

	void DataProcessing(std::vector<std::vector<unsigned>>& PMR_copy) {
		//这里进行数据处理工作，此处处理为每个结果单行输出
		string record_line = "";
        for (unsigned i = 0; i < PMR_copy.size(); ++i) {
            record_line += 'P' + to_string(i) + ": ";
            for (auto j : PMR_copy[i]) {
                record_line += to_string(j) + " ";
            }
        }
		_sf->write(record_line);
	}
	void DataProcessing(std::vector<unsigned>& PMR_copy){
		string record_line = "";
		for (unsigned i = 0; i < PMR_copy.size(); ++i){
			record_line += 'P' + to_string(i) + ": ";
			record_line += to_string(PMR_copy[i]) + " ";
		}
		_sf->write(record_line);
	}
private:
	std::shared_ptr<SynchronizedFile> _sf;
};