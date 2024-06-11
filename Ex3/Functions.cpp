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

unordered_map<string, string> getTranslations(const string& language)
{
    unordered_map<string, string> translations;

    if (language == "en") {
        translations["header"] = "Items";
        translations["welcome"] = "Welcome to our website!";
    }
    else if (language == "he") {
        translations["header"] = "פריטים";
        translations["welcome"] = "ברוכים הבאים לאתר!";
    }
    else if (language == "fr") {
        translations["header"] = "Articles";
        translations["welcome"] = "Bienvenue sur notre site web!";
    }

    return translations;
}

string GenerteHTMLBody(const char* lang)
{
    wifstream file(HTML_FILE_NAME);
    if (!file.is_open()) {
        throw runtime_error("Unable to open file: " + string(HTML_FILE_NAME));
    }

    file.imbue(locale(file.getloc(), new codecvt_utf8_utf16<wchar_t>));

    wstringstream buffer;
    buffer << file.rdbuf();
    wstring html = buffer.str();
    file.close();

    wstring_convert<codecvt_utf8<wchar_t>> converter;
    string narrowHtml = converter.to_bytes(html);

    auto translations = getTranslations(lang);

    string itemsHtml;
    for (const auto& item : phrases) {
        itemsHtml += "<div class=\"item\">\n";
        itemsHtml += "    <h2>Item " + to_string(item.id) + "</h2>\n";
        itemsHtml += "    <p>" + item.str + "</p>\n";
        itemsHtml += "</div>\n";
    }

    string headerPlaceholder = "<h1 id=\"header\">";
    size_t headerPos = narrowHtml.find(headerPlaceholder);
    if (headerPos != string::npos) {
        headerPos += headerPlaceholder.length();
        narrowHtml.insert(headerPos, translations["header"] + "\n");
    }
    else {
        throw runtime_error("Header placeholder not found in HTML template");
    }

    string welcomePlaceholder = "<div id=\"welcome-message\">\n";
    size_t welcomePos = narrowHtml.find(welcomePlaceholder);
    if (welcomePos != string::npos) {
        welcomePos += welcomePlaceholder.length();
        narrowHtml.insert(welcomePos, translations["welcome"] + "\n");
    }
    else {
        throw runtime_error("Welcome placeholder not found in HTML template");
    }

    // Insert items HTML into the template
    string itemsPlaceholder = "<div id=\"items\">\n";
    size_t pos = narrowHtml.find(itemsPlaceholder);
    if (pos != string::npos) {
        pos += itemsPlaceholder.length();
        narrowHtml.insert(pos, itemsHtml);
    }
    else {
        throw runtime_error("Items placeholder not found in HTML template");
    }

    // Insert current time into the template
    string timeStampPlaceHolder = "<div id=\"current-time\">\n";
    size_t currentTimePos = narrowHtml.find(timeStampPlaceHolder);
    if (currentTimePos != string::npos) {
        currentTimePos += timeStampPlaceHolder.length();
        string currentTime = GetTimeAsString();
        narrowHtml.insert(currentTimePos, currentTime + "\n");
    }
    else {
        throw runtime_error("Current time placeholder not found in HTML template");
    }

    // Update the language attribute of the HTML tag
    string langAttribute = "lang=\"" + string(lang) + "\"";
    size_t langPos = narrowHtml.find("lang=\"");
    if (langPos != string::npos) {
        size_t start = langPos + 6; // 6 is the length of "lang=\""
        size_t end = narrowHtml.find("\"", start);
        narrowHtml.replace(start, end - start, lang);
    }
    else {
        throw runtime_error("Language attribute not found in HTML template");
    }

    return narrowHtml;
}

void AddPhrase(const char* str)
{
    int id = phrases.size() + 1;
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