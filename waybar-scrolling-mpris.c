#include <stdio.h>
#include <string.h>
#include <time.h>

#define PLAYER_PAUSED 1
#define PLAYER_PLAYING 2
#define NO_PLAYER 3

#define MAX_BUFFER_LEN 512
#define STATUS_BUFFER_LEN 32

int GetCurrentStatus()
{
    const char* command = "playerctl metadata --format \'{{ status }}\' -s";

    FILE* p = popen(command, "r");
    fflush(stdout);
    fflush(stderr);

    if(!p)
        return -1;

    char status_buffer[STATUS_BUFFER_LEN];
    fscanf(p, "%s", status_buffer);

    pclose(p);

    if(strcmp(status_buffer, "Paused") == 0)
        return PLAYER_PAUSED;
    else if(strcmp(status_buffer, "Playing") == 0)
        return PLAYER_PLAYING;
    else
        return NO_PLAYER;
}

int GetCurrentMediaString(char* buffer, int buffer_size)
{
    const char* command = "playerctl metadata --all-players --format \'{{ artist }} - {{ title }}\' -s";

    FILE* p = popen(command, "r");
    
    if(!p)
        return -1;

    char* s = fgets(buffer, buffer_size, p);
    pclose(p);

    if(s == NULL)
        return -1;
    int length = strlen(s);

    if(buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';
        --length;
    }        

    // Add padding to the end of string.
    if(length + 5 < MAX_BUFFER_LEN)
    {
        buffer[length] = ' ';
        buffer[length + 1] = ' ';
        buffer[length + 2] = ' ';
        buffer[length + 3] = ' ';
        buffer[length + 4] = '\0';
        length += 5;
    }
    
    return length;
}

int GetCharLengthUTF8(char* c)
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

int GetWidthStringUTF8(char* string)
{
    int width = 0;
    char* pChar = string;
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

int main(int argc, const char* argv[])
{
    if(argc != 2)
    {
        printf("usage waybar-scrolling-mpris [max_width]. (max_width: 5-511)\n");
        return -1;
    }

    int maxWidth;
    sscanf(argv[1], "%d", &maxWidth);
    if(maxWidth < 5 || maxWidth > 511)
    {
        printf("Invalid max_width value.");
        return -1;
    }
    
    // Subtraction pause/play icon and padding width
    maxWidth -= 3;

    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 500000000L;

    char buffer[MAX_BUFFER_LEN];
    char tempBuffer[MAX_BUFFER_LEN];
    int width = 0;
    int windowBegin = 0, windowEnd = 0, initWindowEnd = 0;
    int lastCharLen = 0;
    while(1)
    {
        int printedWidth = 0;
        int status = GetCurrentStatus();
        if(status == NO_PLAYER)
        {
            printf("\n");
        }
        else
        {
            int length = GetCurrentMediaString(tempBuffer, sizeof(tempBuffer));
            if(strcmp(buffer, tempBuffer) != 0)
            {
                strncpy(buffer, tempBuffer, length);
                width = GetWidthStringUTF8(buffer);

                windowBegin = 0;
                if(width <= maxWidth)
                    windowEnd = length;
                else
                {
                    windowEnd = 0;
                    for(int n = 0; n < maxWidth; ++n)
                    {
                        windowEnd += GetCharLengthUTF8(&buffer[windowEnd]);
                    }
                    initWindowEnd = windowEnd;
                }
            }

            switch (status)
            {
            case PLAYER_PAUSED:
                printf("⏸  ");
                break;
            case PLAYER_PLAYING:
                printf("▶  ");
                break;
            };

            if(width < maxWidth)
            {            
                printf("%s\n", buffer);
                fflush(stdout);
            }
            else
            {
                if(windowBegin < windowEnd)
                {
                    lastCharLen = 0;
                    int len = 0;
                    for(int n = 0; n < maxWidth; ++n)
                    {
                        lastCharLen = GetCharLengthUTF8(&buffer[windowBegin + len]);
                        len += lastCharLen;
                    }
                    strncpy(tempBuffer, &buffer[windowBegin], len);
                    tempBuffer[len] = '\0';
                }
                else
                {
                    lastCharLen = 0;
                    int len = 0;
                    int numChar = 0;
                    while(buffer[windowBegin + len] != '\0')
                    {
                        lastCharLen = GetCharLengthUTF8(&buffer[windowBegin + len]);
                        len += lastCharLen;
                        numChar++;
                    }
                    strncpy(tempBuffer, &buffer[windowBegin], len);
                    strncpy(&tempBuffer[len], buffer, windowEnd);
                    tempBuffer[len + windowEnd] = '\0';
                }
                printf("%s\n", tempBuffer);
                fflush(stdout);                

                int len = GetCharLengthUTF8(&buffer[windowBegin]);
                if(windowBegin + len >= length)
                    windowBegin = 0;
                else
                    windowBegin += len;
                
                if(windowBegin == 0)
                    windowEnd = initWindowEnd;
                else
                {
                    len = GetCharLengthUTF8(&buffer[windowEnd]);
                    if(windowEnd + len > length - 1)
                        windowEnd = GetCharLengthUTF8(buffer);
                    else
                        windowEnd += len;              
                }
            }
        }
        nanosleep(&tim, &tim2);
    }

    return 0;
}