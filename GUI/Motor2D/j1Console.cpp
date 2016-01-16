#include "p2Defs.h"
#include "p2Log.h"

#include "j1FileSystem.h"
#include "j1Console.h"
#include "j1Render.h"
#include "j1Input.h"
 
#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

j1Console::j1Console() : j1Module()
{
	name.create("console");
}

// Destructor
j1Console::~j1Console()
{}

// Called before render is available
bool j1Console::Awake(pugi::xml_node& config)
{
	bool ret = true;

	AddCommand(&command_commandList);
	AddCommand(&command_closeConsole);
	AddCommand(&command_clearConsole);
	AddCommand(&command_quitCommand);

	return ret;
}

// Called before the first frame
bool j1Console::Start()
{
	TTF_Font* inputFont = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf", 16);

	//Console rect
	consoleRect = App->gui->CreateRect("Console rect", { 0, 0, App->render->camera.w, 250 }, 0, 0, 0, 200);
	consoleRect->SetLayer(GUI_MAX_LAYERS);
	consoleRect->interactive = true;
	consoleRect->listener = this;
	
	//Console input rect
	inputRect = App->gui->CreateRect("Console Input rect", { 0, 250, App->render->camera.w, 40 }, 130, 130, 130);
	inputRect->SetLayer(GUI_MAX_LAYERS);

	//Console input label
	UILabel* console_defLabel;
	console_defLabel = App->gui->CreateText("Console default label", { 10, 8 }, "Command", NULL, true, NULL, inputFont, SDL_Color{ 0, 0, 0 });
	console_defLabel->SetLayer(GUI_MAX_LAYERS);

	//Console input
	inputText = App->gui->CreateInputText("Console Input", { 0, 250, App->render->camera.w, 40}, NULL, console_defLabel, App->gui->GetScreen(), 10, 10, true, this);
	inputText->Center_x(App->gui->GetScreen());
	inputText->maxCharacters = 67;
	inputText->SetLayer(GUI_MAX_LAYERS);


	return true;
	
}

bool j1Console::PostUpdate()
{
	if (closeGame)
		return false;
	return true;

}
bool j1Console::Update(float dt)
{
	if (dragText)
	{
		int x, y;
		App->input->GetMouseMotion(x, y);
		if ((y > 0 && textStart + y <=0) || (y < 0 && textStart + outputHeight + y > 250 - 15))
		{
			textStart += y;
			for (uint n = 0; n < output.Count(); n++)
			{
				iPoint pos = output[n]->GetLocalPosition();
				output[n]->SetLocalPosition(pos.x, pos.y + y);
			}
		}

	}
	return true;
}
// Called before quitting
bool j1Console::CleanUp()
{
	return true;
}

void j1Console::OnGUI(UI_Event _event, UIElement* _element)
{
	if (_element == inputText)
	{
		if (_event == RETURN_DOWN)
		{
			UIInputText* input = (UIInputText*)_element;
			GetNewInput(input->GetString());
			input->DeleteText();
		}
	}
	if (_element == consoleRect)
	{
		if (_event == MOUSE_DOWN)
		{
			dragText = true;
		}
		if (_event == MOUSE_UP || _event == MOUSE_EXIT)
		{
			dragText = false;
		}
	}
}
void j1Console::AddCommand(Command* command)
{
	commandList.add(command);
}

uint j1Console::AddCVar(const char* newName, float* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint j1Console::AddCVar(const char* newName, int* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint j1Console::AddCVar(const char* newName, char* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}
uint j1Console::AddCVar(const char* newName, bool* reference, j1Module* listener, bool serialize)
{
	CVar* newCVar = new CVar(newName, reference, serialize);
	newCVar->SetListener(listener);
	CVarList.add(newCVar);
	return CVarList.count() - 1;
}

void j1Console::GetNewInput(char* src)
{
	p2DynArray<p2SString> input;
	CutString(src, &input);

	Command* command = FindCommand(input[0].GetString(), input.Count() - 1);
	if (command)
	{
		command->function(&input);
	}
	else
	{
		CVar* cVar = FindCVar(input.At(0)->GetString());
		if (cVar)
		{
			if (input.Count() == 1)
			{
				cVar->Display();
			}
			else if (input.Count() == 2 || input.Count() == 3)
			{
				if (input.Count() == 3)
				{
					cVar->serialize = std::stoi(input.At(2)->GetString());
				}
				p2SString* str = input.At(1);
				cVar->Set(str);
			}
			else
			{
				LOG("Sent more than 1 argument for a variable");
			}
		}
		else
		{
			LOG("'%s' is not an intern command", input.At(0)->GetString());
		}
	}
	input.Clear();
}

void j1Console::CutString(char* src, p2DynArray<p2SString>* dst)
{
	p2SString str(src);
	p2SString tmp;

	bool onComas = false;

	for (uint i = 0; i < str.Length(); i++)
	{
		if (!onComas && str.GetString()[i] == ' ')
		{
			tmp = str;
			tmp.Cut(i);
			dst->PushBack(tmp);
			str.Cut(0, i);
			i = 0;
		}

		if (str.GetString()[i] == '"')
		{
			onComas = !onComas;

			if (onComas)
			{
				str.Cut(0, 0);
				i--;
			}
			else
			{
				str.Cut(i, i);
				i--;
			}
		}
	}
	dst->PushBack(str);

}

void j1Console::Output(char* str)
{
	int y = output.Count() * LINE_SPACING;

	UILabel* newOutput = App->gui->CreateText("outputLine", { 10, textStart + y }, str, consoleRect, active, NULL, NULL);
	outputHeight = 10 + y;
	newOutput->layer = 1;
	output.PushBack(newOutput);

	int offset = (250 - 15) - (textStart + y + LINE_SPACING);
	if (offset < 0)
	{
		textStart += offset;
		for (uint n = 0; n < output.Count(); n++)
		{
			iPoint pos = output[n]->GetLocalPosition();
			output[n]->SetLocalPosition(pos.x, pos.y + offset);
		}
	}
}
// Find a command by  a string
Command* j1Console::FindCommand(const char* str, uint nArgs) const
{
	p2List_item<Command*>* item;
	Command* ret = NULL;
	for (item = commandList.start; item && !ret; item = item->next)
	{
		if (item->data->command == str)
		{
			ret = item->data;
		}
	}
	if (ret)
	{
		if (nArgs != ret->nArgs)
		{
			LOG("Command '%s' should recieve %i arguments, sent %i.", str, ret->nArgs, nArgs);
			ret = NULL;
		}
	}

	return ret;

}

CVar* j1Console::FindCVar(const char* str)
{
	CVar* ret = NULL;
	//Looking for the command to execute
	p2List_item<CVar*>* Vars = CVarList.start;
	while (Vars)
	{
		if (Vars->data->GetName() == str)
		{
			ret = Vars->data;
			break;
		}
		Vars = Vars->next;
	}
	return ret;
}

void j1Console::Open()
{
	consoleRect->Activate();
	inputRect->Activate();
	inputText->Activate();

	active = true;
}

void j1Console::Close()
{
	inputText->DeleteText();
	consoleRect->Deactivate();
	inputRect->Deactivate();
	inputText->Deactivate();

	active = false;
}

void j1Console::Clear()
{
	for (uint i = 0; i < output.Count(); i++)
	{
		output[i]->active = false;
		//We should erase those texts, not just deactivate them
	}
	textStart = 0;
	output.Clear();
}

void j1Console::DisplayCommands() const
{
	p2List_item<Command*>* item;
	p2SString str;
	LOG("   ");
	LOG("Command List:");
	for (item = commandList.start; item; item = item->next)
	{
		str.Clear();
		str += "  ";
		str += item->data->command.GetString();
		str += " -- ";
		str += item->data->desc.GetString();
		LOG("%s", str.GetString());
	}
	LOG(" ");
	p2List_item<CVar*>* citem;
	LOG("Variables List:");
	for (citem = CVarList.start; citem; citem = citem->next)
	{
		str.Clear();
		str += "  ";
		if (citem->data->GetListener())
			str += citem->data->GetListener()->name.GetString();
		str += " -- ";
		str += citem->data->GetName().GetString();

		LOG("%s", str.GetString());
	}
}

bool j1Console::isActive() const
{
	return active;
}


bool j1Console::SaveCVars(pugi::xml_node& Vars) const
{
	pugi::xml_node tmp;
	p2List_item<CVar*>* item = CVarList.start;

	while (item)
	{
		switch (item->data->GetType())
		{
		case c_float:
		{
			float toAppend = 0.0f;
			item->data->Read(&toAppend);
			Vars.append_child(item->data->GetName().GetString()).append_attribute("value") = toAppend;
			break;
		}
		case c_int:
		{
			int toAppend = 0;
			item->data->Read(&toAppend);
			Vars.append_child(item->data->GetName().GetString()).append_attribute("value") = toAppend;
			break;
		}
		case c_string:
		{
			char* toAppend = "";
			item->data->Read(toAppend);
			Vars.append_child(item->data->GetName().GetString()).append_attribute("value") = toAppend;
			break;
		}
		case c_bool:
		{
			bool toAppend = true;
			item->data->Read(&toAppend);
			Vars.append_child(item->data->GetName().GetString()).append_attribute("value") = toAppend;
			break;
		}
		}
		item = item->next;
	}

	return true;
}
bool j1Console::LoadCVars(pugi::xml_node& conf)
{
	pugi::xml_node vars = conf.first_child();
	while (vars)
	{
		char* name = (char*)vars.name();
		p2SString toSend(name);
		CVar* cVarToSet = FindCVar(toSend.GetString());
		if (cVarToSet)
		{
			char* name2 = (char*)vars.attribute("value").as_string();
			p2SString toSend2(name2);
			cVarToSet->Set(&toSend2);
		}

		vars = vars.next_sibling();
	}
	return true;
}