//#pragma once
//#include "ZeroGUI.h"
//
//void OnMenuDraw() 
//{
//	ZeroGUI::Input::Handle();
//
//	FVector2D pos; (100.0f, 100.0f);
//
//	int test_number;
//	static bool menu_opened = false;
//	if (GetAsyncKeyState(VK_F2) & 1) menu_opened = !menu_opened; //Our menu key
//
//	if (ZeroGUI::Window((char*)"Superior UE4 GUI", &pos, FVector2D{ 500.0f, 400.0f }, menu_opened))
//	{
//		//Simple Tabs
//		static int tab = 0;
//		if (ZeroGUI::ButtonTab((char*)"Tab 1", FVector2D{ 110, 25 }, tab == 0)) tab = 0;
//		if (ZeroGUI::ButtonTab((char*)"Tab 2", FVector2D{ 110, 25 }, tab == 1)) tab = 1;
//		if (ZeroGUI::ButtonTab((char*)"Tab 3", FVector2D{ 110, 25 }, tab == 2)) tab = 2;
//		if (ZeroGUI::ButtonTab((char*)"Tab 4", FVector2D{ 110, 25 }, tab == 3)) tab = 3;
//		ZeroGUI::NextColumn(130.0f);
//		//
//
//		//Some Elements
//		static bool text_check = false;
//		static float text_slider = 15.0f;
//		static int test_hotkey = 0x2;
//		static FLinearColor test_color{ 0.0f, 0.0f, 1.0f, 1.0f };
//
//		ZeroGUI::Checkbox((char*)"Test Checkbox", &text_check);
//		ZeroGUI::SliderFloat((char*)"Test Slider", &text_slider, 0.0f, 180.0f);
//		ZeroGUI::Hotkey((char*)"Test Hotkey", FVector2D{ 80, 25 }, &test_hotkey);
//
//		ZeroGUI::Text((char*)"Left aligned text!");
//		ZeroGUI::Text((char*)"Outline and Center aligned text!", true, true);
//
//		//Element with padding
//		ZeroGUI::PushNextElementY(50.0f);
//		ZeroGUI::Combobox((char*)"Combobox", FVector2D{ 100, 25 }, &test_number, "None", "First", "Second", "Third", NULL); //NULL at end is required!
//		ZeroGUI::SameLine();//inline items
//		if (ZeroGUI::Button((char*)"It's a Button!", FVector2D{ 100, 25 })) { /*clicked!*/ }
//
//		//Color Picker
//		ZeroGUI::ColorPicker((char*)"Color Picker", &test_color);
//	}
//	ZeroGUI::Render();//Custom Render. I use it for drawing Combobox and ColorPicker over the menu
//	ZeroGUI::Draw_Cursor(menu_opened);
//}