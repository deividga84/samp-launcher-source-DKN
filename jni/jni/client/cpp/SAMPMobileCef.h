/*
	EN: CEF client library for SA:MP Mobile.
	UK: Клієнтська бібліотека CEF для SA:MP Mobile.

	Copyright © 2024 Denis Akazuki <https://github.com/denis-akazuki>

	EN: Out of respect, please do not delete this comment if you use code from this project.
	UK: З поваги, будь ласка, не видаляйте цей коментар, якщо використовуєте код із цього проєкту.
*/

#ifndef SAMPMOBILECEF_H
#define SAMPMOBILECEF_H

#include <jni.h>

#include "../raknet/RakClientInterface.h"

namespace cef
{
	void setGamePath(const char *path);

	void initNetwork(RakClientInterface *rakClient, int cefPacketId);
	void handleServerConnection();
	void handlePacket(Packet *packet);
}

#endif