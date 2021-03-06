#include "j1App.h"
#include "j1Textures.h"
#include "p2Log.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "UIElements.h"



UIElement::UIElement()
{}
UIElement::~UIElement()
{}

void UIElement::OnMouseEnter(){}
void UIElement::OnMouseExit(){}
void UIElement::OnMouseDown(){}
void UIElement::OnMouseUp(){}
void UIElement::OnGetFocus(){}
void UIElement::OnLooseFocus(){}

void UIElement::Deactivate()
{
	for (uint i = 0; i < childs.Count(); i++)
	{
		childs[i]->Deactivate();
	}
	if (this == App->gui->GetFocus())
	{
		App->gui->LooseFocus();
	}
	active = false;
}

void UIElement::Activate()
{
	for (uint i = 0; i < childs.Count(); i++)
	{
		childs[i]->Activate();
	}

	active = true;
}

void UIElement::CheckInput()
{
	if (interactive)
	{
		int mouseX, mouseY;
		App->input->GetMousePosition(mouseX, mouseY);
		int elementX = GetWorldRect().x;
		int elementY = GetWorldRect().y;

		if (App->gui->debugMode)
		{
			int colX = GetWorldRect().x;
			int colY = GetWorldRect().y;
			App->render->DrawQuad(SDL_Rect{ colX - App->render->camera.x, colY - App->render->camera.y, collider.w, collider.h }, 255, 0, 0, 50, false);
		}
		if (mouseX > elementX && mouseX < elementX + collider.w && mouseY >elementY && mouseY < elementY + collider.h)
		{
			if (!mouseWasIn)
			{
				//LOG("%s - Mouse Enter", name.GetString());
				mouseWasIn = true;
				App->gui->SetHovering(this);
				OnMouseEnter();
				if (listener)
				{
					listener->OnGUI(MOUSE_ENTER, this);
				}

				App->gui->inputRecieved = true;
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				//LOG("%s - Mouse Down", name.GetString());
				mouseWasClicked = true;
				OnMouseDown();
				if (listener)
				{
					listener->OnGUI(MOUSE_DOWN, this);
				}

				App->gui->inputRecieved = true;
				App->gui->SetFocus(this);
			}
			else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			{
				if (mouseWasClicked)
				{
					//LOG("%s - Mouse Up", name.GetString());
					mouseWasClicked = false;
					OnMouseUp();
					if (listener)
					{
						listener->OnGUI(MOUSE_UP, this);
					}
					App->gui->inputRecieved = true;
				}
				else if (App->gui->GetFocus())
				{
					if(App->gui->GetFocus()->mouseWasClicked)
					{
						App->gui->GetFocus()->OnMouseUp();
					}
				}


			}
		}
		else
		{
		//	if (mouseWasClicked)
		//		mouseWasClicked = false;
			if (mouseWasIn)
			{
				//LOG("%s - Mouse Exit", name.GetString());
				mouseWasIn = false;
				OnMouseExit();
				App->gui->SetHovering(NULL);
				if (listener)
				{
					listener->OnGUI(MOUSE_EXIT, this);
				}

				App->gui->inputRecieved = true;
			}
		}
		if (App->gui->GetFocus() == this)
		{
			if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
			{
				//LOG("%s - Return down", name.GetString());
				if (listener)
				{
					listener->OnGUI(RETURN_DOWN, this);
				}
			}
		}
	}
}

bool UIElement::Update(float dt)
{
	return true;
}

void UIElement::SetParent(UIElement* newParent)
{
	parent = newParent;
	if (newParent)
		parent->childs.PushBack(this);
}

void UIElement::SetHoveringTexture(SDL_Texture* texture)
{
	hoveringTexture = texture;
}
 
void UIElement::SetLayer(int lay)
{
	layer = lay;
}

void UIElement::SetIgnoreCamera()
{
	for (uint i = 0; i < childs.Count(); i++)
	{
		childs[i]->ignoreCamera = true;
	}
	ignoreCamera = true;
}

UIImage::UIImage()
{}

UIImage::~UIImage()
{}
UIImage::UIImage(char* newName, iPoint newPosition, const SDL_Texture* newTexture, p2DynArray<SDL_Rect>& newRects)
{
	name = newName;
	SetCollider(newPosition.x, newPosition.y, newRects[0].w, newRects[0].h);
	for (int i = 0; i < (int)newRects.Count(); i++)
		rects.PushBack(newRects[i]);
	
	texture = newTexture;
}

bool UIImage::Update(float dt)
{
	SDL_Rect rect = GetWorldRect();
	int x = rect.x;
	int y = rect.y;
	if (!ignoreCamera)
	{
		x -= App->render->camera.x;
		y -= App->render->camera.y;
	}
	App->render->Blit(texture, x, y, &rects[currentRect]);
	return true;
}

UILabel::UILabel()
{}
UILabel::UILabel(char* newName, iPoint newPosition, char* newText)
{
	name = newName;
	SetLocalPosition(newPosition.x, newPosition.y);
	text = newText;
}
UILabel::~UILabel()
{}

bool UILabel::Update(float dt)
{
	App->render->Blit(texture, GetWorldRect().x - App->render->camera.x, GetWorldRect().y - App->render->camera.y);
	if (drawLine)
	{
		int x1 = GetWorldRect().x - App->render->camera.x;
		int y1 = GetWorldRect().y - App->render->camera.y + GetWorldRect().h;
		int x2 = x1 + GetWorldRect().w;
		int y2 = y1;

		App->render->DrawLine(x1, y1, x2, y2, 255, 255, 255);
	}
	return true;
}

void UILabel::SetNewTexture(char* newText,  SDL_Color newColor, TTF_Font* newFont)
{
	if (newFont)
		font = newFont;
	color = newColor;
	text = newText;
	texture = App->font->Print(text.GetString(), color, font);

	//Copy the texture collider into collider
	int w, h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	SetCollider(GetLocalPosition().x, GetLocalPosition().y, w, h);
}
void UILabel::OnMouseEnter()
{
	drawLine = true;
}

void UILabel::OnMouseExit()
{
	drawLine = false;
}
UIButton::UIButton()
{}

UIButton::UIButton(char* newName, iPoint position, UIImage* newImage, UILabel* newLabel, UILabel* newLabel2)
{
	name = newName;
	image = newImage;
	SetCollider(position.x, position.y, image->GetWorldRect().w, image->GetWorldRect().h);
	label = newLabel;
	label2 = newLabel2;
}

UIButton::~UIButton()
{}

bool UIButton::Update(float dt)
{
	return true;
}

void UIButton::OnMouseEnter()
{
	if (image && image->rects.Count() == 3)
		image->currentRect = 1;
}

void UIButton::OnMouseExit()
{
	if (image)
		image->currentRect = 0;
}

void UIButton::OnMouseDown()
{
	if (image)
	{
		if (image->rects.Count() == 3)
			image->currentRect = 2;
		else if (image->rects.Count() == 2)
			image->currentRect = 1;
	}
}

void UIButton::OnMouseUp()
{
	if (image)
	{
		if (image->rects.Count() == 3)
		{
			int mouseX, mouseY;
			App->input->GetMousePosition(mouseX, mouseY);

			int x, y;
			UIElement* element = (UIElement*)this;
			x = element->GetWorldRect().x;
			y = element->GetWorldRect().y;

			if (mouseX > x && mouseX < x + GetWorldRect().w && mouseY > y && mouseY < y + GetWorldRect().h)
				image->currentRect = 1;
			else
				image->currentRect = 0;
		}
		else
			image->currentRect = 0;
	}
}

UIInputText::UIInputText()
{}

UIInputText::UIInputText(char* newName, SDL_Rect newRect, UIImage* newImage, UILabel* newDefaultLabel, int newOffsetX, int newOffsetY)
{
	name = newName;

	image = newImage;
	if (image)
		SetCollider(newRect.x, newRect.y, image->GetWorldRect().w, image->GetWorldRect().h);
	else
		SetCollider(newRect);

	defaultText = newDefaultLabel;
	font = defaultText->font;
	offsetX = newOffsetX;
	offsetY = newOffsetY;

}

UIInputText::~UIInputText()
{}

void UIInputText::OnMouseEnter()
{
	App->input->DisableCursorImage();
	App->gui->inputEnabled = true;
}
void UIInputText::OnMouseExit()
{
	App->gui->inputEnabled = false;
	App->input->EnableCursorImage();
}
void UIInputText::OnMouseDown()
{
	if (defaultText)
	{
		if (defaultOn)
		{
			defaultText->active = false;
			defaultOn = false;
			textChanged = true;
		}
	}
}
void UIInputText::OnMouseUp()
{

}
void UIInputText::OnGetFocus()
{
	App->input->FreezeInput();
	SDL_StartTextInput();
}
void UIInputText::OnLooseFocus()
{
	SDL_StopTextInput();
	App->input->UnFreezeInput();
}
void UIInputText::UpdateCursorPosition()
{
	p2List_item<char>* item = textList.start;
	char* str = new char[cursorPosition];
	for (int i = 0; i < cursorPosition && item; i++)
	{
		if (!hiddenText)
		{
			str[i] = item->data;
		}
		else
		{
			str[i] = '*';
		}
		item = item->next;
	}
	str[cursorPosition] = '\0';
	int x = 0, y = 0;
	if (cursorPosition > 0)
	{
		TTF_SizeText(font, str, &x, &y);
	}

	//We set crusorStart position relative to the button position, we will add global position later on
	cursorStart.x = offsetX + x;
	cursorStart.y = offsetY;
	//Starting the process to move the cursor if its "out" of the input box
	/*
	if (cursorStart.x > image->GetWorldRect().x + image->GetWorldRect().w - offsetX)
	{
		textDisplacement = cursorStart.x - (image->GetLocalPosition().x + image->GetWorldRect().w - offsetX);
		cursorStart.x -= textDisplacement;
		textRect.w += textDisplacement;
	}*/
	//if (cursorStart.x == image->GetWorldPosition().x + offsetX)

	cursorNeedUpdate = false;
}

void UIInputText::RenderCursor()
{
	int timeSinceStart = (int)(App->GetTimeSinceStart() * cursorBlinkSpeed);
	int rest = timeSinceStart % 2;
	if (rest == 0)
	{
		if (cursorNeedUpdate)
			UpdateCursorPosition();
		int x1 = cursorStart.x - App->render->camera.x + GetWorldRect().x;
		int x2 = x1;
		int y1 = cursorStart.y - App->render->camera.y + GetWorldRect().y;
		int y2 = y1 + defaultText->GetWorldRect().h;
		App->render->DrawLine(x1, y1, x2, y2, 255, 255, 255);
	}
}
void UIInputText::GetNewInput(char* text)
{
	bool end = false;
	if (textList.count() < (uint)maxCharacters)
	{
		for (uint i = 0; !end && textList.count() <= (uint)maxCharacters; i++)
		{
			if (text[i] == '\0')
				end = true;
			else
			{
				p2List_item<char>* charItem = new p2List_item<char>(text[i]);
				textList.Insert(currentChar, charItem);
				textChanged = true;
				cursorPosition++;
				cursorNeedUpdate = true;
				currentChar = charItem;
			}
		}
	}
}

//Deletes the character in the list at the cursor position
void UIInputText::DeleteCharacterOnCursor()
{
	if (textList.count() > 0)
	{
		if (currentChar)
		{
			cursorPosition--;
			p2List_item<char>* nextCurrent = currentChar->prev;
			textList.del(currentChar);
			currentChar = nextCurrent;
			textChanged = true;
		}
	}
}

void UIInputText::DeleteNextCharacterToCursor()
{
	if (textList.count() > 0)
	{
		if (currentChar)
		{
			textList.del(currentChar->next);;
		}
		else
		{
			textList.del(textList.start);
		}
		textChanged = true;
		cursorNeedUpdate = true;
	}
}

//Update the input text texture based on the character list
void UIInputText::UpdateTextTexture()
{
	if (textList.count() > 0)
	{
		char* str = new char[textList.count()];
		if (!hiddenText)
		{
			//Building the string from the list
			p2List_item<char>* item = textList.start;
			for (uint i = 0; i < textList.count() && item; i++)
			{
				str[i] = item->data;
				item = item->next;
			}
			str[textList.count()] = '\0';
		}
		else
		{
			for (uint i = 0; i < textList.count(); i++)
			{
				str[i] = '*';
			}
			str[textList.count()] = '\0';
		}
		//Printing the string into the texture
		text_texture = App->font->Print(str, SDL_Color{ 255, 255, 255 }, font);
		SDL_QueryTexture(text_texture, NULL, NULL, &textRect.w, &textRect.h);

		//Starting the process to move the text if its larger that the box
		/*
		if (textRect.w > image->GetWorldRect().w - (2 * offsetX))
		{
			textDisplacement = textRect.w - (image->GetWorldRect().w - (2 * offsetX));
			textRect.w = image->GetWorldRect().w - (2 * offsetX);
			textRect.x += textDisplacement;
		}
		*/
	}
	else
		text_texture = NULL;
}
bool UIInputText::Update(float dt)
{
	if (textChanged)
	{
		UpdateTextTexture();
		textChanged = false;
	}

	if (App->gui->GetFocus() == this)
	{
		//Deactivate default text
		if (defaultText)
		{
			if (defaultText->active = true)
			{
				defaultText->active = false;
				defaultOn = false;
			}
		}

		ManageInput();
		RenderCursor();
	}

	if (!defaultOn)
	{
		//Rendering input text
		int x, y;
		x = GetWorldRect().x + offsetX - App->render->camera.x - textDisplacement;
		y = GetWorldRect().y + offsetY - App->render->camera.y;
		if (text_texture)
			App->render->Blit(text_texture, x, y, &textRect);
	}

	return true;
}

void UIInputText::ManageInput()
{
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
	{
		if ((uint)cursorPosition < textList.count())
		{
			cursorPosition++;
			cursorNeedUpdate = true;
			if (cursorPosition == 1)
			{
				currentChar = textList.start;
			}
			else
				currentChar = currentChar->next;
		}

	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
	{
		if (cursorPosition > 0)
		{
			cursorPosition--;
			cursorNeedUpdate = true;
			currentChar = currentChar->prev;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_BACKSPACE) == KEY_DOWN)
	{
		DeleteCharacterOnCursor();
		cursorNeedUpdate = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		DeleteNextCharacterToCursor();
	}
}
char* UIInputText::GetString() const
{
	if (textList.count() == 0)
	{
		LOG("%s has no string", name.GetString());
		return NULL;
	}

	else
	{
		char* str = new char[textList.count()];
		p2List_item<char>* item = textList.start;
		for (int i = 0; item; i++)
		{
			str[i] = item->data;
			item = item->next;
		}

		str[textList.count()] = '\0';
		return str;
	}

}

void UIInputText::DeleteText()
{
	textList.clear();
	UpdateTextTexture();
	cursorPosition = 0;
	UpdateCursorPosition();
}
//Checking Button
UICheckingButton::UICheckingButton()
{}
UICheckingButton::UICheckingButton(char* newName, iPoint newPosition, UIImage* newImage1, UIImage* newImage2)
{
	name = newName;
	SetCollider(newPosition.x, newPosition.y, newImage1->GetWorldRect().w, newImage1->GetWorldRect().h);
	defImage = newImage1;
	checkedImage = newImage2;
}
UICheckingButton::~UICheckingButton()
{}
void UICheckingButton::OnMouseDown()
{}
void UICheckingButton::OnMouseUp()
{
	checked = !checked;
	if (checked)
	{
		defImage->Deactivate();
		checkedImage->Activate();
	}
	else
	{
		checkedImage->Deactivate();
		defImage->Activate();
	}
}

//Scroll Bar
UIScrollBar::UIScrollBar()
{}
UIScrollBar::UIScrollBar(char* newName, iPoint newPosition, UIElement* newBar, UIElement* newThumb)
{
	name = newName;
	bar = newBar;
	thumb = newThumb;
	SetCollider(newPosition.x, newPosition.y, newBar->GetWorldRect().w, newBar->GetWorldRect().h);
}
UIScrollBar::~UIScrollBar()
{}

bool UIScrollBar::Update(float dt)
{
	int x = 0, y = 0;
	SDL_Rect thumbRect = thumb->GetWorldRect();
	iPoint thumbPos = thumb->GetLocalPosition();
	SDL_Rect rect = bar->GetWorldRect();
	moved = false;
	if (thumbClicked)
	{
		CheckThumbMovement(x, y);
	}
	else if (this == App->gui->GetFocus())
	{
		CheckInputMovement(x, y, dt);
	}
	if (barClicked)
	{
		CheckBarMovement(x, y, dt);
	}
	if (moved)
	{
		if (type == HORIZONTAL)
		{
			if (thumbPos.x + x + thumbRect.w > rect.w - offsetR)
				thumbPos.x = rect.w - offsetR - x - thumbRect.w;
			if (thumbPos.x + x < offsetL)
				thumbPos.x = offsetL - x;
			thumb->SetLocalPosition(thumbPos.x + x, thumbPos.y);
		}
		else if (type == VERTICAL)
		{
			if (thumbPos.y + y + thumbRect.h > rect.h - offsetD)
				thumbPos.y = rect.h - offsetR - y - thumbRect.h;
			if (thumbPos.y + y < offsetU)
				thumbPos.y = offsetU - y;
			thumb->SetLocalPosition(thumbPos.x, thumbPos.y + y);
		}
		listener->OnGUI(SCROLL_CHANGE, this);
	}
	return true;
}
void UIScrollBar::OnMouseDown()
{
	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);
	SDL_Rect barRect = GetWorldRect();
	SDL_Rect thumbRect = thumb->GetWorldRect();

	if (mouseX > thumbRect.x && mouseX < thumbRect.x + thumbRect.w && mouseY >thumbRect.y && mouseY < thumbRect.y + thumbRect.h)
	{
		thumbClicked = true;
		if (type == HORIZONTAL)
			thumbClickOffset = mouseX - thumbRect.x;
		else if (type == VERTICAL)
			thumbClickOffset = mouseY - thumbRect.y;
	}
	else
	{
		if (type == HORIZONTAL)
			if (mouseX > barRect.x + offsetL + thumbRect.w / 2 && mouseX < barRect.x + barRect.w - offsetR - thumbRect.w / 2)
				barClicked = true;
		if (type == VERTICAL)
			if (mouseY > barRect.y + offsetU + thumbRect.h / 2 && mouseY < barRect.y + barRect.h - offsetD - thumbRect.h / 2)
				barClicked = true;
	}


}

void UIScrollBar::OnMouseUp()
{
	thumbClicked = false;
	barClicked = false;
}

void UIScrollBar::OnMouseExit()
{
	barClicked = false;
}

void UIScrollBar::OnGetFocus()
{
	App->input->FreezeInput();
}
void UIScrollBar::OnLooseFocus()
{
	App->input->UnFreezeInput();
}

void UIScrollBar::CheckThumbMovement(int& x, int& y)
{
	SDL_Rect thumbRect = thumb->GetWorldRect();
	iPoint thumbPos = thumb->GetLocalPosition();
	SDL_Rect rect = bar->GetWorldRect();
	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);

	if (type == HORIZONTAL)
	{
		if (mouseX - thumbClickOffset >= rect.x + offsetL && mouseX - thumbClickOffset <= rect.x + rect.w - offsetR - thumbRect.w)
		{
			x = mouseX - thumbPos.x - thumbClickOffset;
			moved = true;
		}
	}
	else if (type == VERTICAL)
	{
		if (mouseY - thumbClickOffset >= rect.y + offsetU && mouseY - thumbClickOffset <= rect.y + rect.h - offsetD - thumbRect.h)
		{
			y = mouseY - thumbPos.y - thumbClickOffset;
			moved = true;
		}
	}
}

void UIScrollBar::CheckInputMovement(int& x, int& y, float dt)
{
	SDL_Rect thumbRect = thumb->GetWorldRect();
	iPoint thumbPos = thumb->GetLocalPosition();
	SDL_Rect rect = bar->GetWorldRect();

	if (type == HORIZONTAL)
	{
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (thumbPos.x > offsetL)
			{
				x = (int)(-arrowsValue * dt * 100);
				moved = true;
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (thumbPos.x + thumbRect.w < rect.w - offsetR)
			{
				x = (int)(arrowsValue * dt * 100);
				moved = true;
			}
		}
	}
	else if (type == VERTICAL)
	{
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			if (thumbPos.y + thumbRect.h < rect.h - offsetD)
			{
				y = (int)(arrowsValue * dt * 100);
				moved = true;
			}
		}
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			if (thumbPos.y > offsetU)
			{
				y = (int)(-arrowsValue * dt * 100);
				moved = true;
			}
		}
	}
}

void UIScrollBar::CheckBarMovement(int& x, int& y, float dt)
{
	int mouseX, mouseY;
	App->input->GetMousePosition(mouseX, mouseY);
	SDL_Rect thumbRect = thumb->GetWorldRect();
	iPoint thumbPos = thumb->GetLocalPosition();
	SDL_Rect rect = bar->GetWorldRect();


	if (type == HORIZONTAL)
	{
		if (mouseX > thumbRect.x + thumbRect.w / 2)
		{
			if (thumbPos.x + thumbRect.w < rect.w - offsetR)
			{
				x = (int)(barValue * dt * 100);
				moved = true;
			}
		}
		if (mouseX < thumbRect.x + thumbRect.w / 2)
		{
			if (thumbPos.x > offsetL)
			{
				x = (int)(-barValue * dt * 100);
				moved = true;
			}
		}
	}
	else if (type == VERTICAL)
	{
		if (mouseY > thumbRect.y + thumbRect.h / 2)
		{
			if (thumbPos.y + thumbRect.h < rect.h - offsetD)
			{
				y = (int)(barValue * dt * 100);
				moved = true;
			}
		}
		if (mouseY < thumbRect.y + thumbRect.h / 2)
		{
			if (thumbPos.y > offsetU)
			{
				y = (int)(-barValue * dt * 100);
				moved = true;
			}
		}
	}
}

void UIScrollBar::SetValue(float value)
{
	if (value > 1)
		value = 1;
	SDL_Rect thumbRect = thumb->GetWorldRect();
	SDL_Rect barRect = bar->GetWorldRect();

	int minPos, maxPos, thumbPos;
	if (type == VERTICAL)
	{
		minPos = offsetU;
		maxPos = barRect.h - offsetD - thumbRect.h;
	}
	else if (type == HORIZONTAL)
	{
		minPos = offsetL;
		maxPos = barRect.x + barRect.w - offsetR - thumbRect.w;
	}
	int totalLenght = maxPos - minPos;
	thumbPos = (int)(value * totalLenght + minPos);

	if (type == VERTICAL)
		thumb->SetLocalPosition(thumb->GetLocalPosition().x, thumbPos);
	else if (type == HORIZONTAL)
		thumb->SetLocalPosition(thumbPos, thumb->GetLocalPosition().y);
}

float UIScrollBar::GetValue()
{
	SDL_Rect thumbRect = thumb->GetWorldRect();
	SDL_Rect barRect = bar->GetWorldRect();

	int minPos, maxPos, thumbPos;
	if (type == VERTICAL)
	{
		minPos = barRect.y + offsetU;
		maxPos = barRect.y + barRect.h - offsetD - thumbRect.h;
		thumbPos = thumbRect.y - offsetU;
	}
	else if (type == HORIZONTAL)
	{
		minPos = barRect.x + offsetL;
		maxPos = barRect.x + barRect.w - offsetR - thumbRect.w;
		thumbPos = thumbRect.x - offsetL;
	}
	int totalLenght = maxPos - minPos;
	float ret = (float)(thumbPos / totalLenght);

	return ret;
}

UIRect::UIRect(char* newName, SDL_Rect newRect, int newR, int newG, int newB, int newA, bool newFilled)
{
	name = newName;
	SetCollider(newRect);
	r = newR;
	g = newG;
	b = newB;
	a = newA;
	filled = newFilled;
}

bool UIRect::Update(float dt)
{
	App->render->DrawQuad(GetWorldRect(), r, g, b, a, filled, ignoreCamera);
	return true;
}

UIBar::UIBar(){ name = "Startard bar"; };

UIBar::UIBar(char* newName, UIElement* bgImage, UIElement* filledimg, int* maxV, int* currV)
{
	name = newName;
	background = bgImage;
	fillImage = filledimg;
	maxValue = maxV;
	currValue = currV;
	maxW = filledimg->GetWorldRect().w;
}

bool UIBar::Update(float dt)
{
	SDL_Rect rect = fillImage->GetWorldRect();
	iPoint pos = fillImage->GetLocalPosition();

	float value = (float)*currValue / (float)*maxValue;
	CAP(value, 0, 1.0f);
	fillImage->SetCollider(pos.x, pos.y, (int)(value * maxW), rect.h);
	rect = fillImage->GetWorldRect();
	return true;
}