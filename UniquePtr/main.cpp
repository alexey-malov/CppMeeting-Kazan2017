#include <memory>
#include <string>
#include <windows.h>
#include <shlobj.h>
#include <experimental/filesystem>
#include <cassert>

namespace fs = std::experimental::filesystem;
using namespace std;

void OpenFolderAndSelectItem(const fs::path& filePath) {
	auto createFromPath = [](const fs::path& path) {
		using Ptr = unique_ptr<ITEMIDLIST, decltype(&ILFree)>;
		return Ptr(ILCreateFromPathW(path.c_str()), &ILFree);
	};
		
	auto dir = createFromPath(filePath.parent_path());
	auto file = createFromPath(filePath);
	LPCITEMIDLIST selection = file.get();
	auto hr = SHOpenFolderAndSelectItems(dir.get(), 1, &selection, 0);
	assert(SUCCEEDED(hr));
}

int main() {
	CoInitialize(NULL);
	OpenFolderAndSelectItem(L"c:\\windows\\system32\\notepad.exe");
	CoUninitialize();
}
