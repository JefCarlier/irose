#ifndef __TRIGGERINFO_H__
#define __TRIGGERINFO_H__

#ifdef TRIGGERINFO_DLL
#define TRIGGERINFO_API __declspec(dllexport)
#elif TRIGGERINFO_STATIC
#define TRIGGERINFO_API
#else
#define TRIGGERINFO_API __declspec(dllimport)
#endif

#include <string>

struct SystemResolution{
      
	int pixel_width;
	int pixel_height;
	int pixel_color;
    int frequency; 
	int	adaptor_number;
};

struct SystemAdapter{
    
	char Driver[ 100 ];
    char Description[ 100 ];
    char DeviceName[ 32 ];
    int  AdapterNumber;
	int	 nResolution;
    
	SystemResolution *pResolution;
};


struct AdapterInformation
{
  	  SystemAdapter *	pAdapter;
	  int				iAdapterCNT;
};

// Resolution 정보를 Release한다
TRIGGERINFO_API void TI_ReleaseAdptInfo (void);

// 현재 Dectect되어 있는 정보를 가지고 온다.
TRIGGERINFO_API AdapterInformation * TI_GetAdptInfo (void);

TRIGGERINFO_API bool TI_DXInfo(void);


#endif
