#include "Functions.h"

string GetTimeAsString();

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

string GetHtmlBodyAsString()
{
    ifstream file("website.html");

    if (!file.is_open())
    {
        return "<h1>Error: Unable to open HTML file</h1>";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

string GenerateHtmlBody()
{
    string html = "<!DOCTYPE html>\n";
    html += "<html lang=\"en\">\n";
    html += "<head>\n";
    html += "<meta charset=\"UTF-8\">\n";
    html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "<title>Phrases List</title>\n";
    html += "</head>\n";
    html += "<body>\n";
    html += "<header>\n";
    html += "<h1>Phrases List</h1>\n";
    html += "</header>\n";
    html += "<div class=\"container\">\n";
    html += "<ul>\n";

    // Add each phrase to the list
    for (const auto& phrase : phrases)
    {
        html += "<li>Phrase " + to_string(phrase.id) + ": " + phrase.str + "</li>\n";
    }

    html += "</ul>\n";
    html += "</div>\n";
    html += "</body>\n";
    html += "</html>\n";

    return html;
}