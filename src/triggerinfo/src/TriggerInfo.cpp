// TriggerInfo.cpp : DLL 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "triggerinfo.h"
#include "SysInfo.h"
#include "Http.h"
#include "__d3d9.h"
#include "AdapterInfo.h"


/*************************************************************
 * Adapter 정보를 얻기 위한 클래스, ReleaseAdapterInfo 로 call해야
 * delete 됨
 */
static CAdapterInfo * g_pAdInfo = NULL;

#ifdef TRIGGERINFO_DLL
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
#endif

TRIGGERINFO_API AdapterInformation * TI_GetAdptInfo (void)
{
	if (g_pAdInfo != nullptr) {
		return g_pAdInfo->GetData();
	}

	IDirect3D9 * pID3D = Direct3DCreate9 (D3D_SDK_VERSION);
	if(pID3D)
	{
		g_pAdInfo = new CAdapterInfo (pID3D);
	 	AdapterInformation * pInfo = g_pAdInfo->GetAllInformation ();

		pID3D->Release ();
		pID3D = NULL;

		return pInfo;
	}

	return NULL;
}

TRIGGERINFO_API void TI_ReleaseAdptInfo (void)
{
	delete g_pAdInfo;
	g_pAdInfo = NULL;
}

TRIGGERINFO_API bool TI_DXInfo(void)
{
	CSysInfo SI;
	
	return SI.GetDXInfo(); 
}

TRIGGERINFO_API bool TI_SendHttpPostData (const char * szServerIP, const char * szPage, const char * pData)
{
	return CHttp::SendPostData (szServerIP, szPage, pData);
}

TRIGGERINFO_API bool TI_SendHttpQueryData (const char * szURL)
{
	return CHttp::SendQueryData (szURL);
}


