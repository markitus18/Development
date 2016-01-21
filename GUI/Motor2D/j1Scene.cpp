#include "p2Defs.h"
#include "p2Log.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "UIElements.h"
#include "j1Scene.h"
#include "j1Fonts.h"
#include "j1Console.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& node)
{

	LOG("Loading Scene");
	bool ret = true;

	App->console->AddCommand(&command_closeGUI);
	App->console->AddCommand(&command_openGUI);
	App->console->AddCommand(&move_labels);
	App->console->AddCommand(&save_labels);
	App->console->AddCommand(&load_labels);
	App->console->AddCommand(&c_SaveGame);
	App->console->AddCommand(&c_LoadGame);

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	pugi::xml_node config = App->GetConfig("scene");
	App->GetConfig("scene");

	// Loading texts from XML
	/*
	for (pugi::xml_node label = config.child("ui_label"); label; label = label.next_sibling("ui_label"))
	{
		TTF_Font* font = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf", 24);

		p2SString str(label.child_value());
		iPoint labelPos;
		labelPos.x = label.attribute("x").as_int();
		labelPos.y = label.attribute("y").as_int();
		bool drag = label.attribute("drag").as_bool();
		UILabel* newLabel = App->gui->CreateText("Config Label", labelPos, (char*)str.GetString(), NULL, true, NULL, font, SDL_Color{ 255, 255, 255 });

		//I do not have implemented the GUI elements to be dragged if the bool is true. I just change it here but it has no functionality.
		newLabel->dragable = drag;

		configLabels.PushBack(newLabel);
	}
	*/


	App->map->Load("iso.tmx");

	//LoadGUI();

	debug_tex = App->tex->Load("maps/path.png");
	
	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	// Gui ---
	if (elementHold)
	{
		int x, y;
		App->input->GetMouseMotion(x, y);
		elementHold->SetLocalPosition(elementHold->GetLocalPosition().x + x, elementHold->GetLocalPosition().y + y);
	}
	// -------
	ManageInput(dt);

	App->map->Draw();
	
	// Debug pathfinding ------------------------------

	//Getting current mouse tile
	int x, y;
	App->input->GetMousePosition(x, y);
	int mapX, mapY;
	mapX = x - App->render->camera.x;
	mapY = y - App->render->camera.y;
	iPoint map_coordinates = App->map->WorldToMap(mapX, mapY);
	//Fixing offset 
	map_coordinates.x -= 1;

	iPoint currentTile = App->map->MapToWorld(map_coordinates.x, map_coordinates.y);
	currentTile_x = map_coordinates.x;
	currentTile_y = map_coordinates.y;

	App->render->Blit(debug_tex, currentTile.x, currentTile.y, new SDL_Rect{0, 0, 64, 64});

	if (App->pathFinding->pathFinished)
	{
		for (uint i = 0; i < App->pathFinding->path.Count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->path[i].x, App->pathFinding->path[i].y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 0, 64, 64 });
		}
	}

	else if (App->pathFinding->pathStarted)
	{
		for (uint i = 0; i < App->pathFinding->openList.count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->openList[i]->tile.x, App->pathFinding->openList[i]->tile.y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 0, 64, 64 });
		}
		for (uint i = 0; i < App->pathFinding->closedList.count(); i++)
		{
			iPoint position = App->map->MapToWorld(App->pathFinding->closedList[i]->tile.x, App->pathFinding->closedList[i]->tile.y);
			App->render->Blit(debug_tex, position.x, position.y, new SDL_Rect{ 0, 64, 64, 64 });
		}
	}

	//Drawing Start and End tiles
	iPoint startPosition = App->map->MapToWorld(App->pathFinding->startTile.x, App->pathFinding->startTile.y);
	iPoint endPosition = App->map->MapToWorld(App->pathFinding->endTile.x, App->pathFinding->endTile.y);
	if (App->pathFinding->startTileExists)
		App->render->Blit(App->map->data.tilesets.start->next->data->texture, startPosition.x, startPosition.y, new SDL_Rect{ 0, 64, 64, 64 });
	if (App->pathFinding->endTileExists)
		App->render->Blit(App->map->data.tilesets.start->next->data->texture, endPosition.x, endPosition.y, new SDL_Rect{ 64, 64, 64, 64 });
	
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;
	
	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void j1Scene::ManageInput(float dt)
{
	if (App->input->GetInputState() == false)
	{
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			App->render->camera.y += (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			App->render->camera.y -= (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
			App->render->camera.x += (int)floor(200.0f * dt);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
			App->render->camera.x -= (int)floor(200.0f * dt);

		//change pahtfinding start tile
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_UP)
		{
			if (App->pathFinding->startTile.x != currentTile_x || App->pathFinding->startTile.y != currentTile_y)
			{
				LOG("Pathfinding: Starting tile updated");
				App->pathFinding->startTile.x = currentTile_x;
				App->pathFinding->startTile.y = currentTile_y;
				App->pathFinding->startTileExists = true;
			}
			else
			{
				if (!App->pathFinding->startTileExists)
					App->pathFinding->startTileExists = true;
				else
					App->pathFinding->startTileExists = false;
			}


		}
		//Change pathfinding end tile
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
		{
			if (App->pathFinding->endTile.x != currentTile_x || App->pathFinding->endTile.y != currentTile_y)
			{
				LOG("Pathfinding: End tile updated");
				App->pathFinding->endTile.x = currentTile_x;
				App->pathFinding->endTile.y = currentTile_y;
				App->pathFinding->endTileExists = true;
			}
			else
			{
				if (!App->pathFinding->endTileExists)
					App->pathFinding->endTileExists = true;
				else
					App->pathFinding->endTileExists = false;
			}
		}



	}
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		if (App->gui->GetFocus() == passwordButton_button || App->gui->GetFocus() == nickButton_button)
		{
			CheckLoginData();
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		if (App->console->isActive())
		{
			App->console->Close();
		}
		else
		{
			App->console->Open();
		}
	}
}

void j1Scene::LoadGUI()
{
	TTF_Font* inputFont = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf", 24);
	TTF_Font* inputTitleFont = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf", 18);
	TTF_Font* hoveringFont = App->font->Load("fonts/open_sans/OpenSans-Regular.tff", 8);
	TTF_Font* errorFont = App->font->Load("fonts/open_sans/OpenSans-Regular.tff", 12);
	

#pragma region LoginWindow
	
	//Window ------------------------
	//Window image
	p2DynArray<SDL_Rect>* wRects = new p2DynArray<SDL_Rect>;
	wRects->PushBack(SDL_Rect{ 0, 512, 483, 512 });
	window_image = App->gui->CreateImage("Window Image", { 0, 0 }, *wRects, NULL, true, this);

	//Window button
	iPoint windowPosition = { 0, 0 };
	window_button = App->gui->CreateButton("Window Button", windowPosition, window_image, NULL, true, this);
	window_button->Center(App->gui->GetScreen());
	//Window title
	
	iPoint wTextPos{ 483/2 - 150/2, 50 };
	window_label = App->gui->CreateText("Window Title", { 0, 50 }, "Window  Title", window_button, true, this, inputFont);
	window_label->Center_x(window_button);
	
	//------------------------------

	//Nickname Input Text ----------
	//Nickname Button image
	p2DynArray<SDL_Rect>* nicknameRects = new p2DynArray<SDL_Rect>;;
	nicknameRects->PushBack(SDL_Rect{ 494, 574, 332, 51 });
	nickButton_image = App->gui->CreateImage("Nickname Image", { 0, 0 }, *nicknameRects, NULL, true, NULL);
	
	//Nickname Button label
	nickButton_label = App->gui->CreateText("Nickname Label", { 10, 8 }, "Nickname", NULL, true, NULL, inputFont, SDL_Color{ 130, 130, 130 });
	
	//Nickname Button
	nickButton_button = App->gui->CreateInputText("Nickname Button", { 0, 150 }, nickButton_image, nickButton_label, window_button, 10, 10, true, this);
	nickButton_button->Center_x(window_button);
	nickButton_button->maxCharacters = 30;
	
	//Nickname Label
	nickButton_label = App->gui->CreateText("Nickname Title", { 0, 125 }, "Nickname:", window_button, true, NULL, inputTitleFont, SDL_Color{ 255, 255, 255 });
	nickButton_label->Align_x(nickButton_button);
	//-----------------------------
	

	//Password Input Text ---------
	//Password Button Image
	p2DynArray<SDL_Rect>* passwordRects = new p2DynArray<SDL_Rect>;;
	passwordRects->PushBack(SDL_Rect{ 494, 574, 332, 51 });
	passwordButton_image = App->gui->CreateImage("Password Image", { 0, 0 }, *passwordRects, NULL, true, this);
	
	//Password Button Label
	passwordButton_label = App->gui->CreateText("Password Label", { 10, 8 }, "***********", NULL, true, this, inputFont, SDL_Color{ 130, 130, 130 });
	
	//Password Button
	passwordButton_button = App->gui->CreateInputText("Password Button", { 0, 250 }, passwordButton_image, passwordButton_label, window_button, 10, 10, true, this);
	passwordButton_button->Center_x(window_button);
	passwordButton_button->hiddenText = true;
	passwordButton_button->maxCharacters = 14;
	
	//Password Label
	nickButton_label = App->gui->CreateText("Password Title", { 0, 225 }, "Password:", window_button, true, NULL, inputTitleFont, SDL_Color{ 255, 255, 255 });
	nickButton_label->Align_x(passwordButton_button);
	//-----------------------------
	

	//Enter Button ----------------
	//Login Button image
	p2DynArray<SDL_Rect>* iRects = new p2DynArray<SDL_Rect>;;
	iRects->PushBack({ 0, 113, 229, 69 });
	iRects->PushBack({ 411, 169, 229, 69 });
	iRects->PushBack({ 642, 169, 229, 69 });

	login_image = App->gui->CreateImage("Login Image", { 0, 0 }, *iRects, NULL, true, this);
	
	//Login Button
	iPoint buttonPos = { 483 / 2 - 229 / 2, 325 };
	login_button = App->gui->CreateButton("Login Button", buttonPos, login_image, window_button, true, this);
	
	//Login Button text
	login_label = App->gui->CreateText("Login Label", { 83, 13 }, "Login", login_button, true, this, inputFont);
	//-----------------------------
	
	//Conection error Label
	connectionErrorLabel = App->gui->CreateText("Connection error label", { 0, 300}, "Connection rejected. Wrong username or password", window_button, false, NULL, errorFont, SDL_Color{ 255, 0, 0 }, false);
	connectionErrorLabel->Center_x(window_button);
	
	//Register Label
	registerLabel = App->gui->CreateText("Register Label", { 300, 400}, "Register", window_button, true, this, inputTitleFont, SDL_Color{ 255, 255, 255 }, true);
	SDL_Texture* hoveringText = App->font->Print("Service not available yet :(");
	registerLabel->SetHoveringTexture(hoveringText);
	

#pragma endregion LoginWindow 
	// Checking button and scrollbar
	/*
	//Checking button default image
	p2DynArray<SDL_Rect>* defIRects = new 	p2DynArray<SDL_Rect>;
	defIRects->PushBack({ 0, 113, 229, 69 });
	checkingButton_defImage = App->gui->CreateImage("Checking button Default image", { 0, 0 }, *defIRects, NULL, true, this);

	//Checking button checked image
	p2DynArray<SDL_Rect>* checkIRects = new 	p2DynArray<SDL_Rect>;
	checkIRects->PushBack({ 411, 169, 229, 69 });
	checkingButton_checkedImage = App->gui->CreateImage("Checking button Checked image", { 0, 0 }, *checkIRects, NULL, true, this);

	checkingButton = App->gui->CreateCheckingButton("Checking Button", { 100, 100 }, checkingButton_checkedImage, checkingButton_defImage, NULL);
	
	p2DynArray<SDL_Rect>* sRects = new 	p2DynArray<SDL_Rect>;
	sRects->PushBack({ 0, 11, 307, 11 });
	scrollBar_bar = App->gui->CreateImage("ScrollBar Bar", { 0, 0 },  *sRects, NULL, true, this, "", false);

	p2DynArray<SDL_Rect>* tRects = new 	p2DynArray<SDL_Rect>;
	tRects->PushBack({ 805, 318, 26, 15 });
	scrollBar_thumb = App->gui->CreateImage("ScrollBar Thumb", { 0, 0 }, *tRects, NULL, true, this, "", false);
	
	scrollBar = App->gui->CreateScrollBar("Scroll Bar", { 0, 200 }, scrollBar_bar, scrollBar_thumb, window_button, 5, 5, -1, true, this);
	scrollBar->Center_x(window_button);
	
	scrollBar_value = App->gui->CreateText("ScrollBar Value", { 0, 100 }, "Value: ", window_button, true, this, inputFont);
	scrollBar_value->Center_x(window_button);
	*/
}




void j1Scene::OnGUI(UI_Event _event, UIElement* _element)
{
	
	if (_element == login_button)
	{
		//Dragging object testing
		/*
		if (_event == MOUSE_DOWN)
		{
			elementHold = login_button;
		}
		if (_event == MOUSE_UP)
		{
			if (elementHold == login_button)
				elementHold = NULL;
		}
		*/
		if (_event == MOUSE_UP)
		{
			CheckLoginData();
		}
	}
	
	if (_element == window_button)
	{
		if (_event == MOUSE_DOWN)
		{
			elementHold = window_button;
		}
		if (_event == MOUSE_UP)
		{
			if(elementHold == window_button)
				elementHold = NULL;
		}
	}

	if (_element == registerLabel)
	{
		if (_event == MOUSE_UP)
		{

		}
	}
	
	if (_element == scrollBar)
	{
		if (_event == SCROLL_CHANGE)
		{
			UpdateValueText(scrollBar->GetValue());
		}
	}
}

bool j1Scene::CheckLoginData()
{

	bool ret = false;
	
	pugi::xml_node config = App->GetConfig("user_data");
	p2SString nick = nickButton_button->GetString();
	p2SString pass = passwordButton_button->GetString();

	pugi::xml_node node;

	for (node = config.child("user"); node; node = node.next_sibling("user"))
	{
		p2SString nickStr = (char*)node.attribute("nickname").as_string();
		p2SString passStr = (char*)node.attribute("password").as_string();
		if (nick == nickStr && pass == passStr)
		{
			ret = true;
		}
	}
	if (ret)
	{
		LoadWelcomeText();
		window_button->Deactivate();
		LOG("Conection Made!! :)");
	}
	else
	{
		connectionErrorLabel->active = true;
		LOG("Could not connect to the server! :(");
	}
	
	return ret;
	
}

void j1Scene::RegisterNewUser()
{
	//Next step:  Create a new file similar to "save_game" in
	//which we store all user accounts data
	/*
	pugi::xml_node config = App->GetConfig("user_data");
	p2SString nick = nickButton_button->GetString();
	p2SString pass = passwordButton_button->GetString();

	pugi::xml_node node = config.append_child("uuuser");
	node.append_attribute("nickname") = nick.GetString();
	node.append_attribute("password") = pass.GetString();
	*/
}

void j1Scene::LoadWelcomeText()
{
	
	TTF_Font* font = App->font->Load("fonts/mirage_gothic/mirage_gothic.ttf", 140);
	p2SString userName = nickButton_button->GetString();
	int stringSize = userName.Length();
	p2SString str = new char[stringSize + 8];
	str = "Welcome, ";
	str += nickButton_button->GetString();
	welcomeLabel = App->gui->CreateText("Welcome Title", { 0, 0 }, (char*)str.GetString(), NULL, true, NULL, font, SDL_Color{ 180, 0, 0 });
	welcomeLabel->Center(App->gui->GetScreen());
	
}

void j1Scene::UpdateValueText(float value)
{
	char* valueStr = new char;
	sprintf_s(valueStr, 72, "Value: %0.2f", value);
	scrollBar_value->SetNewTexture(valueStr, scrollBar_value->color, scrollBar_value->font);
	scrollBar_value->Center_x(window_button);
}

//EXERCISE 4
bool j1Scene::SaveDrag(pugi::xml_node node) const
{
	pugi::xml_node tmp;
	for (uint i = 0; i < configLabels.Count(); i++)
	{
		SDL_Rect rect = configLabels[i]->GetWorldRect();
		iPoint pos = { rect.x, rect.y };
		bool drag = configLabels[i]->dragable;

		pugi::xml_node child = node.append_child("ui_label");

		child.append_attribute("pos_x") = rect.x;
		child.append_attribute("pos_y") = rect.y;
		child.append_attribute("drag") = drag;
	}
	return true;
}

bool j1Scene::LoadDrag(pugi::xml_node node)
{
	pugi::xml_node label = node.first_child();
	uint i = 0;

	while (label)
	{
		iPoint pos;
		pos.x = label.attribute("pos_x").as_int();
		pos.y = label.attribute("pos_y").as_int();

		bool drag = label.attribute("drag").as_bool();

		if (i < configLabels.Count())
		{
			configLabels[i]->SetGlobalPosition(pos.x, pos.y);
			configLabels[i]->dragable = drag;
		}
	
		i++;
		label = label.next_sibling();
	}

	return true;
}
