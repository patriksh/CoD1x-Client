#include "discord.hpp"
#include "functions.hpp"

using namespace std;

int discordUpdate = time(0);
int64_t discordTimestamp = time(0);
static char serverInf[1024];
static char lastServer[21];
char discordMap[1024];
char discordHost[1024];

#define GETSTATUS "\xff\xff\xff\xff" "getstatus"

vector<string> split(const string& s, char delim) {
	vector<string> result;
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim)) {
		result.push_back(item);
	}

	return result;
}

short BigShort(short l) {
	byte b1, b2;

	b1 = l & 255;
	b2 = (l >> 8) & 255;

	return (b1 << 8) + b2;
}

// TODO: Implement error handling because this is a frickin mess.

void discordServerinfo(netadr_t* a) {
	char ipStr[50];
	strncpy(ipStr, va("%u.%u.%u.%u", a->ip[0], a->ip[1], a->ip[2], a->ip[3]), sizeof(ipStr));

	WSASession Session;
	UDPSocket Socket;
	std::string data = "\xff\xff\xff\xff" "getstatus";
	char infoResponse[1024];

	Socket.SendTo(ipStr, BigShort(a->port), data.c_str(), data.size());
	Socket.RecvFrom(infoResponse, 1024);

	std::string infoResponseStr(infoResponse);
	vector<string> infoResponseLines = split(infoResponseStr, '\n');
	vector<string> vars = split(infoResponseLines[1], '\\');

	int i = 0;
	for (string v : vars) {
		if (v == "mapname") {
			strncpy(discordMap, vars[i + 1].c_str(), strlen(vars[i + 1].c_str()) + 1);
		} else if (v == "sv_hostname") {
			// why the hell I'm not using like a new variable instead of vars[i + 1]?! fix
			vars[i + 1] = clearSymbols(vars[i + 1]);
			vars[i + 1] = clearColors(vars[i + 1]);
			strncpy(discordHost, vars[i + 1].c_str(), strlen(vars[i + 1].c_str()) + 1);
		}
		i++;
	}
}

void discordInit() {
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize("804794761696509972", &handlers, 1, NULL);
}

void discordPresence(const char* state, const char* details) {
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = state;
	discordPresence.details = details;
	discordPresence.startTimestamp = discordTimestamp;
	if (discordMap != nullptr) {
		discordPresence.largeImageKey = discordMap;
		discordPresence.largeImageText = discordMap;
	}
	discordPresence.instance = 0;
	Discord_UpdatePresence(&discordPresence);
}

void X_Discord_Frame() {
	discordInit();
	netadr_t* server = (netadr_t*)0x15CE86C;
	double discordPassed = difftime(time(0), discordUpdate);

	if (discordPassed >= 10) {
		if (server->type == NA_IP) {
			discordServerinfo(server);
			discordPresence(NET_AdrToString(*server), discordHost);
		} else {
			discordPresence("In the menus.", "");
		}

		discordUpdate = time(0);
	}

	Discord_RunCallbacks();
}