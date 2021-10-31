using System;
using System.Text;
using System.Net.Sockets;

namespace ConsoleClient
{
    public enum Address
    {
        MIDDLEWARE = 0,
        BROADCAST = -1,
    };

    public enum MessageTypes
    {
        MT_INIT,
        MT_EXIT,
        MT_CONFIRM,
        MT_GETDATA,
        MT_DATA,
        MT_NODATA
    };

    struct MsgHeader
    {
        public int msgFrom;
        public int msgTo;
        public int msgType;
        public int msgSize;
    };

    class Msg
    {
        private MsgHeader msgHeader;
        private string msgData;

        public MsgHeader getMsgHeader()
        {
            return msgHeader;
        }

        public void setMsgData(string msgData)
        {
            this.msgData = msgData;
        }

        public string getMsgData()
        {
            return msgData;
        }

        public Msg()
        {
            msgHeader.msgTo = 0;
            msgHeader.msgFrom = 0;
            msgHeader.msgSize = 0;
            msgHeader.msgType = 0;
        }

        public Msg(int to, int from, MessageTypes type = MessageTypes.MT_DATA, string data = "")
        {
            msgHeader.msgTo = to;
            msgHeader.msgFrom = from;
            msgHeader.msgType = (int)type;
            msgHeader.msgSize = data.Length;
            msgData = data;
        }

        public void Send(Socket s)
        {
            //преобразует указанные данные в массив байтов
            s.Send(BitConverter.GetBytes(msgHeader.msgFrom), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(msgHeader.msgTo), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes((int)msgHeader.msgType), sizeof(int), SocketFlags.None);
            s.Send(BitConverter.GetBytes(msgHeader.msgSize), sizeof(int), SocketFlags.None);

            if (msgHeader.msgSize != 0)
            {
                //набор символов в последовательность байтов
                s.Send(Encoding.UTF8.GetBytes(msgData), msgHeader.msgSize, SocketFlags.None);
            }

        }

        public MessageTypes Receive(Socket s)
        {

            byte[] buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            msgHeader.msgFrom = BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            msgHeader.msgTo = BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            msgHeader.msgType = (int)(MessageTypes)BitConverter.ToInt32(buffer, 0);

            buffer = new byte[4];
            s.Receive(buffer, sizeof(int), SocketFlags.None);
            msgHeader.msgSize = BitConverter.ToInt32(buffer, 0);

            if (msgHeader.msgSize != 0)
            {
                buffer = new byte[msgHeader.msgSize];
                s.Receive(buffer, msgHeader.msgSize, SocketFlags.None);
                msgData = Encoding.UTF8.GetString(buffer, 0, msgHeader.msgSize);
            }

            return (MessageTypes)(int)msgHeader.msgType;
        }
    }
}
