#pragma once
class Session
{
private:
	// ������������� ������ (�������� ��������)
	int session_id;
	// �������  ���������
	queue<Message> messages;
	// ����������� ������ ��� ������ ������� �� �������������� �������������
	CRITICAL_SECTION criricalSection;
	// ��������� ����� ���������� �������
	clock_t  time;
public:
	// ����������� �� ���������
	Session() {
		InitializeCriticalSection(&criricalSection);
	}

	// ����������� � �����������
	Session(int ID, clock_t t)
		:session_id(ID), time(t) {
		InitializeCriticalSection(&criricalSection);
	}

	// ���������� �������� �� ������������ ����������� ������ 
	~Session() {
		DeleteCriticalSection(&criricalSection);
	}

	// ������ �������������� ������
	int getId() {
		return session_id;
	}

	// ������ �������������� ������
	void setId(int i) {
		session_id = i;
	}

	// ������ ������� ��������� ����������
	clock_t getTime() {
		return time;
	}


	// ������ ������� ��������� ����������
	void setTime(clock_t t) {
		time = t;
	}

	// ���������� ��������� � �������
	void Add(Message& m) {
		EnterCriticalSection(&criricalSection);

		messages.push(m);

		LeaveCriticalSection(&criricalSection);
	}

	// �������� ���������
	void Send(CSocket& s) {
		EnterCriticalSection(&criricalSection);

		if (messages.empty())
		{
			Message::SendMessage(s, session_id, MIDDLEWARE, MT_NODATA);
		}
		else
		{
			messages.front().Send(s);
			messages.pop();
		}

		LeaveCriticalSection(&criricalSection);
	}
};

