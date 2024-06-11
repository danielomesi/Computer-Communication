#pragma once
#include "Server.h"

string GetTimeAsString();
string GenerteHTMLBody(const char * lang);
unordered_map<string, string> getTranslations(const string& language);
void AddPhrase(const char* str);
bool RemovePhrase(int id);
bool UpdatePhrase(int id, string str);
bool IsInPhrases(int id);
string constConverter(const char* chars);

