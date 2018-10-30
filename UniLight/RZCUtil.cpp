// UniLight by HunterZ

#include "RZCUtil.h"

#include <tchar.h>
#include <vector>
#include <windows.h> // COLORREF, HMODULE
#include "razer/RzChromaSDKDefines.h"
#include "razer/RzChromaSDKTypes.h"
#include "razer/RzErrors.h"

//#ifdef _WIN64
//#define CHROMASDKDLL        _T("RzChromaSDK64.dll")
//#else
#define CHROMASDKDLL        _T("RzChromaSDK.dll")
//#endif

namespace
{
	typedef std::vector<RZEFFECTID> EffectListT;

	typedef RZRESULT(*INIT)(void);
	typedef RZRESULT(*UNINIT)(void);
	typedef RZRESULT(*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
	typedef RZRESULT(*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
	typedef RZRESULT(*CREATEHEADSETEFFECT)(ChromaSDK::Headset::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
	typedef RZRESULT(*CREATEMOUSEPADEFFECT)(ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
	typedef RZRESULT(*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
	typedef RZRESULT(*CREATEKEYPADEFFECT)(ChromaSDK::Keypad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
	typedef RZRESULT(*CREATECHROMALINKEFFECT)(ChromaSDK::ChromaLink::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
	typedef RZRESULT(*SETEFFECT)(RZEFFECTID EffectId);
	typedef RZRESULT(*DELETEEFFECT)(RZEFFECTID EffectId);
	typedef RZRESULT(*REGISTEREVENTNOTIFICATION)(HWND hWnd);
	typedef RZRESULT(*UNREGISTEREVENTNOTIFICATION)(void);
	typedef RZRESULT(*QUERYDEVICE)(RZDEVICEID DeviceId, ChromaSDK::DEVICE_INFO_TYPE &DeviceInfo);

	INIT Init(NULL);
	UNINIT UnInit(NULL);
	CREATEEFFECT CreateEffect(NULL);
	CREATEKEYBOARDEFFECT CreateKeyboardEffect(NULL);
	CREATEMOUSEEFFECT CreateMouseEffect(NULL);
	CREATEHEADSETEFFECT CreateHeadsetEffect(NULL);
	CREATEMOUSEPADEFFECT CreateMousepadEffect(NULL);
	CREATEKEYPADEFFECT CreateKeypadEffect(NULL);
	CREATECHROMALINKEFFECT CreateChromaLinkEffect(NULL);
	SETEFFECT SetEffect(NULL);
	DELETEEFFECT DeleteEffect(NULL);
	QUERYDEVICE QueryDevice(NULL);

	bool RZC_INITIALIZED(false);
	HMODULE DLL_HANDLE(NULL);
	EffectListT ACTIVE_EFFECT_LIST;

	ResultT InitRZC()
	{
		if (RZC_INITIALIZED)
			return ResultT(true, _T("Already initialized"));

		// Razer is stupid and forces us to manually load the DLL and bind its functions
		DLL_HANDLE = ::LoadLibrary(CHROMASDKDLL);
		if (DLL_HANDLE == NULL)
			return ResultT(false, _T("LoadLibrary() failed"));

		INIT Init((INIT)::GetProcAddress(DLL_HANDLE, "Init"));
		if (Init == NULL)
			return ResultT(false, _T("Failed to find Init() in DLL"));

		if (Init() != RZRESULT_SUCCESS)
			return ResultT(false, _T("DLL Init() failed"));

		CreateEffect = (CREATEEFFECT)::GetProcAddress(DLL_HANDLE, "CreateEffect");
		CreateKeyboardEffect = (CREATEKEYBOARDEFFECT)::GetProcAddress(DLL_HANDLE, "CreateKeyboardEffect");
		CreateMouseEffect = (CREATEMOUSEEFFECT)::GetProcAddress(DLL_HANDLE, "CreateMouseEffect");
		CreateMousepadEffect = (CREATEMOUSEPADEFFECT)::GetProcAddress(DLL_HANDLE, "CreateMousepadEffect");
		CreateKeypadEffect = (CREATEKEYPADEFFECT)::GetProcAddress(DLL_HANDLE, "CreateKeypadEffect");
		CreateHeadsetEffect = (CREATEHEADSETEFFECT)::GetProcAddress(DLL_HANDLE, "CreateHeadsetEffect");
		CreateChromaLinkEffect = (CREATECHROMALINKEFFECT)::GetProcAddress(DLL_HANDLE, "CreateChromaLinkEffect");
		SetEffect = (SETEFFECT)GetProcAddress(DLL_HANDLE, "SetEffect");
		DeleteEffect = (DELETEEFFECT)GetProcAddress(DLL_HANDLE, "DeleteEffect");
		if (CreateEffect == NULL)
			return ResultT(false, _T("Failed to find CreateEffect() in DLL"));
		if (CreateKeyboardEffect == NULL)
			return ResultT(false, _T("Failed to find CreateKeyboardEffect() in DLL"));
		if (CreateMouseEffect == NULL)
			return ResultT(false, _T("Failed to find CreateMouseEffect() in DLL"));
		if (CreateMousepadEffect == NULL)
			return ResultT(false, _T("Failed to find CreateMousematEffect() in DLL"));
		if (CreateKeypadEffect == NULL)
			return ResultT(false, _T("Failed to find CreateKeypadEffect() in DLL"));
		if (CreateHeadsetEffect == NULL)
			return ResultT(false, _T("Failed to find CreateHeadsetEffect() in DLL"));
		if (CreateChromaLinkEffect == NULL)
			return ResultT(false, _T("Failed to find CreateChromaLinkEffect() in DLL"));
		if (SetEffect == NULL)
			return ResultT(false, _T("Failed to find SetEffect() in DLL"));
		if (DeleteEffect == NULL)
			return ResultT(false, _T("Failed to find DeleteEffect() in DLL"));

		RZC_INITIALIZED = true;
		return ResultT(true, _T("InitRZC() success"));
	}

	void DeleteEffects()
	{
		if (RZC_INITIALIZED)
		{
			for (EffectListT::iterator i(ACTIVE_EFFECT_LIST.begin());
				i != ACTIVE_EFFECT_LIST.end();
				++i)
			{
				DeleteEffect(*i);
			}
		}
		ACTIVE_EFFECT_LIST.clear();
	}
}

namespace RZCUtil
{
	RZCUtilC::~RZCUtilC()
	{
		if (!ACTIVE_EFFECT_LIST.empty())
			DeleteEffects();

		if (!RZC_INITIALIZED) return;

		UNINIT UnInit = (UNINIT)::GetProcAddress(DLL_HANDLE, "UnInit");
		if (UnInit != NULL)
			UnInit();

		::FreeLibrary(DLL_HANDLE);
		DLL_HANDLE = NULL;

		RZC_INITIALIZED = false;
	}

	ResultT RZCUtilC::SetRZCColor(unsigned char red, unsigned char green, unsigned char blue)
	{
		if (!RZC_INITIALIZED)
		{
			// perform lazy initialization
			// abort if initialization fails
			const ResultT& result(InitRZC());
			if (!result.first)
				return result;
		}

		// clean up previously-activated effects
		if (!ACTIVE_EFFECT_LIST.empty())
			DeleteEffects();

		const COLORREF colorRef(RGB(red, green, blue));

		// Razer requires calling a separate effect creation function per device
		//  type because reasons
		// Valid effect(s) will be gathered into a vector for subsequent activation
		EffectListT effectList;
		RZEFFECTID effectId;
		// ChromaLink
		{
			ChromaSDK::ChromaLink::STATIC_EFFECT_TYPE chromaLinkParam = { colorRef };
			if (CreateChromaLinkEffect(ChromaSDK::ChromaLink::CHROMA_STATIC, &chromaLinkParam, &effectId) == RZRESULT_SUCCESS)
				effectList.push_back(effectId);
		}
		// Headset
		{
			ChromaSDK::Headset::STATIC_EFFECT_TYPE headsetParam = { colorRef };
			if (CreateHeadsetEffect(ChromaSDK::Headset::CHROMA_STATIC, &headsetParam, &effectId) == RZRESULT_SUCCESS)
				effectList.push_back(effectId);
		}
		// Keyboard
		{
			ChromaSDK::Keyboard::STATIC_EFFECT_TYPE keyboardParam = { colorRef };
			if (CreateKeyboardEffect(ChromaSDK::Keyboard::CHROMA_STATIC, &keyboardParam, &effectId) == RZRESULT_SUCCESS)
				effectList.push_back(effectId);
		}
		// Keypad
		{
			ChromaSDK::Keypad::STATIC_EFFECT_TYPE keypadParam = { colorRef };
			if (CreateKeypadEffect(ChromaSDK::Keypad::CHROMA_STATIC, &keypadParam, &effectId) == RZRESULT_SUCCESS)
				effectList.push_back(effectId);
		}
		// Mouse
		{
			ChromaSDK::Mouse::STATIC_EFFECT_TYPE mouseParam = { ChromaSDK::Mouse::RZLED_ALL, colorRef };
			if (CreateMouseEffect(ChromaSDK::Mouse::CHROMA_STATIC, &mouseParam, &effectId) == RZRESULT_SUCCESS)
				effectList.push_back(effectId);
		}
		// Mousepad
		{
			ChromaSDK::Mousepad::STATIC_EFFECT_TYPE mousepadParam = { colorRef };
			if (CreateMousepadEffect(ChromaSDK::Mousepad::CHROMA_STATIC, &mousepadParam, &effectId) == RZRESULT_SUCCESS)
				effectList.push_back(effectId);
		}

		// abort if nothing was created (no devices connected?)
		if (effectList.empty())
			return ResultT(false, _T("SetRZColor() failed to create any effects"));

		// activate effects
		std::size_t successCount(0);
		for (EffectListT::iterator i(effectList.begin());
			i != effectList.end();
			++i)
		{
			if (SetEffect(*i) == RZRESULT_SUCCESS)
				++successCount;
		}
		
		// store effects regardless of whether they activated, as we still need to
		//  clean them up later
		ACTIVE_EFFECT_LIST.swap(effectList);

		if (!successCount)
			return ResultT(false, _T("SetRZCColor() failed to activate any effects"));

		if (successCount < ACTIVE_EFFECT_LIST.size())
			return ResultT(true, _T("SetRZCColor() partial success"));

		return ResultT(true, _T("SetRZCColor() success"));
	}
}
