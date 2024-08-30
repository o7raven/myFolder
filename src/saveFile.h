#ifndef saveFile_H_
#define saveFile_H_
#include <unistd.h>
#include "network/packet.h"
#include "misc/notifications.h"
#include "structures.h"
int saveFile(CLIENT* client, const char* directory);

#endif // !saveFile_H_
