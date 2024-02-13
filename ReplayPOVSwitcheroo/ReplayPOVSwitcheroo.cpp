#include "pch.h"
#include "ReplayPOVSwitcheroo.h"
#include "bakkesmod/wrappers/includes.h"
#include <iostream>
using namespace std;



BAKKESMOD_PLUGIN(ReplayPOVSwitcheroo, "Replay POV Switcher", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void ReplayPOVSwitcheroo::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("Replay POV Switcher Plugin loaded!");

	//Reset goals to 0
	gameWrapper->HookEvent("Function TAGame.Team_TA.PostBeginPlay",
		[this](std::string eventName) {
			playerGoals = 0;
			matchGoals = 0;
		});

	gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode",
		[this](std::string eventName) {

			//Make sure we are in game
			if (gameWrapper->IsInOnlineGame()) {
				ServerWrapper server = gameWrapper->GetOnlineGame();

				//Make sure there is a server
				if (!server.IsNull()) {
					GameSettingPlaylistWrapper playlist = server.GetPlaylist();

					//Make sure it's not local
					if (!playlist.IsLanMatch() && !playlist.IsPrivateMatch()) {
						PlayerControllerWrapper localPrimaryPlayerController = server.GetLocalPrimaryPlayer();

						//Get the primary player (you)
						if (!localPrimaryPlayerController.IsNull()) {
							PriWrapper localPrimaryPlayer = localPrimaryPlayerController.GetPRI();

							//Make sure its not a replay causing a false trigger
							if (matchGoals == server.GetTotalScore()) {
								//Do nothing
							}

							else if (playerGoals == localPrimaryPlayer.GetMatchGoals()) {
								//Someone else scored so make the pov replay true
								cvarManager->executeCommand("cl_goalreplay_pov 1");
								matchGoals = server.GetTotalScore();
							}

							else {
								//You scored so make the pov replay false
								cvarManager->executeCommand("cl_goalreplay_pov 0");
								playerGoals = localPrimaryPlayer.GetMatchGoals();
								matchGoals = server.GetTotalScore();
							}
						}
					}
				}
			}
		});

	//Reset goals to 0
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",
		[this](std::string eventName) {
			playerGoals = 0;
			matchGoals = 0;
		});

	//Reset goals to 0
	gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded",
		[this](std::string eventName) {
			playerGoals = 0;
			matchGoals = 0;
		});
}
