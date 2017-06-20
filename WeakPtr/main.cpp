#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <string>

using namespace std;

string GetFileContent(const char *fileName);

class DocumentStorage : public enable_shared_from_this<DocumentStorage> {
  using StringPtr = shared_ptr<string>;
  using StringWeakPtr = weak_ptr<string>;
  map<string, StringWeakPtr> m_items;
public:
  using DocumentContent = function<string()>;

  DocumentContent GetDocumentContent(const string &fileName) {
    StringPtr content;
    auto it = m_items.find(fileName);
    if (it != m_items.end()) {
      content = it->second.lock();
    }

    if (!content) {
      weak_ptr<DocumentStorage> weakSelf = shared_from_this();
      auto deleter = [weakSelf, fileName](string *s) {
        delete s;
        if (auto strongSelf = weakSelf.lock()) {
          strongSelf->m_items.erase(fileName);
        }
      };
      content.reset(new string(GetFileContent(fileName.c_str())), deleter);
      m_items.insert_or_assign(fileName, content);
    }

    return [content] { return *content; };
  }
};

int main() {
  auto storage = make_shared<DocumentStorage>();
  {
    auto content1 = storage->GetDocumentContent("main.cpp");
    cout << content1().length() << endl;
    auto content2 = storage->GetDocumentContent("main.cpp");
    cout << content2().length() << endl;
  }
  {
    auto content1 = storage->GetDocumentContent("main.cpp");
    cout << content1().length() << endl;
  }
}

string GetFileContent(const char *fileName) {
	cout << "Loading file content " << fileName << "\n";
	ifstream strm(fileName);
	string text;
	string line;
	while (getline(strm, line)) {
		text.append(line);
		text.append("\n");
	}
	return text;
}
