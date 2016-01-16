#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Console.h"

struct SDL_Texture;

class UIImage;
class UILabel;
class UIButton;
class UIInputText;
class UICheckingButton;
class UIScrollBar;

class j1Scene : public j1Module
{

public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ManageInput(float dt);

	void LoadGUI();
	void OnGUI(UI_Event _event, UIElement* _element);

	bool CheckLoginData();
	void RegisterNewUser();
	void LoadWelcomeText();


	void UpdateValueText(float value);

	//EXERCICE 4
	bool SaveDrag(pugi::xml_node) const;
	bool LoadDrag(pugi::xml_node);

public:
	int currentTile_x;
	int currentTile_y;

	bool startTileExists;
	bool endTileExists;
	iPoint startTile;
	iPoint endTile;

	bool renderInstructions = false;

private:

	bool logStart;

	SDL_Texture* debug_tex = NULL;
	UIElement* elementHold = NULL;

	//GUI Elements and management
	
	UIImage* login_image = NULL;
	UILabel* login_label = NULL;
	UIButton* login_button = NULL;

	UIImage* window_image = NULL;
	UILabel* window_label = NULL;
	UIButton* window_button = NULL;

	UILabel* nickLabel = NULL;
	UIImage* nickButton_image = NULL;
	UILabel* nickButton_label = NULL;
	UIInputText* nickButton_button = NULL;

	UILabel* passwordLabel = NULL;
	UIImage* passwordButton_image = NULL;
	UILabel* passwordButton_label = NULL;
	UIInputText* passwordButton_button = NULL;

	UILabel* connectionErrorLabel = NULL;

	UILabel* registerLabel = NULL;
	bool nick_button_clicked = false;
	
	UIImage* checkingButton_defImage;
	UIImage* checkingButton_checkedImage;
	UICheckingButton* checkingButton;
	
	UILabel* welcomeLabel = NULL;
	
	UIImage* scrollBar_bar;
	UIImage* scrollBar_thumb;
	UIScrollBar* scrollBar;

	UILabel* scrollBar_value;
	
	p2DynArray<UILabel*> configLabels;

#pragma region Commands
	struct CloseGUI : public Command
	{
		CloseGUI() : Command("close_gui", "Close GUI window", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			if (App->scene->window_button->active)
				App->scene->window_button->Deactivate();
				
		}
	};
	CloseGUI command_closeGUI;

	struct OpenGUI : public Command
	{
		OpenGUI() : Command("open_gui", "Open GUI window", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			if (!App->scene->window_button->active)
			{
				App->scene->window_button->Activate();
				App->scene->connectionErrorLabel->Deactivate();
			}

		}
	};
	OpenGUI command_openGUI;

	//EXERCISE 4
	struct MoveLabels : public Command
	{
		MoveLabels() : Command("move_labels", "Move labels from scene:", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			if (App->scene->configLabels.Count() > 0)
			{
				App->scene->configLabels[0]->SetGlobalPosition(250, 370);
			}
			if (App->scene->configLabels.Count() > 1)
			{
				App->scene->configLabels[1]->SetGlobalPosition(450, 50);
			}
		}
	};
	MoveLabels move_labels;
	// EXERCICE 5

	struct SaveLabels : public Command
	{
		SaveLabels() : Command("save_labels", "Save labels from scene:", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			App->SaveGUI();
		}
	};
	SaveLabels save_labels;

	struct LoadLabels : public Command
	{
		LoadLabels() : Command("load_labels", "Save labels from scene:", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			App->LoadGUI();
		}
	};
	LoadLabels load_labels;
#pragma endregion
};

#endif // __j1SCENE_H__