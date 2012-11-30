/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"

#define NUM_NATIVE_PARAMS 1
 
SH_DECL_HOOK2_void(IVEngineServer, ClientPrintf, SH_NOATTRIB, 0, edict_t *, const char *);


IChangeableForward *g_pHookConsolePrint = NULL;
CGlobalVars *gpGlobals = NULL;

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

ConsoleHook g_ConsoleHook;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_ConsoleHook);


void Hook_ClientPrintf(edict_t *pEdict, const char *szMsg)
{
	char cMsg [192];
	strcpy(cMsg, szMsg);
	int client = engine->IndexOfEdict(pEdict);

	if (client > 0 && client <= gpGlobals->maxClients)
	{
		cell_t result = Pl_Continue;
		g_pHookConsolePrint->PushCell(client);
		g_pHookConsolePrint->PushStringEx(cMsg, sizeof(cMsg), SM_PARAM_STRING_UTF8|SM_PARAM_STRING_COPY, SM_PARAM_COPYBACK);
		g_pHookConsolePrint->Execute(&result);

		if (result == Pl_Continue)
		{
			RETURN_META(MRES_IGNORED);
		}
		else if (result == Pl_Changed)
		{
			RETURN_META_NEWPARAMS(MRES_IGNORED, &IVEngineServer::ClientPrintf, (pEdict, ((const char *)cMsg)));
		}
		else
		{
			RETURN_META(MRES_SUPERCEDE);
		}
	}
	RETURN_META(MRES_IGNORED);
}

void AddHooks()
{
   SH_ADD_HOOK(IVEngineServer, ClientPrintf, engine, SH_STATIC(Hook_ClientPrintf), false);
}
 
void RemoveHooks()
{
   SH_REMOVE_HOOK(IVEngineServer, ClientPrintf, engine, SH_STATIC(Hook_ClientPrintf), false);
}

bool ConsoleHook::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	gpGlobals = ismm->GetCGlobals();
	return true;
}

static cell_t AddConsolePrintHook(IPluginContext *pContext, const cell_t *params)
{
	if (params[0] == NUM_NATIVE_PARAMS)
	{
		return g_pHookConsolePrint->AddFunction(pContext, static_cast<funcid_t>(params[1]));
	}
	return 0;
}

static cell_t RemoveConsolePrintHook(IPluginContext *pContext, const cell_t *params)
{
	if (params[0] == NUM_NATIVE_PARAMS)
	{
		IPluginFunction *pFunction = pContext->GetFunctionById(static_cast<funcid_t>(params[1]));
		return g_pHookConsolePrint->RemoveFunction(pFunction);
	}
	return 0;
}

const sp_nativeinfo_t MyNatives[] = 
{
	{"AddConsolePrintHook",	AddConsolePrintHook},
	{"RemoveConsolePrintHook", RemoveConsolePrintHook},
	{NULL,			NULL},
};

bool ConsoleHook::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	sharesys->AddNatives(myself, MyNatives);
	sharesys->RegisterLibrary(myself, "consolehook");
	AddHooks();
	return true;
}

void ConsoleHook::SDK_OnUnload()
{
	RemoveHooks();
	forwards->ReleaseForward(g_pHookConsolePrint);
}

void ConsoleHook::SDK_OnAllLoaded()
{
	g_pHookConsolePrint = forwards->CreateForwardEx(NULL, ET_Hook, 2, NULL, Param_Cell, Param_String);
}