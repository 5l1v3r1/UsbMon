                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    #include "UsbHid.h"
#include "UsbUtil.h" 
#include "WinParse.h"
#include "ReportUtil.h"
#define HIDP_MAX_UNKNOWN_ITEMS 4
/*
struct _CHANNEL_REPORT_HEADER
{
	USHORT Offset;  // Position in the _CHANNEL_ITEM array
	USHORT Size;    // Length in said array
	USHORT Index;
	USHORT ByteLen; // The length of the data including reportID.
					// This is the longest such report that might be received
					// for the given collection.
};
/*
typedef struct _HIDP_CHANNEL_DESC
{
	USHORT   UsagePage;		  //+0x0
	UCHAR    ReportID;		  //+0x2
	UCHAR    BitOffset;		  // 0 to 8 value describing bit alignment	+0x3
	USHORT   ReportSize;	  // HID defined report size	+0x4
	USHORT   ReportCount;	  // HID defined report count	+0x6
	USHORT   ByteOffset;	  // byte position of start of field in report packet +0x8
	USHORT   BitLength;		  // total bit length of this channel	+0xA
	ULONG    BitField;		  // The 8 (plus extra) bits associated with a main item +0xC
	USHORT   ByteEnd;		  // First byte not containing bits of this channel. +0x10
	USHORT   LinkCollection;  // A unique internal index pointer	+0x12
	USHORT   LinkUsagePage;   // +0x14
	USHORT   LinkUsage;		  // +0x16
	union
	{
		struct
		{
			ULONG  MoreChannels : 1; // Are there more channel desc associated with
									 // this array.  This happens if there is a
									 // several usages for one main item.
			ULONG  IsConst : 1;		// Does this channel represent filler
			ULONG  IsButton : 1;	// Is this a channel of binary usages, not value usages.
			ULONG  IsAbsolute : 1;  // As apposed to relative
			ULONG  IsRange : 1;
			ULONG  IsAlias : 1;		// a usage described in a delimiter
			ULONG  IsStringRange : 1;
			ULONG  IsDesignatorRange : 1;
			ULONG  Reserved : 20;
			ULONG  NumGlobalUnknowns : 4;
		};
		ULONG  all;
	};
	struct _HIDP_UNKNOWN_TOKEN GlobalUnknowns[HIDP_MAX_UNKNOWN_ITEMS];

	union {
		struct {
			USHORT   UsageMin, UsageMax;
			USHORT   StringMin, StringMax;
			USHORT   DesignatorMin, DesignatorMax;
			USHORT   DataIndexMin, DataIndexMax;
		} Range;
		struct {
			USHORT   Usage, Reserved1;
			USHORT   StringIndex, Reserved2;
			USHORT   DesignatorIndex, Reserved3;
			USHORT   DataIndex, Reserved4;
		} NotRange;
	};

	union {
		struct {
			LONG     LogicalMin, LogicalMax;
		} button;
		struct {
			BOOLEAN  HasNull;  // Does this channel have a null report
			UCHAR    Reserved[3];
			LONG     LogicalMin, LogicalMax;
			LONG     PhysicalMin, PhysicalMax;
		} Data;
	};

	ULONG Units;
	ULONG UnitExp;
} HIDP_CHANNEL_DESC, *PHIDP_CHANNEL_DESC;

typedef struct _HIDP_SYS_POWER_INFO {
	ULONG   PowerButtonMask;

} HIDP_SYS_POWER_INFO, *PHIDP_SYS_POWER_INFO;
typedef struct _HIDP_PREPARSED_DATA
{
	LONG   Signature1, Signature2;
	USHORT Usage;
	USHORT UsagePage;

	HIDP_SYS_POWER_INFO;

	// The following channel report headers point to data within
	// the Data field below using array indices.
	struct _CHANNEL_REPORT_HEADER Input;
	struct _CHANNEL_REPORT_HEADER Output;
	struct _CHANNEL_REPORT_HEADER Feature;

	// After the CANNEL_DESC array the follows a LinkCollection array nodes.
	// LinkCollectionArrayOffset is the index given to RawBytes to find
	// the first location of the _HIDP_LINK_COLLECTION_NODE structure array
	// (index zero) and LinkCollectionArrayLength is the number of array
	// elements in that array.
	USHORT LinkCollectionArrayOffset;
	USHORT LinkCollectionArrayLength;

	union {
		HIDP_CHANNEL_DESC    Data[];
		UCHAR                RawBytes[];
	};
} HIDP_PREPARSED_DATA;
*/
 
/////////////////////////////////////////////////////////////////////////////////////////////// 
//// Global/Extern Variable 
//// 
HID_DEVICE_LIST*  g_hid_client_pdo_list = NULL; 
HIDP_DEVICE_DESC* g_hid_collection = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////// 
//// Marco
//// 
#define HID_USB_DEVICE				 	 L"\\Driver\\hidusb"
#define ARRAY_SIZE						 100
#define HIDP_PREPARSED_DATA_SIGNATURE1	 'PdiH'
#define HIDP_PREPARSED_DATA_SIGNATURE2   'RDK '
  

/////////////////////////////////////////////////////////////////////////////////////////////// 
//// Prototype
//// 

/////////////////////////////////////////////////////////////////////////////////////////////// 
//// Implementation
////  
//----------------------------------------------------------------------------------------//
LOOKUP_STATUS LookupPipeFromPipeList(
	PHID_DEVICE_NODE Data,
	void* Context
)
{

	NTSTATUS								   status = STATUS_UNSUCCESSFUL;
	USBD_PIPE_HANDLE			 pipe_handle_from_urb = Context;
	USBD_PIPE_INFORMATION* pipe_handle_from_whitelist = Data->mini_extension->InterfaceDesc->Pipes;
	ULONG								NumberOfPipes = Data->mini_extension->InterfaceDesc->NumberOfPipes;
	ULONG i;
	for (i = 0; i < NumberOfPipes; i++)
	{
		if (pipe_handle_from_urb == pipe_handle_from_whitelist[i].PipeHandle)
		{
			status = STATUS_SUCCESS;
			break;
		}
	}

	if (!NT_SUCCESS(status))
	{
		return 	CLIST_FINDCB_CTN;
	}
	return CLIST_FINDCB_RET;
}

//----------------------------------------------------------------------------------------// 
BOOLEAN IsHidDevicePipe(
	_In_ TChainListHeader* header,
	_In_ USBD_PIPE_HANDLE handle,
	_Out_ PHID_DEVICE_NODE* node
)
{
	BOOLEAN exist = FALSE;

	*node = QueryFromChainListByCallback(header, LookupPipeFromPipeList, handle);

	if (*node)
	{
		exist = TRUE;
	}

	return exist;
}

//----------------------------------------------------------------------------------------------------------//
NTSTATUS GetReportByDeviceExtension(
	_In_  HIDCLASS_DEVICE_EXTENSION* hid_common_extension, 
	_Out_ HIDP_DEVICE_DESC* ret
)
{
	PDO_EXTENSION* pdoExt = NULL;
	FDO_EXTENSION* fdoExt = NULL; 
	HIDCLASS_DEVICE_EXTENSION* addr = NULL;
	WCHAR name[256] = { 0 };
	HIDP_DEVICE_DESC hid_device_desc = { 0 };
	NTSTATUS status = STATUS_SUCCESS;
	do
	{
		if (!hid_common_extension || !ret)
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		pdoExt = &hid_common_extension->pdoExt;

		if (!pdoExt)
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		addr = (HIDCLASS_DEVICE_EXTENSION*)pdoExt->deviceFdoExt;
		if (!addr)
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		fdoExt = &addr->fdoExt;
		if (!fdoExt)
		{
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		GetDeviceName(fdoExt->fdo, name);
		USB_MON_DEBUG_INFO("Pdo->fdoExt: %I64x \r\n", fdoExt);
		USB_MON_DEBUG_INFO("Pdo->fdo: %I64x \r\n", fdoExt->fdo);
		USB_MON_DEBUG_INFO("Pdo->fdo DriverName: %ws \r\n", fdoExt->fdo->DriverObject->DriverName.Buffer);
		USB_MON_DEBUG_INFO("Pdo->CollectionIndex: %I64x \r\n", pdoExt->collectionIndex);
		USB_MON_DEBUG_INFO("Pdo->CollectionNum: %I64x \r\n", pdoExt->collectionNum);

		// = (HIDP_DEVICE_DESC*)((PUCHAR)fdoExt + 0x58);
		//ExAllocatePoolWithTag(NonPagedPool, sizeof(HIDP_DEVICE_DESC), 'pdih'); // = (HIDP_DEVICE_DESC*)((PUCHAR)fdoExt + 0x58);

		//USB_MON_DEBUG_INFO("[rawReportDescription] %I64x rawReportDescriptionLength: %xh \r\n", fdoExt->rawReportDescription, fdoExt->rawReportDescriptionLength);

		MyGetCollectionDescription(fdoExt->rawReportDescription, fdoExt->rawReportDescriptionLength, NonPagedPool, &hid_device_desc);

		DumpReport(&hid_device_desc);

		RtlMoveMemory(ret, &hid_device_desc, sizeof(HIDP_DEVICE_DESC));

	} while (FALSE);

	return status;
} 

//-------------------------------------------------------------------------------------------//
HID_DEVICE_NODE* CreateHidDeviceNode(
	_In_ PDEVICE_OBJECT device_object,
	_In_ HID_USB_DEVICE_EXTENSION* mini_extension,
	_In_ HIDP_DEVICE_DESC* parsedReport
)
{
	HID_DEVICE_NODE* node = NULL;
	if (!device_object || !mini_extension || !parsedReport)
	{
		return NULL;
	}

	node = ExAllocatePoolWithTag(NonPagedPool, sizeof(HID_DEVICE_NODE), 'edon');
	if (!node)
	{
		return NULL;
	}

	RtlZeroMemory(node, sizeof(HID_DEVICE_NODE));
	node->device_object  = device_object;
	node->mini_extension = mini_extension;
	RtlMoveMemory(&node->parsedReport , parsedReport , sizeof(HIDP_DEVICE_DESC));

	return node;
}
 
//---------------------------------------------------------------------------------------------------------//
BOOLEAN  IsKeyboardOrMouseDevice(
	_In_  PDEVICE_OBJECT				   device_object, 
	_Out_ PHID_USB_DEVICE_EXTENSION*   hid_mini_extension
)
{
	HID_USB_DEVICE_EXTENSION* mini_extension    = NULL;
	HIDCLASS_DEVICE_EXTENSION* hid_common_extension  = NULL;
	WCHAR						DeviceName[256] = { 0 };
	GetDeviceName(device_object, DeviceName);

	hid_common_extension = (HIDCLASS_DEVICE_EXTENSION*)device_object->DeviceExtension;
	if (!hid_common_extension)
	{
		return FALSE;
	}
 
	 
	//USB_MON_DEBUG_INFO("Extension_common: %I64X sizeof: %x \r\n", hid_common_extension, sizeof(HID_USB_DEVICE_EXTENSION));
	mini_extension = (HID_USB_DEVICE_EXTENSION*)hid_common_extension->hidExt.MiniDeviceExtension;
	if (!mini_extension)
	{
		return FALSE;
	} 	
	 
	//DumpHidMiniDriverExtension(hid_common_extension); 

	//Device Name = HID_0000000X
	if (!hid_common_extension->isClientPdo)
	{	
		*hid_mini_extension = NULL;
		return FALSE; 
	}
	 
	if ( mini_extension->InterfaceDesc->Class == 3 &&			//HidClass Device
		(mini_extension->InterfaceDesc->Protocol == 1 ||		//Keyboard
		 mini_extension->InterfaceDesc->Protocol == 2 ||		//Mouse
		 mini_extension->InterfaceDesc->Protocol == 0) )			
	{ 
		FDO_EXTENSION       *fdoExt = NULL;
		PDO_EXTENSION       *pdoExt = NULL;

		if (hid_common_extension->isClientPdo)
		{
			USB_MON_DEBUG_INFO("---------------------------------------------------------------------------------------------------- \r\n");

			USB_MON_DEBUG_INFO("hid_common_extension: %I64x \r\n", hid_common_extension);
			USB_MON_DEBUG_INFO("DeviceObj: %I64X  DriverName: %ws DeviceName: %ws \r\n", device_object, device_object->DriverObject->DriverName.Buffer, DeviceName);
			USB_MON_DEBUG_INFO("collectionIndex: %x collectionNum: %x \r\n", hid_common_extension->pdoExt.collectionIndex, hid_common_extension->pdoExt.collectionNum);

			USB_MON_DEBUG_INFO("---------------------------------------------------------------------------------------------------- \r\n"); 
			*hid_mini_extension = mini_extension;
			
		}
		return TRUE;
	}
 
	return FALSE;
}
//---------------------------------------------------------------------------------------------------------//
NTSTATUS FreeHidClientPdoList()
{
	NTSTATUS status = STATUS_SUCCESS;
	//Free White List
	if (g_hid_client_pdo_list)
	{
		if (g_hid_client_pdo_list->head)
		{
			CHAINLIST_SAFE_FREE(g_hid_client_pdo_list->head);
		}
		ExFreePool(g_hid_client_pdo_list);
		g_hid_client_pdo_list = NULL;
	}
	return status;
} 
//----------------------------------------------------------------------------------------------------------//
NTSTATUS GetAllClientPdo(PDRIVER_OBJECT driver_object, ULONG* client_pdo_count)
{
	PDEVICE_OBJECT	device_object = driver_object->DeviceObject; 
	ULONG	hid_client_pdo_count  = 0;
	NTSTATUS		status = STATUS_UNSUCCESSFUL;
	while (device_object)
	{
		HID_DEVICE_NODE*					node = NULL;
		HIDP_DEVICE_DESC				  report = { 0 };
		HID_USB_DEVICE_EXTENSION* mini_extension = NULL;

		if (!IsKeyboardOrMouseDevice(device_object, &mini_extension))
		{
			goto Next;
		}
		 
		if (!mini_extension)
		{
			goto Next;
		}

		if (!NT_SUCCESS(GetReportByDeviceExtension(device_object->DeviceExtension, &report)))
		{
			goto Next;
		}

		node = CreateHidDeviceNode(device_object, mini_extension, &report);
		if (!node)
		{
			goto Next;
		}

		AddToChainListTail(g_hid_client_pdo_list->head, node);
		hid_client_pdo_count++;

		USB_MON_DEBUG_INFO("Inserted one element: %I64x InferfaceDesc: %I64X device_object: %I64x \r\n", node->device_object, node->mini_extension, device_object);
		USB_MON_DEBUG_INFO("Added one element :%x \r\n", hid_client_pdo_count);
Next:
		device_object = device_object->NextDevice;
	}

	if (hid_client_pdo_count)
	{
		*client_pdo_count = hid_client_pdo_count;
		status = STATUS_SUCCESS;
	}

	return status;
}
//----------------------------------------------------------------------------------------------------------//
NTSTATUS InitPdoList(_Out_ PULONG current_size)
{
	NTSTATUS		  	  status = STATUS_UNSUCCESSFUL; 
	PDRIVER_OBJECT driver_object = NULL;
	ULONG			 return_size = 0;

	do {
		if (!current_size || !g_hid_client_pdo_list)
		{
			USB_MON_DEBUG_INFO("Empty Params \r\n");
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		if (!NT_SUCCESS(GetDriverObjectByName(HID_USB_DEVICE, &driver_object)))
		{
			USB_MON_DEBUG_INFO("Get Drv Obj Error \r\n");
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		if (!driver_object)
		{
			USB_MON_DEBUG_INFO("Empty DrvObj \r\n");
			status = STATUS_UNSUCCESSFUL; 
			break;
		}

		if (!NT_SUCCESS(GetAllClientPdo(driver_object, &return_size)))
		{
			USB_MON_DEBUG_INFO("GetAllClientPdo Error \r\n");
			status = STATUS_UNSUCCESSFUL;
			break; 
		}
		if (return_size > 0)
		{
			status = STATUS_SUCCESS;
			*current_size = return_size;
		}

	} while (FALSE);
	
	return status;
}
//----------------------------------------------------------------------------------------------------------//
NTSTATUS AllocatePdoList()
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	do
	{
		if (!g_hid_client_pdo_list)
		{
			g_hid_client_pdo_list = (HID_DEVICE_LIST*)ExAllocatePoolWithTag(NonPagedPool, sizeof(HID_DEVICE_LIST), 'ldih');
		}

		if (!g_hid_client_pdo_list)
		{
 			status = STATUS_UNSUCCESSFUL;
			break;
		}

		RtlZeroMemory(g_hid_client_pdo_list, sizeof(HID_DEVICE_LIST));

		g_hid_client_pdo_list->head = NewChainListHeaderEx(LISTFLAG_SPINLOCK | LISTFLAG_AUTOFREE, NULL, 0);
		if (!g_hid_client_pdo_list->head)
		{
			ExFreePool(g_hid_client_pdo_list);
			g_hid_client_pdo_list = NULL;
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		status = STATUS_SUCCESS;
	} while (0);
	return status;
}
//----------------------------------------------------------------------------------------------------------//
NTSTATUS InitHidClientPdoList(
	_Out_ PHID_DEVICE_LIST* device_object_list,
	_Out_ PULONG size
)
{
 	PDRIVER_OBJECT	    pDriverObj = NULL;
	ULONG			  current_size = 0;
	NTSTATUS			status = STATUS_UNSUCCESSFUL;

	do {  
		if (!NT_SUCCESS(AllocatePdoList()))
		{
			USB_MON_DEBUG_INFO("Create List Error \r\n");
			status = STATUS_UNSUCCESSFUL;
			break;
		}

		if (!NT_SUCCESS(InitPdoList(&current_size)))
		{
			USB_MON_DEBUG_INFO("Init List Error \r\n");
			FreeHidClientPdoList();
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		 
		if (current_size > 0 && g_hid_client_pdo_list)
		{
			*device_object_list = g_hid_client_pdo_list;
			*size = current_size;
			g_hid_client_pdo_list->RefCount = 1;
			USB_MON_DEBUG_INFO("Create Success\r\n");
			status = STATUS_SUCCESS;
			break;
		}
	} while (FALSE);
	return status;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           