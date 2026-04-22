#pragma once
#include "../includes/ehandle.h"



struct InputEvent_t
{
	int m_nType;				// Type of the event (see InputEventType_t)
	int m_nTick;				// Tick on which the event occurred
	int m_nData;				// Generic 32-bit data, what it contains depends on the event
	int m_nData2;				// Generic 32-bit data, what it contains depends on the event
	int m_nData3;				// Generic 32-bit data, what it contains depends on the event
};

#define JOYSTICK_AXIS_INTERNAL( _joystick, _axis ) ( JOYSTICK_FIRST_AXIS + ((_joystick) * MAX_JOYSTICK_AXES) + (_axis) )
#define JOYSTICK_AXIS( _joystick, _axis ) ( (AnalogCode_t)JOYSTICK_AXIS_INTERNAL( _joystick, _axis ) )
enum AnalogCode_t
{
	ANALOG_CODE_INVALID = -1,
	MOUSE_X = 0,
	MOUSE_Y,
	MOUSE_XY,		// Invoked when either x or y changes
	MOUSE_WHEEL,

	JOYSTICK_FIRST_AXIS,
	JOYSTICK_LAST_AXIS = JOYSTICK_AXIS_INTERNAL(MAX_JOYSTICKS - 1, MAX_JOYSTICK_AXES - 1),

	ANALOG_CODE_LAST,
};

class IInputSystem : public IAppSystem
{
public:
	// Attach, detach input system from a particular window
	// This window should be the root window for the application
	// Only 1 window should be attached at any given time.
	virtual void AttachToWindow(void* hWnd) = 0;
	virtual void DetachFromWindow() = 0;

	// Enables/disables input. PollInputState will not update current 
	// button/analog states when it is called if the system is disabled.
	virtual void EnableInput(bool bEnable) = 0;

	// Enables/disables the windows message pump. PollInputState will not
	// Peek/Dispatch messages if this is disabled
	virtual void EnableMessagePump(bool bEnable) = 0;

	// Polls the current input state
	virtual void PollInputState() = 0;

	// Gets the time of the last polling in ms
	virtual int GetPollTick() const = 0;

	// Is a button down? "Buttons" are binary-state input devices (mouse buttons, keyboard keys)
	virtual bool IsButtonDown(ButtonCode_t code) const = 0;

	// Returns the tick at which the button was pressed and released
	virtual int GetButtonPressedTick(ButtonCode_t code) const = 0;
	virtual int GetButtonReleasedTick(ButtonCode_t code) const = 0;

	// Gets the value of an analog input device this frame
	// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogValue(AnalogCode_t code) const = 0;

	// Gets the change in a particular analog input device this frame
	// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogDelta(AnalogCode_t code) const = 0;

	// Returns the input events since the last poll
	virtual int GetEventCount() const = 0;
	virtual const InputEvent_t* GetEventData() const = 0;

	// Posts a user-defined event into the event queue; this is expected
	// to be called in overridden wndprocs connected to the root panel.
	virtual void PostUserEvent(const InputEvent_t &event) = 0;

	// Returns the number of joysticks
	virtual int GetJoystickCount() const = 0;

	// Enable/disable joystick, it has perf costs
	virtual void EnableJoystickInput(int nJoystick, bool bEnable) = 0;

	// Enable/disable diagonal joystick POV (simultaneous POV buttons down)
	virtual void EnableJoystickDiagonalPOV(int nJoystick, bool bEnable) = 0;

	// Sample the joystick and append events to the input queue
	virtual void SampleDevices(void) = 0;

	// FIXME: Currently force-feedback is only supported on the Xbox 360
	virtual void SetRumble(float fLeftMotor, float fRightMotor, int userId = -1) = 0;
	virtual void StopRumble(void) = 0;

	// Resets the input state
	virtual void ResetInputState() = 0;

	// Sets a player as the primary user - all other controllers will be ignored.
	virtual void SetPrimaryUserId(int userId) = 0;

	// Convert back + forth between ButtonCode/AnalogCode + strings
	virtual const char *ButtonCodeToString(ButtonCode_t code) const = 0;
	virtual const char *AnalogCodeToString(AnalogCode_t code) const = 0;
	virtual ButtonCode_t StringToButtonCode(const char *pString) const = 0;
	virtual AnalogCode_t StringToAnalogCode(const char *pString) const = 0;

	// Sleeps until input happens. Pass a negative number to sleep infinitely
	virtual void SleepUntilInput(int nMaxSleepTimeMS = -1) = 0;

	// Convert back + forth between virtual codes + button codes
	// FIXME: This is a temporary piece of code
	virtual ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey) const = 0;
	virtual int ButtonCodeToVirtualKey(ButtonCode_t code) const = 0;
	virtual ButtonCode_t ScanCodeToButtonCode(int lParam) const = 0;

	// How many times have we called PollInputState?
	virtual int GetPollCount() const = 0;

	// Sets the cursor position
	virtual void SetCursorPosition(int x, int y) = 0;

	// NVNT get address to haptics interface
	virtual void *GetHapticsInterfaceAddress() const = 0;

	virtual void SetNovintPure(bool bPure) = 0;

	// read and clear accumulated raw input values
	virtual bool GetRawMouseAccumulators(int& accumX, int& accumY) = 0;

	// tell the input system that we're not a game, we're console text mode.
	// this is used for dedicated servers to not initialize joystick system.
	// this needs to be called before CInputSystem::Init (e.g. in PreInit of
	// some system) if you want ot prevent the joystick system from ever
	// being initialized.
	virtual void SetConsoleTextMode(bool bConsoleTextMode) = 0;
};



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CKeyboardKey
{
public:
	// Name for key
	char				name[32];
	// Pointer to the underlying structure
	kbutton_t			*pkey;
	// Next key in key list.
	CKeyboardKey		*next;
};

// How many data slots to use when in multiplayer.
#define MULTIPLAYER_BACKUP			150

class IInput
{
public:
	// Initialization/shutdown of the subsystem
	virtual	void		Init_All(void) = 0;
	virtual void		Shutdown_All(void) = 0;
	// Latching button states
	virtual int			GetButtonBits(int) = 0;
	// Create movement command
	virtual void		CreateMove(int sequence_number, float input_sample_frametime, bool active) = 0;
	virtual void		ExtraMouseSample(float frametime, bool active) = 0;
	virtual bool		WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, bool isnewcommand) = 0;
	virtual void		EncodeUserCmdToBuffer(bf_write& buf, int slot) = 0;
	virtual void		DecodeUserCmdFromBuffer(bf_read& buf, int slot) = 0;

	// 濡傛灉 solt 涓嶇煡閬撳氨缁欎釜 -1锛岃瀹冭嚜鍔ㄨ幏鍙?
	virtual CUserCmd*	GetUserCmd(int solt, int sequence_number) = 0;

	virtual void		MakeWeaponSelection(C_BaseEntity* *weapon) = 0;

	// Retrieve key state
	virtual float		KeyState(kbutton_t *key) = 0;
	// Issue key event
	virtual int			KeyEvent(int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding) = 0;

	// Look for key
	virtual kbutton_t	*FindKey(const char *name) = 0;

	// Issue commands from controllers
	virtual void		ControllerCommands(void) = 0;
	// Extra initialization for some joysticks
	virtual void		Joystick_Advanced(void) = 0;

	/*
	virtual void		Joystick_SetSampleTime(float frametime) = 0;
	virtual void		IN_SetSampleTime(float frametime) = 0;
	*/

	virtual void		Unknown_1() = 0;
	virtual void		Unknown_2() = 0;
	virtual void		Unknown_3() = 0;
	virtual void		Unknown_4() = 0;

	// Accumulate mouse delta
	virtual void		AccumulateMouse(void) = 0;
	// Activate/deactivate mouse
	virtual void		ActivateMouse(void) = 0;
	virtual void		DeactivateMouse(void) = 0;

	// Clear mouse state data
	virtual void		ClearStates(void) = 0;
	// Retrieve lookspring setting
	virtual float		GetLookSpring(void) = 0;

	// Retrieve mouse position
	virtual void		GetFullscreenMousePos(int *mx, int *my, int *unclampedx = 0, int *unclampedy = 0) = 0;
	virtual void		SetFullscreenMousePos(int mx, int my) = 0;
	virtual void		ResetMouse(void) = 0;
	virtual	float		GetLastForwardMove(void) = 0;
	// virtual	float		Joystick_GetForward(void) = 0;
	// virtual	float		Joystick_GetSide(void) = 0;
	// virtual	float		Joystick_GetPitch(void) = 0;
	// virtual	float		Joystick_GetYaw(void) = 0;

	// Third Person camera ( TODO/FIXME:  Move this to a separate interface? )
	virtual void		CAM_Think(void) = 0;
	virtual int			CAM_IsThirdPerson(void) = 0;
	virtual void		CAM_ToThirdPerson(void) = 0;
	virtual void		CAM_ToFirstPerson(void) = 0;
	virtual void		CAM_StartMouseMove(void) = 0;
	virtual void		CAM_EndMouseMove(void) = 0;
	virtual void		CAM_StartDistance(void) = 0;
	virtual void		CAM_EndDistance(void) = 0;
	virtual int			CAM_InterceptingMouse(void) = 0;

	// orthographic camera info	( TODO/FIXME:  Move this to a separate interface? )
	virtual void		CAM_ToOrthographic() = 0;
	virtual	bool		CAM_IsOrthographic() const = 0;
	virtual	void		CAM_OrthographicSize(float& w, float& h) const = 0;

	virtual void		LevelInit(void) = 0;

	// Causes an input to have to be re-pressed to become active
	virtual void		ClearInputButton(int bits) = 0;

	virtual	void		CAM_SetCameraThirdData(void *pCameraData, const void* vecCameraOffset) = 0;
	virtual void		CAM_CameraThirdThink(void) = 0;

	virtual	bool		EnableJoystickMode() = 0;

public:
	typedef struct
	{
		unsigned int AxisFlags;
		unsigned int AxisMap;
		unsigned int ControlMap;
	} joy_axis_t;

	void		DescribeJoystickAxis(char const *axis, joy_axis_t *mapping);
	char const	*DescribeAxis(int index);

	enum
	{
		GAME_AXIS_NONE = 0,
		GAME_AXIS_FORWARD,
		GAME_AXIS_PITCH,
		GAME_AXIS_SIDE,
		GAME_AXIS_YAW,
		MAX_GAME_AXES
	};

	enum
	{
		CAM_COMMAND_NONE = 0,
		CAM_COMMAND_TOTHIRDPERSON = 1,
		CAM_COMMAND_TOFIRSTPERSON = 2
	};

	enum
	{
		MOUSE_ACCEL_THRESHHOLD1 = 0,	// if mouse moves > this many mickey's double it
		MOUSE_ACCEL_THRESHHOLD2,		// if mouse moves > this many mickey's double it a second time
		MOUSE_SPEED_FACTOR,				// 0 = disabled, 1 = threshold 1 enabled, 2 = threshold 2 enabled

		NUM_MOUSE_PARAMS,
	};

	// Has the mouse been initialized?
	bool		m_fMouseInitialized;
	// Is the mosue active?
	bool		m_fMouseActive;
	// Has the joystick advanced initialization been run?
	bool		m_fJoystickAdvancedInit;
	// Used to support hotplugging by reinitializing the advanced joystick system when we toggle between some/none joysticks.
	bool		m_fHadJoysticks;

	// Accumulated mouse deltas
	float		m_flAccumulatedMouseXMovement;
	float		m_flAccumulatedMouseYMovement;
	float		m_flPreviousMouseXPosition;
	float		m_flPreviousMouseYPosition;
	float		m_flRemainingJoystickSampleTime;
	float		m_flKeyboardSampleTime;

	// Flag to restore systemparameters when exiting
	bool		m_fRestoreSPI;
	// Original mouse parameters
	int			m_rgOrigMouseParms[NUM_MOUSE_PARAMS];
	// Current mouse parameters.
	int			m_rgNewMouseParms[NUM_MOUSE_PARAMS];
	bool		m_rgCheckMouseParam[NUM_MOUSE_PARAMS];
	// Are the parameters valid
	bool		m_fMouseParmsValid;
	// Joystick Axis data
	joy_axis_t	m_rgAxes[MAX_JOYSTICK_AXES];
	// List of queryable keys
	CKeyboardKey *m_pKeys;

	// Is the 3rd person camera using the mouse?
	bool		m_fCameraInterceptingMouse;
	// Are we in 3rd person view?
	bool		m_fCameraInThirdPerson;
	// Should we move view along with mouse?
	bool		m_fCameraMovingWithMouse;


	// Is the camera in distance moving mode?
	bool		m_fCameraDistanceMove;
	// Old and current mouse position readings.
	int			m_nCameraOldX;
	int			m_nCameraOldY;
	int			m_nCameraX;
	int			m_nCameraY;

	// orthographic camera settings
	bool		m_CameraIsOrthographic;

	void*		m_angPreviousViewAngles;

	float		m_flLastForwardMove;

	float m_flPreviousJoystickForward;
	float m_flPreviousJoystickSide;
	float m_flPreviousJoystickPitch;
	float m_flPreviousJoystickYaw;

	CUserCmd*	m_pCommands;
	void* m_pVerifiedCommands;

	void* m_pCameraThirdData;

	// Set until polled by CreateMove and cleared
	CHandle<C_BaseEntity> m_hSelectedWeapon;


};

namespace I { inline IInputSystem* InputSystem = nullptr; }
