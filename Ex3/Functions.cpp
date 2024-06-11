#include "Functions.h"

string GetTimeAsString()
{
    time_t timer;
    time(&timer);
    char timeStr[128];
    strcpy(timeStr, ctime(&timer));
    timeStr[strlen(timeStr) - 1] = 0;
    string timeString = timeStr;

    return timeString;
}

unordered_map<string, wstring> getTranslations(const string& language)
{
    unordered_map<string, wstring> translations;

    if (language == "en") 
    {
        translations["header"] = L"Introduction To Computer Communication - Ex. 3";
        translations["welcome"] = L"Welcome to our page!";
    }
    else if (language == "he") 
    {
        translations["header"] = L"מבוא לתקשורת מחשבים - תרגיל 3";
        translations["welcome"] = L"!ברוכים הבאים לעמוד שלנו";
    }
    else if (language == "fr")
    {
        translations["header"] = L"Introduction à la communication informatique - Ex. 3";
        translations["welcome"] = L"Bienvenue sur notre page !";
    }
    else if (language == "es")
    {
        translations["header"] = L"Introducción a la comunicación informática - Ej. 3";
        translations["welcome"] = L"¡Bienvenido a nuestra página!";
    }

    return translations;
}


std::wstring ReadFileIntoWString(const char* filename) {
    std::wifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: ");
    }

    // Read the entire file into a string
    std::wstring content((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>());

    return content;
}

std::wstring InsertTextIntoWString(const std::wstring& data, const std::wstring& newText, const std::wstring& insertionPoint) {
    size_t insertionPos = data.find(insertionPoint);
    if (insertionPos == std::wstring::npos) {
        throw std::runtime_error("Insertion point not found");
    }

    // Copy the original data before the insertion point
    std::wstring newData = data.substr(0, insertionPos);

    // Copy the new text
    newData += newText;

    // Copy the original data after the insertion point
    newData += data.substr(insertionPos);

    return newData;
}

std::wstring GenerateHTMLBody(const char* lang) {
    std::wstring htmlBody = ReadFileIntoWString(HTML_FILE_NAME);

    auto translations = getTranslations(lang);

    std::wstring itemsHtml;

    for (const auto& item : phrases) {
        itemsHtml += L"<div class=\"item\">\n";
        itemsHtml += L"      <h2>#" + std::to_wstring(item.id) + L"</h2>\n";
        itemsHtml += L"    <p>" + ConvertStrToWstr(item.str) + L"</p>\n";
        itemsHtml += L"</div>\n";
    }

    // Insert items HTML into the template
    std::wstring itemsPlaceholder = L"<div id=\"items\">\n";
    htmlBody = InsertTextIntoWString(htmlBody, itemsHtml, itemsPlaceholder);

    // Insert translations into the template
    std::wstring headerPlaceholder = L"<h1 id=\"header\">";
    htmlBody = InsertTextIntoWString(htmlBody, translations.at("header"), headerPlaceholder);

    std::wstring welcomePlaceholder = L"<div id=\"welcome-message\">\n";
    htmlBody = InsertTextIntoWString(htmlBody, translations.at("welcome"), welcomePlaceholder);

    // Insert current time into the template
    std::wstring timeStampPlaceholder = L"<div id=\"current-time\">\n";
    htmlBody = InsertTextIntoWString(htmlBody, ConvertStrToWstr(GetTimeAsString()), timeStampPlaceholder);

    // Update the language attribute of the HTML tag
    std::wstring langAttributeStr = L"lang=\"";
    htmlBody = InsertTextIntoWString(htmlBody, ConvertStrToWstr(lang), langAttributeStr);

    return htmlBody;
}


void AddPhrase(const char* str, int* givenId)
{
    int id = phrases.size() + 1;
    *givenId = id;
    Phrase phrase;
    phrase.id = id;
    phrase.str = str;

    phrases.push_back(phrase);
}

bool RemovePhrase(int id) 
{
    for (auto it = phrases.begin(); it != phrases.end(); ++it)
    {
        if (it->id == id) 
        {
            phrases.erase(it);
            return true;
        }
    }
    return false;
}

bool UpdatePhrase(int id, string str)
{
    for (Phrase& phrase : phrases)
    {
        if (phrase.id == id)
        {
            phrase.str = str;
            return true;
        }
    }

    return false;
}

bool IsInPhrases(int id)
{
    for (Phrase phrase : phrases)
    {
        if (phrase.id == id)
        {
            return true;
        }
    }

    return false;
}

string constConverter(const char* chars)
{
    string str;
    int i = 0;
    while (chars[i] != '\0')
    {
        str += chars[i];
        i++;
    }

    str[i] = '\0';

    return str;
}

wstring ConvertStrToWstr(const string& narrowString)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

    return converter.from_bytes(narrowString);
}

char* ConvertWstrToCharArray(const wstring& wstr)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    string narrowString = converter.to_bytes(wstr);
    char* charArray = new char[narrowString.length() + 1];
    strcpy(charArray, narrowString.c_str());

    return charArray;
}