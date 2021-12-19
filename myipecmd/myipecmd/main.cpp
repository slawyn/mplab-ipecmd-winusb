#include "pch.h"


// INFO
// https://www.beyondlogic.org/usbnutshell/usb5.shtml#DeviceDescriptors
// https://hackaday.io/project/11864-tritiled/log/72158-programming-pics-with-python
// Microchip: Readme%20for%20IPECMD.htm
// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/developing-windows-applications-that-communicate-with-a-usb-device
// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/using-winusb-api-to-communicate-with-a-usb-device
// https://docs.microsoft.com/en-us/windows/win32/api/winusb/

LONG __cdecl _tmain(LONG     Argc, LPTSTR* Argv){
	DEVICE_DATA           stcDeviceData;
	HRESULT               hrSucceeded;
	USB_DEVICE_DESCRIPTOR stcDeviceDescriptor;
	USB_STRING_DESCRIPTOR stcStringDescriptor;
	BOOL                  bNoDevice;

	CONFIGURATION	stcConfiguration;


	UNREFERENCED_PARAMETER(Argc);
	UNREFERENCED_PARAMETER(Argv);

	//
	// Find a device connected to the system that has WinUSB installed using our
	// INF
	//
	hrSucceeded = OpenDevice(&stcDeviceData, &bNoDevice);

	if (FAILED(hrSucceeded)) {
		if (bNoDevice) {
			logw(L"Device not connected or driver not installed\n");
		}
		else {
			logw(L"Failed looking for device, HRESULT 0x%x\n", hrSucceeded);
		}
		return 0;
	}

	if(bGetDeviceDescriptor(stcDeviceData.WinusbHandle, &stcDeviceDescriptor)!= FALSE){

		bGetStringDescriptor(stcDeviceData.WinusbHandle, &stcStringDescriptor, 0);

		// iManufacturer
		if (stcDeviceDescriptor.iManufacturer != 0)
			bGetStringDescriptor(stcDeviceData.WinusbHandle, &stcStringDescriptor, stcDeviceDescriptor.iManufacturer);
		
		// iProduct
		if(stcDeviceDescriptor.iProduct!=0)
			bGetStringDescriptor(stcDeviceData.WinusbHandle, &stcStringDescriptor, stcDeviceDescriptor.iProduct);

		// iSerialNumber
		if (stcDeviceDescriptor.iSerialNumber != 0)
			bGetStringDescriptor(stcDeviceData.WinusbHandle, &stcStringDescriptor, stcDeviceDescriptor.iSerialNumber);

		//
		// Get String Descriptors
		//
		if (bGetConfiguration(stcDeviceData.WinusbHandle, &stcConfiguration) != FALSE) {
			
			//UCHAR ucNumberOfInterfaceDescriptors = stcConfiguration.byNumberOfInterfaces;

			PUCHAR pucBuffer;
			ULONG ulLength;
			ULONG pulTransferedLength;
		
			UCHAR chSpeed;
			ENDPOINTADDRESSES inPipes, outPipes;

			inPipes.ucLength = 0;
			outPipes.ucLength = 0;
			pulTransferedLength = 0;

			// Data Buffer
			ulLength = 16;
			pucBuffer = new UCHAR[ulLength];
			memset(pucBuffer, 0xCC, ulLength);

			bGetUSBDeviceSpeed(stcDeviceData.WinusbHandle, &chSpeed);

			// Generally how to startup a USB
			// Set Configuration to 1
			// 0.Endpoint IN
			// 1.Endpoint OUT
			// 2.Endpoint IN
			// 3.Endpoint OUT

			if (bAssignInOutEndpoints(&stcConfiguration, &inPipes, &outPipes)) 
				WinUsb_WritePipe(stcDeviceData.WinusbHandle, outPipes.ucAddresses[0], pucBuffer, ulLength, &pulTransferedLength, NULL);
			}

			
			// Send information over the pipe
		

				//WinUsb_GetPipePolicy(stcDeviceData.WinusbHandle,)
				//WinUsb_SetPipePolicy  // Set 
			

	

		

	}
	else {
		CloseDevice(&stcDeviceData);
	}
	return 0;
}


