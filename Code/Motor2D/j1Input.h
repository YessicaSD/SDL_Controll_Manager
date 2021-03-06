#ifndef __j1INPUT_H__
#define __j1INPUT_H__

#include <vector>

#include "SDL\include\SDL_gamecontroller.h"
#include "SDL\include\SDL_haptic.h"

#include "SDL\include\SDL.h"
#include "j1Module.h"
#include "p2Point.h"
#include "p2Log.h"


//#define NUM_KEYS 352
#define NUM_MOUSE_BUTTONS 5
#define MAX_CONTROLLERS 4
//#define LAST_KEYS_PRESSED_BUFFER 50

struct SDL_Rect;

enum j1EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum j1KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};
class j1Input;
struct Controller
{
private:
	int index_number = -1;
	SDL_JoystickID joyId = -1;
	j1KeyState key_state[SDL_CONTROLLER_BUTTON_MAX];
	SDL_GameController* ctr_pointer = nullptr;
	
public:
	SDL_Haptic* haptic = nullptr;


	j1KeyState Get_Button_State(SDL_GameControllerButton button)
	{
		return key_state[button];
	}
	//This funtion returns axis and triggers state value
	// The state is a value ranging from -32768 to 32767.
	Sint16 Get_Axis(SDL_GameControllerAxis axis)
	{
		return SDL_GameControllerGetAxis(ctr_pointer, axis);
	}

	int Create_haptic_effect(SDL_HapticEffect effect)
	{
		if ((SDL_HapticQuery(haptic)) == 0) {
			SDL_HapticClose(haptic); // No sine effect
			return -1;
		}

		return SDL_HapticNewEffect(haptic, &effect);
	}

	// type -> choose between this ones: https://wiki.libsdl.org/SDL_HapticPeriodic#type (SDL_HAPTIC_SINE...)
	// direction_type-> choose between this ones: https://wiki.libsdl.org/SDL_HapticDirection#type (SDL_HAPTIC_POLAR...)
	// dir -> depens on direction_type so also see descriptions of each:  https://wiki.libsdl.org/SDL_HapticDirection#type
	// length-> duration of the effect

	/*int Create_haptic_effect(Uint16 type, Uint8 direction_type, Sint32 dir[3], Uint16 period, Uint32 length,)
	{
		SDL_HapticEffect effect;


	}*/
	
	int test_haptic() {

		SDL_HapticEffect effect;
		int effect_id;

		if (haptic == NULL)
		{
			return -1;
		}
		// See if it can do sine waves
		if ((SDL_HapticQuery(haptic) ) == 0) {
			LOG("%s", SDL_GetError());
			SDL_HapticClose(haptic); // No sine effect
			return -1;
		}

		// Create the effect
		SDL_memset(&effect, 0, sizeof(SDL_HapticEffect)); // 0 is safe default
		//effect.type = SDL_HAPTIC_SINE;
		effect.periodic.direction.type = SDL_HAPTIC_CUSTOM; // Polar coordinates
		effect.periodic.direction.dir[0] = 18000; // Force comes from south
		effect.periodic.period = 1000; // 1000 ms
		effect.periodic.magnitude = 32767; // 20000/32767 strength
		effect.periodic.length = 5000; // 5 seconds long
		effect.periodic.attack_length = 1000; // Takes 1 second to get max strength
		effect.periodic.fade_length = 1000; // Takes 1 second to fade away

		int ret = SDL_HapticEffectSupported(haptic, &effect);
		// Upload the effect
		effect_id = SDL_HapticNewEffect(haptic, &effect);

		// Test the effect
		SDL_HapticRunEffect(haptic, effect_id, 1);
		SDL_Delay(5000); // Wait for the effect to finish

		// We destroy the effect, although closing the device also does this
		SDL_HapticDestroyEffect(haptic, effect_id);

		// Close the device
		//SDL_HapticClose(haptic);

		return 0; // Success
	}
	friend class j1Input;
};

class j1Input : public j1Module
{

public:

	j1Input();

	// Destructor
	virtual ~j1Input();

	// Called before render is available
	bool Awake(pugi::xml_node&) override;

	// Called before the first frame
	bool Start()override;

	// Called each loop iteration
	bool PreUpdate(float dt)override;

	// Called before quitting
	bool CleanUp()override;

	// Gather relevant win events
	bool GetWindowEvent(j1EventWindow ev);

	// Check key states (includes mouse and joy buttons)
	j1KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	j1KeyState GetMouseButtonState(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Check if a certain window event happened
	bool GetWindowEvent(int code);

	// Get mouse / axis position
	void GetMousePosition(int &x, int &y);
	void GetMouseMotion(int& x, int& y);
	iPoint GetMousePos_Tiles();

private:
	void Update_Keyboard_State();
	void Update_Mouse_State();
	void Update_Controllers();

private:
	bool		windowEvents[WE_COUNT];
	j1KeyState*	keyboard;
	j1KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	int			mouse_motion_x;
	int			mouse_motion_y;
	int			mouse_x;
	int			mouse_y;

private:
	


public:
	std::vector<Controller*> controllers;
};

#endif // __j1INPUT_H__