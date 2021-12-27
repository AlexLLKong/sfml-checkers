#include "SFML/Network.hpp"
#include "netcode.h"
#include <iostream>
#include <list>
int main() 
{
	std::cout << "Creating listener" << "\n";
    sf::TcpListener listener;
    listener.listen(55001);
    std::list<std::shared_ptr<sf::TcpSocket>> clients;
    sf::SocketSelector selector;
    selector.add(listener);

    struct Room {
        std::string ID;
        std::shared_ptr<sf::TcpSocket> player1;
        std::shared_ptr<sf::TcpSocket> player2;
    };
    std::list<Room> rooms;

    bool running = true;
    while (running)
    {
        // Make the selector wait for data on any socket
        if (selector.wait())
        {
            // Test the listener
            if (selector.isReady(listener))
            {
                // The listener is ready: there is a pending connection
                std::shared_ptr<sf::TcpSocket> client = std::make_shared<sf::TcpSocket>();
                if (listener.accept(*client) == sf::Socket::Done)
                {
                    std::cout << "New client connected: " << client->getRemoteAddress() << "\n";
                    // Add the new client to the clients list
                    clients.push_back(client);
                    // Add the new client to the selector so that we will
                    // be notified when they send something
                    selector.add(*client);
                    // The client has sent some data, we can receive it
                    sf::Packet packet;
                    if (client->receive(packet) == sf::Socket::Done)
                    {
                        // handle data
                        std::cout << "recieved packet from new client at: " << client->getRemoteAddress() << "\n";
                        std::string recString;
                        packet >> recString;

                        bool roomExists = false;
                        std::string answer = "Recieved room id: " + recString + "\n";
                        for (Room& room : rooms)
                        {
                            if (room.ID == recString)
                            {
                                roomExists = true;
                                room.player2 = client;
                                answer += "Added to existing room.";
                                sf::Packet sendPacket;
                                sendPacket << answer << roomExists;
                                client->send(sendPacket);
                                for (std::shared_ptr<sf::TcpSocket> clt : clients)
                                {
                                    if (clt == room.player1)
                                    {
                                        sf::Packet p1;
                                        sf::Int8 p2Connected = (sf::Int8)my::Netcode::CONNECTED;
                                        p1 << p2Connected;
                                        clt->send(p1);
                                    }
                                }
                            }
                        }

                        if (!roomExists)
                        {
                            Room newRoom;
                            newRoom.ID = recString;
                            newRoom.player1 = client;
                            answer += "Created new room: " + newRoom.ID;
                            rooms.push_back(newRoom);
                            sf::Packet sendPacket;
                            sendPacket << answer << roomExists;
                            client->send(sendPacket);
                        }
                    }
                }
                else
                {
                    // Error, we won't get a new connection, delete the socket
                    clients.erase(std::remove(clients.begin(), clients.end(), client));
                }
            }
            else
            {
                // The listener socket is not ready, test all other sockets (the clients)
                for (std::shared_ptr<sf::TcpSocket> clt : clients)
                {
                    sf::TcpSocket& client = *clt;
                    if (selector.isReady(client))
                    {
                        // The client has sent some data, we can receive it
                        sf::Packet packet;
                        if (client.receive(packet) == sf::Socket::Done)
                        {
                            std::cout << "recieved packet from client at: " << client.getRemoteAddress() << "\n";
                            // handle data
                            std::string clientIP = client.getRemoteAddress().toString();
                            for (Room room : rooms)
                            {
                                // TODO: Temporary while working with localhost clients. Normally should only send to other player
                                if (clt == room.player1)
                                {
                                    for (std::shared_ptr<sf::TcpSocket> clt2 : clients)
                                    {
                                        if (clt2 == room.player2)
                                        {
                                            clt2->send(packet);
                                        }
                                    }
                                }
                                else if (clt == room.player2)
                                {
                                    for (std::shared_ptr<sf::TcpSocket> clt2 : clients)
                                    {
                                        if (clt2 == room.player1)
                                        {
                                            clt2->send(packet);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}