// SocketClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "SocketClient.h"
#include "../SocketServer/Message.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void connect(CSocket& S) {
    S.Create();
    S.Connect(_T("127.0.0.1"), 12345);
}

void disconnect(CSocket& S) {
    S.Close();
}

int id_client;

void GetData() {
    while (true) {
        CSocket s;
        connect(s);
        Message m;
        Message::SendMessage(s, MIDDLEWARE, id_client, MT_GETDATA);
        if (m.Receive(s) == MT_DATA) {
            cout << "Message from " << m.getHeader().msgFrom << ": " << m.getData() << endl;
        }
        disconnect(s);
        Sleep(1000);
    }
}

void Client() {
    // Инициализация сокетов и подключения
    AfxSocketInit();
    CSocket client;
    connect(client);
    Message::SendMessage(client, 0, 0, MT_INIT);
    Message m;
    if (m.Receive(client) == MT_CONFIRM) {
        id_client = m.getHeader().msgTo;
        cout << "My id is " << id_client << endl;
        thread t(GetData);
        t.detach();
    }
    else {
        cout << "error" << endl;
        return;
    }
    disconnect(client);

    // Передача и прием сообщений
    while (true) {
        cout << "1. Send Message\n2. Exit\n" << endl;
        int choice;
        cin >> choice;

        switch (choice) {
        case 1: {
            int ClientID = BROADCAST;
            cout << "1. Only for one client\n2. For All Clients\n" << endl;
            int choice2;
            cin >> choice2;
            if (choice2 == 1) {
                cout << "\nEnter ID of client\n";
                cin >> ClientID;
            }


            cout << "\nEnter your Message\n";
            string st;
            cin.ignore();
            getline(cin, st);
            connect(client);
            Message::SendMessage(client, ClientID, id_client, MT_DATA, st);
            if (m.Receive(client) == MT_CONFIRM) {
                cout << "\nsuccess\n" << endl;
            }
            else {
                cout << "\nerror\n" << endl;
            }
            disconnect(client);

            break;
        }

        case 2: {
            connect(client);
            Message::SendMessage(client, MIDDLEWARE, id_client, MT_EXIT);
            if (m.Receive(client) == MT_CONFIRM) {
                cout << "\nsuccess\n" << endl;
            }
            else {
                cout << "\nerror\n" << endl;
            }
            disconnect(client);
            return;
        }
        default:
            break;
        }
    }

}

int main()
{
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
            std::cout << "Client is starting\n" << endl;
            Client();
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