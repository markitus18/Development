#ifndef __j1CONSOLE_H__
#define __j1CONSOLE_H__

#include "j1Module.h"
#include "j1Gui.h"
#include "p2DynArray.h"
#include "j1App.h"
#include "UIElements.h"
#include "j1Fonts.h"

#define LINE_SPACING 16

enum CVarTypes
{
	c_float = 0,
	c_int,
	c_string,
	c_bool,
};

struct Command
{
public:
	Command(){}
	Command(char* str, char* dsc, uint n){ command = str; desc = dsc, nArgs = n;}

public:
	p2SString desc;
	p2SString command;
	uint nArgs;
	virtual void function(const p2DynArray<p2SString>* arg){ LOG("Executing command function"); }

};

class CVar
{
public:
	CVar(const char* newName, float* newRference, bool newSerialize = false);
	CVar(const char* newName, int* newReference, bool newSerialize = false);
	CVar(const char* newName, char* newReference, bool newSerialize = false);
	CVar(const char* newName, bool* newReference, bool newSerialize = false);

public:
	bool serialize;

private:
	p2SString desc;
	p2SString name;
	CVarTypes type;
	Command* command = NULL;
	j1Module* listener = NULL;

	union {
		float* floatRef;
		int* intRef;
		char* stringRef;
		bool* boolRef;
	} reference;

	union {
		float floatVar;
		int intVar;
		char* stringVar;
		bool boolVar;
	} value;

public:
	void LinkCommand(Command* toLink);

	void Set(float newValue);
	void Set(int newValue);
	void Set(char* newValue);
	void Set(bool newValue);
	void Set(p2SString* data);

	void SetListener(j1Module* module);

	CVarTypes GetType();
	p2SString GetName();
	const j1Module* GetListener() const;

	void Read(void* ret, CVarTypes expectedData);
	bool Read(float* output);
	bool Read(int* output);
	bool Read(char* output);
	bool Read(bool* output);
	void* ForceRead();

	void Display();
};


class j1Console : public j1Module
{
public:

	j1Console();

	// Destructor
	virtual ~j1Console();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called every frame
	bool Update(float dt);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void OnGUI(UI_Event _event, UIElement* _element);

	void AddCommand(Command*);
	uint AddCVar(const char* _name, float* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, int* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, char* reference, j1Module* listener = NULL, bool serialize = false);
	uint AddCVar(const char* _name, bool* reference, j1Module* listener = NULL, bool serialize = false);

	void GetNewInput(char* src);


	void Output(char* str);

	void Open();
	void Close();
	void Clear();

	void DisplayCommands() const;

	bool isActive() const;

	bool SaveCVars(pugi::xml_node& data) const;
	bool LoadCVars(pugi::xml_node&);

private:
	void CutString(char* src, p2DynArray<p2SString>* dst);
	Command* FindCommand(const char* str, uint nArgs) const;
	CVar* FindCVar(const char* str);
	void SetCVar(const char* value);

private:
	p2List<Command*> commandList;
	p2List<CVar*> CVarList;

	UIInputText* inputText;
	UIRect* consoleRect;

	UIRect* inputRect;
	UILabel* console_defLabel;

	p2DynArray<UILabel*> output;

	bool active = true;
	bool dragText = false;

	int textStart = 0;
	int outputHeight = 0;

	bool closeGame = false;



	struct Command_commandList : public Command
	{
		Command_commandList() : Command("list", "Display command list", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			App->console->DisplayCommands();
		}
	};

	Command_commandList command_commandList;

	struct Command_closeConsole : public Command
	{
		Command_closeConsole() : Command("close", "Close console", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			App->console->Close();
		}
	};

	Command_closeConsole command_closeConsole;

	struct Command_clearConsole : public Command
	{
		Command_clearConsole() : Command("cls", "Clear console output", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			App->console->Clear();
		}
	};

	Command_clearConsole command_clearConsole;

	struct QuitCommand : public Command
	{
		QuitCommand() : Command("quit", "Quit the application", 0){}
		void function(const p2DynArray<p2SString>* arg)
		{
			App->console->closeGame = true;
		}
	};
	QuitCommand command_quitCommand;
};

#endif // __j1CONSOLE_H__