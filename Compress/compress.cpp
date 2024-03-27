#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <bitset>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
using namespace std;

// str[0] : 資料的第一個字元， true: 1開頭, false: 0開頭
bool start = false;
// 第一個位置: 壓縮後01交錯的資料 ， 第二個位置: 關係碼，方便計算關係碼大小、壓縮率
string compressAndrelation[2] = { "", "" };
// 儲存各個0、1游長值
vector<int> vec;
// 儲存各個游長的關係碼
vector<string> remainders;
// 判斷資料是否壓到極限: 呈現 01 or 10 交錯
bool limit = true;
// 寫入關係碼檔案的stream
ofstream outfile;
// 測資名稱
string fileName;

/*
計算01游長
	str : 原字串
*/
void computeLength(string str) {
	int zero = 0;
	int one = 0;
	int i = 0;

	// 因為只會計算一次游長值，計算壓縮率也會用到壓縮完的資料，所以在這邊把01交錯的資料用出來放在compressRelation的第一格
	while (i != str.size()) {
		zero = 0;
		while (str[i] == '0') {
			zero++;
			i++;
		}
		if (zero != 0) {
			compressAndrelation[0].append("0");
			vec.push_back(zero);
		}
		one = 0;
		while (str[i] == '1') {
			one++;
			i++;
		}
		if (one != 0) {
			compressAndrelation[0].append("1");
			vec.push_back(one);
		}
	}
}

/*
將資料從十進位轉二進位表示
*/
string Decimal2Binary(int n) {
	int a[20], i;
	string bin = "";
	for (i = 0; n > 0; i++)
	{
		a[i] = n % 2;
		n = n / 2;
	}
	for (i = i - 1; i >= 0; i--)
	{
		bin.append(to_string(a[i]));
	}
	return bin;
}

/*
讀取指定檔案的內容
*/
string readFileContent(string filePath) {
	std::ifstream file(filePath, std::ios::binary);

	// 處理文件內容（在這裡執行您的操作）
	// 例如，將內容轉換為二進位數並保存在變數中
	std::string str;

	if (file) {
		// 確定文件大小
		file.seekg(0, std::ios::end);
		std::streampos fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// 分配足夠的內存來存儲文件內容
		char* buffer = new char[fileSize];

		// 讀取文件內容
		file.read(buffer, fileSize);

		for (std::streampos i = 0; i < fileSize; i += 1) {
			unsigned char byte = static_cast<unsigned char>(buffer[i]);
			// 處理每個字節，可以按需求進行相應操作
			// 例如，將每個字節轉換為二進位數並保存在變數中
			std::string binaryString = std::bitset<8>(byte).to_string();
			str += binaryString;
		}

		// 釋放內存並關閉文件
		delete[] buffer;
		file.close();

		// 在這裡可以使用 binaryData 變數進行後續的處理
		// 例如，可以輸出 binaryData 或進行其他操作
		// std::cout << "轉換後的二進位數: " << binaryData << std::endl;
	}
	else {
		std::cerr << "Open File Error." << std::endl;
		exit(1);
	}

	return str;
}

/*
計算最後壓縮的資料有幾組01或10，再轉換成二進位表示
*/
string computeGroupAnd2Binary() {
	string bin = "";
	int size = compressAndrelation[0].size();
	if (start) { // 1開頭
		bin.append("1");
		// 檢查最後一個bit是0 or 1，如果是0代表剛好10配對完，1代表多出一個bit
		if (compressAndrelation[0][size - 1] == '0') {
			bin.append("0");
			size /= 2;
		}
		else {
			bin.append("1");
			size -= 1;
			size /= 2;
		}
	}
	else { // 0開頭
		bin.append("0");
		// 檢查最後一個bit是0 or 1，如果是1代表剛好01配對完，0代表多出一個bit
		if (compressAndrelation[0][size - 1] == '1') {
			bin.append("0");
			size /= 2;
		}
		else {
			bin.append("1");
			size -= 1;
			size /= 2;
		}
	}
	bin.append(Decimal2Binary(size));
	return bin;
}

/*
將關係碼 or 壓縮後字串 寫入bin檔中
	input: 要寫入的資料
	filePath: 指定檔案路徑
	i: -1代表寫入壓縮後字串，其他代表寫入關係碼
*/
void writeBinaryFile(string input, string filePath, int i) {
	// 寫入壓縮後字串
	if (i == -1) {
		outfile.open(filePath + "/compressedCode" + ".bin", ios::binary);
		// 開啟成功
		if (outfile) {
			// 將數據寫入檔案
			outfile << input;
		}
		// 開啟失敗
		else {
			cout << filePath + "/compressedCode" << ".bin can't open" << endl;
			exit(1);
		}
	}
	// 寫入關係碼
	else {
		outfile.open(filePath + "/relation" + to_string(i) + ".bin", ios::binary);
		// 開啟成功
		if (outfile) {
			// 將數據寫入檔案
			outfile << input;
		}
		// 開啟失敗
		else {
			cout << filePath + "/relation" << i << ".bin can't open" << endl;
			exit(1);
		}
	}

	// 關閉檔案
	outfile.close();
}

/*
壓縮資料與將關係碼寫入檔案的過程
	filePath: 指定檔案路徑
*/
string compressProcess(string str, string filePath) {
	cout << "Compress " << fileName << "..." << endl;
	// 計算資料的各個0、1游長值
	computeLength(str);

	// 判斷0 or 1開頭
	if (str[0] == '1') start = true;

	// 記錄游長的關係碼
	string temp = "";

	// 用for loop對每段游長進行壓縮
	for (int i = 0; i < vec.size(); i++) {
		// 游長值為1，跳過
		if (vec[i] == 1) continue;

		// 游長值大於1的才會進行處理
		while (vec[i] != 1) {
			temp.append(to_string(vec[i] % 2));
			vec[i] /= 2;
		}

		// 寫入關係碼到相對應的檔案
		writeBinaryFile(temp, filePath, i);

		// 將關係碼加上後，初始化temp
		compressAndrelation[1].append(temp);
		temp = "";
	}

	// 記錄01交錯的資料，並寫入檔案中
	string bin = "";
	bin = computeGroupAnd2Binary();
	writeBinaryFile(bin, filePath, -1);

	return bin;
}

int main(int argc, char* argv[]) {
	// Read file, return data by string.
	fileName = argv[1]; //read parameter in command.
	string str = readFileContent(fileName);

	// Create corresponding directory.
	fileName = fileName.substr(0, fileName.length()-4);
	const char* c = fileName.c_str();
	int check = mkdir(c, 0777);
	if(!check){
		cout << "Dircetory created\n" << endl;
	}else{
		cout << "Unable to create directory" << endl;
		exit(1);
	}

	// Start compression process.
	string bin = compressProcess(str, fileName);

	return 0;
}

