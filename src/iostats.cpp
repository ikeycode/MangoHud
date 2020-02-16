#include "iostats.h"
#include "string_utils.h"
#include <fstream>

pthread_t ioThread;
void *getIoStats(void *args) {
    iostats *io = reinterpret_cast<iostats *>(args);
    if (io) {
        io->prev.read_bytes  = io->curr.read_bytes;
        io->prev.write_bytes = io->curr.write_bytes;

        std::string line;
        std::ifstream f("/proc/self/io");
        while (std::getline(f, line)) {
            if (starts_with(line, "read_bytes:")) {
                try_stoull(io->curr.read_bytes, line.substr(12));
            }
            else if (starts_with(line, "write_bytes:")) {
                try_stoull(io->curr.write_bytes, line.substr(13));
            }
        }
        io->diff.read_bytes  = io->curr.read_bytes  - io->prev.read_bytes;
        io->diff.write_bytes = io->curr.write_bytes - io->prev.write_bytes;
    }
    pthread_detach(ioThread);
    return NULL;
}
