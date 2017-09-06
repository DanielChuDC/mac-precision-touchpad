// Hid.c: HID-related routine

#include "Driver.h"
#include "Hid.tmh"

#ifndef _AAPL_HID_DESCRIPTOR_H_
#define _AAPL_HID_DESCRIPTOR_H_

HID_REPORT_DESCRIPTOR AAPLMagicTrackpad2ReportDescriptor[] = {
	AAPL_MAGIC_TRACKPAD2_PTP_TLC,
	AAPL_PTP_CONFIGURATION_TLC
};

CONST HID_DESCRIPTOR AAPLMagicTrackpad2DefaultHidDescriptor = {
	0x09,   // bLength
	0x21,   // bDescriptorType
	0x0100, // bcdHID
	0x00,   // bCountryCode
	0x01,   // bNumDescriptors
	{
		0x22,                                         // bDescriptorType
		sizeof(AAPLMagicTrackpad2ReportDescriptor)    // bDescriptorLength
	}
};
#endif

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetHidDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	NTSTATUS        status   = STATUS_SUCCESS;
	PDEVICE_CONTEXT pContext = DeviceGetContext(Device);
	size_t			szCopy   = 0;
	WDFMEMORY       reqMemory;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	status = WdfRequestRetrieveOutputMemory(
		Request, 
		&reqMemory
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", 
			status
		);
		return status;
	}

	if (pContext->DeviceDescriptor.idProduct == USB_DEVICE_ID_APPLE_MAGICTRACKPAD2) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION, 
			TRACE_DRIVER, 
			"%!FUNC! Request HID Report Descriptor"
		);

		szCopy = AAPLMagicTrackpad2DefaultHidDescriptor.bLength;
		status = WdfMemoryCopyFromBuffer(
			reqMemory, 
			0, 
			(PVOID) &AAPLMagicTrackpad2DefaultHidDescriptor, 
			szCopy
		);

		if (!NT_SUCCESS(status)) {
			TraceEvents(
				TRACE_LEVEL_ERROR, 
				TRACE_DRIVER, 
				"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!", 
				status
			);
			return status;
		}

		WdfRequestSetInformation(
			Request, 
			szCopy
		);
	} else {
		TraceEvents(
			TRACE_LEVEL_WARNING, 
			TRACE_DRIVER, 
			"%!FUNC! Device HID registry is not found"
		);
		status = STATUS_INVALID_DEVICE_STATE;
		return status;
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);
	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetDeviceAttribs(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	NTSTATUS               status = STATUS_SUCCESS;
	PDEVICE_CONTEXT        pContext = DeviceGetContext(Device);
	PHID_DEVICE_ATTRIBUTES pDeviceAttributes = NULL;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	status = WdfRequestRetrieveOutputBuffer(
		Request, 
		sizeof(HID_DEVICE_ATTRIBUTES), 
		&pDeviceAttributes, 
		NULL);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", 
			status
		);
		return status;
	}

	pDeviceAttributes->Size          = sizeof(HID_DEVICE_ATTRIBUTES);
	pDeviceAttributes->ProductID     = pContext->DeviceDescriptor.idProduct;
	pDeviceAttributes->VendorID      = pContext->DeviceDescriptor.idVendor;
	pDeviceAttributes->VersionNumber = DEVICE_VERSION;
	
	WdfRequestSetInformation(
		Request, 
		sizeof(HID_DEVICE_ATTRIBUTES)
	);

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);

	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetReportDescriptor(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	
	NTSTATUS               status            = STATUS_SUCCESS;
	PDEVICE_CONTEXT        pContext          = DeviceGetContext(Device);
	size_t			       szCopy            = 0;
	WDFMEMORY              reqMemory;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	status = WdfRequestRetrieveOutputMemory(
		Request, 
		&reqMemory
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfRequestRetrieveOutputBuffer failed with %!STATUS!", 
			status
		);
		return status;
	}

	if (pContext->DeviceDescriptor.idProduct == USB_DEVICE_ID_APPLE_MAGICTRACKPAD2) {
		szCopy = AAPLMagicTrackpad2DefaultHidDescriptor.DescriptorList[0].wReportLength;
		if (szCopy == 0) {

			status = STATUS_INVALID_DEVICE_STATE;
			TraceEvents(
				TRACE_LEVEL_WARNING, 
				TRACE_DRIVER, 
				"%!FUNC! Device HID report length is zero"
			);
			return status;

		}

		status = WdfMemoryCopyFromBuffer(
			reqMemory, 
			0, 
			(PVOID) &AAPLMagicTrackpad2ReportDescriptor, 
			szCopy
		);

		if (!NT_SUCCESS(status)) {

			TraceEvents(
				TRACE_LEVEL_ERROR, 
				TRACE_DRIVER, 
				"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!", 
				status
			);
			return status;

		}

		WdfRequestSetInformation(
			Request, 
			szCopy
		);
	} else {
		TraceEvents(
			TRACE_LEVEL_WARNING, 
			TRACE_DRIVER, 
			"%!FUNC! Device HID registry is not found"
		);

		status = STATUS_INVALID_DEVICE_STATE;
		return status;
	}

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);
	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpGetStrings(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{
	
	NTSTATUS               status = STATUS_SUCCESS;
	PDEVICE_CONTEXT        pContext = DeviceGetContext(Device);
	void                   *pStringBuffer = NULL;
	WDFMEMORY              memHandle;
	USHORT                 wcharCount;
	size_t                 actualSize;
	UCHAR                  strIndex;

	ULONG                  inputValue;
	WDFMEMORY              inputMemory;
	size_t                 inputBufferLength;
	PVOID                  inputBuffer;
	ULONG                  languageId, stringId;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	status = WdfRequestRetrieveInputMemory(
		Request, 
		&inputMemory
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_INFORMATION,
			TRACE_DRIVER, 
			"%!FUNC! WdfRequestRetrieveInputMemory failed with status %!STATUS!",
			status
		);
		return status;
	}

	inputBuffer = WdfMemoryGetBuffer(
		inputMemory, 
		&inputBufferLength
	);

	//
	// make sure buffer is big enough.
	//
	if (inputBufferLength < sizeof(ULONG)) {
		status = STATUS_INVALID_BUFFER_SIZE;
		TraceEvents(
			TRACE_LEVEL_INFORMATION, 
			TRACE_DRIVER, 
			"%!FUNC! GetStringId: invalid input buffer. size %d, expect %d",
			(int)inputBufferLength, 
			(int) sizeof(ULONG)
		);
		return status;
	}

	inputValue = (*(PULONG)inputBuffer);
	stringId = (inputValue & 0x0ffff);
	languageId = (inputValue >> 16);

	// Get actual string from USB device
	switch (stringId)
	{
		case HID_STRING_ID_IMANUFACTURER:
			strIndex = pContext->DeviceDescriptor.iManufacturer;
			break;
		case HID_STRING_ID_IPRODUCT:
			strIndex = pContext->DeviceDescriptor.iProduct;
			break;
		case HID_STRING_ID_ISERIALNUMBER:
			strIndex = pContext->DeviceDescriptor.iSerialNumber;
			break;
		default:
			TraceEvents(
				TRACE_LEVEL_WARNING, 
				TRACE_DRIVER, 
				"%!FUNC! gets invalid string type"
			);
			return status;
	}

	status = WdfUsbTargetDeviceAllocAndQueryString(
		pContext->UsbDevice, 
		WDF_NO_OBJECT_ATTRIBUTES, 
		&memHandle, 
		&wcharCount, 
		strIndex, 
		(USHORT) languageId
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, "%!FUNC! WdfUsbTargetDeviceAllocAndQueryString failed with %!STATUS!", 
			status
		);
		return status;
	}

	status = WdfRequestRetrieveOutputBuffer(
		Request, 
		wcharCount * sizeof(WCHAR), 
		&pStringBuffer, 
		&actualSize
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfMemoryCopyFromBuffer failed with %!STATUS!", 
			status
		);
		return status;
	}

	WdfMemoryCopyToBuffer(
		memHandle,
		0, 
		&pStringBuffer, 
		actualSize
	);

	WdfRequestSetInformation(
		Request, 
		actualSize
	);

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);
	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
RequestGetHidXferPacketToReadFromDevice(
	_In_  WDFREQUEST        Request,
	_Out_ HID_XFER_PACKET  *Packet
)
{

	//
	// Driver need to write to the output buffer (so that App can read from it)
	//
	//   Report Buffer: Output Buffer
	//   Report Id    : Input Buffer
	//

	NTSTATUS                status;
	WDFMEMORY               inputMemory;
	WDFMEMORY               outputMemory;
	size_t                  inputBufferLength;
	size_t                  outputBufferLength;
	PVOID                   inputBuffer;
	PVOID                   outputBuffer;

	//
	// Get report Id from input buffer
	//
	status = WdfRequestRetrieveInputMemory(
		Request, 
		&inputMemory
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! WdfRequestRetrieveInputMemory failed with %!STATUS!", 
			status
		);
		return status;
	}
	inputBuffer = WdfMemoryGetBuffer(
		inputMemory, 
		&inputBufferLength
	);

	if (inputBufferLength < sizeof(UCHAR)) {
		status = STATUS_INVALID_BUFFER_SIZE;
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveInputMemory: invalid input buffer. size %d, expect %d",
			(int) inputBufferLength, 
			(int) sizeof(UCHAR)
		);
		return status;
	}

	Packet->reportId = *(PUCHAR) inputBuffer;

	//
	// Get report buffer from output buffer
	//
	status = WdfRequestRetrieveOutputMemory(
		Request, 
		&outputMemory
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputMemory failed with %!STATUS!",
			status
		);
		return status;
	}

	outputBuffer = WdfMemoryGetBuffer(
		outputMemory, 
		&outputBufferLength
	);

	Packet->reportBuffer = (PUCHAR) outputBuffer;
	Packet->reportBufferLen = (ULONG) outputBufferLength;

	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
RequestGetHidXferPacketToWriteToDevice(
	_In_  WDFREQUEST        Request,
	_Out_ HID_XFER_PACKET  *Packet
)
{

	//
	// Driver need to read from the input buffer (which was written by App)
	//
	//   Report Buffer: Input Buffer
	//   Report Id    : Output Buffer Length
	//
	// Note that the report id is not stored inside the output buffer, as the
	// driver has no read-access right to the output buffer, and trying to read
	// from the buffer will cause an access violation error.
	//
	// The workaround is to store the report id in the OutputBufferLength field,
	// to which the driver does have read-access right.
	//

	NTSTATUS                status;
	WDFMEMORY               inputMemory;
	WDFMEMORY               outputMemory;
	size_t                  inputBufferLength;
	size_t                  outputBufferLength;
	PVOID                   inputBuffer;

	//
	// Get report Id from output buffer length
	//
	status = WdfRequestRetrieveOutputMemory(
		Request, 
		&outputMemory
	);
	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveOutputMemory failed with %!STATUS!",
			status
		);
		return status;
	}
	WdfMemoryGetBuffer(
		outputMemory, 
		&outputBufferLength
	);
	Packet->reportId = (UCHAR) outputBufferLength;

	//
	// Get report buffer from input buffer
	//
	status = WdfRequestRetrieveInputMemory(
		Request, 
		&inputMemory
	);
	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR,
			TRACE_DRIVER,
			"%!FUNC! WdfRequestRetrieveInputMemory failed with %!STATUS!",
			status
		);
		return status;
	}
	inputBuffer = WdfMemoryGetBuffer(
		inputMemory, 
		&inputBufferLength
	);

	Packet->reportBuffer = (PUCHAR) inputBuffer;
	Packet->reportBufferLen = (ULONG) inputBufferLength;

	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpReportFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	NTSTATUS status;
	PDEVICE_CONTEXT deviceContext;
	HID_XFER_PACKET packet;
	size_t reportSize;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	status = STATUS_SUCCESS;
	deviceContext = DeviceGetContext(Device);

	status = RequestGetHidXferPacketToReadFromDevice(
		Request, 
		&packet
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! RequestGetHidXferPacketToReadFromDevice failed with status %!STATUS!", 
			status
		);
		goto exit;
	}

	switch (packet.reportId)
	{
		case REPORTID_DEVICE_CAPS:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_DEVICE_CAPS is requested"
			);

			// Size sanity check
			reportSize = sizeof(PPTP_DEVICE_CAPS_FEATURE_REPORT) + sizeof(packet.reportId);
			if (packet.reportBufferLen < reportSize) {
				status = STATUS_INVALID_BUFFER_SIZE;
				TraceEvents(
					TRACE_LEVEL_ERROR, 
					TRACE_DRIVER, 
					"%!FUNC! Report buffer is too small"
				);
				goto exit;
			}

			PPTP_DEVICE_CAPS_FEATURE_REPORT capsReport = (PPTP_DEVICE_CAPS_FEATURE_REPORT) packet.reportBuffer;

			capsReport->MaximumContactPoints = PTP_MAX_CONTACT_POINTS;
			capsReport->ButtonType = PTP_BUTTON_TYPE_CLICK_PAD;
			capsReport->ReportID = REPORTID_DEVICE_CAPS;

			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_DEVICE_CAPS has maximum contact points of %d", 
				capsReport->MaximumContactPoints
			);
			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_DEVICE_CAPS has touchpad type %d", 
				capsReport->ButtonType
			);
			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_DEVICE_CAPS is fulfilled"
			);

			WdfRequestSetInformation(
				Request, 
				reportSize
			);
			break;
		}
		case REPORTID_PTPHQA:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_PTPHQA is requested"
			);

			// Size sanity check
			reportSize = sizeof(PPTP_DEVICE_HQA_CERTIFICATION_REPORT) + sizeof(packet.reportId);
			if (packet.reportBufferLen < reportSize) {
				status = STATUS_INVALID_BUFFER_SIZE;
				TraceEvents(
					TRACE_LEVEL_ERROR, 
					TRACE_DRIVER, 
					"%!FUNC! Report buffer is too small."
				);
				goto exit;
			}

			PPTP_DEVICE_HQA_CERTIFICATION_REPORT certReport = (PPTP_DEVICE_HQA_CERTIFICATION_REPORT) packet.reportBuffer;

			*certReport->CertificationBlob = DEFAULT_PTP_HQA_BLOB;
			certReport->ReportID = REPORTID_PTPHQA;

			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_PTPHQA is fulfilled"
			);

			WdfRequestSetInformation(
				Request, 
				reportSize
			);
			break;
		}
		default:
			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Unsupported type %d is requested", 
				packet.reportId
			);

			status = STATUS_NOT_SUPPORTED;
			goto exit;
	}
exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);
	return status;

}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
AmtPtpSetFeatures(
	_In_ WDFDEVICE Device,
	_In_ WDFREQUEST Request
)
{

	NTSTATUS        status;
	HID_XFER_PACKET packet;
	PDEVICE_CONTEXT deviceContext;

	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Entry"
	);

	status = STATUS_SUCCESS;
	deviceContext = DeviceGetContext(Device);

	status = RequestGetHidXferPacketToWriteToDevice(
		Request, 
		&packet
	);

	if (!NT_SUCCESS(status)) {
		TraceEvents(
			TRACE_LEVEL_ERROR, 
			TRACE_DRIVER, 
			"%!FUNC! RequestGetHidXferPacketToWriteToDevice failed with status %!STATUS!", 
			status
		);
		goto exit;
	}

	switch (packet.reportId)
	{
		case REPORTID_REPORTMODE:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_REPORTMODE is requested"
			);
			PPTP_DEVICE_INPUT_MODE_REPORT devInputMode = (PPTP_DEVICE_INPUT_MODE_REPORT) packet.reportBuffer;

			switch (devInputMode->Mode)
			{
			case PTP_COLLECTION_MOUSE:
				TraceEvents(
					TRACE_LEVEL_INFORMATION, 
					TRACE_DRIVER, 
					"%!FUNC! Report REPORTID_REPORTMODE requested Mouse Input"
				);
				status = AmtPtpSetWellspringMode(
					deviceContext,
					FALSE
				);
				if (!NT_SUCCESS(status)) {
					TraceEvents(
						TRACE_LEVEL_ERROR, 
						TRACE_DRIVER, 
						"%!FUNC! MagicTrackpad2PtpDeviceSetWellspringMode failed with status %!STATUS!", 
						status
					);
					goto exit;
				}
				break;
			case PTP_COLLECTION_WINDOWS:
				TraceEvents(
					TRACE_LEVEL_INFORMATION, 
					TRACE_DRIVER, 
					"%!FUNC! Report REPORTID_REPORTMODE requested Windows PTP Input"
				);

				status = AmtPtpSetWellspringMode(
					deviceContext, 
					TRUE
				);
				if (!NT_SUCCESS(status)) {
					TraceEvents(
						TRACE_LEVEL_ERROR, 
						TRACE_DRIVER, 
						"%!FUNC! MagicTrackpad2PtpDeviceSetWellspringMode failed with status %!STATUS!", 
						status
					);
					goto exit;
				}
				break;
			}

			WdfRequestSetInformation(
				Request, 
				sizeof(PTP_DEVICE_INPUT_MODE_REPORT)
			);

			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_REPORTMODE is fulfilled"
			);
			break;
		}
		case REPORTID_FUNCSWITCH:
		{
			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_FUNCSWITCH is requested"
			);
			PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT secInput = (PPTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT) packet.reportBuffer;

			deviceContext->IsButtonReportOn = secInput->ButtonReport;
			deviceContext->IsSurfaceReportOn = secInput->SurfaceReport;

			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_FUNCSWITCH requested Button = %d, Surface = %d",
				secInput->ButtonReport, 
				secInput->SurfaceReport
			);

			WdfRequestSetInformation(
				Request,
				sizeof(PTP_DEVICE_SELECTIVE_REPORT_MODE_REPORT)
			);

			TraceEvents(
				TRACE_LEVEL_INFORMATION,
				TRACE_DRIVER, 
				"%!FUNC! Report REPORTID_FUNCSWITCH is fulfilled"
			);
			break;
		}
		default:
			TraceEvents(
				TRACE_LEVEL_INFORMATION, 
				TRACE_DRIVER, 
				"%!FUNC! Unsupported type %d is requested",
				packet.reportId
			);
			status = STATUS_NOT_SUPPORTED;
			goto exit;
	}

exit:
	TraceEvents(
		TRACE_LEVEL_INFORMATION, 
		TRACE_DRIVER, 
		"%!FUNC! Exit"
	);
	return STATUS_SUCCESS;

}