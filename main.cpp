#include "main.h"

uint32_t layout_change_mode = 1;

std::map<wchar_t, wchar_t> rus_table =
{
	// Russian to English
	{L'�', L'~'}, {L'\"', L'@'}, {L'�', L'#'}, {L';', L'$'},
	{L':', L'^'}, {L'?', L'&'}, {L'�', L'Q'}, {L'�', L'W'},
	{L'�', L'E'}, {L'�', L'R'}, {L'�', L'T'}, {L'�', L'Y'},
	{L'�', L'U'}, {L'�', L'I'}, {L'�', L'O'}, {L'�', L'P'},
	{L'�', L'{'}, {L'�', L'}'}, {L'/', L'|'}, {L'�', L'A'},
	{L'�', L'S'}, {L'�', L'D'}, {L'�', L'F'}, {L'�', L'G'},
	{L'�', L'H'}, {L'�', L'J'}, {L'�', L'K'}, {L'�', L'L'},
	{L'�', L':'}, {L'�', L'\"'}, {L'�', L'Z'}, {L'�', L'X'},
	{L'�', L'C'}, {L'�', L'V'}, {L'�', L'B'}, {L'�', L'N'},
	{L'�', L'M'}, {L'�', L'<'}, {L'�', L'>'}, {L',', L'?'},
	{L'�', L'`'}, {L'�', L'q'}, {L'�', L'w'}, {L'�', L'e'},
	{L'�', L'r'}, {L'�', L't'}, {L'�', L'y'}, {L'�', L'u'},
	{L'�', L'i'}, {L'�', L'o'}, {L'�', L'p'}, {L'�', L'['},
	{L'�', L']'}, {L'�', L'a'}, {L'�', L's'}, {L'�', L'd'},
	{L'�', L'f'}, {L'�', L'g'}, {L'�', L'h'}, {L'�', L'j'},
	{L'�', L'k'}, {L'�', L'l'}, {L'�', L';'}, {L'�', L'\''},
	{L'�', L'z'}, {L'�', L'x'}, {L'�', L'c'}, {L'�', L'v'},
	{L'�', L'b'}, {L'�', L'n'}, {L'�', L'm'}, {L'�', L','},
	{L'�', L'.'}, {L'.', L'/'}
};

std::map<wchar_t, wchar_t> eng_table =
{
	// English to Russian
	{L'~', L'�'}, {L'@', L'\"'}, {L'#', L'�'}, {L'$', L';'},
	{L'^', L':'}, {L'&', L'?'}, {L'Q', L'�'}, {L'W', L'�'},
	{L'E', L'�'}, {L'R', L'�'}, {L'T', L'�'}, {L'Y', L'�'},
	{L'U', L'�'}, {L'I', L'�'}, {L'O', L'�'}, {L'P', L'�'},
	{L'{', L'�'}, {L'}', L'�'}, {L'|', L'/'}, {L'A', L'�'},
	{L'S', L'�'}, {L'D', L'�'}, {L'F', L'�'}, {L'G', L'�'},
	{L'H', L'�'}, {L'J', L'�'}, {L'K', L'�'}, {L'L', L'�'},
	{L':', L'�'}, {L'\"', L'�'}, {L'Z', L'�'}, {L'X', L'�'},
	{L'C', L'�'}, {L'V', L'�'}, {L'B', L'�'}, {L'N', L'�'},
	{L'M', L'�'}, {L'<', L'�'}, {L'>', L'�'}, {L'?', L','},
	{L'`', L'�'}, {L'q', L'�'}, {L'w', L'�'}, {L'e', L'�'},
	{L'r', L'�'}, {L't', L'�'}, {L'y', L'�'}, {L'u', L'�'},
	{L'i', L'�'}, {L'o', L'�'}, {L'p', L'�'}, {L'[', L'�'},
	{L']', L'�'}, {L'a', L'�'}, {L's', L'�'}, {L'd', L'�'},
	{L'f', L'�'}, {L'g', L'�'}, {L'h', L'�'}, {L'j', L'�'},
	{L'k', L'�'}, {L'l', L'�'}, {L';', L'�'}, {L'\'', L'�'},
	{L'z', L'�'}, {L'x', L'�'}, {L'c', L'�'}, {L'v', L'�'},
	{L'b', L'�'}, {L'n', L'�'}, {L'm', L'�'}, {L',', L'�'},
	{L'.', L'�'}, {L'/', L'.'}
};

void ChangeLayout(wchar_t *in, wchar_t break_symbol = L'\0', bool reverse = false)
{
	bool now_russian = true;
	do
	{
		if (*in == break_symbol)
			return;

		if (reverse)
		{
			if (*in >= L'�' && *in <= L'�')
				now_russian = true;
			else
				if (*in >= L'A' && *in <= L'z')
					now_russian = false;
		}

		auto &table = now_russian ? rus_table : eng_table;

		auto result = table.find(*in);
		if (result != table.end())
			*in = result->second;

	} while (*in++ != L'\0');
}

hookedProcessInput_t orig_ProcessInput;
void __fastcall hooked_ProcessInput(void* _this, DWORD edx)
{
	if (layout_change_mode != 0)
	{
		stDXUTEditBox* input_box = reinterpret_cast<stInputInfo*>(_this)->pDXUTEditBox;

		if (input_box->m_nBufferSize > 2 && input_box->m_pwszBuffer[0] == L'.')
		{
			if (input_box->m_pwszBuffer[1] >= L'�' && input_box->m_pwszBuffer[1] <= L'�')
			{
				ChangeLayout(input_box->m_pwszBuffer, ((layout_change_mode == 3) ? L' ' : L'\0'), (layout_change_mode == 1));
			}
		}
	}
	return orig_ProcessInput(_this, edx);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
		case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hModule);

			DWORD dwSAMPAddr = (DWORD)GetModuleHandle("samp.dll");
			if (dwSAMPAddr == NULL)
			{
				MessageBoxA(NULL, "SA-MP was not loaded.", "CommandLayoutChanger.asi", MB_OK | MB_ICONERROR);
				return false;
			}

			IMAGE_NT_HEADERS *ntheader = reinterpret_cast<IMAGE_NT_HEADERS*>(dwSAMPAddr + reinterpret_cast<IMAGE_DOS_HEADER*>(dwSAMPAddr)->e_lfanew);
			uintptr_t ep = ntheader->OptionalHeader.AddressOfEntryPoint;
			switch (ep)
			{
			case 0x31DF13:	orig_ProcessInput = (hookedProcessInput_t)(dwSAMPAddr + 0x65D30); break; // R1
			case 0xCC4D0:	orig_ProcessInput = (hookedProcessInput_t)(dwSAMPAddr + 0x69260); break; // R3
			case 0xCBCB0:	orig_ProcessInput = (hookedProcessInput_t)(dwSAMPAddr + 0x69990); break; // R4
			default:
				{
					MessageBoxA(NULL, "Unknown SA-MP version.", "CommandLayoutChanger.asi", MB_OK | MB_ICONERROR);
					return false;
				}
				break;
			}

			layout_change_mode = GetPrivateProfileInt("main", "change-mode", 1, ".\\CommandLayoutChanger.ini");
			if (layout_change_mode < 0 || layout_change_mode > 3)
				layout_change_mode = 1;

			WritePrivateProfileString("main", "change-mode", std::to_string(layout_change_mode).c_str(), ".\\CommandLayoutChanger.ini");

			MH_Initialize();
			MH_CreateHookEx((void*)orig_ProcessInput, &hooked_ProcessInput, &orig_ProcessInput);
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			MH_RemoveHook((void*)orig_ProcessInput);
			MH_Uninitialize();
		}
		break;
	}
	return true;
}
