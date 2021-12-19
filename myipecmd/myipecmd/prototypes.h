#pragma once


typedef struct endp {
	USB_ENDPOINT_DESCRIPTOR stcUsbEndpoint;
	struct endp * pstcEndpointBlockNext;
}ENDPOINT_BLOCK,*PENDPOINT_BLOCK;

typedef struct desc {
	USB_INTERFACE_DESCRIPTOR stcUsbInterface;
	struct desc* pstcInterfaceBlockNext;
	PENDPOINT_BLOCK pstcEndpointBlockHead;
	BYTE byNumberOfEndpoints;
} INTERFACE_BLOCK, *PINTERFACE_BLOCK;

typedef struct config{
	USB_CONFIGURATION_DESCRIPTOR stcConfigurationDescriptor;
	PINTERFACE_BLOCK pstcInterfaceDescriptorHead;
	BYTE byNumberOfInterfaces;
} CONFIGURATION, *PCONFIGURATION;

typedef struct address {
	UCHAR* ucAddresses;
	UCHAR ucLength;
}ENDPOINTADDRESSES, *PENDPOINTADDRESSES;


struct PIPE_ID
{
	UCHAR  PipeInId;
	UCHAR  PipeOutId;
};


BOOL bGetDeviceDescriptor(WINUSB_INTERFACE_HANDLE stcwinusbhandle, PUSB_DEVICE_DESCRIPTOR pstcusbdevicehandle);
BOOL bGetConfiguration(WINUSB_INTERFACE_HANDLE stcwinusbhandle, PCONFIGURATION pstcconfiguration);
BOOL bGetStringDescriptor(WINUSB_INTERFACE_HANDLE stcwinusbhandle, PUSB_STRING_DESCRIPTOR pstcusbconfigurationhandle, UCHAR uindex);
BOOL bGetUSBDeviceSpeed(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pDeviceSpeed);
BOOL bQueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid);
BOOL bAssignInOutEndpoints(PCONFIGURATION pstcconfiguration, PENDPOINTADDRESSES pucInEndpoints, PENDPOINTADDRESSES pucOutEndpoints);