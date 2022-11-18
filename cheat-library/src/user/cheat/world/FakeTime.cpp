#include "pch-il2cpp.h"
#include "FakeTime.h"
#include <cheat/events.h>


namespace cheat::feature
{
	//CNLouisLiu
	void* LevelTimeManager = NULL;
	FakeTime::FakeTime() : Feature(),
		NFP(f_Enabled, "FakeTime", "Fake Time", false),
		NF(f_TimeHour, "FakeTime", 12),
		NF(f_TimeMinute, "FakeTime", 0)
	{
		HookManager::install(app::LevelTimeManager_SetInternalTimeOfDay, LevelTimeManager_SetInternalTimeOfDay_Hook);

		events::GameUpdateEvent += MY_METHOD_HANDLER(FakeTime::OnGameUpdate);
	}

	FakeTime& FakeTime::GetInstance()
	{
		static FakeTime instance;
		return instance;
	}

	const FeatureGUIInfo& FakeTime::GetGUIInfo() const
	{
		TRANSLATED_GROUP_INFO("Fake Time", "World");
		return info;
	}

	void FakeTime::DrawMain()
	{
		ConfigWidget(_TR("Enabled"), f_Enabled, _TR("Keep game time the same"));
		ConfigWidget(_TR("TimeHour"), f_TimeHour, 1, 0, 24);
		ConfigWidget(_TR("TimeMinute"), f_TimeMinute, 1, 0, 60);
	}

	bool FakeTime::NeedStatusDraw() const
	{
		return f_Enabled->enabled();
	}

	void FakeTime::DrawStatus()
	{
		ImGui::Text("%s|%d:%d", _TR("Fake Time"), f_TimeHour.value(), f_TimeMinute.value());
	}

	float FakeTime::ConversionTime() 
	{
		float time = float(f_TimeHour);
		float timemin = float(f_TimeMinute) / 60;
		return time + timemin;
	}

	void FakeTime::OnGameUpdate()
	{
		if (LevelTimeManager != NULL && f_Enabled->enabled())
		{
			auto& faketime = GetInstance();
			CALL_ORIGIN(LevelTimeManager_SetInternalTimeOfDay_Hook, LevelTimeManager, faketime.ConversionTime(), false, false, (MethodInfo*)0);
		}
	}

	void FakeTime::LevelTimeManager_SetInternalTimeOfDay_Hook(void* __this, float inHours, bool force, bool refreshEnviroTime, MethodInfo* method) 
	{
		float Hours = inHours;
		auto& faketime = GetInstance();
		if (faketime.f_Enabled->enabled())
		{
			Hours = faketime.ConversionTime();
		}
		CALL_ORIGIN(LevelTimeManager_SetInternalTimeOfDay_Hook, __this, Hours, force, refreshEnviroTime, method);
	}
}
