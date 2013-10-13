#include "AuthSocket.h"
#include "AuthServer.h"
#include "Utils/Util.h"
#include "Opcodes/Opcodes.h"
#include "Logs/Log.h"
#include "Configuration/ConfigMgr.h"

AuthSocket::AuthSocket(QTcpSocket *socket) : SocketHandler(socket)
{
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(OnClose()));
    Log::Write(LOG_TYPE_NORMAL, "New incoming connection from %s", m_socket->peerAddress().toString().toLatin1().data());
}

AuthSocket::~AuthSocket() {}

void AuthSocket::ProcessPacket()
{
    QDataStream in(m_socket);

    while (m_socket->bytesAvailable())
    {
        if (m_packetSize == 0)
        {
            if (m_socket->bytesAvailable() < (qint64)sizeof(quint16))
                return;

            in >> m_packetSize;
        }

        if ((m_socket->bytesAvailable() + 2) < m_packetSize)
            return;

        qint8 unk;
        quint16 opcode;

        in >> unk;
        in >> opcode;

        if (OpcodeTable::Exists(opcode))
        {
            Log::Write(LOG_TYPE_DEBUG, "Received packet opcode %s <%u> (size : %u).", OpcodeTable::Get(opcode).name.toLatin1().data(), opcode, m_packetSize);

            WorldPacket packet(opcode, in.device()->readAll());
            HandlePacket(opcode, packet);
        }
        else
            Log::Write(LOG_TYPE_DEBUG, "Received unhandled packet opcode %u (size : %u).", opcode, m_packetSize);

        m_packetSize = 0;
    }
}

void AuthSocket::OnClose()
{
    AuthServer::Instance()->RemoveSocket(this);
    SocketHandler::OnClose();
}

void AuthSocket::HandlePacket(quint16 opcode, WorldPacket& packet)
{
    switch (opcode)
    {
    case CMSG_CLIENT_VERSION:
        qint8 version, change;
        qint16 revision;
        QString build;
        qint8 buildSize;

        packet >> version;
        packet >> revision;
        packet >> change;
        packet >> buildSize;

        build = packet.ReadString(buildSize);

        QString clientVersion = QString(QString::number(version) + "." + QString::number(revision)
                                        + "." + QString::number(change) + "." + build);
        qDebug() << clientVersion;
        break;
    }
}
