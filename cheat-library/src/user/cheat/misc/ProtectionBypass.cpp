#include "pch-il2cpp.h"
#include "ProtectionBypass.h"

#include <cheat/native.h>
#include <helpers.h>

namespace cheat::feature 
{
	static app::Byte__Array* RecordUserData_Hook(int32_t nType)
	{
		auto& inst = ProtectionBypass::GetInstance();

		return inst.OnRecordUserData(nType);
	}

	static void LuaShellManager_ReportLuaShellResult_Hook(void* __this, app::String* type, app::String* value, MethodInfo* method)
	{
		auto& inst = ProtectionBypass::GetInstance();
		inst.OnReportLuaShell(__this, type, value, method);
	}

	static int CrashReporter_Hook(__int64 a1, __int64 a2, const char* a3)
	{
		return 0;
	}

    ProtectionBypass::ProtectionBypass() : Feature(),
        NFEX(f_Enabled, "Disable Protection", "m_DisableMhyProt", "General", true, false),
		m_CorrectSignatures({}),
		NFEX(f_SpoofLuaShell, "Spoof Anticheat result", "m_SpoofLuaShell", "General", true, false)
    {
		HookManager::install(app::Unity_RecordUserData, RecordUserData_Hook);
		HookManager::install(app::CrashReporter, CrashReporter_Hook);
	}

	void ProtectionBypass::Init()
	{
		for (int i = 0; i < 4; i++) {
			LOG_TRACE("Emulating call of RecordUserData with type %d", i);
			app::Application_RecordUserData(i, nullptr);
		}

		if (f_Enabled) {
			LOG_TRACE("Trying to close mhyprot handle.");
			if (util::CloseHandleByName(L"\\Device\\mhyprot2"))
				LOG_INFO("The Mhyprot2 handle was successfully closed. Happy hacking!");
			else
				LOG_ERROR("Failed to close mhyprot2 handle. Report this Issue and describe it.");
		}

		LOG_DEBUG("Initialized");

		// Don't hook anything from the UserAssembly before init.
		HookManager::install(app::MoleMole_LuaShellManager_ReportLuaShellResult, LuaShellManager_ReportLuaShellResult_Hook);
	}

    const FeatureGUIInfo& ProtectionBypass::GetGUIInfo() const
    {
        static const FeatureGUIInfo info { "", "Settings", false };
        return info;
    }

    void ProtectionBypass::DrawMain()
    {
		ConfigWidget(f_Enabled, 
			"Close mhyprot2 handle (changes will take effect after relaunch).");

		ConfigWidget(f_SpoofLuaShell,
			"Spoofs/Blocks anticheat reports made by WindSeedClientNotify.");
    }

    ProtectionBypass& ProtectionBypass::GetInstance()
    {
        static ProtectionBypass instance;
        return instance;
    }

	app::Byte__Array* ProtectionBypass::OnRecordUserData(int32_t nType)
	{
		if (m_CorrectSignatures.count(nType))
		{
			auto byteClass = app::GetIl2Classes()[0x25];

			auto& content = m_CorrectSignatures[nType];
			auto newArray = (app::Byte__Array*)il2cpp_array_new(byteClass, content.size());
			memmove_s(newArray->vector, content.size(), content.data(), content.size());

			return newArray;
		}

		app::Byte__Array* result = CALL_ORIGIN(RecordUserData_Hook, nType);
		auto resultArray = TO_UNI_ARRAY(result, byte);

		auto length = resultArray->length();
		if (length == 0)
			return result;

		auto stringValue = std::string((char*)result->vector, length);
		m_CorrectSignatures[nType] = stringValue;

		LOG_DEBUG("Sniffed correct signature for type %d value '%s'", nType, stringValue.c_str());

		return result;
	}

	void ProtectionBypass::OnReportLuaShell(void* __this, app::String* type, app::String* value, MethodInfo* method)
	{
		auto xor_payload = [](std::vector<uint8_t> &value_bytes) -> void
		{
			auto length = value_bytes.size() - 1;
			for (signed long long i = length; i >= 0; i -= 1)
			{
				if (i == length)
					value_bytes[i] ^= 0xA3;
				else
					value_bytes[i] ^= value_bytes[i + 1];
			}
		};

		auto value_bytes = util::from_hex_string(il2cppi_to_string(value));
		xor_payload(value_bytes);

		auto value_string = std::string((char*)value_bytes.data(), value_bytes.size());
		LOG_DEBUG("ReportLuaShellResult: %s, %s", il2cppi_to_string(type).c_str(), value_string.c_str());

		if (f_SpoofLuaShell)
		{
			//First ReportLuaShellResult will always have this payload:
			// {"1":["3.1.0"],"0":[3]}

			auto json_report = nlohmann::json::parse(value_string);

			if (json_report.contains("1"))
			{
				LOG_DEBUG("Letting the first LuaShellResult pass, blocking the rest.");
				report_sent = false;
			}
			else
				report_sent = true;
		}

		if (!report_sent)
		{
			CALL_ORIGIN(LuaShellManager_ReportLuaShellResult_Hook, __this, type, value, method);
		}

		return;
	}
}

