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
//char* ReadFileIntoCharArray(const string filename) 
//{
//    FILE* file = fopen(filename.c_str(), "r");
//    if (!file) 
//    {
//        throw std::runtime_error("Unable to open file: " + filename);
//    }
//
//    fseek(file, 0, SEEK_SET);
//    long fileSize = 0;
//    while (fgetc(file) != EOF)
//    {
//        fileSize++;
//    }
//    rewind(file); 
//
//    char* buffer = new char[fileSize + 1]; 
//    fread(buffer, 1, fileSize, file); 
//    buffer[fileSize] = '\0'; 
//
//    fclose(file);
//
//    return buffer;
//}

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

//char* InsertTextIntoCharArray(const char* data, const char* newText, const char* insertionPoint)
//{
//    const char* insertionPos = strstr(data, insertionPoint);
//    if (insertionPos == nullptr) 
//    {
//        throw std::runtime_error("Insertion point not found");
//    }
//
//    size_t originalLength = std::strlen(data);
//    size_t insertionIndex = insertionPos - data;
//    size_t newTextLength = std::strlen(newText);
//
//    // Allocate memory for the new char array
//    char* newData = new char[originalLength + newTextLength + 1]; // +1 for null terminator
//
//    // Copy the original data before the insertion point
//    std::memcpy(newData, data, insertionIndex);
//
//    // Copy the new text
//    std::memcpy(newData + insertionIndex, newText, newTextLength);
//
//    // Copy the original data after the insertion point
//    std::memcpy(newData + insertionIndex + newTextLength, insertionPos, originalLength - insertionIndex);
//
//    // Null-terminate the new data
//    newData[originalLength + newTextLength] = '\0';
//
//    return newData;
//}


//char* GenerateHTMLBody(const char* lang) 
//{
//    char* htmlBody = ReadFileIntoCharArray(HTML_FILE_NAME);
//
//    auto translations = getTranslations(lang);
//
//    string itemsHtml;
//
//    for (const auto& item : phrases)
//    {
//        itemsHtml+="<div class=\"item\">\n";
//        itemsHtml += "      <h2>Item " + to_string(item.id) + "</h2>\n";
//        itemsHtml += "    <p>" + item.str + "</p>\n";
//        itemsHtml+= "</div>\n";
//    }
//
//    const char* itemsHtmlChar = itemsHtml.c_str();
//
//    const char* headerPlaceholder = "<h1 id=\"header\">";
//    htmlBody = InsertTextIntoCharArray(htmlBody, translations.at("header"), headerPlaceholder);
//
//
//    const char* welcomePlaceholder = "<div id=\"welcome-message\">\n";
//    htmlBody = InsertTextIntoCharArray(htmlBody, translations.at("welcome"), welcomePlaceholder);
//
//    // Insert items HTML into the template
//    const char* itemsPlaceholder = "<div id=\"items\">\n";
//    htmlBody = InsertTextIntoCharArray(htmlBody, itemsHtmlChar, itemsPlaceholder);
//
//    // Insert current time into the template
//    const char* timeStampPlaceholder = "<div id=\"current-time\">\n";
//    htmlBody = InsertTextIntoCharArray(htmlBody, GetTimeAsString().c_str(), timeStampPlaceholder);
//
//    // Update the language attribute of the HTML tag
//    std::string langAttributeStr = "lang=\"";
//    const char* langAttribute = langAttributeStr.c_str();
//    htmlBody = InsertTextIntoCharArray(htmlBody, lang, langAttribute);
//
//
//
//    return htmlBody;
//}


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

wstring ConvertStrToWstr(const string& narrowString) {
    // Create a wstring_convert object with UTF-8 <-> UTF-16 codecvt facet
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;

    // Convert the narrow string to wide string and return
    return converter.from_bytes(narrowString);
}