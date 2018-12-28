
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <utils/Log.h>

#include <softwinner/homlet/displayd/1.0/IDisplayd.h>

using softwinner::homlet::displayd::V1_0::IDisplayd;
using softwinner::homlet::displayd::V1_0::Dataspace;

static void usage() {
    fprintf(stderr,
        "usage: dispdebug\n"
        "           To dump displayd debug infor.\n"
        "or:\n"
        "       dispdebug [--help | -i INDEX\n"
        "           | --output HDR/SDR\n"
        "           | --pixelformat RGB/Y444/Y422/Y420\n"
        "           | --depth 8/10/12\n"
        "           | --list\n"
        "           | --mode MODE]\n");
}

static struct option longOptions[] = {
    {"help",        no_argument,       0,  'h'},
    {"list",        no_argument,       0,  'l'},
    {"transfer",    required_argument, 0,  't'},
    {"pixelformat", required_argument, 0,  'p'},
    {"depth",       required_argument, 0,  'd'},
    {"mode",        required_argument, 0,  'm'},
    {NULL,          0,                 0,   0 }
};

enum OptionRequest {
    kListMode       = 1 << 0,
    kSetHdrOrSdr    = 1 << 1,
    kSetPixelFormat = 1 << 2,
    kSetDepth       = 1 << 3,
    kSetMode        = 1 << 4,
};

struct option_contex {
    int displayIndex;
    Dataspace transfer;
    int pixelformat;
    int depth;
    int mode;
    int mask;
};


static void dumpSupportedModes(const android::sp<IDisplayd>& proxy, const option_contex *option);
static void setTranser(const android::sp<IDisplayd>& proxy, const option_contex *option);

int main(int argc, char* const argv[]) {
    signal(SIGPIPE, SIG_IGN);
    android::sp<IDisplayd> displayd = IDisplayd::getService();
    fflush(stdout);
    if (displayd == nullptr) {
        ALOGE("Unable to get Displayd service!");
        fprintf(stdout, "dispdebug: unable to get default displayd service!");
        return 0;
    }

    // Dump debug info only.
    if (argc == 1) {
        std::string info;
        displayd->dumpDebugInfo([&](const auto& tmpInfo) {
            info = tmpInfo.c_str();
        });
        fprintf(stdout, "\n%s\n", info.c_str());
        return 0;
    }

    struct option_contex option;
    memset(&option, 0, sizeof(option));

    while (true) {
        int c;
        int optionIndex = 0;

        c = getopt_long(argc, argv, "+hli:t:p:d:m:", longOptions, &optionIndex);
        if (c == -1)
            break;

        switch (c) {
        case 'i':
            char *endptr;
            option.displayIndex = strtol(optarg, &endptr, 10);
            if (*endptr != 0 || option.displayIndex < 0) {
                fprintf(stderr, "Error: invalid display index: %s\n", optarg);
                return -1;
            }
            break;
        case 'l':
            option.mask |= kListMode;
            break;
        case 't':
            if (!strcmp("HDR", optarg))
                option.transfer = Dataspace::DATASPACE_MODE_HDR;
            else
                option.transfer = Dataspace::DATASPACE_MODE_SDR;
            option.mask |= kSetHdrOrSdr;
            break;
        case 'h':
        default:
            fprintf(stderr, "\n");
            usage();
            return -1;
        }
    }

    if (option.mask & kListMode)
        dumpSupportedModes(displayd, &option);

    if (option.mask & kSetHdrOrSdr)
        setTranser(displayd, &option);

    return 0;
}


static void dumpSupportedModes(const android::sp<IDisplayd>& proxy, const option_contex *option) {

}


static void setTranser(const android::sp<IDisplayd>& proxy, const option_contex *option) {
    proxy->setDataspaceMode(option->displayIndex, option->transfer);
}

