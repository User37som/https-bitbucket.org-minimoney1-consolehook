#if defined __consolehook_included
 #endinput
#define __consolehook_included

funcenum ConsoleHook
{
	/**
	 *
	 * Called when a IVEngineServer::ClientPrintf is called
	 * 
	 * @param client 	Client index of the receiver
	 * @param msg		The message being printed (by-ref)
	 *
	 *
	 * @return 			Plugin_Continue to let the engine handle the message. Plugin_Changed to let the engine handle the modified message through the msg param. Plugin_Handled or Plugin_Stop to block the message.
	 * 
	 */
	Action:public(client, String:msg[]),
}

/**
 *
 * Adds a ConsoleHook hook
 *
 * @param func 			The function to be called when the hook is called.
 *
 * @return				True if successful. False otherwise.
 *
 */
native bool:AddConsolePrintHook(ConsoleHook:func);

/**
 *
 * Removes a ConsoleHook hook
 *
 * @param func 			The ConsoleHook function to be removed.
 *
 * @return				True if successful. False otherwise.
 *
 */
native bool:RemoveConsolePrintHook(ConsoleHook:func);

public Extension:__ext_consolehook = 
{
	name = "consolehook",
	file = "consolehook.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};
