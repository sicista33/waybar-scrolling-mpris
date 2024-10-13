/*
 * Copyright (c) 2024 sicista33 <sicista33@gmail.com>
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <iostream>
#include <vector>
#include <sstream>
#include <regex>
#include <cstring>
#include <cstring>
#include <tuple>
#include "config.hpp"
#include "playerctl_helper.hpp"
#include "util.hpp"

std::tuple<std::string, std::vector<std::string>> BuildStringFormatSequence(std::string format);
std::string GetCurrentMediaString(std::string format, const std::vector<std::string>& variables, const playerctl::Playerctl& pctl);

int main(int argc, char* argv[])
{
    if(argc > 2)
    {
        std::cerr << "usage waybar-scrooling-mpris [(optional)path]\npath: Path of waybar module config file. Default value is \'$HOME/.config/waybar/modules\'\n";
        exit(-1);
    }

    const char* configPath = (argc == 1) ? "" : argv[1];

    Config config(configPath);
    if(!config.Initialize())
        return -1;
    
    auto tuple = BuildStringFormatSequence(config.GetMediaFormat());
    const std::string formatString = std::get<0>(tuple);
    const std::vector<std::string> variables = std::get<1>(tuple);

    int maxWidth = config.GetLengthToDisplay();
    if(config.UseStatusIcon())
        maxWidth -= 4;

    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 500000000L;

    playerctl::Playerctl pctl;
    std::string showMediaString;
    int width = 0;
    int windowBegin = 0, windowEnd = 0, initWindowEnd = 0;
    int lastCharLen = 0;
    while(1)
    {
        int printedWidth = 0;
        pctl.UpdateCurrentMetaData();
        const auto& status = pctl.GetMetadata("status");
        
        if(config.UseStatusIcon())
            printf("%s%*s", config.GetStatusIcon(status), config.GetIconPadding(), " ");

        if(strcmp(status, "Stopped") == 0)
            printf("\n");
        else
        {
            const auto& currentMediaString = GetCurrentMediaString(formatString, variables, pctl);
            int length = currentMediaString.size();
            if(currentMediaString != showMediaString)
            {
                showMediaString = currentMediaString;
                width = util::GetWidthStringUTF8(showMediaString.c_str());
                windowBegin = 0;
                if(width <= maxWidth)
                {
                    windowEnd = length;
                    initWindowEnd = windowEnd;
                }
                else
                {
                    windowEnd = 0;
                    for(int n = 0; n < maxWidth; ++n)
                        windowEnd += util::GetCharLengthUTF8(&showMediaString[windowEnd]);
                    initWindowEnd = windowEnd;
                }
            }

            if(width < maxWidth)
            {   
                printf("%s\n", showMediaString.c_str());
                fflush(stdout);
            }
            else
            {
                std::string temp;
                if(windowBegin < windowEnd)
                    temp = std::string(&showMediaString[windowBegin], &showMediaString[windowEnd]);                    
                else
                {
                    lastCharLen = 0;
                    int len = 0;
                    int numChar = 0;
                    while(showMediaString[windowBegin + len] != '\0')
                    {
                        lastCharLen = util::GetCharLengthUTF8(&showMediaString[windowBegin + len]);
                        len += lastCharLen;
                        numChar++;
                    }
                    temp = std::string(&showMediaString[windowBegin], &showMediaString[windowBegin + len]);
                    temp += std::string(&showMediaString[0], &showMediaString[windowEnd]);
                }
                
                printf("%s\n", temp.c_str());
                fflush(stdout);                

                int len = util::GetCharLengthUTF8(&showMediaString[windowBegin]);
                if(windowBegin + len >= length)
                    windowBegin = 0;
                else
                    windowBegin += len;
                
                if(windowBegin == 0)
                    windowEnd = initWindowEnd;
                else
                {
                    len = util::GetCharLengthUTF8(&showMediaString[windowEnd]);
                    if(windowEnd + len > length - 1)
                        windowEnd = util::GetCharLengthUTF8(&showMediaString[0]);
                    else
                        windowEnd += len;
                }
            }
        }
        nanosleep(&tim, &tim2);
    }

    return 0;
}

std::tuple<std::string, std::vector<std::string>> BuildStringFormatSequence(std::string format)
{
    std::vector<std::string> names;
    std::regex variableReg(R"(\{[^{}]*\})");
    std::smatch m;

    std::string result = std::regex_replace(format, variableReg, "%s");

    while(std::regex_search(format, m, variableReg))
    {
        std::string name;
        for(const auto c : m.str())
        {
            if(std::isalpha(c))
                name += c;
        }
        names.push_back(name);
        format = m.suffix().str();
    }    

    return std::make_tuple(result, names);
}

std::string GetCurrentMediaString(std::string format, const std::vector<std::string>& variables, const playerctl::Playerctl& pctl)
{
    std::stringstream ss;
    int i = 0;
    do
    {
        int pos = format.find("%s");
        ss << format.substr(0, pos);
        ss << pctl.GetMetadata(variables[i++]);
        format = format.substr(pos + 2);       
    } while (i < variables.size());
    ss << format;

    return ss.str();
}
