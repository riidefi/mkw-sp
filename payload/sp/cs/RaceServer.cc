#include "RaceServer.hh"

#include "sp/cs/RoomServer.hh"

#include <game/kart/KartObjectManager.hh>
#include <game/system/RaceManager.hh>
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

namespace SP {

void RaceServer::destroyInstance() {
    DestroyInstance();
}

void RaceServer::calcRead() {
    std::bitset<12> stacked;
    ConnectionGroup connectionGroup(*this);

    while (true) {
        u8 buffer[RaceClientFrame_size];
        auto read = m_socket.read(buffer, sizeof(buffer), connectionGroup);
        if (!read) {
            break;
        }

        pb_istream_t stream = pb_istream_from_buffer(buffer, read->size);

        RaceClientFrame frame;
        if (!pb_decode(&stream, RaceClientFrame_fields, &frame)) {
            continue;
        }

        u32 clientId = connectionGroup.clientId(read->index);
        assert(m_clients[clientId]);
        if (isFrameValid(frame, clientId)) {
            if (stacked[clientId]) {
                // TODO 2P
                auto &framePlayer = frame.players[0];
                auto &prevFramePlayer = m_clients[clientId]->frame->players[0];
                if (!framePlayer.inputState.item) {
                    framePlayer.inputState.item = prevFramePlayer.inputState.item;
                }
                if (framePlayer.inputState.trick == 0) {
                    framePlayer.inputState.trick = prevFramePlayer.inputState.trick;
                }
            }
            m_clients[clientId]->frame = frame;
            stacked[clientId] = true;
        }
    }

    for (u32 i = 0; i < m_playerCount; i++) {
        if (!m_clients[m_players[i].clientId]->frame) {
            return;
        }
    }

    System::RaceManager::Instance()->m_canStartCountdown = true;

    if (System::RaceManager::Instance()->hasReachedStage(System::RaceManager::Stage::Countdown)) {
        for (u32 i = 0; i < m_playerCount; i++) {
            // TODO 2P
            auto &framePlayer = m_clients[m_players[i].clientId]->frame->players[0];
            auto *pad = System::InputManager::Instance()->extraUserPad(i);
            pad->m_userInputState.accelerate = framePlayer.inputState.accelerate;
            pad->m_userInputState.brake = framePlayer.inputState.brake;
            pad->m_userInputState.item = framePlayer.inputState.item;
            pad->m_userInputState.drift = framePlayer.inputState.drift;
            pad->m_userInputState.brakeDrift = framePlayer.inputState.brakeDrift; // TODO check for 200cc
            System::RaceInputState::SetStickX(pad->m_userInputState, framePlayer.inputState.stickX);
            System::RaceInputState::SetStickY(pad->m_userInputState, framePlayer.inputState.stickY);
            System::RaceInputState::SetTrick(pad->m_userInputState, framePlayer.inputState.trick);
        }
    }
}

void RaceServer::calcWrite() {
    if (!System::RaceManager::Instance()->m_canStartCountdown) {
        return;
    }

    RaceServerFrame frame;
    frame.players_count = m_playerCount;
    for (u32 i = 0; i < m_playerCount; i++) {
        auto *inputManager = System::InputManager::Instance();
        auto &inputState = inputManager->extraGhostProxy(i)->currentRaceInputState();
        frame.players[i].inputState.accelerate = inputState.accelerate;
        frame.players[i].inputState.brake = inputState.brake;
        frame.players[i].inputState.item = inputState.item;
        frame.players[i].inputState.drift = inputState.drift;
        frame.players[i].inputState.brakeDrift = inputState.brakeDrift;
        frame.players[i].inputState.stickX = inputState.rawStick.x;
        frame.players[i].inputState.stickY = inputState.rawStick.y;
        frame.players[i].inputState.trick = inputState.rawTrick;
        auto *object = Kart::KartObjectManager::Instance()->object(i);
        frame.players[i].timeBeforeRespawn = object->getTimeBeforeRespawn();
        frame.players[i].timeInRespawn = object->getTimeInRespawn();
        frame.players[i].timesBeforeBoostEnd_count = 3;
        for (u32 j = 0; j < 3; j++) {
            frame.players[i].timesBeforeBoostEnd[j] = object->getTimeBeforeBoostEnd(j * 2);
        }
        frame.players[i].pos = *object->getPos();
        frame.players[i].mainRot = *object->getMainRot();
        frame.players[i].internalSpeed = object->getInternalSpeed();
    }
    for (u32 i = 0; i < 12; i++) {
        if (m_clients[i] && m_clients[i]->frame) {
            frame.time = System::RaceManager::Instance()->time();
            frame.clientTime = m_clients[i]->frame->time;

            u8 buffer[RaceServerFrame_size];
            pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

            assert(pb_encode(&stream, RaceServerFrame_fields, &frame));

            m_socket.write(buffer, stream.bytes_written, m_clients[i]->connection);
        }
    }
}

RaceServer *RaceServer::CreateInstance() {
    assert(s_block);
    assert(!s_instance);
    auto *roomServer = RoomServer::Instance();
    std::array<std::optional<Client>, 12> clients{};
    for (u32 i = 0; i < 12; i++) {
        if (auto keypair = roomServer->clientKeypair(i)) {
            clients[i] = {{}, {0, 0, *keypair}};
        }
    }
    s_instance = new (s_block) RaceServer(clients, 21330);
    RaceManager::s_instance = s_instance;
    return s_instance;
}

void RaceServer::DestroyInstance() {
    assert(s_instance);
    s_instance->~RaceServer();
    RaceManager::s_instance = nullptr;
    s_instance = nullptr;
}

RaceServer *RaceServer::Instance() {
    return s_instance;
}

RaceServer::RaceServer(std::array<std::optional<Client>, 12> clients, u16 port)
    : m_clients(clients), m_socket("race    ", port) {}

RaceServer::~RaceServer() {
    hydro_memzero(&m_clients, sizeof(m_clients));
}

bool RaceServer::isFrameValid(const RaceClientFrame &frame, u32 clientId) {
    if (m_clients[clientId]->frame && frame.time <= m_clients[clientId]->frame->time) {
        return false;
    }

    for (u32 i = 0; i < frame.players_count; i++) {
        if (!IsInputStateValid(frame.players[i].inputState)) {
            return false;
        }
    }

    return true;
}

RaceServer::ConnectionGroup::ConnectionGroup(RaceServer &server) : m_server(server) {
    for (u32 i = 0; i < 12; i++) {
        if (m_server.m_clients[i]) {
            m_clientIds[m_clientCount++] = i;
        }
    }
}

u32 RaceServer::ConnectionGroup::clientId(u32 index) const {
    return m_clientIds[index];
}

u32 RaceServer::ConnectionGroup::count() {
    return m_clientCount;
}

Net::UnreliableSocket::Connection &RaceServer::ConnectionGroup::operator[](u32 index) {
    return m_server.m_clients[m_clientIds[index]]->connection;
}

RaceServer *RaceServer::s_instance = nullptr;

} // namespace SP
