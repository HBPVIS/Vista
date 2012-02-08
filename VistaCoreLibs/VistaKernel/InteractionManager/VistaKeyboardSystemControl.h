/*============================================================================*/
/*                              ViSTA VR toolkit                              */
/*               Copyright (c) 1997-2012 RWTH Aachen University               */
/*============================================================================*/
/*                                  License                                   */
/*                                                                            */
/*  This program is free software: you can redistribute it and/or modify      */
/*  it under the terms of the GNU Lesser General Public License as published  */
/*  by the Free Software Foundation, either version 3 of the License, or      */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Lesser General Public License for more details.                       */
/*                                                                            */
/*  You should have received a copy of the GNU Lesser General Public License  */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.     */
/*============================================================================*/
/*                                Contributors                                */
/*                                                                            */
/*============================================================================*/
// $Id$

#ifndef _VISTAKEYBOARDSYSTEMCONTROL_H
#define _VISTAKEYBOARDSYSTEMCONTROL_H

/*============================================================================*/
/* INCLUDES                                                                   */
/*============================================================================*/
#include <VistaKernel/VistaKernelConfig.h>

#include <string>
#include <map>
#include <list>
#include <vector>


/*============================================================================*/
/* MACROS AND DEFINES                                                         */
/*============================================================================*/
// artificial keycodes for cursor arrow keys
#define VISTA_KEY_UPARROW		(1000)
#define VISTA_KEY_DOWNARROW 	(1001)
#define VISTA_KEY_RIGHTARROW	(1002)
#define VISTA_KEY_LEFTARROW 	(1003)
#define	VISTA_KEY_ESC			(27)

// function keys
#define VISTA_KEY_F1		(1059)
#define VISTA_KEY_F2		(1060)
#define VISTA_KEY_F3		(1061)
#define VISTA_KEY_F4		(1062)
#define VISTA_KEY_F5		(1063)
#define VISTA_KEY_F6		(1064)
#define VISTA_KEY_F7		(1065)
#define VISTA_KEY_F8		(1066)
#define VISTA_KEY_F9		(1067)
#define VISTA_KEY_F10		(1068)
#define VISTA_KEY_F11		(1069)
#define VISTA_KEY_F12		(1070)

// misc keys
#define VISTA_KEY_ENTER     (1013)
#define VISTA_KEY_TAB       (1009)
#define VISTA_KEY_BACKSPACE (1008)
#define VISTA_KEY_DELETE    ( 127)


// The following keycodes are 1000 + the IBM scancode
#define	VISTA_KEY_HOME		(1071)
#define	VISTA_KEY_END		(1079)
#define	VISTA_KEY_PAGEUP	(1073)
#define	VISTA_KEY_PAGEDOWN	(1081)
#define	VISTA_KEY_MIDDLE	(1076)


/**
 * Bitmask values for modifier keys shift, control and alt
 * additionally, VISTA_KEYMOD_ANY matches any modifiers
 */
#define VISTA_KEYMOD_ANY    (-1)
#define VISTA_KEYMOD_NONE   (0)
#define VISTA_KEYMOD_SHIFT  (0x01)
#define VISTA_KEYMOD_CTRL   (0x02)
#define VISTA_KEYMOD_ALT    (0x04)

/*============================================================================*/
/* FORWARD DECLARATIONS                                                       */
/*============================================================================*/
class IVistaExplicitCallbackInterface;

/*============================================================================*/
/* CLASS DEFINITIONS                                                          */
/*============================================================================*/
/**
 * This class can be used as an event handler and action trigger, usually
 * fed with input from the keyboard. The VistaSystem instance creates one
 * during the init phase, this can be used to register actions that are
 * triggered. This means that you usually do not have to create one on your
 * own. Instead, query it from the System using the
 * VistaSystem::GetKeyboardSystemControl() after init.
 * The RegisterAction() method is the method number one for the normal user.
 * One should regard its return value to check, whether a token was already
 * set or not.
 * You can chose to
 * - inject key commands on your own VistaKeyboardSystemControl::InjectKey
 * - intercept injected key by setting a direct key sink
 */
class VISTAKERNELAPI VistaKeyboardSystemControl
{
public:
	/**
	 * A registration structure with no key sink.
	 */
	VistaKeyboardSystemControl( const bool bCheckForKeyRepeat = true );

	/**
	 * Destructor. Deletes the Callbacks iff the ManageDeletion flag was set on bind
	 */
	virtual ~VistaKeyboardSystemControl();

	// #########################################################################
	// STATE API
	// #########################################################################

	bool GetCheckForKeyRepeat() const;
	void SetCheckForKeyRepeat( const bool bSet );

	/**
	 * The callback that is given with pInt is called when action nKeyCode
	 * is injected using the InjectKey API.
	 * @param nKeyCode: the token is used to match an incoming signal on
					slot 0 on the context that is created by this instance.
	 * @param nModifiers: the modifier: either ANY, NONE, or a bitmask combination
					of  CTRL, SHIFT, and ALT. If the modifiers is set to ANY (or the
					modifier-free BindAction is called) the Callback is called whenever
					the key is pressed, regardless of the modifiers, as long as no
					other modifiers+key-Action has been bound (those have higher priority).
					Note that the modifier CTRL means that only! control may be pressed,
					but neither SHIFT nor ALT.
	 * @param pCallback: the callback to trigger
	 * @param sHelpText the help text that is assigned to the action that can
					be queried by GetHelpTextForToken()
	 * @param bManageDeletion: if set to true, deletion of the callback is managed by
					the Ctrl (on Ctrl-Delete or Unbind)
	 * @param bCallOnKeyRepeat: Specifies whether or nor the callback will be called
					multiple times if a key is kept pressed
	 * @param bForce: set this to true iff you want to overwrite an existing
					callback interface. Note that only the mapping is overwritten,
					but the old callback is not deleted. As such, a memory leak
					can be the result.
	 * @return false iff GetActionForToken(nKeyCode) != NULL and bForce == false.
	 */

	bool BindAction( int nKeyCode,
					IVistaExplicitCallbackInterface* pCallback,
					const std::string &sHelpText = "<none>",
					bool bManageDeletion = true,
					bool bCallOnKeyRepeat = true,					
					bool bForce = false );
	bool BindAction( int nKeyCode,
					int nModifiers,
					IVistaExplicitCallbackInterface* pCallback,
					const std::string& sHelpText = "<none>",
					bool bManageDeletion = true,
					bool bCallOnKeyRepeat = true,
					bool bForce = false );

	/**
	 * Unregisters the mapping for token nKeyCode, and delete if the ManageDeletion flag
					was set in BindAction
	 * @param nKeyCode: the token to unregister an existing mapping
	 * @param nModifiers: Modifiers flag od the bound action
	 * @return true iff anything was unregistered, false if this
					token was not registered.
	 */
	bool UnbindAction( int nKeyCode, int nModifiers = VISTA_KEYMOD_ANY );

	/**
	 * Unregisters the given Action, and delete if the ManageDeletion flag
					was set in BindAction
	 * @param pAction: the action to unregister	 
	 * @return true iff anything was unregistered, false if this
					action was not registered.
	 */
	bool UnbindAction( IVistaExplicitCallbackInterface *pAction );


	/**
	 * Sort-of-reflective API, returns an action for a specific token.
	 * @param nKeyCode: a token
	 * @param nModifiers: Modifiers flag od the bound action
	 * @return NULL iff no action for the token was registered, a pointer to
						the registered action else
	 */
	IVistaExplicitCallbackInterface* GetActionForToken( int nKeyCode, 
									int nModifiers = VISTA_KEYMOD_ANY ) const;


	// #########################################################################
	// REFLECTIVE API
	// #########################################################################
	/**
	 * Query a list of tokens that are currently registered with an action.
	 * @return the number of elements in liTarget, saves a call to list::size()
	 * @param liTarget the list of tokens to fill. It is assumed that the list
			  is empty already.
	 */
	int GetTokenList( std::list<int>& liTarget, int nModifiers = VISTA_KEYMOD_ANY ) const;

	/**
	 * Query a registered help text for a given token.
	 * @param nToken the token that was used for action registration.
	 * @return a string that was given during the call to RegisterAction() with the
			   token nToken. Can be empty (no text was given during registration)
	 */
	std::string GetHelpTextForToken( int nToken, int nModifiers = VISTA_KEYMOD_ANY ) const;

	// #########################################################################
	// DIRECTKEYHANDLER API
	// #########################################################################

	/**
	 * Useful API to plug in between the registered actions and the raw keyboard
	 * input. Usually, one needs this when dispatching keyboard streams, e.g.,
	 * virtual consoles and the like. You could use this API to query the modifier
	 * state, which is not passed to the IVistaExplicitCallback Interface for
	 * normal keystroke dispatching.
	 */
	class IVistaDirectKeySink
	{
	public:
		virtual ~IVistaDirectKeySink() {}

		/**
		 * @param nKey the key code to dispatch, as usual, if below zero,
				  this is an up key, down key else. Iff 0, no key was pressed,
				  but maybe a modifier, although this might not be supported by
				  the underlying driver.
		 * @param nMod a bit mask for the modifier tag
		 * @param bIsKeyRepeat specifies if the key is a keyrepreat event
		 * @return true iff the key is to be swallowed, false if it should be
				   passed to the normal action procedure.
		 */
		virtual bool HandleKeyPress( int nKey, int nMod, bool bIsKeyRepeat = false ) = 0;
	protected:
		IVistaDirectKeySink() {}		
	};

	/**
	 * @return the currently set direct key sink, NULL iff none is set
	 */
	IVistaDirectKeySink *GetDirectKeySink() const;

	/**
	 * @param pSink the direct key sink to set. The pointer is not owned
			  but the keyboard control, so it is neither cleaned up nor checked.
	 */
	void SetDirectKeySink(IVistaDirectKeySink *pSink);


	/**
	 * Macro API, calls InjectKey() for every entry in the KeyCodeList and ModifierList.
	 * callers must assert that KeyCodeList.size() == ModifierList.size().
	 * @see InjectKey()
	 * @param vecKeyCodeList the tokens to be called as an ordered vector of int
	 * @param vecModifierList the modifiers for each key code (needed for direct key sink API)
	 * @return true always
	 */
	bool InjectKeys( const std::vector<int>& vecKeyCodeList,
			         const std::vector<int>& vecModifierList,
			         std::vector<int>& vecUnswallowedKeys,
			         std::vector<int>& vecUnswallowedMods );

	/**
	 * Calls the registered callback to nKeyCode. The modifier is passed to the direct key sink
	 * and is not used for the action processing.
	 * @param nKeyCode the token for an action to be called
	 * @param nModifier the modifier value for the given key code.
	 * @return true iff the nKeyCode value triggered some action (or the return value of Do()), false else
	 */
	bool InjectKey( int nKeyCode, int nModifier, bool bIsKeyRepeat = false );

	std::string GetKeyBindingTableString() const;

	static std::string GetKeyName( const int nKeyCode );
	static std::string GetModifiersName( const int nModifiers );

private:
	IVistaDirectKeySink         *m_pKeySink;

	/**
	 * helper structure to associate a help text to an action callback.
	 */
	struct SKeyboardAction
	{
		SKeyboardAction();
		SKeyboardAction( IVistaExplicitCallbackInterface *pCommand,
							const std::string &sHelpText,
							const bool bCallOnKeyRepreat,
							const bool bManageDeletion );

		IVistaExplicitCallbackInterface*	m_pCallback;
		std::string							m_strHelpText;
		bool								m_bCallOnKeyRepreat;
		bool								m_bManageDeletion;
	};

	typedef std::map<int, SKeyboardAction> CommandMap;
	CommandMap						m_mapCommandMap;
	std::vector<CommandMap>			m_vecModdedCommandMaps;
	std::vector<int>				m_vecLastFrameKeys;
	bool							m_bCheckForKeyRepeat;
};


/*============================================================================*/
/* LOCAL VARS AND FUNCS                                                       */
/*============================================================================*/

#endif //_VISTAKEYBOARDSYSTEMCONTROL_H
