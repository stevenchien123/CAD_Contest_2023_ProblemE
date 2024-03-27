#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <sstream>
#include <math.h>
#include <stdexcept>
#include <stdio.h>

using namespace std;

// Execute linux command and output result.
std::string exec(const char* cmd) {
	char buffer[128];
	std::string result = "";
	FILE* pipe = popen(cmd, "r");
	if (!pipe) throw std::runtime_error("popen() failed!");
	try {
		while (fgets(buffer, sizeof buffer, pipe) != NULL) {
			result += buffer;
		}
	}
	catch (...) {
		pclose(pipe);
		throw;
	}
	pclose(pipe);
	return result;
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
		std::cerr << "無法打開文件。" << std::endl;
		return "0";
		exit(1);
	}

	return str;
}


int main(int argc, char* argv[]) {
	string inputFile = argv[1]; // read input parameter
	string fileName = inputFile.substr(0, inputFile.length() - 4); // relation code file name

	/*
	ex:    home
		|
	     username
	      /    \
	Compress   Decompress
	assume compress and decompress put in the same dir
	*/
  	string currentPath = exec("pwd");
	string rootPath = currentPath.substr(0, currentPath.rfind("/"));
	string parentDirectory = rootPath + "/Compress/" + fileName; // relation code path
	
	// start decompressing
	ifstream file;
	file.open(parentDirectory + "/compressedCode.bin");
	if (!file) {
		cout << "Error open relation file!";
		exit(1);
	}
	cout << "Decompressing " + inputFile + "..." << endl;
	string str = ""; //compressed data
	file >> str;
	file.close();
	string a;//check 10 or 01
	int last;//有沒有多出一位
	//判斷前兩位數值個別是0或1
	if (str[0] == '0')
	{
		a = "01";
	}
	else
	{
		a = "10";
	}
	if (str[1] == '0')
	{
		last = 0;
	}
	else
	{
		last = 1;
	}
	string compressed = str.substr(2); //求出扣除前兩位的剩餘代碼
	int decimal = bitset<32>(compressed).to_ulong(); //轉成十進位的剩餘代碼
	string base; //壓縮到最底的01、10壓縮碼

	//還原壓縮至最極限的01、10代碼
	for (int i = 0; i < decimal; i++) //把01、10代碼還原回去
	{
		base += a;
	}
	if (last == 1) //判斷有無最後一位
	{
		if (base.back() == '0') //前一位為0就加1，反之加0
		{
			base += '1';
		}
		else
		{
			base += '0';
		}
	}

	string bin; //原檔

	//恢復二進制原檔
	for (int i = 0; i < base.length(); i++) //壓縮到最底的01、10壓縮碼為迴圈大小
	{
		int n = 1; //n:長度
		string r, line; //r:關係碼，line:0、1總共有多少
		ifstream relation; //開檔

		relation.open(parentDirectory + "/relation" + to_string(i) + ".bin");//打開每段游長的檔案讀取
		relation >> r;
		if (!relation)//沒有這段游長檔案
		{
			n = 1;
		}
		else
		{
			for (int j = r.length() - 1; j >= 0; j--)
			{
				n *= 2;
				if (r[j] == '1')
				{
					n += 1;
				}
			}
		}

		if (base[i] == '0')
		{
			for (int j = 0; j < n; j++)
			{
				line += '0';
			}
		}
		else
		{
			for (int j = 0; j < n; j++)
			{
				line += '1';
			}
		}
		bin += line;
		relation.close();
	}
	// 将二进制字符串转换为二进制数据
	string binaryData;
	for (size_t i = 0; i < bin.length(); i += 8) {
		string byteString = bin.substr(i, 8);
		char byte = static_cast<char>(stoi(byteString, nullptr, 2));
		binaryData.push_back(byte);
	}

	// 写入二进制文件
	ofstream outputFile("decompressed.bin", ios::binary);
	if (outputFile.is_open()) {
		outputFile.write(binaryData.c_str(), binaryData.size());
		outputFile.close();
		std::cout << "Binary data has been written" << std::endl;
	}
	else {
		std::cerr << "Error opening the output file." << std::endl;
	}
	return 0;
}

