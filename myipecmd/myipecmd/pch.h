#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <winusb.h>
#include <usb.h>
//
// Define below GUIDs
//
#include <initguid.h>

//
// Device Interface GUID.
// Used by all WinUsb devices that this application talks to.
// Must match "DeviceInterfaceGUIDs" registry value specified in the INF file.
// 627b0cba-3e3a-48b5-bbc6-212d3117576e
//
DEFINE_GUID(GUID_DEVINTERFACE_myipecmd, 0xB9BE6B42, 0x2039, 0x4057, 0xA6, 0x6A, 0x4A, 0xAF, 0xB6, 0xD3, 0x7D, 0xF4);
// 0xdeb97e2c,0x8b0f,0x446f,0xb2,0x80,0x7c,0xfa,0xc4,0x1c,0x3b,0xd9);
// 0x627b0cba,0x3e3a,0x48b5,0xbb,0xc6,0x21,0x2d,0x31,0x17,0x57,0x6e);

typedef struct _DEVICE_DATA {

    BOOL                    HandlesOpen;
    WINUSB_INTERFACE_HANDLE WinusbHandle;
    HANDLE                  DeviceHandle;
    TCHAR                   DevicePath[MAX_PATH];

} DEVICE_DATA, * PDEVICE_DATA;

HRESULT
OpenDevice(
    _Out_     PDEVICE_DATA DeviceData,
    _Out_opt_ PBOOL        FailureDeviceNotFound
);

VOID
CloseDevice(
    _Inout_ PDEVICE_DATA DeviceData
);



#define logw(args,...) wprintf((args),__VA_ARGS__)
#define log(args,...) printf((args_), __VA_ARGS__)


#include "prototypes.h"

