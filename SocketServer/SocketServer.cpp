// SocketServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "SocketServer.h"
#include "Message.h"
#include "Session.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
map<int, shared_ptr<Session>> Sessions;

// Отключение неактивных пользователей
void TimeOut() 
{
    while (true)
    {
        for (auto i = Sessions.begin(); i != Sessions.end();) {
            if (Sessions.find(i->first) != Sessions.end()) {
                if (double(clock() - i->second->getTime()) > 10000) 
                {
                    cout << "Client " << i->first << " has been disconnected" << endl;
                    i = Sessions.erase(i);
                }
                else
                    i++;
            }
        }
        Sleep(1000);
    }
}

int ID = 1;
int createID()
{
    return ID++;
}

// Работа с клиентом
void ProcessClient(SOCKET hSOCK) {
    CSocket s;
    s.Attach(hSOCK);
    Message m;

    switch (m.Receive(s)) {
    case MT_INIT: {
        int ID = createID();
        Sessions[ID] = make_shared<Session>(ID, clock());
        cout << "Client " << ID << " connect\n";
        Message::SendMessage(s, ID, MIDDLEWARE, MT_CONFIRM);
        break;
    }
    case MT_EXIT: {
        Sessions.erase(m.getHeader().msgFrom);
        cout << "Client " << m.getHeader().msgFrom << " disconnect\n";
        Message::SendMessage(s, m.getHeader().msgFrom, MIDDLEWARE, MT_CONFIRM);
        break;
    }
    case MT_GETDATA: {
        if (Sessions.find(m.getHeader().msgFrom) != Sessions.end()) {
            Sessions[m.getHeader().msgFrom]->Send(s);
            Sessions[m.getHeader().msgFrom]->setTime(clock());
        }
        break;
    }
    default: {
        if (Sessions.find(m.getHeader().msgFrom) != Sessions.end()) {
            if (Sessions.find(m.getHeader().msgTo) != Sessions.end()) {
                Sessions[m.getHeader().msgTo]->Add(m);
            }
            else if (m.getHeader().msgTo == BROADCAST) {
                for (auto i : Sessions) {
                    if (i.first != m.getHeader().msgFrom)
                        i.second->Add(m);
                }

            }
            Message::SendMessage(s, m.getHeader().msgFrom, MIDDLEWARE, MT_CONFIRM);
            Sessions[m.getHeader().msgFrom]->setTime(clock());
        }
        break;
    }
    }
}


void Server() 
{
    AfxSocketInit();
    CSocket Server;
    Server.Create(12345);
    thread tt(TimeOut);
    tt.detach();
    while (true)
    {
        Server.Listen();
        CSocket s;
        Server.Accept(s);
        thread t(ProcessClient, s.Detach());
        t.detach();
    }
}

int main()
{
    std::cout << "Server is starting \n" << endl;
    int nRetCode = 0;
    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
            Server();
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}