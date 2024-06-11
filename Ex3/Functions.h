#pragma once
#include "Server.h"

string GetTimeAsString();
wstring GenerateHTMLBody(const char* lang);
//char* ReadFileIntoCharArray(const string filename);
wstring ConvertStrToWstr(const string& narrowString);
unordered_map<string, wstring> getTranslations(const string& language);
std::wstring InsertTextIntoWString(const std::wstring& data, const std::wstring& newText, const std::wstring& insertionPoint);
std::wstring ReadFileIntoWString(const char* filename);
void AddPhrase(const char* str, int* givenId);
bool RemovePhrase(int id);
bool UpdatePhrase(int id, string str);
bool IsInPhrases(int id);
string constConverter(const char* chars);
std::vector<unsigned char> encodeUTF8(const std::string& str);

