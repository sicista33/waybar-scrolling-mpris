/*
 * Copyright (c) 2024 sicista33 <sicista33@gmail.com>
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "config.hpp"
#include <iostream>
#include <cstring>
#include <regex>
#include <json-c/json.h>
#include <filesystem>

Config::Config(std::string file) :
    file(file),
    config(nullptr),
    textFormat(Config::DEFAULT_OPTION_VALUE_FORMAT),
    lengthToDisplay(Config::DEFAULT_OPTION_VALUE_LENGTH),
    iconPadding(0),
    useIcon(false)
{
}

Config::~Config()
{
    if (this->config)
    {
        json_object_put(this->config);
        this->config = nullptr;
    }
}

bool Config::Initialize()
{
    if (this->config)
        return false;

    if (this->file == "")
    {
        if (!LoadDefaultConfigFile())
        {
            std::cerr << "Default config file is not found.\n";
            return false;
        }
    }
    else
    {
        if (!FindConfigFile(this->file))
        {
            std::cerr << "Config file is not found :" << this->file << "\n";
            return false;
        }
    }

    this->config = json_object_from_file(this->file.c_str());
    if (!config)
    {
        std::cerr << "Failed to parse config file: " << this->file << "\n";
        return false;
    }

    json_object* moduleObj = json_object_object_get(config, Config::MODULE_NAME);
    if (!moduleObj)
    {
        std::cerr << "Could not find \"custom/waybar-scrolling-mpris\" module definition in config file: " << this->file << "\n";
        json_object_put(config);
        return false;
    }

    json_object_object_foreach(moduleObj, key, value)
    {
        int type = json_object_get_type(value);
        if (strcmp(key, "display-format") == 0)
        {
            if (type == json_type_string)
            {
                const char* format = json_object_get_string(value);
                if (strcmp(format, "{}") != 0)
                    this->textFormat = format;
                this->mediaFormat = this->textFormat;
            }

            this->textFormat += "    ";
        }
        else if (strcmp(key, "length") == 0)
        {
            if (type == json_type_int)
                this->lengthToDisplay = json_object_get_int(value);

            // If length is too short or too long, set it default value.
            if (this->lengthToDisplay < 5 || this->lengthToDisplay > 511)
                this->lengthToDisplay = DEFAULT_OPTION_VALUE_LENGTH;
        }
        else if (strcmp(key, "icons") == 0)
        {
            if (type == json_type_object)
            {
                json_object_object_foreach(value, status, icon)
                {
                    this->statusIcons[status] = json_object_get_string(icon);
                }
            }
        }
    }

    json_object_put(this->config);
    this->config = nullptr;

    // Thats why you must place icon first in format string.
    std::regex regIcon(R"(\{icon\})");
    std::smatch m;

    if (std::regex_search(this->textFormat, m, regIcon))
    {
        this->useIcon = true;
        std::string ignoreIcon = m.suffix();
        this->iconPadding = ignoreIcon.find('{');
        if (this->iconPadding == std::string::npos)
            this->iconPadding = 0;

        this->mediaFormat = ignoreIcon.substr(this->iconPadding);
    }

    return true;
}

int Config::GetLengthToDisplay() const
{
    return this->lengthToDisplay;
}

int Config::GetIconPadding() const
{
    return this->iconPadding;
}

std::string Config::GetTextFormat() const
{
    return this->textFormat;
}

std::string Config::GetMediaFormat() const
{
    return this->mediaFormat;
}

const char* Config::GetStatusIcon(const std::string& status) const
{
    auto iter = this->statusIcons.find(status);
    if (iter == this->statusIcons.end())
        return "";

    return iter->second.c_str();
}

bool Config::UseStatusIcon() const
{
    return this->useIcon;
}

bool Config::LoadDefaultConfigFile()
{
    for (const auto& dir : Config::DEFAULT_CONFIG_FILE_DIRECTORIES)
    {
        for (const auto& file : Config::DEFAULT_CONFIG_FILE_NAMES)
        {
            std::string configFile(dir);
            configFile += file;

            if (FindConfigFile(configFile))
                return true;
        }
    }

    return false;
}

bool Config::FindConfigFile(const std::string& file)
{
    namespace fs = std::filesystem;

    fs::path path = BuildRealPath(file);
    if (!fs::exists(path))
        return false;

    this->file = path.string();

    return true;
}

std::string Config::BuildRealPath(const std::string& path)
{
    std::string realPath;
    for (size_t i = 0; i < path.size();)
    {
        if (path[i] == '~')
            realPath += GetSystemEnv("HOME");
        else if (path[i] == '$')
        {
            size_t dashPos = path.substr(i).find('/');
            if (dashPos == std::string::npos)
                realPath += GetSystemEnv(path.substr(i + 1));
            else
                realPath += GetSystemEnv(path.substr(i + 1, dashPos - 1));
            i += dashPos;
            continue;
        }
        else
            realPath += path[i];
        ++i;
    }

    return realPath;
}

std::string Config::GetSystemEnv(const std::string& variable)
{
    // It works only linux system maybe
    char buffer[256];
    int len = snprintf(buffer, sizeof(buffer), "%s", getenv(variable.c_str()));

    return buffer;
}
