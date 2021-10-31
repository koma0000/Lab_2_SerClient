#pragma once
#include "pch.h"


// ������������ �������
enum Address
{
	MIDDLEWARE = 0,// ���������� � ���� �������
	BROADCAST = -1,// ����������������� �����
};

enum MessageTypes {
	MT_INIT,
	MT_EXIT,
	MT_CONFIRM,
	MT_GETDATA,
	MT_DATA,
	MT_NODATA
};

struct MsgHeader
{
	int msgFrom;
	int msgTo;
	int msgType;
	int msgSize;
};

class Message
{
private:
	MsgHeader msgHeader;
	string msgData;
public:

	// ����������� ��� ����������
	Message() {
		msgHeader = { 0 };
		msgData = "";
	}

	// ����������� � ����������
	Message(int to, int from, int type = MT_DATA, const string& data = "") {
		msgHeader.msgFrom = from;
		msgHeader.msgTo = to;
		msgHeader.msgType = type;
		msgHeader.msgSize = data.length();
		msgData = data;
	}

	// ������ ���������
	MsgHeader getHeader() {
		return msgHeader;
	}

	// ������ ������ ���������
	string getData() {
		return msgData;
	}

	// �������� ���������
	void Send(CSocket& s) {
		s.Send(&msgHeader, sizeof(MsgHeader));
		if (msgHeader.msgSize != 0) {
			s.Send(msgData.c_str(), msgHeader.msgSize + 1);
		}
	}


	// ��������� ���������
	int Receive(CSocket& s) {
		s.Receive(&msgHeader, sizeof(MsgHeader));
		if (msgHeader.msgSize != 0) {
			char* pBuff = new char[msgHeader.msgSize + 1];
			s.Receive(pBuff, msgHeader.msgSize + 1);
			pBuff[msgHeader.msgSize] = '\0';
			msgData = pBuff;
			delete[] pBuff;
		}

		return msgHeader.msgType;
	}

	static void SendMessage(CSocket& s, int to, int from, int type = MT_DATA, const string& data = "") {
		Message m(to, from, type, data);
		m.Send(s);
	}
};

