#include "pch.h"
#include <cfgmgr32.h>


#define halt() logw(L"ERROR:NULL POINTER"); while(1){}

BOOL bGetDeviceDescriptor(WINUSB_INTERFACE_HANDLE stcwinusbhandle, PUSB_DEVICE_DESCRIPTOR pstcusbdevicehandle) {
	BOOL bResult;
	ULONG ulLengthReceived;

	//
	// Get device descriptor
	//
	bResult = WinUsb_GetDescriptor(stcwinusbhandle,
		USB_DEVICE_DESCRIPTOR_TYPE,
		0,
		0,
		(PBYTE)pstcusbdevicehandle,
		sizeof(USB_DEVICE_DESCRIPTOR),
		&ulLengthReceived);

	if (FALSE == bResult || ulLengthReceived != sizeof(USB_DEVICE_DESCRIPTOR)) {

		logw(L"Error among LastError %d or ulLengthReceived %d\n",
			FALSE == bResult ? GetLastError() : 0,
			ulLengthReceived);
		return FALSE;
	}

	//
	// Print a few parts of the device descriptor
	//

	logw(L"Device Descriptor:: VID_%04X&PID_%04X\n", pstcusbdevicehandle->idVendor, pstcusbdevicehandle->idProduct);

	logw(L"\t%-20s\t%-10d\t%-40s\n", L"bLength:", pstcusbdevicehandle->bLength, L"Size of the Descriptor in Bytes");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"TbDescriptorType:", pstcusbdevicehandle->bDescriptorType, L"Device Descriptor (0x01)");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"bcdUSB:", pstcusbdevicehandle->bcdUSB, L"USB Specification Number which device complies too");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"bDeviceClass:", pstcusbdevicehandle->bDeviceClass, L"Class Code (Assigned by USB Org) If equal to Zero, each interface specifies it’s own class code If equal to 0xFF, the class code is vendor specified.Otherwise field is valid Class Code");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"bDeviceSubClass:", pstcusbdevicehandle->bDeviceSubClass, L"Subclass Code (Assigned by USB Org)");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"bDeviceProtocol:", pstcusbdevicehandle->bDeviceProtocol, L"Protocol Code (Assigned by USB Org)");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"bMaxPacketSize:", pstcusbdevicehandle->bMaxPacketSize0, L"Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"bcdDevice:", pstcusbdevicehandle->bcdDevice, L"Device Release Number");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"iManufacturer:", pstcusbdevicehandle->iManufacturer, L"Index of Manufacturer String Descriptor");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"iProduct:", pstcusbdevicehandle->iProduct, L"Index of Product String Descriptors");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"iSerialNumber:", pstcusbdevicehandle->iSerialNumber, L"Index of Serial Number String Descriptor");
	logw(L"\t%-20s\t%-10d\t%-40s\n", L"bNumConfigurations:", pstcusbdevicehandle->bNumConfigurations, L"Number of Possible Configurations");/**/

	return TRUE;
}

BOOL bGetConfiguration(WINUSB_INTERFACE_HANDLE stcwinusbhandle, PCONFIGURATION pstcconfiguration){

	

	BOOL bResult;
	ULONG ulLengthReceived;
	PINTERFACE_BLOCK pstcCurrentInterfaceBlock = NULL;
	PENDPOINT_BLOCK pstcCurrentEndpointBlock = NULL;

	BYTE arrBuffer[100];
	BYTE offset = 0;

	//
	// Get Configuration Descriptor
	// You can parse this descript for all the interfaces and endpoints
	bResult = WinUsb_GetDescriptor(stcwinusbhandle,
		USB_CONFIGURATION_DESCRIPTOR_TYPE,
		0,
		0,
		(PBYTE)arrBuffer,
		sizeof(arrBuffer),
		&ulLengthReceived);

	if (FALSE == bResult ) {
		//logw(L"Error among LastError", GetLastError());
		return FALSE;
	}

	memset(pstcconfiguration, 0, sizeof(CONFIGURATION));
	if(ulLengthReceived>0){

		//
		// Print a few parts of the configuration descriptor
		//	
		
		
		memcpy((PBYTE)&(pstcconfiguration->stcConfigurationDescriptor), arrBuffer, sizeof(USB_CONFIGURATION_DESCRIPTOR));
		logw(L"\n\nConfiguration Descriptor::\n");
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"bLength:", pstcconfiguration->stcConfigurationDescriptor.bLength, L"Size of the Descriptor in Bytes");
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"bDescriptorType:", pstcconfiguration->stcConfigurationDescriptor.bDescriptorType, L"Configuration Descriptor (0x02)");
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"wTotalLength:", pstcconfiguration->stcConfigurationDescriptor.wTotalLength, L"Total length in bytes of data returned");
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"bNumInterfaces:", pstcconfiguration->stcConfigurationDescriptor.bNumInterfaces, L"Number of Interfaces");
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"bConfigurationValue:", pstcconfiguration->stcConfigurationDescriptor.bConfigurationValue, L"Value to use as an argument to select this configuration");
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"iConfiguration:", pstcconfiguration->stcConfigurationDescriptor.iConfiguration, L"Index of String Descriptor describing this configuration");
		logw(L"\t%-20s\t%-10x\t%-40s\n", L"bmAttributes:", pstcconfiguration->stcConfigurationDescriptor.bmAttributes, L"D7 Reserved, set to 1. (USB 1.0 Bus Powered) D6 Self Powered D5 Remote Wakeup D4..0 Reserved, set to 0.");
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"bMaxPower:", pstcconfiguration->stcConfigurationDescriptor.MaxPower, L"Maximum Power Consumption in 2mA units ");

		
		offset = pstcconfiguration->stcConfigurationDescriptor.bLength;
		pstcconfiguration->pstcInterfaceDescriptorHead = NULL;
		
		
		// Get interface and endpoints
		while (offset < pstcconfiguration->stcConfigurationDescriptor.wTotalLength) {
			switch (arrBuffer[offset+1]) {
				case 0x04:
					
					// if it is the first interface
					if (pstcconfiguration->pstcInterfaceDescriptorHead == NULL) {
						pstcCurrentInterfaceBlock = new INTERFACE_BLOCK;
						pstcCurrentInterfaceBlock->byNumberOfEndpoints = 0;
						pstcCurrentInterfaceBlock->pstcEndpointBlockHead = NULL;
						pstcCurrentInterfaceBlock->pstcInterfaceBlockNext = NULL;
						pstcconfiguration->pstcInterfaceDescriptorHead = pstcCurrentInterfaceBlock;
					}
					// Connect Previous to the current
					else {
	
						pstcCurrentInterfaceBlock->pstcInterfaceBlockNext = new INTERFACE_BLOCK;
						pstcCurrentInterfaceBlock = pstcCurrentInterfaceBlock->pstcInterfaceBlockNext;
						pstcCurrentInterfaceBlock->byNumberOfEndpoints = 0;
						pstcCurrentInterfaceBlock->pstcEndpointBlockHead = NULL;
						pstcCurrentInterfaceBlock->pstcInterfaceBlockNext = NULL;
					}

					memcpy((PBYTE)&pstcCurrentInterfaceBlock->stcUsbInterface, arrBuffer + offset, sizeof(USB_INTERFACE_DESCRIPTOR));

					// Print Interface Info
					logw(L"\n\tInterface Descriptor:: # %d\n", pstcconfiguration->byNumberOfInterfaces);
					logw(L"\t\t%-20s\t%-10d\t%-40s\n", L"bLength:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->bLength, L"Size of the Descriptor in Bytes");
					logw(L"\t\t%-20s\t%-10d\t%-40s\n", L"bDescriptorType:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->bDescriptorType, L"String Descriptor (0x04)");
					logw(L"\t\t%-20s\t%-10d\t%-40s\n", L"bInterfaceNumber:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->bInterfaceNumber, L"Number of Interface");
					logw(L"\t\t%-20s\t%-10d\t%-40s\n", L"bAlternateSetting:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->bAlternateSetting, L"Value used to select alternative setting");
					logw(L"\t\t%-20s\t%-10d\t%-40s\n", L"bNumEndpoints:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->bNumEndpoints, L"Number of Endpoints used for this interface");
					logw(L"\t\t%-20s\t%-10x\t%-40s\n", L"bInterfaceClass:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->bInterfaceClass, L"Class Code (Assigned by USB Org)");
					logw(L"\t\t%-20s\t%-10x\t%-40s\n", L"bInterfaceSubClass:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->bInterfaceSubClass, L"Subclass Code (Assigned by USB Org)");
					logw(L"\t\t%-20s\t%-10x\t%-40s\n", L"bInterfaceProtocol:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->bInterfaceProtocol, L"Protocol Code (Assigned by USB Org)");
					logw(L"\t\t%-20s\t%-10x\t%-40s\n", L"iInterface:", (&pstcCurrentInterfaceBlock->stcUsbInterface)->iInterface, L"Index of String Descriptor Describing this interface");



					offset += sizeof(USB_INTERFACE_DESCRIPTOR);
					++pstcconfiguration->byNumberOfInterfaces;
					break;

				case 0x05:
					
					if(pstcCurrentInterfaceBlock != NULL){
						// First Endpoint 
						if (pstcCurrentInterfaceBlock->pstcEndpointBlockHead == NULL) {
							pstcCurrentEndpointBlock = new ENDPOINT_BLOCK;
							pstcCurrentEndpointBlock->pstcEndpointBlockNext= NULL;
							pstcCurrentInterfaceBlock->pstcEndpointBlockHead = pstcCurrentEndpointBlock;
						
					
						}// Continue with the endspoints
						else {
							pstcCurrentEndpointBlock->pstcEndpointBlockNext = new ENDPOINT_BLOCK;
							pstcCurrentEndpointBlock = pstcCurrentEndpointBlock->pstcEndpointBlockNext;
							pstcCurrentEndpointBlock->pstcEndpointBlockNext = NULL;
						}
						memcpy((PBYTE)&pstcCurrentEndpointBlock->stcUsbEndpoint, arrBuffer + offset, sizeof(USB_ENDPOINT_DESCRIPTOR));
						// Print 
						logw(L"\n\t\tEndpoint Descriptor:: #%d\n", pstcCurrentInterfaceBlock->byNumberOfEndpoints);
						logw(L"\t\t\t%-20s\t%-10d\t%-40s\n", L"bLength:", (&pstcCurrentEndpointBlock->stcUsbEndpoint)->bLength, L"Size of the Descriptor in Bytes(7Bytes)");
						logw(L"\t\t\t%-20s\t%-10d\t%-40s\n", L"bDescriptorType:", (&pstcCurrentEndpointBlock->stcUsbEndpoint)->bDescriptorType, L"Endpoint  Descriptor (0x05)");
						logw(L"\t\t\t%-20s\t0x%-8x\t%-40s\n", L"bEndpointAddress:", (&pstcCurrentEndpointBlock->stcUsbEndpoint)->bEndpointAddress, L"Endpoint Address Bits 0..3b Endpoint Number. Bits 4..6b Reserved.Set to Zero Bits 7 Direction 0 = Out, 1 = In(Ignored for Control Endpoints)");
						logw(L"\t\t\t%-20s\t%-10d\t%-40s\n", L"bmAttributes:", (&pstcCurrentEndpointBlock->stcUsbEndpoint)->bmAttributes, L"Bits 0..1 Transfer Type 00 = Control 01 = Isochronous 10 = Bulk 11 = Interrupt \n\t\t\t\t\t\t\t\tBits 2..4 are reserved.If Isochronous endpoint, Bits 3..2 = Synchronisation Type(Iso Mode) 00 = No Synchonisation 01 = Asynchronous 10 = Adaptive 11 = Synchronous \n\t\t\t\t\t\t\t\tBits 5..4 = Usage Type(Iso Mode) 00 = Data Endpoint 01 = Feedback Endpoint 10 = Explicit Feedback Data Endpoint 11 = Reserved");
						logw(L"\t\t\t%-20s\t%-10d\t%-40s\n", L"wMaxPacketSize:", (&pstcCurrentEndpointBlock->stcUsbEndpoint)->wMaxPacketSize, L"Maximum Packet Size this endpoint is capable of sending or receiving");
						logw(L"\t\t\t%-20s\t%-10x\t%-40s\n", L"bInterval:", (&pstcCurrentEndpointBlock->stcUsbEndpoint)->bInterval, L"Interval for polling endpoint data transfers. Value in frame counts. Ignored for Bulk & Control Endpoints. Isochronous must equal 1 and field may range from 1 to 255 for interrupt endpoints.");
					
						offset += sizeof(USB_ENDPOINT_DESCRIPTOR);
						++pstcCurrentInterfaceBlock->byNumberOfEndpoints;
					}
					else {
						halt();
					}
					break;

			break;
			
			}
			
		}
	}

	return TRUE;
		
}

BOOL bGetStringDescriptor(WINUSB_INTERFACE_HANDLE stcwinusbhandle, PUSB_STRING_DESCRIPTOR pstcusbstringhandle, UCHAR ucndex) {
	BOOL bResult;
	ULONG ulLengthReceived;
	UCHAR ucStringIndex;

	bResult = WinUsb_GetDescriptor(stcwinusbhandle,
		USB_STRING_DESCRIPTOR_TYPE,
		ucndex,
		0,
		(PBYTE)pstcusbstringhandle,
		sizeof(USB_STRING_DESCRIPTOR),
		&ulLengthReceived);

	if (FALSE == bResult || ulLengthReceived != sizeof(USB_STRING_DESCRIPTOR)) {

		logw(L"Error among LastError %d or ulLengthReceived %d\n",
			FALSE == bResult ? GetLastError() : 0,
			ulLengthReceived);
		return FALSE;
	}

	logw(L"\n\nString Descriptor:: #%d\n", ucndex);

	if(ucndex == 0){
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"bLength:", pstcusbstringhandle->bLength, L"Size of the Descriptor in Bytes");
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"bDescriptorType:", pstcusbstringhandle->bDescriptorType, L"String Descriptor (0x03)");
		logw(L"\n\tSupported Language Code Zero(e.g. 0x0409 English - United States)\n");

		for (ucStringIndex = 0; ucStringIndex < (pstcusbstringhandle->bLength-2); ucStringIndex = ucStringIndex + 2) {
			logw(L"\t%-20s\t%04X\t%-40s\n", L"wLANGID:", pstcusbstringhandle->bString[ucStringIndex], L" ");
		}

	}
	else {
		logw(L"\t%-20s\t%-10d\t%-40s\n", L"bLength:", pstcusbstringhandle->bLength, L"Size of the Descriptor in Bytes");
		logw(L"\t%-20s\t%-10d\t%-40s\n\n\t", L"bDescriptorType:", pstcusbstringhandle->bDescriptorType, L"String Descriptor (0x03)");

		logw(L"\n\t(Hex:)\n\t");
		for (ucStringIndex = 0; ucStringIndex < (pstcusbstringhandle->bLength - 2); ucStringIndex = ucStringIndex+2) {
			logw(L"%04X ", pstcusbstringhandle->bString[ucStringIndex]);

			if (ucStringIndex %16 ==0) {
				logw(L"\n\t");
			}
		}

		logw(L"\n\t(String:)\n\t");
		for (ucStringIndex = 0; ucStringIndex < (pstcusbstringhandle->bLength - 2); ucStringIndex = ucStringIndex + 2) {
			logw(L"%c", pstcusbstringhandle->bString[ucStringIndex]);
		}

	}

	return TRUE;
}


BOOL bGetUSBDeviceSpeed(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pDeviceSpeed)
{
	if (!pDeviceSpeed || hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bResult = TRUE;

	ULONG length = sizeof(UCHAR);

	bResult = WinUsb_QueryDeviceInformation(hDeviceHandle, DEVICE_SPEED, &length, pDeviceSpeed);
	if (!bResult)
	{
		printf("Error getting device speed: %d.\n", GetLastError());
		goto done;
	}

	if (*pDeviceSpeed == LowSpeed)
	{
		printf("Device speed: %d (Low speed).\n", *pDeviceSpeed);
		goto done;
	}
	if (*pDeviceSpeed == FullSpeed)
	{
		printf("Device speed: %d (Full speed).\n", *pDeviceSpeed);
		goto done;
	}
	if (*pDeviceSpeed == HighSpeed)
	{
		printf("Device speed: %d (High speed).\n", *pDeviceSpeed);
		goto done;
	}

done:
	return bResult;
}

BOOL bAssignInOutEndpoints(PCONFIGURATION pstcconfiguration, PENDPOINTADDRESSES pucInEndpoints, PENDPOINTADDRESSES pucOutEndpoints) {
	BOOL bStatus = FALSE;
	UINT32 ui32SpaceToAllocate = 0;
	PINTERFACE_BLOCK ifHead = pstcconfiguration->pstcInterfaceDescriptorHead;

	for (int i = 0; i<pstcconfiguration->byNumberOfInterfaces; i++) {
		ui32SpaceToAllocate += ifHead->byNumberOfEndpoints;
		ifHead = ifHead->pstcInterfaceBlockNext;
	}

	if (ui32SpaceToAllocate) {
		
		// Need to differentiate between ins and outs to save memory, but don't care 
		pucInEndpoints->ucAddresses = new UCHAR[ui32SpaceToAllocate];
		pucOutEndpoints->ucAddresses = new UCHAR[ui32SpaceToAllocate];

	
		ifHead = pstcconfiguration->pstcInterfaceDescriptorHead;
		for (int i = 0; i<pstcconfiguration->byNumberOfInterfaces; i++) {

			PENDPOINT_BLOCK epHead = ifHead->pstcEndpointBlockHead;
			for (int j = 0; j < ifHead->byNumberOfEndpoints; j++) {
				// IN
				if (epHead->stcUsbEndpoint.bEndpointAddress & 0x80) {
					pucInEndpoints->ucAddresses[pucInEndpoints->ucLength] = epHead->stcUsbEndpoint.bEndpointAddress;
					++(pucInEndpoints->ucLength);
				}// OUT
				else {
					pucOutEndpoints->ucAddresses[pucOutEndpoints->ucLength] = epHead->stcUsbEndpoint.bEndpointAddress;
					++(pucOutEndpoints->ucLength);
				}

				epHead = epHead->pstcEndpointBlockNext;
			}

			ifHead = ifHead->pstcInterfaceBlockNext;
		}

		bStatus = TRUE;

	}
	

	return bStatus;
}



BOOL bQueryDeviceEndpoints(WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid)
{
	if (hDeviceHandle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	BOOL bResult = TRUE;

	USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
	ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

	WINUSB_PIPE_INFORMATION  Pipe;
	ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));


	bResult = WinUsb_QueryInterfaceSettings(hDeviceHandle, 0, &InterfaceDescriptor);

	if (bResult)
	{
		for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++)
		{
			bResult = WinUsb_QueryPipe(hDeviceHandle, 0, index, &Pipe);

			if (bResult)
			{
				if (Pipe.PipeType == UsbdPipeTypeControl)
				{
					printf("Endpoint index: %d Pipe type: Control Pipe ID: %d.\n", index, Pipe.PipeType, Pipe.PipeId);
				}
				if (Pipe.PipeType == UsbdPipeTypeIsochronous)
				{
					printf("Endpoint index: %d Pipe type: Isochronous Pipe ID: %d.\n", index, Pipe.PipeType, Pipe.PipeId);
				}
				if (Pipe.PipeType == UsbdPipeTypeBulk)
				{
					if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
					{
						printf("Endpoint index: %d Pipe type: Bulk Pipe ID: %c.\n", index, Pipe.PipeType, Pipe.PipeId);
						pipeid->PipeInId = Pipe.PipeId;
					}
					if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId))
					{
						printf("Endpoint index: %d Pipe type: Bulk Pipe ID: %c.\n", index, Pipe.PipeType, Pipe.PipeId);
						pipeid->PipeOutId = Pipe.PipeId;
					}

				}
				if (Pipe.PipeType == UsbdPipeTypeInterrupt)
				{
					printf("Endpoint index: %d Pipe type: Interrupt Pipe ID: %d.\n", index, Pipe.PipeType, Pipe.PipeId);
				}
			}
			else
			{
				continue;
			}
		}
	}

done:
	return bResult;
}



HRESULT
RetrieveDevicePath(
	_Out_bytecap_(BufLen) LPTSTR DevicePath,
	_In_                  ULONG  BufLen,
	_Out_opt_             PBOOL  FailureDeviceNotFound
);

HRESULT
OpenDevice(
	_Out_     PDEVICE_DATA DeviceData,
	_Out_opt_ PBOOL        FailureDeviceNotFound
)
/*++

Routine description:

	Open all needed handles to interact with the device.

	If the device has multiple USB interfaces, this function grants access to
	only the first interface.

	If multiple devices have the same device interface GUID, there is no
	guarantee of which one will be returned.

Arguments:

	DeviceData - Struct filled in by this function. The caller should use the
		WinusbHandle to interact with the device, and must pass the struct to
		CloseDevice when finished.

	FailureDeviceNotFound - TRUE when failure is returned due to no devices
		found with the correct device interface (device not connected, driver
		not installed, or device is disabled in Device Manager); FALSE
		otherwise.

Return value:

	HRESULT

--*/
{
	HRESULT hr = S_OK;
	BOOL    bResult;

	DeviceData->HandlesOpen = FALSE;

	hr = RetrieveDevicePath(DeviceData->DevicePath,
		sizeof(DeviceData->DevicePath),
		FailureDeviceNotFound);

	if (FAILED(hr)) {

		return hr;
	}

	DeviceData->DeviceHandle = CreateFile(DeviceData->DevicePath,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);

	if (INVALID_HANDLE_VALUE == DeviceData->DeviceHandle) {

		hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	bResult = WinUsb_Initialize(DeviceData->DeviceHandle,
		&DeviceData->WinusbHandle);

	if (FALSE == bResult) {

		hr = HRESULT_FROM_WIN32(GetLastError());
		CloseHandle(DeviceData->DeviceHandle);
		return hr;
	}

	DeviceData->HandlesOpen = TRUE;
	return hr;
}

VOID
CloseDevice(
	_Inout_ PDEVICE_DATA DeviceData
)
/*++

Routine description:

	Perform required cleanup when the device is no longer needed.

	If OpenDevice failed, do nothing.

Arguments:

	DeviceData - Struct filled in by OpenDevice

Return value:

	None

--*/
{
	if (FALSE == DeviceData->HandlesOpen) {

		//
		// Called on an uninitialized DeviceData
		//
		return;
	}

	WinUsb_Free(DeviceData->WinusbHandle);
	CloseHandle(DeviceData->DeviceHandle);
	DeviceData->HandlesOpen = FALSE;

	return;
}

HRESULT
RetrieveDevicePath(
	_Out_bytecap_(BufLen) LPTSTR DevicePath,
	_In_                  ULONG  BufLen,
	_Out_opt_             PBOOL  FailureDeviceNotFound
)
/*++

Routine description:

	Retrieve the device path that can be used to open the WinUSB-based device.

	If multiple devices have the same device interface GUID, there is no
	guarantee of which one will be returned.

Arguments:

	DevicePath - On successful return, the path of the device (use with CreateFile).

	BufLen - The size of DevicePath's buffer, in bytes

	FailureDeviceNotFound - TRUE when failure is returned due to no devices
		found with the correct device interface (device not connected, driver
		not installed, or device is disabled in Device Manager); FALSE
		otherwise.

Return value:

	HRESULT

--*/
{
	CONFIGRET cr = CR_SUCCESS;
	HRESULT   hr = S_OK;
	PTSTR     DeviceInterfaceList = NULL;
	ULONG     DeviceInterfaceListLength = 0;

	if (NULL != FailureDeviceNotFound) {

		*FailureDeviceNotFound = FALSE;
	}

	//
	// Enumerate all devices exposing the interface. Do this in a loop
	// in case a new interface is discovered while this code is executing,
	// causing CM_Get_Device_Interface_List to return CR_BUFFER_SMALL.
	//
	do {
		cr = CM_Get_Device_Interface_List_Size(&DeviceInterfaceListLength,
			(LPGUID)&GUID_DEVINTERFACE_myipecmd,
			NULL,
			CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES);

		if (cr != CR_SUCCESS) {
			hr = HRESULT_FROM_WIN32(CM_MapCrToWin32Err(cr, ERROR_INVALID_DATA));
			break;
		}

		DeviceInterfaceList = (PTSTR)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			DeviceInterfaceListLength * sizeof(TCHAR));

		if (DeviceInterfaceList == NULL) {
			hr = E_OUTOFMEMORY;
			break;
		}

		cr = CM_Get_Device_Interface_List((LPGUID)&GUID_DEVINTERFACE_myipecmd,
			NULL,
			DeviceInterfaceList,
			DeviceInterfaceListLength,
			CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES);

		if (cr != CR_SUCCESS) {
			HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);

			if (cr != CR_BUFFER_SMALL) {
				hr = HRESULT_FROM_WIN32(CM_MapCrToWin32Err(cr, ERROR_INVALID_DATA));
			}
		}
	} while (cr == CR_BUFFER_SMALL);

	if (FAILED(hr)) {
		return hr;
	}

	//
	// If the interface list is empty, no devices were found.
	//
	if (*DeviceInterfaceList == TEXT('\0')) {
		if (NULL != FailureDeviceNotFound) {
			*FailureDeviceNotFound = TRUE;
		}

		hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
		HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);
		return hr;
	}

	//
	// Give path of the first found device interface instance to the caller. CM_Get_Device_Interface_List ensured
	// the instance is NULL-terminated.
	//
	hr = StringCbCopy(DevicePath,
		BufLen,
		DeviceInterfaceList);

	HeapFree(GetProcessHeap(), 0, DeviceInterfaceList);

	return hr;
}
