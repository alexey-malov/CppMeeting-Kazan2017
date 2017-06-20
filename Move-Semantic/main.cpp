#include "Handle.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Person {
  string name;
  string surname;
};

struct Department {
  string name;
  vector<Person> employees;
};

Department ReadDepartment(istream &input) {
  Department department;
  Person p;
  if (getline(input, department.name)) {
    string line;
    while (getline(input, line)) {
      istringstream strm(line);
      if (strm >> p.name && strm >> p.surname)
        department.employees.emplace_back(move(p));
    }
  }
  return department;
}

FileHandle SafeOpenFile(LPCWSTR fileName) {
  FileHandle fh(CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr,
                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
  if (!fh)
    throw runtime_error("Failed to open file");
  return fh;
}

ifstream OpenFileStream(const char *path) {
  ifstream strm(path);
  return strm;
}

struct Widget1 {
  Widget1(const string &name) : m_name(name) {}

private:
  string m_name;
};

struct Widget {
  Widget(string name) : m_name(move(name)) {}

private:
  string m_name;
};

int main() {
  {
    Widget1 w1("Name");
    Widget w2("Name");
	string widgetName = "w1";
	
	Widget w3(widgetName + "1");
  }

  {
    auto f = SafeOpenFile(L"main.cpp");
    DWORD fileSize = GetFileSize(f, nullptr);
    if (fileSize != INVALID_FILE_SIZE)
      cout << "File size: " << fileSize << "\n";
  }

  {
    istringstream strm("Development Team\nIvanov Ivan\n"
                       "Petrov Petr\nNikolaev Nikolay\n");
    Department department;
    department = ReadDepartment(strm);
    cout << department.name
         << " employee count: " << department.employees.size() << "\n";
  }
}