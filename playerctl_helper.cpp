/*
 * Copyright (c) 2024 sicista33 <sicista33@gmail.com>
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "playerctl_helper.hpp"
#include <fstream>
#include <json-c/json.h>
#include <cstring>

namespace playerctl
{

void Playerctl::UpdateCurrentMetaData()
{
    Clear();

    const char* command = "playerctl metadata --format \'{ \"player\":\"{{markup_escape(playerName)}}\", \"status\":\"{{status}}\", \"artist\":\"{{markup_escape(artist)}}\", \"album\":\"{{markup_escape(album)}}\", \"title\":\"{{markup_escape(title)}}\" }\' -s";

    FILE* p = popen(command, "r");
    fflush(stdout);
    fflush(stderr);

    if (!p)
        return;

    char buffer[1024] = { 0, };
    fgets(buffer, 1024, p);
    pclose(p);

    char unescaped[1024] = { 0, };

    int i = 0, j = 0;
    while (i < 1024 && buffer[i] != '\n')
    {
        if (buffer[i] == '&')
        {
            if (strncmp(&buffer[i], "&quot;", 6) == 0)
            {
                unescaped[j++] = '\\';
                unescaped[j++] = '"';
                i += 6;
                continue;
            }
            else if (strncmp(&buffer[i], "&lt;", 4) == 0)
            {
                unescaped[j++] = '<';
                i += 4;
                continue;
            }
            else if (strncmp(&buffer[i], "&gt;", 4) == 0)
            {
                unescaped[j++] = '>';
                i += 4;
                continue;
            }
            else if (strncmp(&buffer[i], "&nbsp;", 6) == 0)
            {
                unescaped[j++] = '\\';
                unescaped[j++] = '\'';
                i += 6;
                continue;
            }
        }

        unescaped[j++] = buffer[i++];
    }

    json_tokener_error err;
    json_object* json = json_tokener_parse_verbose(unescaped, &err);
    if (!json)
    {
        this->metadata["status"] = "Stopped";
        return;
    }

    json_object_object_foreach(json, key, value)
    {
        int type = json_object_get_type(value);
        if (type == json_type_string)
            this->metadata[std::string(key)] = json_object_get_string(value);
    }
}

const char* Playerctl::GetMetadata(const std::string& variable) const
{
    auto iter = this->metadata.find(variable);
    if (iter == this->metadata.end())
        return "";

    return iter->second.c_str();
}

void Playerctl::Clear()
{
    this->metadata.clear();
}

} // namespace playerctl
