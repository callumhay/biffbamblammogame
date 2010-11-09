#ifndef __GAMECONTROL_H__
#define __GAMECONTROL_H__

class GameControl {
public:
	enum ActionButton { UpButtonAction = 0, DownButtonAction = 1, LeftButtonAction = 2, 
											RightButtonAction = 3, EnterButtonAction = 4, EscapeButtonAction = 5,
											PauseButtonAction = 6,
											OtherButtonAction = 1000};
private:
	GameControl(){};
	~GameControl(){};
};

#endif // __GAMECONTROLACTIONBUTTONS_H__