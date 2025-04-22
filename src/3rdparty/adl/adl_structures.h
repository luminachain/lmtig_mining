



























#ifndef ADL_STRUCTURES_H_
#define ADL_STRUCTURES_H_

#include "adl_defines.h"









typedef struct AdapterInfo
{



    int iSize;

    int iAdapterIndex;

    char strUDID[ADL_MAX_PATH];

    int iBusNumber;

    int iDeviceNumber;

    int iFunctionNumber;

    int iVendorID;

    char strAdapterName[ADL_MAX_PATH];

    char strDisplayName[ADL_MAX_PATH];

    int iPresent;

#if defined (_WIN32) || defined (_WIN64)



    int iExist;

    char strDriverPath[ADL_MAX_PATH];

    char strDriverPathExt[ADL_MAX_PATH];

    char strPNPString[ADL_MAX_PATH];

    int iOSDisplayIndex;
#endif 

#if defined (LINUX)



    int iXScreenNum;

    int iDrvIndex;

    char strXScreenConfigName[ADL_MAX_PATH];

#endif 
} AdapterInfo, *LPAdapterInfo;










#if defined (LINUX)
typedef struct XScreenInfo
{

    int iXScreenNum;

    char strXScreenConfigName[ADL_MAX_PATH];
} XScreenInfo, *LPXScreenInfo;
#endif 









typedef struct ADLMemoryInfo
{

    long long iMemorySize;

    char strMemoryType[ADL_MAX_PATH];

    long long iMemoryBandwidth;
} ADLMemoryInfo, *LPADLMemoryInfo;








typedef struct ADLMemoryRequired
{
    long long iMemoryReq;        
    int iType;                    
    int iDisplayFeatureValue;   
} ADLMemoryRequired, *LPADLMemoryRequired;








typedef struct ADLMemoryDisplayFeatures
{
    int iDisplayIndex;            
    int iDisplayFeatureValue;    
} ADLMemoryDisplayFeatures, *LPADLMemoryDisplayFeatures;








typedef struct ADLDDCInfo
{

    int  ulSize;

    int  ulSupportsDDC;

    int  ulManufacturerID;

    int  ulProductID;

    char cDisplayName[ADL_MAX_DISPLAY_NAME];

    int  ulMaxHResolution;

    int  ulMaxVResolution;

    int  ulMaxRefresh;

    int  ulPTMCx;

    int  ulPTMCy;

    int  ulPTMRefreshRate;

    int  ulDDCInfoFlag;
} ADLDDCInfo, *LPADLDDCInfo;








typedef struct ADLDDCInfo2
{

    int  ulSize;


    int  ulSupportsDDC;

    int  ulManufacturerID;

    int  ulProductID;

    char cDisplayName[ADL_MAX_DISPLAY_NAME];

    int  ulMaxHResolution;

    int  ulMaxVResolution;

    int  ulMaxRefresh;

    int  ulPTMCx;

    int  ulPTMCy;

    int  ulPTMRefreshRate;

    int  ulDDCInfoFlag;

    int bPackedPixelSupported;

    int iPanelPixelFormat;

    int  ulSerialID;

    int ulMinLuminanceData;

    int ulAvgLuminanceData;

    int ulMaxLuminanceData;


    int iSupportedTransferFunction;


    int iSupportedColorSpace;


    int iNativeDisplayChromaticityRedX;

    int iNativeDisplayChromaticityRedY;

    int iNativeDisplayChromaticityGreenX;

    int iNativeDisplayChromaticityGreenY;

    int iNativeDisplayChromaticityBlueX;

    int iNativeDisplayChromaticityBlueY;

    int iNativeDisplayChromaticityWhitePointX;

    int iNativeDisplayChromaticityWhitePointY;

    int iDiffuseScreenReflectance;

    int iSpecularScreenReflectance;

    int iSupportedHDR;

    int iFreesyncFlags;


    int ulMinLuminanceNoDimmingData;

    int ulMaxBacklightMaxLuminanceData;
    int ulMinBacklightMaxLuminanceData;
    int ulMaxBacklightMinLuminanceData;
    int ulMinBacklightMinLuminanceData;

    
    int iReserved[4];
} ADLDDCInfo2, *LPADLDDCInfo2;










typedef struct ADLGamma
{

    float fRed;

    float fGreen;

    float fBlue;
} ADLGamma, *LPADLGamma;







typedef struct ADLCustomMode
{

    int iFlags;

    int iModeWidth;

    int iModeHeight;

    int iBaseModeWidth;

    int iBaseModeHeight;

    int iRefreshRate;
} ADLCustomMode, *LPADLCustomMode;







typedef struct ADLGetClocksOUT
{
    long ulHighCoreClock;
    long ulHighMemoryClock;
    long ulHighVddc;
    long ulCoreMin;
    long ulCoreMax;
    long ulMemoryMin;
    long ulMemoryMax;
    long ulActivityPercent;
    long ulCurrentCoreClock;
    long ulCurrentMemoryClock;
    long ulReserved;
} ADLGetClocksOUT;







typedef struct ADLDisplayConfig
{

  long ulSize;

  long ulConnectorType;

  long ulDeviceData;

  long ulOverridedDeviceData;

  long ulReserved;
} ADLDisplayConfig;












typedef struct ADLDisplayID
{

    int iDisplayLogicalIndex;





    int iDisplayPhysicalIndex;


    int iDisplayLogicalAdapterIndex;





    int iDisplayPhysicalAdapterIndex;
} ADLDisplayID, *LPADLDisplayID;









typedef struct ADLDisplayInfo
{

    ADLDisplayID displayID;


    int  iDisplayControllerIndex;


    char strDisplayName[ADL_MAX_PATH];


    char strDisplayManufacturerName[ADL_MAX_PATH];


    int  iDisplayType;


    int  iDisplayOutputType;


    int  iDisplayConnector;



    int  iDisplayInfoMask;


    int  iDisplayInfoValue;
} ADLDisplayInfo, *LPADLDisplayInfo;









typedef struct ADLDisplayDPMSTInfo
{
    
    ADLDisplayID displayID;

    
    int    iTotalAvailableBandwidthInMpbs;
    
    int    iAllocatedBandwidthInMbps;

    
    
    char    strGlobalUniqueIdentifier[ADL_MAX_PATH];

    
    int        radLinkCount;
    
    int        iPhysicalConnectorID;

    
    char    rad[ADL_MAX_RAD_LINK_COUNT];
} ADLDisplayDPMSTInfo, *LPADLDisplayDPMSTInfo;







typedef struct ADLDisplayMode
{

   int  iPelsHeight;

   int  iPelsWidth;

   int  iBitsPerPel;

   int  iDisplayFrequency;
} ADLDisplayMode;







typedef struct ADLDetailedTiming
{

     int   iSize;

     short sTimingFlags;

     short sHTotal;

     short sHDisplay;

     short sHSyncStart;

     short sHSyncWidth;

     short sVTotal;

     short sVDisplay;

     short sVSyncStart;

     short sVSyncWidth;

     short sPixelClock;

     short sHOverscanRight;

     short sHOverscanLeft;

     short sVOverscanBottom;

     short sVOverscanTop;
     short sOverscan8B;
     short sOverscanGR;
} ADLDetailedTiming;







typedef struct ADLDisplayModeInfo
{

  int  iTimingStandard;

  int  iPossibleStandard;

  int  iRefreshRate;

  int  iPelsWidth;

  int  iPelsHeight;

  ADLDetailedTiming  sDetailedTiming;
} ADLDisplayModeInfo;







typedef struct ADLDisplayProperty
{

  int iSize;

  int iPropertyType;

  int iExpansionMode;

  int iSupport;

  int iCurrent;

  int iDefault;
} ADLDisplayProperty;








typedef struct ADLClockInfo
{

    int iCoreClock;

    int iMemoryClock;
} ADLClockInfo, *LPADLClockInfo;








typedef struct ADLI2C
{

    int iSize;

    int iLine;

    int iAddress;

    int iOffset;

    int iAction;

    int iSpeed;

    int iDataSize;

    char *pcData;
} ADLI2C;








typedef struct ADLDisplayEDIDData
{

  int iSize;

  int iFlag;
  
  int iEDIDSize;

  int iBlockIndex;

  char cEDIDData[ADL_MAX_EDIDDATA_SIZE];

  int iReserved[4];
}ADLDisplayEDIDData;









typedef struct ADLControllerOverlayInput
{

  int  iSize;

  int  iOverlayAdjust;

  int  iValue;

  int  iReserved;
} ADLControllerOverlayInput;








typedef struct ADLAdjustmentinfo
{

  int iDefault;

  int iMin;

  int iMax;

  int iStep;
} ADLAdjustmentinfo;








typedef struct ADLControllerOverlayInfo
{

  int                    iSize;

  ADLAdjustmentinfo        sOverlayInfo;

  int                    iReserved[3];
} ADLControllerOverlayInfo;








typedef struct ADLGLSyncModuleID
{

    int        iModuleID;

    int        iGlSyncGPUPort;

    int        iFWBootSectorVersion;

    int        iFWUserSectorVersion;
} ADLGLSyncModuleID , *LPADLGLSyncModuleID;








typedef struct ADLGLSyncPortCaps
{

    int        iPortType;

    int        iNumOfLEDs;
}ADLGLSyncPortCaps, *LPADLGLSyncPortCaps;









typedef struct ADLGLSyncGenlockConfig
{

    int        iValidMask;

    int        iSyncDelay;

    int        iFramelockCntlVector;

    int        iSignalSource;

    int        iSampleRate;

    int        iSyncField;

    int        iTriggerEdge;

    int        iScanRateCoeff;
}ADLGLSyncGenlockConfig, *LPADLGLSyncGenlockConfig;









typedef struct ADLGlSyncPortInfo
{

    int        iPortType;

    int        iNumOfLEDs;

    int        iPortState;

    int        iFrequency;

    int        iSignalType;

    int        iSignalSource;

} ADLGlSyncPortInfo, *LPADLGlSyncPortInfo;









typedef struct ADLGlSyncPortControl
{

    int        iPortType;

    int        iControlVector;

    int        iSignalSource;
} ADLGlSyncPortControl;









typedef struct ADLGlSyncMode
{

    int        iControlVector;

    int        iStatusVector;

    int        iGLSyncConnectorIndex;
} ADLGlSyncMode, *LPADLGlSyncMode;









typedef struct ADLGlSyncMode2
{

    int        iControlVector;

    int        iStatusVector;

    int        iGLSyncConnectorIndex;

    int        iDisplayIndex;
} ADLGlSyncMode2, *LPADLGlSyncMode2;









typedef struct  ADLInfoPacket
{
    char hb0;
    char hb1;
    char hb2;

    char sb[28];
}ADLInfoPacket;








typedef struct ADLAVIInfoPacket  
{

   char bPB3_ITC;

   char bPB5;
}ADLAVIInfoPacket;










typedef struct ADLODClockSetting
{

    int iDefaultClock;

    int iCurrentClock;

    int iMaxClock;

    int iMinClock;

    int iRequestedClock;

    int iStepClock;
} ADLODClockSetting;








typedef struct ADLAdapterODClockInfo
{

    int iSize;

    int iFlags;

    ADLODClockSetting sMemoryClock;

    ADLODClockSetting sEngineClock;
} ADLAdapterODClockInfo;








typedef struct ADLAdapterODClockConfig
{

  int iSize;

  int iFlags;

  int iMemoryClock;

  int iEngineClock;
} ADLAdapterODClockConfig;








typedef struct ADLPMActivity
{

    int iSize;

    int iEngineClock;

    int iMemoryClock;

    int iVddc;

    int iActivityPercent;

    int iCurrentPerformanceLevel;

    int iCurrentBusSpeed;

    int iCurrentBusLanes;

    int iMaximumBusLanes;

    int iReserved;
} ADLPMActivity;








typedef struct ADLThermalControllerInfo
{

  int iSize;

  int iThermalDomain;

  int iDomainIndex;

  int iFlags;
} ADLThermalControllerInfo;








typedef struct ADLTemperature
{

  int iSize;

  int iTemperature;
} ADLTemperature;








typedef struct ADLFanSpeedInfo
{

  int iSize;

  int iFlags;

  int iMinPercent;

  int iMaxPercent;

  int iMinRPM;

  int iMaxRPM;
} ADLFanSpeedInfo;








typedef struct ADLFanSpeedValue
{

  int iSize;

  int iSpeedType;

  int iFanSpeed;

  int iFlags;
} ADLFanSpeedValue;








typedef struct ADLODParameterRange
{

  int iMin;

  int iMax;

  int iStep;
} ADLODParameterRange;








typedef struct ADLODParameters
{

  int iSize;

  int iNumberOfPerformanceLevels;

  int iActivityReportingSupported;

  int iDiscretePerformanceLevels;

  int iReserved;

  ADLODParameterRange sEngineClock;

  ADLODParameterRange sMemoryClock;

  ADLODParameterRange sVddc;
} ADLODParameters;








typedef struct ADLODPerformanceLevel
{

  int iEngineClock;

  int iMemoryClock;

  int iVddc;
} ADLODPerformanceLevel;








typedef struct ADLODPerformanceLevels
{

  int iSize;
  int iReserved;

  ADLODPerformanceLevel aLevels [1];
} ADLODPerformanceLevels;








typedef struct ADLCrossfireComb
{

  int iNumLinkAdapter;

  int iAdaptLink[3];
} ADLCrossfireComb;








typedef struct ADLCrossfireInfo
{

  int iErrorCode;

  int iState;

  int iSupported;
} ADLCrossfireInfo;








typedef struct ADLBiosInfo
{
    char strPartNumber[ADL_MAX_PATH];    
    char strVersion[ADL_MAX_PATH];        
    char strDate[ADL_MAX_PATH];        
} ADLBiosInfo, *LPADLBiosInfo;









typedef struct ADLAdapterLocation
{

    int iBus;

    int iDevice;

    int iFunction;
} ADLAdapterLocation,ADLBdf;







typedef struct ADLVersionsInfo
{
    
    char strDriverVer[ADL_MAX_PATH];
    
    char strCatalystVersion[ADL_MAX_PATH];
    
    char strCatalystWebLink[ADL_MAX_PATH];

} ADLVersionsInfo, *LPADLVersionsInfo;







typedef struct ADLVersionsInfoX2
{
    
    char strDriverVer[ADL_MAX_PATH];
    
    char strCatalystVersion[ADL_MAX_PATH];
    
    char strCrimsonVersion[ADL_MAX_PATH];
    
    char strCatalystWebLink[ADL_MAX_PATH];

} ADLVersionsInfoX2, *LPADLVersionsInfoX2;








typedef struct ADLMVPUCaps
{

  int iSize;

  int iAdapterCount;

  int iPossibleMVPUMasters;

  int iPossibleMVPUSlaves;

  char cAdapterPath[ADL_DL_MAX_MVPU_ADAPTERS][ADL_DL_MAX_REGISTRY_PATH];
} ADLMVPUCaps;








typedef struct ADLMVPUStatus
{

  int iSize;

  int iActiveAdapterCount;

  int iStatus;

  ADLAdapterLocation aAdapterLocation[ADL_DL_MAX_MVPU_ADAPTERS];
} ADLMVPUStatus;










typedef struct ADLActivatableSource
{
    
    int iAdapterIndex;
    
    int iNumActivatableSources;
    
    int iActivatableSourceMask;
    
    int iActivatableSourceValue;
} ADLActivatableSource, *LPADLActivatableSource;










typedef struct ADLMode
{

    int iAdapterIndex;

    ADLDisplayID displayID;

    int iXPos;

    int iYPos;

    int iXRes;

    int iYRes;

    int iColourDepth;

    float fRefreshRate;

    int iOrientation;

    int iModeFlag;

    int iModeMask;

    int iModeValue;
} ADLMode, *LPADLMode;








typedef struct ADLDisplayTarget
{
    
    ADLDisplayID displayID;

    
    int iDisplayMapIndex;

    
    int  iDisplayTargetMask;

    
    int  iDisplayTargetValue;

} ADLDisplayTarget, *LPADLDisplayTarget;








typedef struct tagADLBezelTransientMode
{
    
    int iAdapterIndex;

    
    int iSLSMapIndex;

    
    int iSLSModeIndex;

    
    ADLMode displayMode;

    
    int  iNumBezelOffset;

    
    int  iFirstBezelOffsetArrayIndex;

    
    int  iSLSBezelTransientModeMask;

    
    int  iSLSBezelTransientModeValue;

} ADLBezelTransientMode, *LPADLBezelTransientMode;










typedef struct ADLAdapterDisplayCap
{
    
    int iAdapterIndex;
    
    int  iAdapterDisplayCapMask;
    
    int  iAdapterDisplayCapValue;
} ADLAdapterDisplayCap, *LPADLAdapterDisplayCap;










typedef struct ADLDisplayMap
{

    int iDisplayMapIndex;


    ADLMode displayMode;


    int iNumDisplayTarget;


    int iFirstDisplayTargetArrayIndex;


     int  iDisplayMapMask;


    int  iDisplayMapValue;

} ADLDisplayMap, *LPADLDisplayMap;









typedef struct ADLPossibleMap
{
    
    int iIndex;
    
    int iAdapterIndex;
    
    int iNumDisplayMap;
    
    ADLDisplayMap* displayMap;
    
    int iNumDisplayTarget;
    
    ADLDisplayTarget* displayTarget;
} ADLPossibleMap, *LPADLPossibleMap;








typedef struct ADLPossibleMapping
{
    int iDisplayIndex;                
    int iDisplayControllerIndex;    
    int iDisplayMannerSupported;    
} ADLPossibleMapping, *LPADLPossibleMapping;








typedef struct ADLPossibleMapResult
{
    
    int iIndex;
    
    int iPossibleMapResultMask;
    
    int iPossibleMapResultValue;
} ADLPossibleMapResult, *LPADLPossibleMapResult;







typedef struct ADLSLSGrid
{

    int iAdapterIndex;


    int  iSLSGridIndex;


    int  iSLSGridRow;


    int  iSLSGridColumn;


    int  iSLSGridMask;


    int  iSLSGridValue;

} ADLSLSGrid, *LPADLSLSGrid;







typedef struct    ADLSLSMap
{
    
    int iAdapterIndex;

    
    int iSLSMapIndex;

    
    ADLSLSGrid grid;

    
    int  iSurfaceMapIndex;

     
     int iOrientation;

    
    int  iNumSLSTarget;

    
    int  iFirstSLSTargetArrayIndex;

    
    int  iNumNativeMode;

    
    int  iFirstNativeModeArrayIndex;

    
    int  iNumBezelMode;

    
    int  iFirstBezelModeArrayIndex;

    
    int  iNumBezelOffset;

    
    int  iFirstBezelOffsetArrayIndex;

    
    int  iSLSMapMask;

    
    int  iSLSMapValue;


} ADLSLSMap, *LPADLSLSMap;







typedef struct ADLSLSOffset
{
    
    int iAdapterIndex;

    
    int iSLSMapIndex;

    
    ADLDisplayID displayID;

    
    int iBezelModeIndex;

    
    int iBezelOffsetX;

    
    int iBezelOffsetY;

    
    int iDisplayWidth;

    
    int iDisplayHeight;

    
    int iBezelOffsetMask;

    
    int  iBezelffsetValue;
} ADLSLSOffset, *LPADLSLSOffset;







typedef struct ADLSLSMode
{
    
    int iAdapterIndex;

    
    int iSLSMapIndex;

    
    int iSLSModeIndex;

    
    ADLMode displayMode;

    
    int iSLSNativeModeMask;

    
    int iSLSNativeModeValue;
} ADLSLSMode, *LPADLSLSMode;










typedef struct ADLPossibleSLSMap
{
    
    
    int iSLSMapIndex;

    
    int iNumSLSMap;

    
    ADLSLSMap* lpSLSMap;

    
    int iNumSLSTarget;

    
    ADLDisplayTarget* lpDisplayTarget;
} ADLPossibleSLSMap, *LPADLPossibleSLSMap;








typedef struct ADLSLSTarget
{
    
    int iAdapterIndex;

    
    int iSLSMapIndex;

    
    ADLDisplayTarget displayTarget;

    
    int iSLSGridPositionX;

    
    int iSLSGridPositionY;

    
    ADLMode viewSize;

    
    int iSLSTargetMask;

    
    int iSLSTargetValue;

} ADLSLSTarget, *LPADLSLSTarget;







typedef struct ADLBezelOffsetSteppingSize
{
    
    int iAdapterIndex;

    
    int iSLSMapIndex;

    
    int iBezelOffsetSteppingSizeX;

    
    int iBezelOffsetSteppingSizeY;

    
    int iBezelOffsetSteppingSizeMask;

    
    int iBezelOffsetSteppingSizeValue;

} ADLBezelOffsetSteppingSize, *LPADLBezelOffsetSteppingSize;







typedef struct ADLSLSOverlappedMode
{
    
    ADLMode SLSMode;
    
    int iNumSLSTarget;
    
    int iFirstTargetArrayIndex;
}ADLSLSTargetOverlap, *LPADLSLSTargetOverlap;







typedef struct ADLPXConfigCaps
{
    
    int iAdapterIndex;

    
    int  iPXConfigCapMask;

    
    int  iPXConfigCapValue;

} ADLPXConfigCaps, *LPADLPXConfigCaps;








enum ADLPxType
{
	
	ADL_PX_NONE = 0,
	
	ADL_SWITCHABLE_AMDAMD = 1,
	
	ADL_HG_AMDAMD = 2,
	
	ADL_SWITCHABLE_AMDOTHER = 3,
	
	ADL_HG_AMDOTHER = 4,
};








typedef struct _ADLApplicationData
{
    
    char strPathName[ADL_MAX_PATH];
    
    char strFileName[ADL_APP_PROFILE_FILENAME_LENGTH];
    
    char strTimeStamp[ADL_APP_PROFILE_TIMESTAMP_LENGTH];
    
    char strVersion[ADL_APP_PROFILE_VERSION_LENGTH];
}ADLApplicationData;







typedef struct _ADLApplicationDataX2
{
    
    wchar_t strPathName[ADL_MAX_PATH];
    
    wchar_t strFileName[ADL_APP_PROFILE_FILENAME_LENGTH];
    
    wchar_t strTimeStamp[ADL_APP_PROFILE_TIMESTAMP_LENGTH];
    
    wchar_t strVersion[ADL_APP_PROFILE_VERSION_LENGTH];
}ADLApplicationDataX2;







typedef struct _ADLApplicationDataX3
{
    
    wchar_t strPathName[ADL_MAX_PATH];
    
    wchar_t strFileName[ADL_APP_PROFILE_FILENAME_LENGTH];
    
    wchar_t strTimeStamp[ADL_APP_PROFILE_TIMESTAMP_LENGTH];
    
    wchar_t strVersion[ADL_APP_PROFILE_VERSION_LENGTH];
    
    unsigned int iProcessId;
}ADLApplicationDataX3;







typedef struct _PropertyRecord
{
    
    char strName [ADL_APP_PROFILE_PROPERTY_LENGTH];
    
    ADLProfilePropertyType eType;
    
    int iDataSize;
    
    unsigned char uData[1];
}PropertyRecord;







typedef struct _ADLApplicationProfile
{
    
    int iCount;
    
    PropertyRecord record[1];
}ADLApplicationProfile;








typedef struct ADLPowerControlInfo
{

int iMinValue;

int iMaxValue;

int iStepValue;
 } ADLPowerControlInfo;







typedef struct _ADLControllerMode
{
    
    
    
    int iModifiers;

    
    int iViewPositionCx;

    
    int iViewPositionCy;

    
    int iViewPanLockLeft;

    
    int iViewPanLockRight;

    
    int iViewPanLockTop;

    
    int iViewPanLockBottom;

    
    int iViewResolutionCx;

    
    int iViewResolutionCy;
}ADLControllerMode;







typedef struct ADLDisplayIdentifier
{
    
    long ulDisplayIndex;

    
    long ulManufacturerId;

    
    long ulProductId;

    
    long ulSerialNo;

} ADLDisplayIdentifier;







typedef struct _ADLOD6ParameterRange
{
    
    int     iMin;
    
    int     iMax;
    
    int     iStep;

} ADLOD6ParameterRange;







typedef struct _ADLOD6Capabilities
{
    
    
    int     iCapabilities;
    
    
    
    int     iSupportedStates;
    
    
    
    
    int     iNumberOfPerformanceLevels;
    
    
    ADLOD6ParameterRange     sEngineClockRange;
    
    
    ADLOD6ParameterRange     sMemoryClockRange;

    
    int     iExtValue;
    
    int     iExtMask;

} ADLOD6Capabilities;







typedef struct _ADLOD6PerformanceLevel
{
    
    int iEngineClock;
    
    int iMemoryClock;

} ADLOD6PerformanceLevel;









typedef struct _ADLOD6StateInfo
{
    
    
    
    int     iNumberOfPerformanceLevels;

    
    int     iExtValue;
    
    int     iExtMask;

    
    
    ADLOD6PerformanceLevel aLevels [1];

} ADLOD6StateInfo;







typedef struct _ADLOD6CurrentStatus
{
    
    int     iEngineClock;
    
    int     iMemoryClock;
    
    
    int     iActivityPercent;
    
    int     iCurrentPerformanceLevel;
    
    int     iCurrentBusSpeed;
    
    int     iCurrentBusLanes;
    
    int     iMaximumBusLanes;

    
    int     iExtValue;
    
    int     iExtMask;

} ADLOD6CurrentStatus;







typedef struct _ADLOD6ThermalControllerCaps
{
    
    
    int     iCapabilities;
    
    int     iFanMinPercent;
    
    int     iFanMaxPercent;
    
    int     iFanMinRPM;
    
    int     iFanMaxRPM;

    
    int     iExtValue;
    
    int     iExtMask;

} ADLOD6ThermalControllerCaps;







typedef struct _ADLOD6FanSpeedInfo
{
    
    int     iSpeedType;
    
    int     iFanSpeedPercent;
    
    int        iFanSpeedRPM;

    
    int     iExtValue;
    
    int     iExtMask;

} ADLOD6FanSpeedInfo;







typedef struct _ADLOD6FanSpeedValue
{
    
    int     iSpeedType;
    
    int     iFanSpeed;

    
    int     iExtValue;
    
    int     iExtMask;

} ADLOD6FanSpeedValue;









typedef struct _ADLOD6PowerControlInfo
{
    
    int     iMinValue;
    
    int     iMaxValue;
    
    int     iStepValue;

    
    int     iExtValue;
    
    int     iExtMask;

} ADLOD6PowerControlInfo;









typedef struct _ADLOD6VoltageControlInfo
{
    
    int     iMinValue;
    
    int     iMaxValue;
    
    int     iStepValue;

    
    int     iExtValue;
    
    int     iExtMask;

} ADLOD6VoltageControlInfo;







typedef struct _ADLECCData
{
    
    int iSec;
    
    int iDed;

} ADLECCData;










typedef void *ADL_CONTEXT_HANDLE;







typedef struct ADLDisplayModeX2
{

   int  iWidth;

   int  iHeight;

   int  iScanType;

   int  iRefreshRate;

   int  iTimingStandard;
} ADLDisplayModeX2;







typedef struct _ADLOD6CapabilitiesEx
{
    
    
    
    
    int iCapabilities;
    
    
    int iSupportedStates;
    
    ADLOD6ParameterRange sEngineClockPercent;
    
    ADLOD6ParameterRange sMemoryClockPercent;
    
    ADLOD6ParameterRange sPowerControlPercent;
    
    int iExtValue;
    
    int iExtMask;
} ADLOD6CapabilitiesEx;







typedef struct _ADLOD6StateEx
{
    
    int iEngineClockPercent;
    
    int iMemoryClockPercent;
    
    int iPowerControlPercent;
    
    int iExtValue;
    
    int iExtMask;
} ADLOD6StateEx;







typedef struct _ADLOD6MaxClockAdjust
{
    
    int iEngineClockMax;
    
    
    
    
    int iMemoryClockMax;
    
    int iExtValue;
    
    int iExtMask;
} ADLOD6MaxClockAdjust;







typedef struct ADLConnectorInfo
{
    
    int iConnectorIndex;
    
    int iConnectorId;
    
    int iSlotIndex;
    
    int iType;
    
    int iOffset;
    
    int iLength;

} ADLConnectorInfo;







typedef struct ADLBracketSlotInfo
{
    
    int iSlotIndex;
    
    int iLength;
    
    int iWidth;
} ADLBracketSlotInfo;







typedef struct ADLMSTRad
{
    
    int iLinkNumber;
    
    char rad[ADL_MAX_RAD_LINK_COUNT];
} ADLMSTRad;







typedef struct ADLDevicePort
{
    
    int iConnectorIndex;
    
    ADLMSTRad aMSTRad;
} ADLDevicePort;







typedef struct ADLSupportedConnections
{
    
    int iSupportedConnections;
    
    int iSupportedProperties[ADL_MAX_CONNECTION_TYPES];
} ADLSupportedConnections;







typedef struct ADLConnectionState
{
    
    int iEmulationStatus;
    
    int iEmulationMode;
    
    int iDisplayIndex;
} ADLConnectionState;








typedef struct ADLConnectionProperties
{
    
    int iValidProperties;
    
    int iBitrate;
    
    int iNumberOfLanes;
    
    int iColorDepth;
    
    int iStereo3DCaps;
    
    int iOutputBandwidth;
} ADLConnectionProperties;







typedef struct ADLConnectionData
{
    
    int iConnectionType;
    
    ADLConnectionProperties aConnectionProperties;
    
    int iNumberofPorts;
    
    int iActiveConnections;
    
    int iDataSize;
    
    char EdidData[ADL_MAX_DISPLAY_EDID_DATA_SIZE];
} ADLConnectionData;







typedef struct ADLAdapterCapsX2
{
    
    int iAdapterID;
    
    int iNumControllers;
    
    int iNumDisplays;
    
    int iNumOverlays;
    
    int iNumOfGLSyncConnectors;
    
    int iCapsMask;
    
    int iCapsValue;
    
    int iNumConnectors;
}ADLAdapterCapsX2;

typedef enum _ADL_ERROR_RECORD_SEVERITY
{
    ADL_GLOBALLY_UNCORRECTED  = 1,
    ADL_LOCALLY_UNCORRECTED   = 2,
    ADL_DEFFERRED             = 3,
    ADL_CORRECTED             = 4
}ADL_ERROR_RECORD_SEVERITY;

typedef union _ADL_ECC_EDC_FLAG
{
    struct
    {
        unsigned int isEccAccessing        : 1;
        unsigned int reserved              : 31;
    }bits;
    unsigned int u32All;
}ADL_ECC_EDC_FLAG;







typedef struct ADLErrorRecord
{
    
    ADL_ERROR_RECORD_SEVERITY Severity;

    
    int  countValid;

    
    unsigned int count;

    
    int locationValid;

    
    unsigned int CU; 
    char StructureName[32]; 

    
    char tiestamp[32];

    unsigned int padding[3];
}ADLErrorRecord;

typedef enum _ADL_EDC_BLOCK_ID
{
    ADL_EDC_BLOCK_ID_SQCIS = 1,
    ADL_EDC_BLOCK_ID_SQCDS = 2,
    ADL_EDC_BLOCK_ID_SGPR  = 3,
    ADL_EDC_BLOCK_ID_VGPR  = 4,
    ADL_EDC_BLOCK_ID_LDS   = 5,
    ADL_EDC_BLOCK_ID_GDS   = 6,
    ADL_EDC_BLOCK_ID_TCL1  = 7,
    ADL_EDC_BLOCK_ID_TCL2  = 8
}ADL_EDC_BLOCK_ID;

typedef enum _ADL_ERROR_INJECTION_MODE
{
    ADL_ERROR_INJECTION_MODE_SINGLE      = 1,
    ADL_ERROR_INJECTION_MODE_MULTIPLE    = 2,
    ADL_ERROR_INJECTION_MODE_ADDRESS     = 3
}ADL_ERROR_INJECTION_MODE;

typedef union _ADL_ERROR_PATTERN
{
    struct
    {
        unsigned long  EccInjVector         :  16;
        unsigned long  EccInjEn             :  9;
        unsigned long  EccBeatEn            :  4;
        unsigned long  EccChEn              :  4;
        unsigned long  reserved             :  31;
    } bits;
    unsigned long long u64Value;
} ADL_ERROR_PATTERN;

typedef struct _ADL_ERROR_INJECTION_DATA
{
    unsigned long long errorAddress;
    ADL_ERROR_PATTERN errorPattern;
}ADL_ERROR_INJECTION_DATA;







typedef struct ADLErrorInjection
{
    ADL_EDC_BLOCK_ID blockId;
    ADL_ERROR_INJECTION_MODE errorInjectionMode;
}ADLErrorInjection;

typedef struct ADLErrorInjectionX2
{
    ADL_EDC_BLOCK_ID blockId;
    ADL_ERROR_INJECTION_MODE errorInjectionMode;
    ADL_ERROR_INJECTION_DATA errorInjectionData;
}ADLErrorInjectionX2;








typedef struct ADLFreeSyncCap
{
    
    int iCaps;
    
    int iMinRefreshRateInMicroHz;
    
    int iMaxRefreshRateInMicroHz;
    
    int iReserved[5];
} ADLFreeSyncCap;








typedef struct _ADLDceSettings
{
    DceSettingsType type;                       
    union
    {
        struct
        {
            bool qualityDetectionEnabled;
        } HdmiLq;
        struct
        {
            DpLinkRate linkRate;                
            unsigned int numberOfActiveLanes;   
            unsigned int numberofTotalLanes;    
            int relativePreEmphasis;            
            int relativeVoltageSwing;           
            int persistFlag;
        } DpLink;
        struct
        {
            bool linkProtectionEnabled;         
        } Protection;
    } Settings;
    int iReserved[15];
} ADLDceSettings;







typedef struct ADLGraphicCoreInfo
{
    
    int iGCGen;

    union
    {
        
        int iNumCUs;
        
        int iNumWGPs;
    };

    union
    {
        
        int iNumPEsPerCU;
        
        int iNumPEsPerWGP;
    };

    
    int iNumSIMDs;

    
    int iNumROPs;

    
    int iReserved[11];
}ADLGraphicCoreInfo;








typedef struct _ADLODNParameterRange
{
    
    int     iMode;
    
    int     iMin;
    
    int     iMax;
    
    int     iStep;
    
    int     iDefault;

} ADLODNParameterRange;







typedef struct _ADLODNCapabilities
{
    
    
    
    int     iMaximumNumberOfPerformanceLevels;
    
    
    ADLODNParameterRange     sEngineClockRange;
    
    
    ADLODNParameterRange     sMemoryClockRange;
    
    
    ADLODNParameterRange     svddcRange;
    
    
    ADLODNParameterRange     power;
    
    
    ADLODNParameterRange     powerTuneTemperature;
    
    
    ADLODNParameterRange     fanTemperature;
    
    
    ADLODNParameterRange     fanSpeed;
    
    
    ADLODNParameterRange     minimumPerformanceClock;
} ADLODNCapabilities;







typedef struct _ADLODNCapabilitiesX2
{
    
    
    
    int     iMaximumNumberOfPerformanceLevels;
    
    
    int iFlags;
    
    
    ADLODNParameterRange     sEngineClockRange;
    
    
    ADLODNParameterRange     sMemoryClockRange;
    
    
    ADLODNParameterRange     svddcRange;
    
    
    ADLODNParameterRange     power;
    
    
    ADLODNParameterRange     powerTuneTemperature;
    
    
    ADLODNParameterRange     fanTemperature;
    
    
    ADLODNParameterRange     fanSpeed;
    
    
    ADLODNParameterRange     minimumPerformanceClock;
    
    ADLODNParameterRange throttleNotificaion;
    
    ADLODNParameterRange autoSystemClock;
} ADLODNCapabilitiesX2;








typedef struct ADLODNPerformanceLevel
{
    
    int iClock;
    
    int iVddc;
    
    int iEnabled;
} ADLODNPerformanceLevel;








typedef struct ADLODNPerformanceLevels
{
    int iSize;
    
    int iMode;
    
    int iNumberOfPerformanceLevels;
    
    ADLODNPerformanceLevel aLevels[1];
} ADLODNPerformanceLevels;








typedef struct ADLODNFanControl
{
    int iMode;
    int iFanControlMode;
    int iCurrentFanSpeedMode;
    int iCurrentFanSpeed;
    int iTargetFanSpeed;
    int iTargetTemperature;
    int iMinPerformanceClock;
    int iMinFanLimit;
} ADLODNFanControl;








typedef struct ADLODNPowerLimitSetting
{
    int iMode;
    int iTDPLimit;
    int iMaxOperatingTemperature;
} ADLODNPowerLimitSetting;

typedef struct ADLODNPerformanceStatus
{
    int iCoreClock;
    int iMemoryClock;
    int iDCEFClock;
    int iGFXClock;
    int iUVDClock;
    int iVCEClock;
    int iGPUActivityPercent;
    int iCurrentCorePerformanceLevel;
    int iCurrentMemoryPerformanceLevel;
    int iCurrentDCEFPerformanceLevel;
    int iCurrentGFXPerformanceLevel;
    int iUVDPerformanceLevel;
    int iVCEPerformanceLevel;
    int iCurrentBusSpeed;
    int iCurrentBusLanes;
    int iMaximumBusLanes;
    int iVDDC;
    int iVDDCI;
} ADLODNPerformanceStatus;







typedef struct ADLODNPerformanceLevelX2
{
    
    int iClock;
    
    int iVddc;
    
    int iEnabled;
    
    int iControl;
} ADLODNPerformanceLevelX2;








typedef struct ADLODNPerformanceLevelsX2
{
    int iSize;
    
    int iMode;
    
    int iNumberOfPerformanceLevels;
    
    ADLODNPerformanceLevelX2 aLevels[1];
} ADLODNPerformanceLevelsX2;

typedef enum _ADLODNCurrentPowerType
{
    ODN_GPU_TOTAL_POWER = 0,
    ODN_GPU_PPT_POWER,
    ODN_GPU_SOCKET_POWER,
    ODN_GPU_CHIP_POWER
} ADLODNCurrentPowerType;


typedef struct _ADLODNCurrentPowerParameters
{
    int   size;
    ADLODNCurrentPowerType   powerType;
    int  currentPower;
} ADLODNCurrentPowerParameters;


typedef struct _ADLODNExtSingleInitSetting
{
	int mode;
	int minValue;
	int maxValue;
	int step;
	int defaultValue;
} ADLODNExtSingleInitSetting;


typedef struct _ADLOD8SingleInitSetting
{
    int featureID;
    int minValue;
    int maxValue;
    int defaultValue;
} ADLOD8SingleInitSetting;








typedef struct _ADLOD8InitSetting
{
    int count;
    int overdrive8Capabilities;
    ADLOD8SingleInitSetting  od8SettingTable[OD8_COUNT];
} ADLOD8InitSetting;







typedef struct _ADLOD8CurrentSetting
{
    int count;
    int Od8SettingTable[OD8_COUNT];
} ADLOD8CurrentSetting;









typedef struct _ADLOD8SingleSetSetting
{
    int value;
    int requested;      
    int reset;          
} ADLOD8SingleSetSetting;


typedef struct _ADLOD8SetSetting
{
    int count;
    ADLOD8SingleSetSetting  od8SettingTable[OD8_COUNT];
} ADLOD8SetSetting;







typedef struct _ADLSingleSensorData
{
    int supported;
    int  value;
} ADLSingleSensorData;

typedef struct _ADLPMLogDataOutput
{
    int size;
    ADLSingleSensorData sensors[ADL_PMLOG_MAX_SENSORS];
}ADLPMLogDataOutput;









typedef struct ADLPPLogSettings
{
    int BreakOnAssert;
    int BreakOnWarn;
    int LogEnabled;
    int LogFieldMask;
    int LogDestinations;
    int LogSeverityEnabled;
    int LogSourceMask;
    int PowerProfilingEnabled;
    int PowerProfilingTimeInterval;
}ADLPPLogSettings;







typedef struct _ADLFPSSettingsOutput
{
    
    int ulSize;
    
    int bACFPSEnabled;
    
    int bDCFPSEnabled;
    
    int ulACFPSCurrent;
    
    int ulDCFPSCurrent;
    
    int ulACFPSMaximum;
    
    int ulACFPSMinimum;
    
    int ulDCFPSMaximum;
    
    int ulDCFPSMinimum;

} ADLFPSSettingsOutput;







typedef struct _ADLFPSSettingsInput
{
    
    int ulSize;
    
    int bGlobalSettings;
    
    int ulACFPSCurrent;
    
    int ulDCFPSCurrent;
    
    int ulReserved[6];

} ADLFPSSettingsInput;







enum { ADL_PMLOG_MAX_SUPPORTED_SENSORS = 256 };

typedef struct _ADLPMLogSupportInfo
{
    
    unsigned short usSensors[ADL_PMLOG_MAX_SUPPORTED_SENSORS];
    
    int ulReserved[16];

} ADLPMLogSupportInfo;








typedef struct _ADLPMLogStartInput
{
    
    unsigned short usSensors[ADL_PMLOG_MAX_SUPPORTED_SENSORS];
    
    unsigned long ulSampleRate;
    
    int ulReserved[15];

} ADLPMLogStartInput;

typedef struct _ADLPMLogData
{
    
    unsigned int ulVersion;
    
    unsigned int ulActiveSampleRate;
    
    unsigned long long ulLastUpdated;
    
    unsigned int ulValues[ADL_PMLOG_MAX_SUPPORTED_SENSORS][2];
    
    unsigned int ulReserved[256];

} ADLPMLogData;







typedef struct _ADLPMLogStartOutput
{
    
    union
    {
        void* pLoggingAddress;
        unsigned long long ptr_LoggingAddress;
    };
    
    int ulReserved[14];

} ADLPMLogStartOutput;









typedef struct _ADLRASGetErrorCountsInput
{
    unsigned int                Reserved[16];
} ADLRASGetErrorCountsInput;









typedef struct _ADLRASGetErrorCountsOutput
{
    unsigned int                CorrectedErrors;    
    unsigned int                UnCorrectedErrors;  
    unsigned int                Reserved[14];
} ADLRASGetErrorCountsOutput;








typedef struct _ADLRASGetErrorCounts
{
    unsigned int                InputSize;
    ADLRASGetErrorCountsInput   Input;
    unsigned int                OutputSize;
    ADLRASGetErrorCountsOutput  Output;
} ADLRASGetErrorCounts;










typedef struct _ADLRASResetErrorCountsInput
{
    unsigned int                Reserved[8];
} ADLRASResetErrorCountsInput;









typedef struct _ADLRASResetErrorCountsOutput
{
    unsigned int                Reserved[8];
} ADLRASResetErrorCountsOutput;









typedef struct _ADLRASResetErrorCounts
{
    unsigned int                    InputSize;
    ADLRASResetErrorCountsInput     Input;
    unsigned int                    OutputSize;
    ADLRASResetErrorCountsOutput    Output;
} ADLRASResetErrorCounts;









typedef struct _ADLRASErrorInjectonInput
{
    unsigned long long Address;
    ADL_RAS_INJECTION_METHOD Value;
    ADL_RAS_BLOCK_ID BlockId;
    ADL_RAS_ERROR_TYPE InjectErrorType;
    ADL_MEM_SUB_BLOCK_ID SubBlockIndex;
    unsigned int padding[9];
} ADLRASErrorInjectonInput;









typedef struct _ADLRASErrorInjectionOutput
{
    unsigned int ErrorInjectionStatus;
    unsigned int padding[15];
} ADLRASErrorInjectionOutput;









typedef struct _ADLRASErrorInjection
{
    unsigned int                           InputSize;
    ADLRASErrorInjectonInput               Input;
    unsigned int                           OutputSize;
    ADLRASErrorInjectionOutput             Output;
} ADLRASErrorInjection;







typedef struct _ADLSGApplicationInfo
{
    
    wchar_t strFileName[ADL_MAX_PATH];
    
    wchar_t strFilePath[ADL_MAX_PATH];
    
    wchar_t strVersion[ADL_MAX_PATH];
    
    long long int timeStamp;
    
    unsigned int iProfileExists;
    
    unsigned int iGPUAffinity;
    
    ADLBdf GPUBdf;
} ADLSGApplicationInfo;







enum { ADLPreFlipPostProcessingInfoInvalidLUTIndex = 0xFFFFFFFF };

enum ADLPreFlipPostProcessingLUTAlgorithm
{
    ADLPreFlipPostProcessingLUTAlgorithm_Default = 0,
    ADLPreFlipPostProcessingLUTAlgorithm_Full,
    ADLPreFlipPostProcessingLUTAlgorithm_Approximation
};

typedef struct _ADLPreFlipPostProcessingInfo
{
    
    int ulSize;
    
    int bEnabled;
    
    int ulSelectedLUTIndex;
    
    int ulSelectedLUTAlgorithm;
    
    int ulReserved[12];

} ADLPreFlipPostProcessingInfo;



typedef struct _ADL_ERROR_REASON
{
	int boost; 
	int delag; 
	int chill; 
}ADL_ERROR_REASON;







typedef struct _ADL_DELAG_NOTFICATION_REASON
{
	int HotkeyChanged; 
	int GlobalEnableChanged; 
	int GlobalLimitFPSChanged; 
}ADL_DELAG_NOTFICATION_REASON;







typedef struct _ADL_DELAG_SETTINGS
{
	int Hotkey; 
	int GlobalEnable; 
	int GlobalLimitFPS; 
	int GlobalLimitFPS_MinLimit; 
	int GlobalLimitFPS_MaxLimit; 
	int GlobalLimitFPS_Step; 
}ADL_DELAG_SETTINGS;







typedef struct _ADL_BOOST_NOTFICATION_REASON
{
	int HotkeyChanged; 
	int GlobalEnableChanged; 
	int GlobalMinResChanged; 
}ADL_BOOST_NOTFICATION_REASON;







typedef struct _ADL_BOOST_SETTINGS
{
	int Hotkey; 
	int GlobalEnable; 
	int GlobalMinRes; 
	int GlobalMinRes_MinLimit; 
	int GlobalMinRes_MaxLimit; 
	int GlobalMinRes_Step; 
}ADL_BOOST_SETTINGS;







typedef struct _ADL_RIS_NOTFICATION_REASON
{
	unsigned int GlobalEnableChanged; 
	unsigned int GlobalSharpeningDegreeChanged; 
}ADL_RIS_NOTFICATION_REASON;







typedef struct _ADL_RIS_SETTINGS
{
	int GlobalEnable; 
	int GlobalSharpeningDegree; 
	int GlobalSharpeningDegree_MinLimit; 
	int GlobalSharpeningDegree_MaxLimit; 
	int GlobalSharpeningDegree_Step; 
}ADL_RIS_SETTINGS;







typedef struct _ADL_CHILL_NOTFICATION_REASON
{
	int HotkeyChanged; 
	int GlobalEnableChanged; 
	int GlobalMinFPSChanged; 
	int GlobalMaxFPSChanged; 
}ADL_CHILL_NOTFICATION_REASON;







typedef struct _ADL_CHILL_SETTINGS
{
	int Hotkey; 
	int GlobalEnable; 
	int GlobalMinFPS; 
	int GlobalMaxFPS; 
	int GlobalFPS_MinLimit; 
	int GlobalFPS_MaxLimit; 
	int GlobalFPS_Step; 
}ADL_CHILL_SETTINGS;
#endif 
