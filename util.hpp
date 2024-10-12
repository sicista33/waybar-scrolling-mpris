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

namespace util
{

int GetCharLengthUTF8(const char* c)
{
    int length = 0;
    if((*c & 0x80) == 0)
        length = 1;
    else if((*c & 0xE0) == 0xC0)
        length = 2;
    else if((*c & 0xF0) == 0xE0)
        length = 3;
    else if ((*c & 0xF8) == 0xF0)
        length = 4;
    // Invalid format string.
    else
        return -1;

    return length;
}

int GetWidthStringUTF8(const char* string)
{
    int width = 0;
    const char* pChar = string;
    int charLen = 0;

    while(1)
    {
        charLen = GetCharLengthUTF8(pChar);
        if(charLen < 0)
            break;
        
        pChar += charLen;
        width++;
    }

    return width;
}

} // namespace util