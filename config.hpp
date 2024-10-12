/*
 * Copyright (c) 2024 sicista33 <sicista33@gmail.com>
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once
#include <string>
#include <unordered_map>

struct json_object;
class Config
{
    inline static const char* MODULE_NAME = "custom/waybar-scrolling-mpris";
    inline static const char* DEFAULT_CONFIG_FILE_PATH = "/.config/waybar/modules";
    inline static const char* DEFAULT_OPTION_VALUE_FORMAT = "{artist} - {title}";
    inline static const int DEFAULT_OPTION_VALUE_LENGTH = 40;

public:
    Config(std::string file);
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    ~Config();

    bool Initialize();

    int GetLengthToDisplay() const;
    int GetIconPadding() const;
    std::string GetTextFormat() const;
    std::string GetMediaFormat() const;
    const char* GetStatusIcon(const std::string& status) const;
    bool UseStatusIcon() const;

private:
    std::string GetHomeDirectory();

    std::string file;
    json_object* config;
    int lengthToDisplay;
    int iconPadding;
    bool useIcon;
    std::string textFormat;
    std::string mediaFormat;
    std::unordered_map<std::string, std::string> statusIcons;
};