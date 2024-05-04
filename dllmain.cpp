#include "SDK.hpp"
#include <vector>
#include <map>
//#include "libs/ZeroGUI/ZeroGUI.h"
//#include "libs/ZeroGUI/ZeroInput.h"

#define RELATIVE(addr, size) ((uintptr_t)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

//inline SDK::UFont* fonts;


std::vector<std::wstring> menuOptions = { L"> Draw Box", L"> Draw Skeleton", L"> Draw FOV", L"> Silent Aimbot", L"> Inf Stamina", L"> No Recoil/Spread"};
std::map<int, bool> optionStates;
int selectedOption = 0;

bool drawBox = false;
bool drawSkeleton = false;
bool drawFov = false;
bool silentAimbot = false;
bool infStamina = false;
bool norecspread = false;
int minValue = 0;
int maxValue = 200;
int fovSize = 100;
int sliderValue = 0;

bool keyPressed = false;

bool menuOpen = false;
int menuKey = VK_INSERT;

// Function to update slider value
void UpdateSliderValue(int change) {
	sliderValue += change;
	if (sliderValue < minValue) {
		sliderValue = minValue;
	}
	else if (sliderValue > maxValue) {
		sliderValue = maxValue;
	}
}

void DrawSlider(SDK::UCanvas* Canvas, SDK::FVector2D position) {
	std::wstring sliderText = L"Slider Value: " + std::to_wstring(sliderValue);
	SDK::FLinearColor textColor = SDK::FLinearColor(255, 255, 255, 255);
	ue->DrawTextLeft(Canvas, position, sliderText.c_str(), textColor);
}

void MainRender(SDK::UObject* object, SDK::UCanvas* Canvas) {
	if (Canvas) {

		if (GetAsyncKeyState(menuKey) & 1) {
			menuOpen = !menuOpen;
		}

		bool upKeyPressed = (GetAsyncKeyState(VK_UP) & 0x8000);
		bool downKeyPressed = (GetAsyncKeyState(VK_DOWN) & 0x8000);
		bool rightKeyPressed = (GetAsyncKeyState(VK_RIGHT) & 0x8000);

		if (upKeyPressed && !keyPressed) {
			selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
			keyPressed = true;
		}
		else if (downKeyPressed && !keyPressed) {
			selectedOption = (selectedOption + 1) % menuOptions.size();
			keyPressed = true;
		}
		else if (rightKeyPressed && !keyPressed) {
			optionStates[selectedOption] = !optionStates[selectedOption];
			if (selectedOption == 0) drawBox = optionStates[selectedOption];
			else if (selectedOption == 1) drawSkeleton = optionStates[selectedOption];
			else if (selectedOption == 2) drawFov = optionStates[selectedOption];
			else if (selectedOption == 3) silentAimbot = optionStates[selectedOption];
			else if (selectedOption == 4) infStamina = optionStates[selectedOption];
			else if (selectedOption == 5) norecspread = optionStates[selectedOption];
			else if (selectedOption == 6) fovSize = optionStates[selectedOption];
			else if (selectedOption == 7) sliderValue = optionStates[selectedOption];
			keyPressed = true;
		}
		else if (!upKeyPressed && !downKeyPressed && !rightKeyPressed) {
			keyPressed = false;
		}

		if (menuOpen) {
			float yOffset = 100.0f;
			float stateOffset = 700.0f;

			for (size_t i = 0; i < menuOptions.size(); ++i) {
				SDK::FVector2D textPosition(10.0f, yOffset + i * 15.0f);
				SDK::FLinearColor textColor = (i == selectedOption) ? SDK::FLinearColor(255, 0, 0, 255) : SDK::FLinearColor(255, 255, 255, 255);
				ue->DrawTextLeft(Canvas, textPosition, menuOptions[i].c_str(), textColor);

				std::wstring stateText = optionStates[i] ? L"Enabled" : L"Disabled";
				SDK::FVector2D statePosition(Canvas->SizeX - stateOffset, yOffset + i * 15.0f);
				SDK::FLinearColor stateColor = optionStates[i] ? SDK::FLinearColor(0, 255, 0, 255) : SDK::FLinearColor(255, 0, 0, 255);
				ue->DrawTextLeft(Canvas, statePosition, stateText.c_str(), stateColor);
			}
		}

		if (menuOpen && optionStates[7]) {
			DrawSlider(Canvas, SDK::FVector2D(10.0f, 10.0f + 15 * (menuOptions.size() + 1)));
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (!ue->SetObjects()) return;

		ue->PlayerController->GetViewportSize(&x, &y);
		ue->DrawTextLeft(Canvas, SDK::FVector2D(10.0f, 15.0f), L"GroundBranch Internal", SDK::FLinearColor(255,255,255,255)); 
		ue->DrawTextLeft(Canvas, SDK::FVector2D(10.0f, 30.0f), L"https://discord.gg/4JuzP6BNJ9", SDK::FLinearColor(255, 255, 255, 255));

		auto MyPawn = ue->World->OwningGameInstance->LocalPlayers[0]->PlayerController->Pawn;
		auto MyPlayer = ue->PlayerController->K2_GetPawn();
		auto CamManager = ue->PlayerController->PlayerCameraManager;
		auto MyLoc = ue->PlayerController->PlayerCameraManager->GetCameraLocation();
		auto MyRot = ue->PlayerController->PlayerCameraManager->GetCameraRotation();

		SDK::AGBCharacter* closest_player = 0;
		float closest_distance = FLT_MAX;

		SDK::FVector2D wnd_size = SDK::FVector2D(x, y);

		SDK::TArray<SDK::AActor*> projectile;
		SDK::TArray<SDK::AActor*> actors;
		ue->GPStatics->STATIC_GetAllActorsOfClass(ue->World, (SDK::AActor*)SDK::AGBCharacter::StaticClass(), &actors);

		ue->GPStatics->STATIC_GetAllActorsOfClass(ue->World, (SDK::AActor*)SDK::AGBBulletProjectile::StaticClass(), &projectile);

		SDK::FVector2D Center(wnd_size.X / 2, wnd_size.Y / 2);

		if (drawFov && optionStates[2]) 
		{
			ue->DrawCircle(Canvas, Center, 100, 10, SDK::FLinearColor(255, 0, 255, 255));
		}

		for (int i = 0; i < actors.Num(); i++) {

			SDK::AGBCharacter* enemies = (SDK::AGBCharacter*)actors[i];

			if (enemies)
			{

				auto EnemyMesh = enemies->Mesh;

				auto color = (true) ? SDK::FLinearColor(0.f, 255.f, 0.f, 255.f) : SDK::FLinearColor(217.f, 17.f, 0.f, 255.f);

				if (!enemies->IsAlive()) continue;

				SDK::FVector ActorLoc = enemies->K2_GetActorLocation();

				SDK::FVector2D ScreenLoc;

				std::vector<std::pair<int, int>> SkeletonConnections = {
				{neck_01, Head},
				{spine_02, neck_01},
				{spine_01, spine_02},
				{spine_03, spine_01}, // stomach to chest
				{pelvis, spine_03},
				{clavicle_r, neck_01},
				{upperarm_r, clavicle_r},
				{lowerarm_r, upperarm_r}, // right elbow
				{hand_r, lowerarm_r},
				{clavicle_l, neck_01},
				{upperarm_l, clavicle_l},
				{lowerarm_l, upperarm_l}, // left elbow
				{hand_l, lowerarm_l},
				{thigh_r, pelvis},
				{calf_r, thigh_r},
				{foot_r, calf_r},
				{thigh_l, pelvis},
				{calf_l, thigh_l},
				{foot_l, calf_l},
				};

				SDK::FVector2D BoneScreen, PrevBoneScreen;
				for (const std::pair<int, int>& Connection : SkeletonConnections)
				{
					const auto Bone1 = Connection.first;
					const auto Bone2 = Connection.second;

					const auto BoneLoc1 = enemies->Mesh->GetSocketLocation(enemies->Mesh->GetBoneName(Bone1));
					const auto BoneLoc2 = enemies->Mesh->GetSocketLocation(enemies->Mesh->GetBoneName(Bone2));

					const auto Screen1 = ue->PlayerController->ProjectWorldLocationToScreen(BoneLoc1, false, &BoneScreen);
					const auto Screen2 = ue->PlayerController->ProjectWorldLocationToScreen(BoneLoc2, false, &PrevBoneScreen);

					if (Screen1 && Screen2) 
					{
						if (!enemies->IsLocallyControlled()) 
						{
							if (drawSkeleton && optionStates[1]) 
							{
								Canvas->K2_DrawLine(BoneScreen, PrevBoneScreen, 1.5f, SDK::FLinearColor(255, 255, 255, 255));
							}
							//printf("DEBUG: SkeletoneDrawing - S\n");
						}
						else
						{
							//printf("DEBUG: SkeletoneDrawing - N\n");
						}
					}

				}

				const auto BoneLoc1 = enemies->Mesh->GetSocketLocation(enemies->Mesh->GetBoneName(Head));
				const auto BoneLoc2 = enemies->Mesh->GetSocketLocation(enemies->Mesh->GetBoneName(foot_l));

				SDK::FVector WorldHead = BoneLoc1;
				SDK::FVector WorldBottom = BoneLoc2;

				SDK::FVector2D MyLoc2D; ue->PlayerController->PlayerCameraManager->GetCameraLocation();
				SDK::FVector2D head; ue->PlayerController->ProjectWorldLocationToScreen(WorldHead, false, &head);
				SDK::FVector2D Top; ue->PlayerController->ProjectWorldLocationToScreen({ BoneLoc1.X, BoneLoc1.Y, BoneLoc1.Z + 20 }, false, &Top);
				SDK::FVector2D Bottom; ue->PlayerController->ProjectWorldLocationToScreen(WorldBottom, false, &Bottom);

				if (!enemies->IsLocallyControlled()) 
				{
					if (drawBox && optionStates[0]) 
					{
						ue->Box2D(Canvas, enemies, Bottom, Top, color);
						ue->DrawTextCenter(Canvas, Top, L"PLAYER", color);
					}
					//printf("DEBUG: PlayerBox - S\n");
				}
				if (enemies->IsBotControlled()) {
					if (drawBox && optionStates[0])
					{
						ue->Box2D(Canvas, enemies, Bottom, Top, SDK::FLinearColor(0, 0, 255, 255));
						ue->DrawTextCenter(Canvas, Top, L"BOT", color);
						//printf("DEBUG: BotBox - S\n");
					}
				}
				if (enemies->IsLocallyControlled()) {
					if (norecspread && optionStates[5]) 
					{
						enemies->SetNoRecoil(norecspread);
						enemies->SetNoSway(norecspread);
					}
					enemies->Stamina = 10000000.0f;
					//printf("DEBUG: LocalPlayer - S\n");
				}
				
				auto width = Canvas->SizeX;
				auto height = Canvas->SizeY;

				auto dx = head.X - (width / 2);
				auto dy = head.Y - (height / 2);
				auto dist = sqrtf(dx * dx + dy * dy);

				if (dist <= FLT_MAX && dist < closest_distance) {

					if (dist > 100)
					{
						continue;
					}
					closest_distance = dist;
					closest_player = enemies;
				}

				SDK::FVector TpLoc = closest_player->K2_GetActorLocation();

				SDK::FVector MyLocNew(MyLoc.X, MyLoc.Y, MyLoc.Z + 50.0f);

				if (GetAsyncKeyState(VK_CAPITAL))
					//MyPlayer->K2_TeleportTo(TpLoc, MyLoc);
					closest_player->K2_TeleportTo(MyLocNew, MyRot);
			}

		}
		if (silentAimbot && optionStates[3]) 
		{
			if (closest_player == 0)
				return;
			else
				for (int i = 0; i < projectile.Num(); i++)
				{

					SDK::AGBBulletProjectile* bullets = (SDK::AGBBulletProjectile*)projectile[i];

					if (bullets)
					{

						SDK::FVector TpLoc = closest_player->Mesh->GetSocketLocation(closest_player->Mesh->GetBoneName(Head));
						SDK::FHitResult HResult;

						bullets->K2_SetActorLocation(TpLoc, false, true, &HResult);

					}

				}
		}

	}

	return origin_renderer(object, Canvas);
}

void main() {

	// WATERMARKA
	printf("Debug Console\n\n\n");

	/***************-UWORLD-**********************/
	ue->GWorld = (SDK::UWorld**)((DWORD64)GetModuleHandleW(NULL) + 0x692c040);
	/********************************************/
	uintptr_t GWorld;

	const auto address4 = FindPattern(GetModuleHandleW(NULL),
		reinterpret_cast<const unsigned char*>("\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\xC8\xE8\x00\x00\x00\x00\x84"),
		"xxx????xxxx????x");

	GWorld = RELATIVE(address4, 7);

	printf("GWorld: 0x%x\n", GWorld - (uintptr_t)GetModuleHandleW(0));

	uintptr_t GlobalObjects;

	const auto address = FindPattern(GetModuleHandleW(NULL),
		reinterpret_cast<const unsigned char*>("\x48\x8B\x05\x00\x00\x0\x00\x48\x8B\x0C\x07\x48\x85\xC9\x74\x20"),
		"xxx????xxxxxxxxx");

	const auto offset = *reinterpret_cast<uint32_t*>(address + 3);
	GlobalObjects = address + 7 + offset;

	printf("GlobalObjects: 0x%x\n", GlobalObjects - (uintptr_t)GetModuleHandleW(0));
	uintptr_t funcGetObjectName;

	const auto address2 = FindPattern(GetModuleHandleW(NULL),
		reinterpret_cast<const unsigned char*>("\x41\xb8\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x48\x8b\xc3\x48\x83\xc4\x00\x5b\xc3\x48\x8b\x42"), "xx????x????xxxxxx?xxxxx");

	funcGetObjectName = address2 - 0x3F;

	printf("funcGetObjectName: 0x%x\n", funcGetObjectName - (uintptr_t)GetModuleHandleW(0));


	/***************-Post render-**********************/
	SDK::UWorld* World = *ue->GWorld;
	auto GameInstance = World->OwningGameInstance;
	auto LocalPlayer = GameInstance->LocalPlayers[0];
	auto ViewportClient = LocalPlayer->ViewportClient;
	// swapping vtables
	auto vTable = *(void***)(ViewportClient);
	origin_renderer = (PostRender_t)vTable[100];
	SwapVtable((void*)ViewportClient, 100, MainRender);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == 1) {
		AllocConsole();
		FILE* fileptr;
		freopen_s(&fileptr, "CONOUT$", "w", stdout);
		freopen_s(&fileptr, "CONOUT$", "w", stderr);
		freopen_s(&fileptr, "CONIN$", "r", stdin);
		main();
	}
	return true;
}