#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <windows.h>

using namespace std;

// ����1��֧��UTF-8�����ȡ
wstring readFileAsUTF8(const string& path) {
    HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        cerr << "�޷����ļ�: " << path << endl;
        exit(1);
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    vector<char> buffer(fileSize + 3);
    DWORD bytesRead;
    ReadFile(hFile, buffer.data(), fileSize, &bytesRead, NULL);
    CloseHandle(hFile);

    // ���BOMͷ
    int offset = 0;
    if (fileSize >= 3 && (uint8_t)buffer[0] == 0xEF && (uint8_t)buffer[1] == 0xBB && (uint8_t)buffer[2] == 0xBF) {
        offset = 3;
    }

    int wideSize = MultiByteToWideChar(CP_UTF8, 0, buffer.data() + offset, fileSize - offset, NULL, 0);
    wstring wstr(wideSize, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, buffer.data() + offset, fileSize - offset, &wstr[0], wideSize);
    return wstr;
}

// ����2����ȷ���Ĺ��ˣ������������ֺͳ��ñ�㣩
bool isChineseChar(wchar_t c) {
    return (c >= 0x4E00 && c <= 0x9FFF) || // ��������
        (c >= 0x3000 && c <= 0x303F);  // ���ı��
}

wstring filterChinese(const wstring& input) {
    wstring result;
    for (wchar_t c : input) {
        if (isChineseChar(c)) {
            result.push_back(c);
        }
    }
    return result;
}

// ����3���Ż�LCS�㷨
int longestCommonSubsequence(const wstring& text1, const wstring& text2) {
    int m = text1.size(), n = text2.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (text1[i - 1] == text2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            }
            else {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    return dp[m][n];
}

void writeResult(const string& path, double value) {
    ofstream file(path);
    file << fixed << setprecision(2) << value << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "�÷�: " << argv[0] << " ԭ���ļ� ��Ϯ���ļ� ����ļ�" << endl;
        return 1;
    }

    wstring orig = filterChinese(readFileAsUTF8(argv[1]));
    wstring copy = filterChinese(readFileAsUTF8(argv[2]));

    int lcs = longestCommonSubsequence(orig, copy);
    double rate = lcs * 2.0 / (orig.size() + copy.size()); // ʹ��Jaccardϵ��������

    writeResult(argv[3], rate);
    return 0;
}