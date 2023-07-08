// #include <string.h>
// #define WIN32_LEAN_AND_MEAN

// #include <windows.h>
// #include <winsock2.h>
// #include <ws2tcpip.h>

// #include <stdio.h>
// #include <cstring>
// #include <stdlib.h>
// #include <algorithm>
// #include <vector>
// #include <string>
// #include <thread>
// #include <chrono>
// #include <atomic>

#include "GUI.h"


/*
int term_width;
int term_height;

int next_free_line = 1;

void write_msg(const char* msg)
{
    if (next_free_line == term_height)
    {
        printf(CSI "1;S"); // scroll up one
        printf(CSI "%d;1H", next_free_line - 1); // go to message line
        printf(CSI "2K"); // clear the line (had editline contents on it)
        printf("%.*s", term_width, msg);
    }
    else
    {
        printf(CSI "%d;1H", next_free_line); // go to free line
        printf("%.*s", term_width, msg);
        next_free_line += 1;
    }
}

void refresh_editline()
{
    printf(CSI "%d;1H", term_height); // go to last line
    printf(CSI "2K"); // clear line
    printf("> %.*s", term_width - 2, editline);
}

int main(int argc, char* argv[])
{
    
    // Put the terminal in 'raw' mode
    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode &= ~ENABLE_ECHO_INPUT;
        dwMode &= ~ENABLE_INSERT_MODE;
        dwMode &= ~ENABLE_LINE_INPUT;
        dwMode &= ~ENABLE_MOUSE_INPUT;
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }

    // Get the terminal size
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        term_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        term_height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    
    // Set up screen
    printf(CSI "2J"); // clear screen
    refresh_editline();
    fflush(0);

    // Set up socket

    WSAData wsadata;
    auto code = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (code != 0)
        fatal("WSA Startup Fail!");

    const char* server_ip = "eudvlg-kierans-etx";
    addrinfo* addressInfo;
    getaddrinfo(server_ip, "3001", nullptr, &addressInfo);

    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        fatal("Could not find socket!");

    connect(sock, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);

    char header = 1;
    send(sock, &header, sizeof(header), 0);

    const char* username = "Andrei Tate\n";
    send(sock, username, strlen(username), 0);

    auto socket_event = WSACreateEvent();
    code = WSAEventSelect(sock, socket_event, FD_READ);
    if (code != 0)
        fatal("Could not select socket event!");

    HANDLE event_handles[] =
    {
        GetStdHandle(STD_INPUT_HANDLE),
        // This event we need to make ourselves which is to say the socket has some data for us
        socket_event
    };



    char buf[1024];

    std::vector<char> messages;
    while (true)
    {
        memset(buf, 0, sizeof(buf));

        auto result = WSAWaitForMultipleEvents(2, event_handles, FALSE, WSA_INFINITE, FALSE);


        if (result == WSA_WAIT_EVENT_0)
        {
            // STDIN ready
            INPUT_RECORD record;
            DWORD numRead;
            if(!ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &record, 1, &numRead))
            {
                fatal("stdin read error\n");
            }

            if(record.EventType != KEY_EVENT) continue;
            if(!record.Event.KeyEvent.bKeyDown) continue;

            char c = record.Event.KeyEvent.uChar.AsciiChar;
            if (c == '\r') c = '\n';

            editline[editline_len] = c;
            editline_len += 1;

            if (c == '\n')
            {
                send(sock, editline, editline_len, 0);
                editline_len = 0;
                memset(editline, 0, sizeof(editline));
            }

            
        }
        else if (result == WSA_WAIT_EVENT_0 + 1)
        {
            // What to run when the socket has some data for you

            auto retValue = recv(sock, buf, sizeof(buf), 0); // This is a continuous stream of at most 512 bytes
            if (retValue == 0)
            {
                printf("Server Closed");
            }
            else if (retValue < 0)
            {
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                    continue;

                printf("This is really bad");
                return 1;
            }
            else // retValue > 0 it has some content!
            {
                auto orig_size = messages.size();
                messages.resize(messages.size() + retValue);
                memcpy(&messages[orig_size], buf, retValue);

                while (true) // Consider the scenario where one receive chunk gives more than one message
                    //there can be many newline characters in the messages vector, so we while to find all of them
                {
                    auto found = std::find(messages.begin(), messages.end(), '\n');
                    if (found != messages.end())
                    {
                        auto msg = std::string(messages.begin(), found + 1);
                        messages.erase(messages.begin(), found + 1);
                        write_msg(msg.c_str());
                    }
                    else
                        break;
                }
            }
        }


        refresh_editline();
        fflush(0);
    }
    freeaddrinfo(addressInfo);
}
*/


// Main code
int main(int, char**)
{
    GUI::MainWindow mainWindow;
    /*
    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

     // Set up socket

    WSAData wsadata;
    auto code = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (code != 0)
        fatal("WSA Startup Fail!");

    const char* server_ip = "eudvlg-kierans-etx";
    addrinfo* addressInfo;
    getaddrinfo(server_ip, "3001", nullptr, &addressInfo);

    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        fatal("Could not find socket!");

    connect(sock, addressInfo->ai_addr, (int)addressInfo->ai_addrlen);

    char header = 1;
    send(sock, &header, sizeof(header), 0);

    const char* username = "Andrei Tate\n";
    send(sock, username, strlen(username), 0);

    auto socket_event = WSACreateEvent();
    code = WSAEventSelect(sock, socket_event, FD_READ);
    if (code != 0)
        fatal("Could not select socket event!");

    HANDLE event_handles[] =
    {
        GetStdHandle(STD_INPUT_HANDLE),
        // This event we need to make ourselves which is to say the socket has some data for us
        socket_event
    };

    char buf[512];
    std::string messagesToDisplay = "";
    std::thread t([&sock, &buf, &messagesToDisplay](){
        while (true)
        {
            // What to run when the socket has some data for you

            auto retValue = recv(sock, buf, sizeof(buf), 0); // This is a continuous stream of at most 512 bytes
            if (retValue == 0)
            {
                printf("Server Closed\n");
            }
            else if (retValue < 0)
            {
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                    continue;

                printf("This is really bad");
                return 1;
            }
            else // retValue > 0 it has some content!
            {
                std::string tempBuf = "";
                tempBuf.append(buf);

                messagesToDisplay.append(tempBuf.substr(0, retValue));
                memset(buf, 0, sizeof(buf));
            }

            std::this_thread::sleep_for(100ms);
        };
    });*/

    // Main loop
    while (!glfwWindowShouldClose(mainWindow.window))
    {
        mainWindow.render();

        glfwSwapBuffers(mainWindow.window);
    }

    return 0;
}

