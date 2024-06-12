#pragma once
#include "Server.h"

string GetTimeAsString();
wstring GenerateHTMLBody(string lang);
unordered_map<string, wstring> GetTranslations(const string& language);
wstring InsertTextIntoWString(const wstring& data, const wstring& newText, const wstring& insertionPoint, bool insertBefore = true);
wstring ReadFileIntoWString(const char* filename);
void AddPhrase(const char* str, int* givenId);
bool RemovePhrase(int id);
bool UpdatePhrase(int id, string str);
bool IsInPhrases(int id);
string ConstConverter(const char* chars);
char* ConvertWstrToCharArray(const wstring& wstr);
wstring ConvertStrToWstr(const string& narrowString);
