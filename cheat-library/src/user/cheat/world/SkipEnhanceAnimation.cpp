#include "pch-il2cpp.h"
#include "SkipEnhanceAnimation.h"

#include "helpers.h"

namespace cheat::feature
{
	static void MoleMole_EquipLevelUpDialogContext_SetupView_Hook(/*MoleMole_EquipLevelUpDialogContext*/ void* __this, MethodInfo* method);
	static void MoleMole_EquipOverviewPageContext_PlayExpAddAnimation_Hook(/*MoleMole_EquipOverviewPageContext*/ void* __this, float startPer, float endPer, /*Action*/ void* callback, MethodInfo* method);
	static void MoleMole_EquipOverviewPageContext_PlayLevelUpSuccessShow_Hook(/*MoleMole_EquipOverviewPageContext*/ void* __this, /*Action*/ void* refreshViewAction, MethodInfo* method);

	SkipEnhanceAnimation::SkipEnhanceAnimation() : Feature(),
		NF(f_Enabled, "SkipEnhanceAnimation", false)
	{
		HookManager::install(app::MoleMole_EquipLevelUpDialogContext_SetupView, MoleMole_EquipLevelUpDialogContext_SetupView_Hook);
		HookManager::install(app::MoleMole_EquipOverviewPageContext_PlayExpAddAnimation, MoleMole_EquipOverviewPageContext_PlayExpAddAnimation_Hook);
		HookManager::install(app::MoleMole_EquipOverviewPageContext_PlayLevelUpSuccessShow, MoleMole_EquipOverviewPageContext_PlayLevelUpSuccessShow_Hook);
	}

	const FeatureGUIInfo& SkipEnhanceAnimation::GetGUIInfo() const
	{
		TRANSLATED_MODULE_INFO("World");
		return info;
	}

	void SkipEnhanceAnimation::DrawMain()
	{
		ConfigWidget(_TR("Skip Enhance Animation"), f_Enabled, _TR("Skip weapon and artifact enhancement animation."));
	}

	bool SkipEnhanceAnimation::NeedStatusDraw() const
	{
		return f_Enabled;
	}

	void SkipEnhanceAnimation::DrawStatus()
	{
		ImGui::Text(_TR("Skip Enhance Animation"));
	}

	SkipEnhanceAnimation& SkipEnhanceAnimation::GetInstance()
	{
		static SkipEnhanceAnimation instance;
		return instance;
	}

	void MoleMole_EquipLevelUpDialogContext_SetupView_Hook(/*MoleMole_EquipLevelUpDialogContext*/ void* __this, MethodInfo* method)
	{
		CALL_ORIGIN(MoleMole_EquipLevelUpDialogContext_SetupView_Hook, __this, method);

		auto& skipEnhaceAnimation = SkipEnhanceAnimation::GetInstance();
		if (skipEnhaceAnimation.f_Enabled)
			app::MoleMole_EquipLevelUpDialogContext_ShowReturnedMaterialAndCloseDialog(__this, method);
	}

	void MoleMole_EquipOverviewPageContext_PlayExpAddAnimation_Hook(/*MoleMole_EquipOverviewPageContext*/ void* __this, float startPer, float endPer, /*Action*/ void* callback, MethodInfo* method)
	{
		auto& skipEnhaceAnimation = SkipEnhanceAnimation::GetInstance();
		if (skipEnhaceAnimation.f_Enabled)
			return app::Action_Invoke(callback, method);

		CALL_ORIGIN(MoleMole_EquipOverviewPageContext_PlayExpAddAnimation_Hook, __this, startPer, endPer, callback, method);
	}

	void MoleMole_EquipOverviewPageContext_PlayLevelUpSuccessShow_Hook(/*MoleMole_EquipOverviewPageContext*/ void* __this, /*Action*/ void* refreshViewAction, MethodInfo* method)
	{
		auto& skipEnhaceAnimation = SkipEnhanceAnimation::GetInstance();
		if (skipEnhaceAnimation.f_Enabled)
			return app::Action_Invoke(refreshViewAction, method);

		CALL_ORIGIN(MoleMole_EquipOverviewPageContext_PlayLevelUpSuccessShow_Hook, __this, refreshViewAction, method);
	}
}