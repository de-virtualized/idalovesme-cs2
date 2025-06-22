#include "Menu.h"
#include "../Renderer/Renderer.h"
#include "../config.h"
#include <unordered_map>

#include <algorithm>

using namespace IdaLovesMe;

bool prevent = false;
bool inventory = false;
int knifebot = 0; // int cause its a keybind apparently
bool weaponauto = false;
bool quickswitch = false;
bool quickplant = false;
bool supertoss = false;
bool hitsound = false;
bool autobuy = false;
bool logdamagedealt = false;
bool drawlogs = false;
bool standalone = false;
bool easystrafe = false;
int jumpatedge = 0;
bool bhop = false;
bool airstrafe = false;
bool fastwalk = false;
int slowmotion = 0;
int Menu_key = VK_INSERT;
int menuColor[4] = { 163, 212, 31, 255 };
std::unordered_map<int, bool> dpiscale;
int menuAnimationSpeed = 50;  // default skeet
bool antiuntrusted = false;

int rageactiveweapon = 0;

std::unordered_map<int, bool> rageactiveweaponse;

CConfig* cfg = CConfig::get();


bool wasKeyPressed = false;


/// <summary>
/// Initialize GTX, and preset some stuff.
/// Called only once at the beginning.
/// </summary>
void CMenu::Initialize() 
{
	if (this->m_bInitialized)
		return;

	ui::CreateContext();
	GuiContext* g = Globals::Gui_Ctx;
	g->ItemSpacing = Vec2(0, 6);
	g->MenuAlpha = 1;

	this->m_bIsOpened = true;
	this->m_bInitialized = true;
}

/// <summary>
/// Drawing the user interface.
/// Please keep in mind that this function is called on each frame.
/// </summary>
void CMenu::Draw()
{
	if (GetAsyncKeyState(VK_INSERT) & 0x8000) // Check if key is currently down
	{
		if (!wasKeyPressed) // Only toggle if it wasn't already pressed
		{
			this->m_bIsOpened = !this->m_bIsOpened;
			wasKeyPressed = true;
			Sleep(150); // Small delay to prevent flickering
		}
	}
	else
	{
		wasKeyPressed = false; // Reset when key is released
	}


	static float alpha = 0.f;
	float fc = ImGui::GetIO().DeltaTime * 255 * 10;
	if (!this->m_bIsOpened && alpha > 0.f)
		alpha = std::clamp(alpha - fc / 255.f, 0.f, 1.f);

	if (this->m_bIsOpened && alpha < 1.f)
		alpha = std::clamp(alpha + fc / 255.f, 0.f, 1.f);

	Globals::Gui_Ctx->MenuAlpha = static_cast<int>(floor(alpha * 255));

	if (!this->m_bIsOpened && alpha == 0.f)
		return;

	ui::GetInputFromWindow(Render::Draw->GetWindow());

	ui::SetNextWindowSize({ 660,560 });
	ui::Begin("Main", GuiFlags_None);

	//rage
	ui::TabButton("A", &this->m_nCurrentTab, 0, 7);

	//legit
	ui::TabButton("B", &this->m_nCurrentTab, 1, 7);

	//visuals
	ui::TabButton("B", &this->m_nCurrentTab, 2, 7);

	//misc
	ui::TabButton("C", &this->m_nCurrentTab, 3, 7);

	//skins
	ui::TabButton("E", &this->m_nCurrentTab, 4, 7);

	//config
	ui::TabButton("D", &this->m_nCurrentTab, 5, 7);

	ui::TabButtonFiller("D", &this->m_nCurrentTab, 6, 7);
	ui::TabButtonFiller("C", &this->m_nCurrentTab, 7, 7);



	switch(this->m_nCurrentTab)
	{
		case 0:
		{
			GuiWindow* main_wnd = ui::GetCurrentWindow();

			ui::SetNextWindowPos(main_wnd->Pos + Vec2(100, 30));
			ui::BeginChild("Weapon type", { Vec2(0, 0), Vec2(3, 0) }, GuiFlags_NoResize | GuiFlags_NoMove);
			ui::GetCurrentWindow()->CursorPos.y = ui::GetCurrentWindow()->Pos.y + 14;
			ui::SingleSelectIcon("Rage_Weapon_Type", &cfg->i["rage_weapon"], &cfg->m["rage_weaponse"], {"Global"}, Render::Fonts::LegitTabIcons);
			ui::EndChild();

			ui::SetNextWindowPos(main_wnd->Pos + Vec2(100, 80));
			ui::SetNextWindowSize(Vec2(((main_wnd->Size.x - 125) / 2) - 9, main_wnd->Size.y - 125));
			ui::BeginChild("Aimbot", { Vec2(0, 2), Vec2(3, 8) }, GuiFlags_None);
			{
				ui::Checkbox("Enabled", &cfg->b["rage_enable"]);
				ui::KeyBind("Aimbot bind", &cfg->i["rage_bind"], &cfg->i["rage_bind_style"]);

				ui::MultiSelect("", &cfg->m["rage_hitbox"], { "Head", "Chest", "Stomach", "Arms", "Legs", "Feet" });
				ui::MultiSelect("Multi-point", &cfg->m["rage_mphitbox"], { "Head", "Chest", "Stomach", "Arms", "Legs", "Feet" });
				if (cfg->m["rage_mphitbox"][0] || cfg->m["rage_mphitbox"][1] || cfg->m["rage_mphitbox"][2] || cfg->m["rage_mphitbox"][3] || cfg->m["rage_mphitbox"][4]) {
					ui::SliderInt("", &cfg->i["mp_scale"], 0, 100, "%d%%");
				}


				ui::SliderInt("Minimum hit chance", &cfg->i["hc_scale"], 0, 100, "%d%%");
				ui::SliderInt("Minimum damage", &cfg->i["md_scale"], 0, 100, "%d%%");


				ui::Checkbox("Minimum hit chance override", &cfg->b["override_hc_bool"]);
				ui::KeyBind("Minimum hit chance override key", &cfg->i["min_hc_override_key"], &cfg->i["min_hc_override_key_style"]);


				if (cfg->b["override_hc_bool"])
				{
					ui::SliderInt("Minimum hit chance override", &cfg->i["min_dmg_override"], 0, 100, "%d");
				}


				ui::Checkbox("Minimum damage override", &cfg->b["override_dmg_bool"]);
				ui::KeyBind("Minimum damage override key", &cfg->i["min_dmg_override_key"], &cfg->i["min_dmg_override_key_style"]);


				if (cfg->b["override_dmg_bool"])
				{
					ui::SliderInt("Minimum damage override", &cfg->i["min_dmg_override"], 0, 100, "%d");
				}

				ui::Checkbox("Prefer body aim", &cfg->b["rage_preferbaim"]); //rage_safepoint
				ui::Label("Force body aim");
				ui::KeyBind("Force baim key", &cfg->i["rage_baim_enabled"], &cfg->i["rage_baim_enabled_style"]);

				ui::Checkbox("Quick stop", &cfg->b["rage_quickstop"]);
				if (cfg->b["rage_quickstop"]) {
					ui::MultiSelect("Quick stop options", &cfg->m["rage_quickstop_options"], { "Early", "Slow motion", "Duck", "Fake duck", "Move between shots", "Ignore molotov", "Taser" });
				}
				ui::KeyBind("Quick stop key", &cfg->i["rage_quickstop_enabled"], &cfg->i["rage_quickstop_enabled_style"]);

				ui::Checkbox("Auto scope", &cfg->b["rage_autoscope"]);


			}
			ui::EndChild();

			ui::BeginChild("Other", { Vec2(6, 0), Vec2(3, 5) });
			{
				ui::SingleSelect("Accuracy boost", &cfg->i["rage_backtrack"], std::vector<const char*>{ "Off", "Low", "Medium", "High", "Maximum" });
				ui::Checkbox("Automatic fire", &cfg->b["rage_autofire"]); //rage_safepoint
				ui::Checkbox("Aim through walls", &cfg->b["rage_aimthroughwalls"]); //rage_safepoint
				ui::Checkbox("Silent aim", &cfg->b["rage_silent"]); //rage_safepoint
				ui::Checkbox("Remove recoil", &cfg->b["rage_removerecoil"]); //rage_safepoint

				ui::Checkbox("Remove spread", &cfg->b["rage_removespread"], true); //rage_safepoint


				ui::Checkbox("Quick peek assist", &cfg->b["rage_quick_peek_assist"]);
				ui::KeyBind("Quick peek assist key", &cfg->i["rage_quickpeek_enabled"], &cfg->i["rage_quickpeek_enabled_style"]);
				if (cfg->b["rage_quick_peek_assist"]) {
					ui::MultiSelect("Quick peek assist mode", &cfg->m["rage_quick_peek_assist"], { "Retreat on shot", "Retreat on key" });
					ui::ColorPicker("quickpeek", cfg->c["rage_quick_peek_assist_col"]);
					ui::SliderInt("Quick peek assist distance", &cfg->i["rage_qpd"], 0, 1000, "%d°");
				}
				ui::Label("Duck peek assist");
				ui::KeyBind("Duck peek assist key", &cfg->i["rage_fd_enabled"], &cfg->i["rage_fd_enabled_style"]);

				ui::Checkbox("Limit aim step", &cfg->b["rage_aim_step"]);
				ui::SliderInt("Maximum FOV", &cfg->i["rage_fov"], 1, 180, "%d°");

			}
			ui::EndChild();

			ui::BeginChild("Anti-aimbot angles", { Vec2(6, 7), Vec2(3, 3) });
			{
				ui::Checkbox("Enabled", &cfg->b["antiaim_enable"]);
				if (cfg->b["antiaim_enable"])
				{
					ui::SingleSelect("Pitch", &cfg->i["aa_pitch"], std::vector<const char*>{ "Off", "Default", "Up", "Down", "Minimal", "Random" });
					ui::SingleSelect("Yaw base", &cfg->i["aa_yaw_base"], std::vector<const char*>{ "Local view", "At targets" });

					ui::SingleSelect("Yaw", &cfg->i["aa_yaw"], std::vector<const char*>{ "Off", "180", "Spin", "Static", "180 Z", "Crosshair" });

					switch (cfg->i["aa_yaw"]) {
					case 1:
						ui::SliderInt("", &cfg->i["aa_yaw_180"], -180, 180, "%d°"); break;
					case 2:
						ui::SliderInt("", &cfg->i["aa_yaw_spin"], -180, 180, "%d°"); break;
					case 3:
						ui::SliderInt("", &cfg->i["aa_yaw_static"], -180, 180, "%d°"); break;
					case 4:
						ui::SliderInt("", &cfg->i["aa_yaw_180Z"], -180, 180, "%d°"); break;
					case 5:
						ui::SliderInt("", &cfg->i["aa_yaw_Crosshair"], -180, 180, "%d°"); break;
					}

					if (cfg->i["aa_yaw"] != 0) {
						ui::SingleSelect("Yaw jitter", &cfg->i["aa_yaw_jitter"], std::vector<const char*>{ "Off", "Offset", "Center", "Random" });
						if (cfg->i["aa_yaw_jitter"] != 0)
							ui::SliderInt("a", &cfg->i["aa_yaw_jitter_amount"], -180, 180, "%d°");
					}
					ui::SingleSelect("Body yaw", &cfg->i["aa_body_yaw"], std::vector<const char*>{ "Off", "Opposite", "Jitter", "Static" });

					switch (cfg->i["aa_body_yaw"]) {
					case 2:	ui::SliderInt("", &cfg->i["aa_body_yaw_jitter_amount"], -180, 180, "%d°"); break;
					case 3: ui::SliderInt("", &cfg->i["aa_body_yaw_static_amount"], -180, 180, "%d°"); break;
					}

					if (cfg->i["aa_body_yaw"] != 0) {
						ui::Checkbox("Freestanding body yaw", &cfg->b["aa_freestanding_body_yaw"]);
						ui::SliderInt("", &cfg->i["aa_fake_yaw_limit"], 0, 60, "%d°");
					}

					ui::Checkbox("Edge yaw", &cfg->b["aa_edge_yaw"]);
					ui::KeyBind("aa_edge_yaw_bind", &cfg->i["aa_edge_yaw_bind"], &cfg->i["aa_edge_yaw_bind_style"]);
					ui::MultiSelect("Freestanding", &cfg->m["aa_freestanding"], { "Default" });
				}
			}
			ui::EndChild();

			break;
		}
		case 1:
		{
			ui::BeginChild("Weapon type#Legit", { Vec2(0,0), Vec2(9, 0) }, GuiFlags_NoMove | GuiFlags_NoResize);
			ui::TabButton("G", &this->m_nCurrentLegitTab, 0, 6, GuiFlags_LegitTab);
			ui::TabButton("P", &this->m_nCurrentLegitTab, 1, 6, GuiFlags_LegitTab);
			ui::TabButton("W", &this->m_nCurrentLegitTab, 2, 6, GuiFlags_LegitTab);
			ui::TabButton("d", &this->m_nCurrentLegitTab, 3, 6, GuiFlags_LegitTab);
			ui::TabButton("f", &this->m_nCurrentLegitTab, 4, 6, GuiFlags_LegitTab);
			ui::TabButton("a", &this->m_nCurrentLegitTab, 5, 6, GuiFlags_LegitTab);
			ui::EndChild();
			ui::BeginChild("Aimbot#Legit", { Vec2(0, 2), Vec2(3, 8) });
			ui::EndChild();
			ui::BeginChild("Triggerbot#Legit", { Vec2(6, 2), Vec2(3, 5) });
			ui::EndChild();
			ui::BeginChild("Other#Legit", { Vec2(6, 9), Vec2(3, 1) });
			ui::EndChild();

			
			break;
		}


		case 2:
		{
			ui::BeginChild("Player ESP", { Vec2(0,0), Vec2(3, 6) });
			{
				ui::Label("Activation type");
				ui::KeyBind("visuals_player_esp_activation_type", &cfg->i["visuals_player_esp_activation_type_key"], &cfg->i["visuals_player_esp_activation_type_keystyle"]);
				ui::Checkbox("Teammates", &cfg->b["visuals_player_esp_teammates"]);
				ui::Checkbox("Dormant", &cfg->b["visuals_player_esp_dormant"]);
				ui::Checkbox("Bounding box", &cfg->b["visuals_player_esp_bounding_box"]);
				ui::ColorPicker("visuals_player_esp_bounding_box_color", cfg->c["visuals_player_esp_bounding_box_color"]);
				ui::Checkbox("Health bar", &cfg->b["visuals_player_esp_health_bar"]);
				ui::Checkbox("Name", &cfg->b["visuals_player_esp_name"]);
				ui::ColorPicker("visuals_player_esp_name_color", cfg->c["visuals_player_esp_name_color"]);
				ui::Checkbox("Flags", &cfg->b["visuals_player_esp_flags"]);
			};
			ui::EndChild();
			ui::BeginChild("Colored models", { Vec2(0, 8), Vec2(3, 2) });
			ui::EndChild();
			ui::BeginChild("Other ESP", { Vec2(6, 0), Vec2(3, 3) });
			ui::EndChild();
			ui::BeginChild("Effects", { Vec2(6, 5), Vec2(3, 5) });
			ui::EndChild();

			break;
		}
	
		case 3:
		{

			ui::BeginChild("Miscellaneous", { Vec2(0,0), Vec2(3, 7) });
			ui::Checkbox("Prevent AFK kick", &prevent);
			ui::Checkbox("Unlock loadout", &inventory);
			ui::KeyBind("Knifebot", &knifebot, 0);
			ui::Checkbox("Automatic weapons", &weaponauto);
			ui::Checkbox("Quick switch", &quickswitch);
			ui::Checkbox("Quick plant", &quickplant);
			ui::Checkbox("Super toss", &supertoss);
			ui::Checkbox("Hit sound", &hitsound);
			ui::Checkbox("Auto Buy", &autobuy);
			ui::Checkbox("Log damage dealt", &logdamagedealt);
			ui::Checkbox("Draw logs", &drawlogs);
			ui::EndChild();
			ui::BeginChild("Matchmaking", { Vec2(0,9), Vec2(3, 1) });
			ui::Checkbox("Auto-accept matchmaking", &cfg->b["misc_autoaccept"]);
			ui::EndChild();
			ui::BeginChild("Movement", { Vec2(6, 0), Vec2(3, 4) });
			ui::Checkbox("Standalone quick stop", &standalone);
			ui::Checkbox("Easy strafe", &easystrafe);
			ui::KeyBind("Jump at edge", &jumpatedge, 0);
			ui::Checkbox("Bunny hop", &bhop);
			ui::Checkbox("Air strafe", &airstrafe);
			ui::Checkbox("Fast walk", &fastwalk);
			ui::KeyBind("Slow motion", &slowmotion, 0);
			ui::EndChild();
			ui::BeginChild("Settings", { Vec2(6, 6), Vec2(3, 4) });
			ui::Label("Menu color");
			ui::ColorPicker("MenuColor", menuColor);
			ui::SliderInt("Menu animation speed", &menuAnimationSpeed, 0, 200, "%d"); // idk if this works tbh
			ui::SingleSelect("DPI scale", &cfg->i["menu_dpi"], std::vector<const char*>{ "100%", "125%", "150%", "175%", "200%" });
			ui::Checkbox("Anti-untrusted", &antiuntrusted);
			ui::EndChild();


			break;
		}

	
		case 4:
		{

			ui::BeginChild("Model options", { Vec2(0,0), Vec2(3, 10) });
			ui::EndChild();
			ui::BeginChild("Weapon skin", { Vec2(6, 0), Vec2(3, 10) });
			ui::EndChild();


			break;
		}


		//
		// CFG & LUA
		//
		case 5:
		{
			ui::BeginChild("Presets", { Vec2(0,0), Vec2(3, 10) });
			{
				if (ui::BeginListbox("ConfigsList"))
				{
					/*ui::Selectable("KAKI", false);
					ui::Selectable("KAKIasdsda", false);*/ // hmm? /mtfy

					for (auto config : CConfig::get()->List) {
						if (ui::Selectable(config.c_str(), CConfig::get()->Current == config.c_str()))
							strcpy_s(cfg->s["config_name"], config.c_str());
					}
				}
				ui::EndListbox();

				ui::InputText("cfg_input", cfg->s["config_name"]);

				if (ui::Button("Load"))
					CConfig::get()->Load();

				if (ui::Button("Save"))
					CConfig::get()->Save();

				if (ui::Button("Delete"))
					CConfig::get()->Delete();

				if (ui::Button("Reset"))
					CConfig::get()->LoadDefaults();

				ui::Button("Import from clipboard");
				ui::Button("Export to clipboard");
			}
			ui::EndChild();

			break;
		}
	}

	ui::End();
}

bool CMenu::IsMenuOpened() 
{
	return this->m_bIsOpened;
}

void CMenu::SetMenuOpened(bool v) 
{
	this->m_bIsOpened = v;
}

D3DCOLOR CMenu::GetMenuColor() {
	GuiContext* g = Globals::Gui_Ctx;
	return D3DCOLOR_RGBA(163, 212, 31);
}