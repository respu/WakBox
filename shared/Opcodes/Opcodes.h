#ifndef OPCODES_H
#define OPCODES_H

#include <QtCore>

class WorldPacket;
class WorldSession;

enum Opcodes
{
    // Client messages (CMSG)
    CMSG_CLIENT_DISCONNECT                      = 1,
    CMSG_CLIENT_VERSION                         = 7,
    CMSG_CLIENT_AUTH                            = 1025,

    // Server messages (SMSG)
    SMSG_CLIENT_AUTH_RESULT                     = 1024,
    SMSG_RSA_PUBLIC_KEY                         = 1032
};

struct OpcodeHandler
{
    QString name;
    void (WorldSession::*handler)(WorldPacket& packet);
};

typedef void(WorldSession::*opcodeHandler)(WorldPacket& packet);
typedef QMap<Opcodes, OpcodeHandler> OpcodesList;

class OpcodeTable
{
public:
    static void Load();

    static void Add(quint16 opcode, char const* name, opcodeHandler handler)
    {
        OpcodeHandler opcodeHandler;
        opcodeHandler.name = QString(name);
        opcodeHandler.handler = handler;

        opcodesList.insert(Opcodes(opcode), opcodeHandler);
    }

    static bool Exists(quint16 id)
    {
        return opcodesList.contains((Opcodes)id);
    }

    static OpcodeHandler Get(quint16 id)
    {
        return opcodesList.value((Opcodes)id);
    }

private:
    static OpcodesList opcodesList;
};

inline QString GetOpcodeName(quint16 id)
{
    if (!OpcodeTable::Exists(id))
        return "Unknown opcode";

    return OpcodeTable::Get(id).name;
}

#endif
