
#include "IHWCPrivateService.h"

class Hwcps: public android::BnHWCPrivateService {
public:
    static Hwcps *instantiate();
    void init();

private:
    Hwcps() {}
   ~Hwcps() {};

    /* API define in IHWCPrivateService.h */
    virtual int blank(int display, int enable);
    virtual int switchDevice(const DeviceTable& tables);
    virtual int setOutputMode(int display, int type, int mode);
    virtual int setMargin(int display, int l, int r, int t, int b);
    virtual int setVideoRatio(int display, int ratio);
    virtual int set3DMode(int display, int mode);
    virtual int setDataspace(int display, int dataspace);
    virtual int registerCallback(android::sp<android::IBinder>& binder);
    virtual void setDebugTag(int32_t tag);
    android::status_t dump(int fd, const android::Vector<android::String16>& args);

    static Hwcps* mInstance;
};

