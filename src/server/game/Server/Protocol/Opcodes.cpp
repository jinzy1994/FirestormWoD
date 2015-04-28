/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by therate
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Opcodes.h"
#include "WorldSession.h"

OpcodeHandler* g_OpcodeTable[TRANSFER_DIRECTION_MAX][NUM_OPCODE_HANDLERS] = { };

template<bool isInValidRange, bool isNonZero>
inline void ValidateAndSetOpcode(uint16 /*p_Opcode*/, char const* /*p_Name*/, SessionStatus /*p_Status*/, PacketProcessing /*p_Processing*/, g_OpcodeHandlerType /*p_Handler*/)
{
    /// if for some reason we are here, that means NUM_OPCODE_HANDLERS == 0 (or your compiler is broken)
}

template<>
void ValidateAndSetOpcode<true, true>(uint16 p_Opcode, char const* p_Name, SessionStatus p_Status, PacketProcessing p_Processing, g_OpcodeHandlerType p_Handler)
{
    OpcodeTransferDirection l_OpcodeDirection = TRANSFER_DIRECTION_MAX;

    switch (p_Name[0])
    {
        case 'S':
        case 's':
            l_OpcodeDirection = WOW_SERVER_TO_CLIENT; // SMSG
            break;

        case 'C':
        case 'c':
            l_OpcodeDirection = WOW_CLIENT_TO_SERVER; // CMSG

        //DEFault: // MSG
            break;
    }

    if (l_OpcodeDirection == TRANSFER_DIRECTION_MAX)
    {
        for (uint8 l_CurrentDirection = 0; l_CurrentDirection < TRANSFER_DIRECTION_MAX; ++l_CurrentDirection)
        {
            if (g_OpcodeTable[l_CurrentDirection][p_Opcode] != NULL) // register MSG opcode as client and server
            {
                sLog->outError(LOG_FILTER_NETWORKIO, "Tried to override handler of %s with %s (opcode %u)", g_OpcodeTable[l_CurrentDirection][p_Opcode]->name, p_Name, p_Opcode);
                return;
            }

            g_OpcodeTable[l_CurrentDirection][p_Opcode] = new OpcodeHandler(p_Name, p_Status, p_Processing, p_Handler);
        }

        return;
    }

    if (g_OpcodeTable[l_OpcodeDirection][p_Opcode] != NULL)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Tried to override handler of %s with %s (opcode %u)", g_OpcodeTable[l_OpcodeDirection][p_Opcode]->name, p_Name, p_Opcode);
        return;
    }

    g_OpcodeTable[l_OpcodeDirection][p_Opcode] = new OpcodeHandler(p_Name, p_Status, p_Processing, p_Handler);
}

template<>
void ValidateAndSetOpcode<false, true>(uint16 p_Opcode, char const* /*p_Name*/, SessionStatus /*p_Status*/, PacketProcessing /*p_Processing*/, g_OpcodeHandlerType /*p_Handler*/)
{
    sLog->outError(LOG_FILTER_NETWORKIO, "Tried to set handler for an invalid opcode %d", p_Opcode);
}

template<>
void ValidateAndSetOpcode<true, false>(uint16 /*p_Opcode*/, char const* p_Name, SessionStatus /*p_Status*/, PacketProcessing /*p_Processing*/, g_OpcodeHandlerType /*p_Handler*/)
{
    sLog->outError(LOG_FILTER_NETWORKIO, "Opcode %s got value 0", p_Name);
}

#define DEFINE_OPCODE_HANDLER(p_Opcode, p_Status, p_Processing, p_Handler)                                      \
    ValidateAndSetOpcode<(p_Opcode < NUM_OPCODE_HANDLERS), (p_Opcode != 0)>(p_Opcode, #p_Opcode, p_Status, p_Processing, p_Handler);

/// Correspondence between opcodes and their names
void InitOpcodes()
{
    memset(g_OpcodeTable, 0, sizeof(g_OpcodeTable));

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Protocol
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamProtocol
        /// Opcodes
        DEFINE_OPCODE_HANDLER(SMSG_AUTH_CHALLENGE,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Dispatch
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamDispatch
        /// Base opcodes
        DEFINE_OPCODE_HANDLER(SMSG_AUTH_RESPONSE,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CACHE_VERSION,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ADDON_INFO,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_FEATURE_SYSTEM_STATUS,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_TIME_ZONE_INFORMATION,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOGIN_VERIFY_WORLD,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_NEW_WORLD,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOGIN_SET_TIME_SPEED,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_HOTFIX_NOTIFY_BLOB,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_HOTFIX_NOTIFY,                                   STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_WORLD_SERVER_INFO,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INITIAL_SETUP,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PONG,                                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UITIME,                                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Query
        DEFINE_OPCODE_HANDLER(SMSG_DB_REPLY,                                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUERY_CREATURE_RESPONSE,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_NPC_TEXT_UPDATE,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_NAME_QUERY_RESPONSE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_REALM_QUERY_RESPONSE,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_QUERY_RESPONSE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUERY_TIME_RESPONSE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Misc
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_ACTION_BUTTONS,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_PROFICIENCY,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INIT_WORLD_STATES,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_WORLD_STATE,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_EMOTE,                                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_EXPLORATION_EXPERIENCE,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOG_XP_GAIN,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LEVELUP_INFO,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ITEM_PUSH_RESULT,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INIT_CURRENCY,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_CURRENCY,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_CURRENCY_WEEK_LIMIT,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_WEEKLY_RESET_CURRENCY,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_EQUIPMENT_SET_LIST,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BINDER_CONFIRM,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BIND_POINT_UPDATE,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_TALENT_DATA,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TRANSFER_ABORTED,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TRANSFER_PENDING,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INVENTORY_CHANGE_FAILURE,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_DUNGEON_DIFFICULTY,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_RAID_DIFFICULTY,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOAD_CUF_PROFILES,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_STANDSTATE_UPDATE,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_START_TIMER,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_START_ELAPSED_TIMER,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_START_ELAPSED_TIMERS,                            STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_STOP_ELAPSED_TIMER,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DEATH_RELEASE_LOC,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CORPSE_RECLAIM_DELAY,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CORPSE_LOCATION,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CORPSE_TRANSPORT_QUERY,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DURABILITY_DAMAGE_DEATH,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAY_MUSIC,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAY_OBJECT_SOUND,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAY_SOUND,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_TOAST,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ITEM_ENCHANT_TIME_UPDATE,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ENCHANTMENT_LOG,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PARTY_KILL_LOG,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PROCRESIST,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CLEAR_TARGET,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_WEATHER,                                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_PHASE_SHIFT,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TRIGGER_CINEMATIC,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TOTEM_CREATED,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RESPEC_WIPE_CONFIRM,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_USE_EQUIPMENT_SET_RESULT,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TRADE_STATUS,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TRADE_UPDATED,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SHOW_NEUTRAL_PLAYER_FACTION_SELECT_UI,           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSPECT_TALENT,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSPECT_HONOR_STATS,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSPECT_RATED_BG_STATS,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TIME_SYNC_REQUEST,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RESUME_TOKEN,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PAGE_TEXT_QUERY_RESPONSE,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PAGE_TEXT,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_READ_ITEM_FAILED,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_READ_ITEM_OK,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TEXT_EMOTE,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TITLE_EARNED,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TITLE_LOST,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_WHOIS,                                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TRIGGER_MOVIE,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_LAST_INSTANCE,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_INSTANCE_OWNERSHIP,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SUMMON_REQUEST,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_STOP_MIRROR_TIMER,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_START_MIRROR_TIMER,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SOCKET_GEMS,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RESURRECT_REQUEST,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RESPOND_INSPECT_ACHIEVEMENTS,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RESET_FAILED_NOTIFY,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SETUP_RESEARCH_HISTORY,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RESEARCH_COMPLETE,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ARCHAEOLOGY_SURVERY_CAST,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_REQUEST_CEMETERY_LIST_RESPONSE,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAYED_TIME,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BREAK_TARGET,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CANCEL_AUTO_REPEAT,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CANCEL_COMBAT,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_COMPLAIN_RESULT,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_XPGAIN_ABORTED,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SUPERCEDED_SPELL,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PVP_CREDIT,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PRE_RESURRECT,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAY_ONE_SHOT_ANIM_KIT,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_AI_ANIM_KIT,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAYER_BOUND,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_OVERRIDE_LIGHT,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PRINT_NOTIFICATION,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPECIAL_MOUNT_ANIM,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ITEM_TIME_UPDATE,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUERY_ITEM_TEXT_RESPONSE,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ITEM_PURCHASE_REFUND_RESULT,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_DESPAWN,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_CUSTOM_ANIM,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GAME_OBJECT_ACTIVATE_ANIM_KIT,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_FISH_NOT_HOOKED,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_FISH_ESCAPED,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CROSSED_INEBRIATION_THRESHOLD,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CLIENT_CONTROL_UPDATE,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_NO_CORPSE,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BAG_SORT_RESULT,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_VIGNETTE_UPDATE,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_MOUNT_UPDATE,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAY_SCENE,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CANCEL_SCENE,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TALENTS_INVOLUNTARILY_RESET,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_ITEM_PURCHASE_DATA,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SHOW_TRADE_SKILL_RESPONSE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Control Alert
        DEFINE_OPCODE_HANDLER(SMSG_LOSS_OF_CONTROL_AURA_UPDATE,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ADD_LOSS_OF_CONTROL,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CLEAR_LOSS_OF_CONTROL,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_REMOVE_LOSS_OF_CONTROL,                          STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Reputations
        DEFINE_OPCODE_HANDLER(SMSG_INITIALIZE_FACTIONS,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_FACTION_BONUS_INFO,                              STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_VISIBLE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_NOT_VISIBLE,                         STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_STANDING,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_FACTION_ATWAR,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_FORCED_REACTIONS,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Interaction
        DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_RESPONSE,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_CANCEL_ACK,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOGOUT_COMPLETE,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_POI,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BARBER_SHOP_RESULT,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ENABLE_BARBER_SHOP,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// World Object management
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_OBJECT,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_POWER_UPDATE,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Character list
        DEFINE_OPCODE_HANDLER(SMSG_ENUM_CHARACTERS_RESULT,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CREATE_CHAR,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAR_DELETE,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT,                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAR_FACTION_CHANGE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAR_RENAME,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAR_CUSTOMIZE_RESULT,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAR_CUSTOMIZE,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Account data
        DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_DATA_TIMES,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_ACCOUNT_DATA,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TUTORIAL_FLAGS,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Combat
        DEFINE_OPCODE_HANDLER(SMSG_ATTACKER_STATE_UPDATE,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ATTACK_START,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ATTACK_STOP,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AI_REACTION,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ATTACK_SWING_ERROR,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Duel
        DEFINE_OPCODE_HANDLER(SMSG_DUEL_REQUESTED,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DUEL_COMPLETE,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DUEL_COUNTDOWN,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DUEL_INBOUNDS,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DUEL_OUT_OF_BOUNDS,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DUEL_WINNER,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CAN_DUEL_RESULT,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Vendor
        DEFINE_OPCODE_HANDLER(SMSG_LIST_INVENTORY,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BUY_FAILED,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BUY_ITEM,                                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SELL_ITEM,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Achievement
        DEFINE_OPCODE_HANDLER(SMSG_ALL_ACHIEVEMENT_DATA,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ACHIEVEMENT_DELETED,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ACHIEVEMENT_EARNED,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_CRITERIA_UPDATE,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_CRITERIA_UPDATE_ALL,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CRITERIA_DELETED,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CRITERIA_UPDATE,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Friends
        DEFINE_OPCODE_HANDLER(SMSG_CONTACT_LIST,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_FRIEND_STATUS,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Taxi
        DEFINE_OPCODE_HANDLER(SMSG_TAXI_NODE_STATUS,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_NEW_TAXI_PATH,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SHOW_TAXI_NODES,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ACTIVATE_TAXI_REPLY,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Loot
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_LIST,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_RESPONSE,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_MONEY_NOTIFY,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_RELEASE,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_RELEASE_ALL,                                STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_REMOVED,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_COIN_REMOVED,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_ALL_PASSED,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_CONTENTS,                                   STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MASTER_LOOT_CANDIDATE_LIST,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_ROLL,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_ROLLS_COMPLETE,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_ROLL_WON,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LOOT_START_ROLL,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AE_LOOT_TARGETS,                                 STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AE_LOOT_TARGETS_ACK,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ITEM_BONUS_DEBUG,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Bank
        DEFINE_OPCODE_HANDLER(SMSG_SHOW_BANK,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Rune
        DEFINE_OPCODE_HANDLER(SMSG_RESYNC_RUNES,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DEBUG_RUNE_REGEN,                                STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ADD_RUNE_POWER,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CONVERT_RUNE,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Group
        DEFINE_OPCODE_HANDLER(SMSG_PARTY_INVITE,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PARTY_UPDATE,                                    STATUS_LOGGEDIN,      PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PARTY_MEMBER_STATE_FULL,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PARTY_MEMBER_STATE_PARTIAL,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GROUP_DECLINE,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PARTY_COMMAND_RESULT,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GROUP_UNINVITE,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GROUP_NEW_LEADER,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ROLE_CHANGED_INFORM,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MINIMAP_PING,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_LOOT_METHOD_FAILED,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RANDOM_ROLL,                                     STATUS_NEVER,         PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SEND_RAID_TARGET_UPDATE_SINGLE,                  STATUS_NEVER,         PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SEND_RAID_TARGET_UPDATE_ALL,                     STATUS_NEVER,         PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_READY_CHECK_RESPONSE,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_READY_CHECK_STARTED,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_READY_CHECK_COMPLETED,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ROLL_POLL_INFORM,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GROUP_DESTROYED,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Battleground
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_QUEUED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_NONE,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_NEED_CONFIRMATION,            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_ACTIVE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_FAILED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_STATUS_WAIT_FOR_GROUPS,              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_PORT_DENIED,                         STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RATED_BATTLEFIELD_INFO,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_JOINED,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_LEFT,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_PLAYER_POSITIONS,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_ENTERING,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE,          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EJECT_PENDING,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EJECTED,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_STATE_CHANGED,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BFMGR_ENTRY_INVITE,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_MGR_EXIT_REQUEST,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_REQUEST_PVP_REWARDS_RESPONSE,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PVP_OPTIONS_ENABLED,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PVPLOG_DATA,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ARENA_OPPONENT_SPECIALIZATIONS,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DESTROY_ARENA_UNIT,                              STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_POINTS,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_INIT,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MAP_OBJECTIVES_INIT,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CONQUEST_FORMULA_CONSTANTS,                      STATUS_LOGGEDIN,      PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PVP_SEASON,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_WARGAME_REQUEST_SUCCESSFULLY_SENT_TO_OPPENENT,   STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPIRIT_HEALER_CONFIRM,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AREA_SPIRIT_HEALER_TIME,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BATTLEFIELD_LIST,                                STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHECK_WARGAME_ENTRY,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Pet
        DEFINE_OPCODE_HANDLER(SMSG_PET_NAME_QUERY_RESPONSE,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_NAME_INVALID,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_ACTION_FEEDBACK,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_ACTION_SOUND,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_ADDED,                                       STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_DISMISS_SOUND,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_GUIDS,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_MODE,                                        STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_SLOT_UPDATED,                                STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_STABLE_LIST,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_PET_SPECIALIZATION,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_STABLE_RESULT,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Battle pet opcodes
        DEFINE_OPCODE_HANDLER(SMSG_BATTLE_PET_JOURNAL,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUERY_PET_NAME_RESPONSE,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_REQUEST_FAILED,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_FIRST_ROUND,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_FULL_UPDATE,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_FINALIZE_LOCATION,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_PVP_CHALLENGE,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_ROUND_RESULT,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_REPLACEMENTS_MADE,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_FINAL_ROUND,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETBATTLE_FINISHED,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_SLOT_UPDATES,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Instances
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_ENGAGE_UNIT,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_DISENGAGE_UNIT,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_CHANGE_PRIORITY,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_TIMER_START,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_START,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_COMPLETE,               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_START,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_OBJECTIVE_UPDATE,                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_END,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_IN_COMBAT_RESURRECTION,           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_PHASE_SHIFT_CHANGED,              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_ENCOUNTER_GAIN_COMBAT_RESURRECTION_CHARGE,  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_RESET,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_RESET_FAILED,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RAID_INSTANCE_INFO,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RAID_GROUP_ONLY,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RAID_MARKERS_CHANGED,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_SAVE_CREATED,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INSTANCE_GROUP_SIZE_CHANGED,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ENCOUNTER_END,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BOSS_KILL_CREDIT,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Scenarios
        DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_POI,                                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_PROGRESS_UPDATE,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SCENARIO_STATE,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Auction House
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_HELLO_RESPONSE,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_BIDDER_LIST_RESULT,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_BIDDER_NOTIFICATION,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_BUYOUT_NOTIFICATION,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_COMMAND_RESULT,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_LIST_PENDING_SALES,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_LIST_RESULT,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_OWNER_LIST_RESULT,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AUCTION_OWNER_BID_NOTIFICATION,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Mail
        DEFINE_OPCODE_HANDLER(SMSG_SEND_MAIL_RESULT,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MAIL_LIST_RESULT,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RECEIVED_MAIL,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MAIL_QUERY_NEXT_TIME_RESULT,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Trainers
        DEFINE_OPCODE_HANDLER(SMSG_TRAINER_LIST,                                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TRAINER_SERVICE,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Void Storage
        DEFINE_OPCODE_HANDLER(SMSG_VOID_ITEM_SWAP_RESPONSE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_CONTENTS,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_FAILED,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_VOID_STORAGE_TRANSFER_CHANGES,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_VOID_TRANSFER_RESULT,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Petition
        DEFINE_OPCODE_HANDLER(SMSG_PETITION_ALREADY_SIGNED,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETITION_SIGN_RESULTS,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUERY_PETITION_RESPONSE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETITION_SHOW_LIST,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETITION_SHOW_SIGNATURES,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_TURN_IN_PETITION_RESULTS,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PETITION_DECLINED,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Threat
        DEFINE_OPCODE_HANDLER(SMSG_THREAT_CLEAR,                                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_THREAT_REMOVE,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_THREAT_UPDATE,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_HIGHEST_THREAT_UPDATE,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Tickets
        DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_GET_TICKET_RESPONSE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_RESPONSE,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_RESPONSE_ERROR,                            STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_RESOLVE_RESPONSE,                          STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_SYSTEM_STATUS,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_UPDATE,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_STATUS_UPDATE,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GM_TICKET_CASE_STATUS,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Toybox
        DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_TOYS_UPDATE,                                 STATUS_NEVER,         PROCESS_THREADSAFE,   &WorldSession::Handle_ServerSide);

        /// Calendar
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_CLEAR_PENDING_ACTION,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_COMMAND_RESULT,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INITIAL_INVITE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_ALERT,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_NOTES,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_NOTES_ALERT,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_REMOVED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_REMOVED_ALERT,                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_STATUS,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_INVITE_STATUS_ALERT,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_MODERATOR_STATUS,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_REMOVED_ALERT,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_EVENT_UPDATED_ALERT,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_ADDED,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_REMOVED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_RAID_LOCKOUT_UPDATED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_CALENDAR,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_EVENT,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CALENDAR_SEND_NUM_PENDING,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Warden
        DEFINE_OPCODE_HANDLER(SMSG_WARDEN_DATA,                                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Challenges
        DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_REQUEST_LEADERS_RESULT,               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_MAP_STATS_UPDATE,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_NEW_PLAYER_RECORD,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_ALL_MAP_STATS,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_START,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_COMPLETE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MOD_REWARDS,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// BlackMarket
        DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_OPEN_RESULT,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_OUTBID,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_REQUEST_ITEMS_RESULT,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_BID_ON_ITEM_RESULT,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_BLACK_MARKET_WON,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Garrison
        DEFINE_OPCODE_HANDLER(SMSG_OPEN_SHIPMENT_NPCFROM_GOSSIP,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GET_SHIPMENT_INFO_RESPONSE,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GET_SHIPMENTS,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CREATE_SHIPMENT_RESPONSE,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Guild
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamGuild
        /// Petition
        DEFINE_OPCODE_HANDLER(SMSG_PETITION_RENAME,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Guild Finder
        DEFINE_OPCODE_HANDLER(SMSG_LFGUILD_BROWSE,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFGUILD_APPLICATIONS,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFGUILD_APPLICATIONS_LIST_CHANGED,               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFGUILD_COMMAND_RESULT,                          STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFGUILD_RECRUITS,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFGUILD_POST,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFGUILD_APPLICANT_LIST_CHANGED,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Bank
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_RANKS,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_LOG_QUERY_RESULT,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_REMAINING_WITHDRAW_MONEY,             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_QUERY_TEXT_RESULT,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_BANK_QUERY_RESULTS,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_PERMISSIONS_QUERY_RESULTS,                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Achievement
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_DELETED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_MEMBERS,                       STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_ACHIEVEMENT_EARNED,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_CRITERIA_UPDATE,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_CRITERIA_DELETED,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ALL_GUILD_ACHIEVEMENTS,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Info
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_ROSTER,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_ROSTER_UPDATE,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUERY_GUILD_INFO_RESPONSE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_UPDATE_NOTE,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_REWARDS_LIST,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_SEND_RANK_CHANGE,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHALLENGE_UPDATED,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_RECIPES,                            STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_INVITE_EXPIRED,                            STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_COMMAND_RESULT,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHALLENGE_COMPLETED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_REPUTATION_REACTION_CHANGED,               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_KNOWN_RECIPES,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBER_DAILY_RESET,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_FLAGGED_FOR_RENAME,                        STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_NAME_CHANGED,                              STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_RESET,                                     STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_CHANGE_NAME_RESULT,                        STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_INVITE,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_INVITE_DECLINED,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_PARTY_STATE,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_MEMBERS_WITH_RECIPE,                       STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_MOVE_STARTING,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_MOVED,                                     STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_NEWS,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_NEWS_DELETED,                              STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAYER_TABAR_VENDOR_SHOW,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAYER_SAVE_GUILD_EMBLEM,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Event system
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_NEW_LEADER,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_PRESENCE_CHANGE,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_PLAYER_JOINED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_PLAYER_LEFT,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_RANKS_UPDATED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_RANK_CHANGED,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_LOG_QUERY_RESULTS,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_MOTD,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_DISBANDED,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_TAB_ADDED,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_TAB_MODIFIED,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_TAB_DELETED,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_TAB_TEXT_CHANGED,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_CONTENTS_CHANGED,               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GUILD_EVENT_BANK_MONEY_CHANGED,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Chat
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamChat
        /// Voice chat
        DEFINE_OPCODE_HANDLER(SMSG_VOICE_SESSION_ROSTER_UPDATE,                     STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_VOICE_SESSION_LEAVE,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_VOICE_PARENTAL_CONTROLS,                         STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_VOICE_CHAT_STATUS,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AVAILABLE_VOICE_CHANNEL,                         STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Misc
        DEFINE_OPCODE_HANDLER(SMSG_SERVER_FIRST_ACHIEVEMENT,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RAID_INSTANCE_MESSAGE,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOTD,                                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_EXPECTED_SPAM_RECORDS,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DEFENSE_MESSAGE,                                 STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ZONE_UNDER_ATTACK,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_WHO,                                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Chat
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_SERVER_MESSAGE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_RESTRICTED,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_RECONNECT,                                  STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_PLAYER_NOTFOUND,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_PLAYER_AMBIGUOUS,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_NOT_IN_PARTY,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_IS_DOWN,                                    STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_IGNORED_ACCOUNT_MUTED,                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_DOWN,                                       STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT_AUTO_RESPONDED,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHAT,                                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        //DEFINE_OPCODE_HANDLER(SMSG_CHAT_PLAYER_NOT_FOUND,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Channel
        DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_NOTIFY_LEFT,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_NOTIFY_JOINED,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_NOTIFY,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CHANNEL_LIST,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_USERLIST_ADD,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_USERLIST_REMOVE,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_USERLIST_UPDATE,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Twitter
        DEFINE_OPCODE_HANDLER(SMSG_REQUEST_TWITTER_STATUS_RESPONSE,                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_OAUTH_SAVED_DATA,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Move
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamMove
        /// Move speeds
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_RUN_SPEED,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_RUN_BACK_SPEED,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_SWIM_SPEED,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_SWIM_BACK_SPEED,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_FLIGHT_SPEED,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_FLIGHT_BACK_SPEED,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_WALK_SPEED,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_TURN_RATE,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_PITCH_RATE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_KNOCK_BACK,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_RUN_SPEED,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_RUN_BACK_SPEED,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_SWIM_SPEED,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_SWIM_BACK_SPEED,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_FLIGHT_BACK_SPEED,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_FLIGHT_SPEED,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_WALK_SPEED,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_TURN_RATE,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_PITCH_RATE,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_KNOCK_BACK,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Player
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_TELEPORT,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_TELEPORT,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_ROOT,                                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNROOT,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_WATER_WALK,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_LAND_WALK,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_FEATHER_FALL,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_NORMAL_FALL,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_FLY,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_FLY,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_ACTIVE_MOVER,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_HOVER,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_HOVER,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_TURN_WHILE_FALLING,                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_TURN_WHILE_FALLING,               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY,    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_UNSET_CAN_TRANSITION_BETWEEN_SWIM_AND_FLY,  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_VEHICLE_REC_ID,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_COLLISION_HEIGHT,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_APPLY_MOVEMENT_FORCE,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UNAPPLY_MOVEMENT_FORCE,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_SKIP_TIME,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_ENABLE_GRAVITY,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_DISABLE_GRAVITY,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_ENABLE_COLLISION,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOVE_DISABLE_COLLISION,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Creature
        DEFINE_OPCODE_HANDLER(SMSG_MONSTER_MOVE,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_SPEED,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_BACK_SPEED,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_SWIM_SPEED,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_SWIM_BACK_SPEED,                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLIGHT_SPEED,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLIGHT_BACK_SPEED,               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WALK_SPEED,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_TURN_RATE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_PITCH_RATE,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_ROOT,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNROOT,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_GRAVITY_DISABLE,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_COLLISION_DISABLE,                   STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_COLLISION_ENABLE,                    STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_GRAVITY_ENABLE,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FEATHER_FALL,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_NORMAL_FALL,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_HOVER,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNSET_HOVER,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WATER_WALK,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_LAND_WALK,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_START_SWIM,                          STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_STOP_SWIM,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_RUN_MODE,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_WALK_MODE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_SET_FLYING,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPLINE_MOVE_UNSET_FLYING,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Spell
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamSpell
        /// Aura
        DEFINE_OPCODE_HANDLER(SMSG_AURA_POINTS_DEPLETED,                            STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AURA_UPDATE,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Misc
        DEFINE_OPCODE_HANDLER(SMSG_CHEAT_IGNORE_DIMISHING_RETURNS,                  STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DISPEL_FAILED,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_INCREASE_CAST_TIME_FOR_SPELL,                    STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MIRROR_IMAGE_COMPONENTED_DATA,                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MIRROR_IMAGE_CREATURE_DATA,                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_CLEAR_SPELLS,                                STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_REFRESH_SPELL_HISTORY,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RESUME_CAST_BAR,                                 STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CAST_FAILED,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_FAILURE,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_FAILED_OTHER,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_CAST_FAILED,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ITEM_COOLDOWN,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MODIFY_COOLDOWN,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_COOLDOWN_CHEAT,                                  STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_TAME_FAILURE,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_FEIGN_DEATH_RESISTED,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_NOTIFY_DEST_LOC_SPELL_CAST,                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA,            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_VEHICLE_REC_ID,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_COOLDOWN_EVENT,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DISMOUNT,                                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DISMOUNTRESULT,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_MOUNT_RESULT,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Spell Book / Bar
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_WEEKLY_SPELL_USAGE,                       STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_WEEKLY_SPELL_USAGE,                              STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SEND_KNOWN_SPELLS,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SEND_SPELL_CHARGES,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SEND_SPELL_HISTORY,                              STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SEND_UNLEARN_SPELLS,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CLEAR_ALL_SPELL_CHARGES,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CLEAR_COOLDOWN,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CLEAR_COOLDOWNS,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CATEGORY_COOLDOWN,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CLEAR_SPELL_CHARGES,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_FLAT_SPELL_MODIFIER,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_PCT_SPELL_MODIFIER,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SET_SPELL_CHARGES,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LEARNED_SPELL,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UNLEARNED_SPELLS,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Casting
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_CHANNEL_START,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_CHANNEL_UPDATE,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_COOLDOWN,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_DAMAGE_SHIELD,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_DELAYED,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_GO,                                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_MULTISTRIKE_EFFECT,                        STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_OR_DAMAGE_IMMUNE,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_START,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_UPDATE_CHAIN_TARGETS,                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Logging
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_DISPELL_LOG,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_ENERGIZE_LOG,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_EXECUTE_LOG,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_NON_MELEE_DAMAGE_LOG,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_HEAL_LOG,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_INSTAKILL_LOG,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_INTERRUPT_LOG,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_MISS_LOG,                                  STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_ENVIRONMENTAL_DAMAGE_LOG,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_PERIODIC_AURA_LOG,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_PROC_SCRIPT_LOG,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_PROCS_PER_MINUTE_LOG,                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_SPELL_CHANCE_PROC_LOG,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_RESIST_LOG,                                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_AURA_CAST_LOG,                                   STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Pet
        DEFINE_OPCODE_HANDLER(SMSG_PET_SPELLS_MESSAGE,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_UNLEARNED_SPELLS,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PET_LEARNED_SPELLS,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Visuals
        DEFINE_OPCODE_HANDLER(SMSG_PLAY_ORPHAN_SPELL_VISUAL,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CANCEL_ORPHAN_SPELL_VISUAL,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAY_SPELL_VISUAL,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CANCEL_SPELL_VISUAL,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_PLAY_SPELL_VISUAL_KIT,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_CANCEL_SPELL_VISUAL_KIT,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Quest
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamQuest
        /// Quest log
        DEFINE_OPCODE_HANDLER(SMSG_QUERY_QUEST_INFO_RESPONSE,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_FAILED_TIMER,                       STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_FAILED,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_COMPLETE,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_ADD_PVP_CREDIT,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_ADD_CREDIT_SIMPLE,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_UPDATE_ADD_CREDIT,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_PUSH_RESULT,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_POIQUERY_RESPONSE,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_LOG_FULL,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_IS_QUEST_COMPLETE_RESPONSE,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_FORCE_REMOVED,                             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_CONFIRM_ACCEPT,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_COMPLETION_NPCRESPONSE,                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_DAILY_QUESTS_RESET,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Quest giver
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_STATUS_MULTIPLE,                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_STATUS,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_REQUEST_ITEMS,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_QUEST_LIST_MESSAGE,                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_QUEST_FAILED,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_QUEST_DETAILS,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_QUEST_COMPLETE,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_OFFER_REWARD_MESSAGE,                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_QUEST_GIVER_INVALID_QUEST,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// Gossip
        DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_MESSAGE,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GOSSIP_COMPLETE,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Lfg
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamLFG
        /// LFG
        DEFINE_OPCODE_HANDLER(SMSG_LFG_BOOT_PLAYER,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_DISABLED,                                    STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_JOIN_RESULT,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_OFFER_CONTINUE,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_OPEN_FROM_GOSSIP,                            STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_PARTY_INFO,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_PLAYER_INFO,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_PLAYER_REWARD,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_PROPOSAL_UPDATE,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_QUEUE_STATUS,                                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_ROLE_CHECK_UPDATE,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_ROLE_CHOSEN,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_SEARCH_RESULT,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_SLOT_INVALID,                                STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_TELEPORT_DENIED,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_LIST,                                 STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_PARTY,                                STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_PLAYER,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_SEARCH,                               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_STATUS,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_UPDATE_STATUS_NONE,                          STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_UPDATE_DUNGEON_ENCOUNTER_FOR_LOOT,               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

        /// LFG List
        DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_JOIN_RESULT,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_UPDATE_BLACKLIST,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_UPDATE_STATUS,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_LFG_LIST_SEARCH_RESULT,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    /// Jam Client Garrison
    //////////////////////////////////////////////////////////////////////////
    #pragma region JamGarrison
        DEFINE_OPCODE_HANDLER(SMSG_GET_GARRISON_INFO_RESULT,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_PLOT_PLACED,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_PLOT_REMOVED,                                     STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_PLACE_BUILDING_RESULT,                            STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_BUILDING_REMOVED,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_LEARN_SPECIALIZATION_RESULT,                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_BUILDING_SET_ACTIVE_SPECIALIZATION_RESULT,        STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_LEARN_BLUEPRINT_RESULT,                           STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_BLUEPRINT_AND_SPECIALIZATION_DATA,                STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_GET_BUILDINGS_DATA,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_REMOTE_INFO,                                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_BUILDING_ACTIVATED,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_UPGRADE_RESULT,                                   STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_ADD_FOLLOWER_RESULT,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_REMOVE_FOLLOWER_RESULT,                           STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_LIST_FOLLOWERS_CHEAT_RESULT,                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_UPDATE_FOLLOWER_ACTIVATION_COUNT,                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_UPDATE_FOLLOWER,                                  STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_FOLLOWER_CHANGED_XP,                              STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_ADD_MISSION_RESULT,                               STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_FOLLOWER_CHANGED_ITEM_LEVEL,                      STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_START_MISSION_RESULT,                             STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_COMPLETE_MISSION_RESULT,                          STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_MISSION_BONUS_ROLL_RESULT,                        STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING_RESULT,               STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING_RESULT,             STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_DELETE_RESULT,                                    STATUS_UNHANDLED,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_OPEN_ARCHITECT,                                   STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_OPEN_MISSION_NPC,                                 STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
        DEFINE_OPCODE_HANDLER(SMSG_GARRISON_REQUEST_UPGRADEABLE_RESULT,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Low protocol
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_HANDSHAKE,                                       STATUS_NEVER,       PROCESS_INPLACE,        &WorldSession::Handle_EarlyProccess             );
    DEFINE_OPCODE_HANDLER(SMSG_HANDSHAKE,                                       STATUS_NEVER,       PROCESS_INPLACE,        &WorldSession::Handle_ServerSide                );

    //////////////////////////////////////////////////////////////////////////
    /// Garrison
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_CREATE_CHEAT,                               STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_DELETE_CHEAT,                               STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_LEVEL_CHEAT,                            STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_PLACE_BUILDING_CHEAT,                       STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REMOVE_BUILDING_CHEAT,                      STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_UPGRADE_BUILDING_CHEAT,                     STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_CANCEL_CONSTRUCTION,                        STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonCancelConstructionOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_LEARN_BUILDING_SPECIALIZATION_CHEAT,        STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_ACTIVE_BUILDING_SPECIALIZATION,         STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_ACTIVE_BUILDING_SPECIALIZATION_CHEAT,   STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GET_GARRISON_INFO,                                   STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGetGarrisonInfoOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_LEARN_BLUEPRINT_CHEAT,                      STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REQUEST_BUILDINGS,                          STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonRequestBuildingsOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_PURCHASE_BUILDING,                          STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonPurchaseBuildingOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_BUILDING_ACTIVE,                        STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_FORCE_BUILDING_ACTIVE,                      STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_PORT_CHEAT,                                 STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_ADD_FOLLOWER_XPCHEAT,                       STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_SET_FOLLOWER_ITEM_LEVEL_CHEAT,              STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_ASSIGN_FOLLOWER_TO_BUILDING,                STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REMOVE_FOLLOWER_FROM_BUILDING,              STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_CHANGE_FOLLOWER_ACTIVATION_STATE,           STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonChangeFollowerActivationStateOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_SHOW_GARRISON_TRANSFER,                              STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_ADD_UNIQUE_FOLLOWER_CHEAT,                  STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REMOVE_FOLLOWER_CHEAT,                      STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_LIST_FOLLOWERS_CHEAT,                       STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_ADD_MISSION_CHEAT,                          STATUS_UNHANDLED,   PROCESS_THREADSAFE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_START_MISSION,                              STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonStartMissionOpcode );
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_COMPLETE_MISSION,                           STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonCompleteMissionOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_MISSION_BONUS_ROLL,                         STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonMissionBonusRollOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REQUEST_UPGRADEABLE,                        STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleRequestGarrisonUpgradeableOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_UPGRADE_GARRISON,                                    STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleUpgradeGarrisonOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_REQUEST_LANDING_PAGE_SHIPMENT_INFO,         STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleRequestLandingPageShipmentInfoOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GARRISON_MISSION_NPC_HELLO,                          STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonMissionNPCHelloOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GET_SHIPMENT_INFO,                                   STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonGetShipmentInfoOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_CREATE_SHIPMENT,                                     STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonCreateShipmentOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GET_SHIPMENTS,                                       STATUS_AUTHED,      PROCESS_THREADSAFE,        &WorldSession::HandleGarrisonGetShipmentsOpcode);

    //////////////////////////////////////////////////////////////////////////
    /// User Router
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_LOG_STREAMING_ERROR,                             STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_QUEUED_MESSAGES_END,                             STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_LOG_DISCONNECT,                                  STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::Handle_EarlyProccess             );
    DEFINE_OPCODE_HANDLER(CMSG_PING,                                            STATUS_NEVER,       PROCESS_INPLACE,        &WorldSession::Handle_EarlyProccess             );
    DEFINE_OPCODE_HANDLER(CMSG_AUTH_CONTINUED_SESSION,                          STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_SUSPEND_TOKEN_RESPONSE,                          STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_AUTH_SESSION,                                    STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::Handle_EarlyProccess             );
    DEFINE_OPCODE_HANDLER(CMSG_ENABLE_NAGLE,                                    STATUS_NEVER,       PROCESS_INPLACE,        &WorldSession::Handle_EarlyProccess             );
    DEFINE_OPCODE_HANDLER(CMSG_SUSPEND_COMMS_ACK,                               STATUS_NEVER,       PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_KEEP_ALIVE,                                      STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::Handle_EarlyProccess             );
    DEFINE_OPCODE_HANDLER(CMSG_OBJECT_UPDATE_FAILED,                            STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleObjectUpdateFailedOpcode   );
    DEFINE_OPCODE_HANDLER(CMSG_OBJECT_UPDATE_RESCUED,                           STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );

    //////////////////////////////////////////////////////////////////////////
    /// Character
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_ENUM_CHARACTERS,                                 STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleCharEnumOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_CREATE_CHARACTER,                                STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleCharCreateOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_DELETE,                                     STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleCharDeleteOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_REORDER_CHARACTERS,                              STATUS_AUTHED,      PROCESS_INPLACE,        &WorldSession::HandleReorderCharacters          );
    DEFINE_OPCODE_HANDLER(CMSG_PLAYER_LOGIN,                                    STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandlePlayerLoginOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_VIOLENCE_LEVEL,                                  STATUS_AUTHED,      PROCESS_INPLACE,        &WorldSession::HandleViolenceLevel              );
    DEFINE_OPCODE_HANDLER(CMSG_LOAD_SCREEN,                                     STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleLoadScreenOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_ACCOUNT_DATA,                            STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleRequestAccountData         );
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_ACCOUNT_DATA,                             STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleUpdateAccountData          );
    DEFINE_OPCODE_HANDLER(CMSG_SET_DUNGEON_DIFFICULTY,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetDungeonDifficultyOpcode );
    DEFINE_OPCODE_HANDLER(CMSG_SET_RAID_DIFFICULTY,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetRaidDifficultyOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_AUTO_DECLINE_GUILD_INVITES,                      STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleAutoDeclineGuildInvites    );
    DEFINE_OPCODE_HANDLER(CMSG_SHOWING_CLOAK,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleShowingCloakOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_SHOWING_HELM,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleShowingHelmOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTIVE_MOVER,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetActiveMoverOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_LEARN_TALENTS,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLearnTalents               );
    DEFINE_OPCODE_HANDLER(CMSG_AUTOEQUIP_ITEM,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAutoEquipItemOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_SWAP_INV_ITEM,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSwapInvItemOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_SWAP_ITEM,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSwapItem                   );
    DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_BAG_ITEM,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAutoStoreBagItemOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PET_INFO,                                STATUS_UNHANDLED,   PROCESS_THREADUNSAFE,   &WorldSession::HandleRequestPetInfoOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_SAVE_CUF_PROFILES,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSaveCUFProfiles            );
    DEFINE_OPCODE_HANDLER(CMSG_STAND_STATE_CHANGE,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleStandStateChangeOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_BINDER_ACTIVATE,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBinderActivateOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_FORCED_REACTIONS,                        STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleForcedReactionsOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_DESTROY_ITEM,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleDestroyItemOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_OPEN_ITEM,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleOpenItemOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_SET_TITLE,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetTitleOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_PLAYED_TIME,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePlayedTime                 );
    DEFINE_OPCODE_HANDLER(CMSG_SAVE_EQUIPMENT_SET,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleEquipmentSetSave           );
    DEFINE_OPCODE_HANDLER(CMSG_USE_EQUIPMENT_SET,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleEquipmentSetUse            );
    DEFINE_OPCODE_HANDLER(CMSG_DELETE_EQUIPMENT_SET,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleEquipmentSetDelete         );
    DEFINE_OPCODE_HANDLER(CMSG_WHO,                                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleWhoOpcode                  );
    DEFINE_OPCODE_HANDLER(CMSG_SOCKET_GEMS,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSocketOpcode               );
    DEFINE_OPCODE_HANDLER(CMSG_RESURRECT_RESPONSE,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleResurrectResponseOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_INSPECT_ACHIEVEMENTS,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQueryInspectAchievements   );
    DEFINE_OPCODE_HANDLER(CMSG_SPLIT_ITEM,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSplitItemOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_SET_PLAYER_DECLINED_NAMES,                       STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleSetPlayerDeclinedNames     );
    DEFINE_OPCODE_HANDLER(CMSG_SORT_BAGS,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSortBags                   );
    DEFINE_OPCODE_HANDLER(CMSG_MOUNT_SET_FAVORITE,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMountSetFavoriteOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_RENAME,                                     STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleCharRenameOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_CUSTOMIZE,                                  STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleCharCustomize              );
    DEFINE_OPCODE_HANDLER(CMSG_CHAR_RACE_OR_FACTION_CHANGE,                     STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleCharRaceOrFactionChange    );
    DEFINE_OPCODE_HANDLER(CMSG_GET_MIRRORIMAGE_DATA,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMirrorImageDataRequest     );
    DEFINE_OPCODE_HANDLER(CMSG_SHOW_TRADE_SKILL,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleShowTradeSkillOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RESEARCH_HISTORY,                        STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleArcheologyRequestHistory   );

    //////////////////////////////////////////////////////////////////////////
    /// Bank
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_BANKER_ACTIVATE,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBankerActivateOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_AUTOBANK_ITEM,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAutoBankItemOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_BANK_ITEM,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAutoStoreBankItemOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_BUY_BANK_SLOT,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBuyBankSlotOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_BUY_REAGENT_BANK,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBuyReagentBankOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_SORT_BANK_BAGS,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSortReagentBankBagsOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_SORT_REAGENT_BANK_BAGS,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSortReagentBankBagsOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_DEPOSIT_ALL_REAGENTS,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleDepositAllReagentsOpcode   );

    //////////////////////////////////////////////////////////////////////////
    /// Auction House
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_BIDDER_ITEMS,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAuctionListBidderItems     );
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_ITEMS,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAuctionListItems           );
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_OWNER_ITEMS,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAuctionListOwnerItems      );
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_LIST_PENDING_SALES,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAuctionListPendingSales    );
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_PLACE_BID,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAuctionPlaceBid            );
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_REMOVE_ITEM,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAuctionRemoveItem          );
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_SELL_ITEM,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAuctionSellItem            );

    //////////////////////////////////////////////////////////////////////////
    /// Loot
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_LOOT,                                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLootOpcode                 );
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_MONEY,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLootMoneyOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_ITEM,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLootItemOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_RELEASE,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLootReleaseOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_CURRENCY,                                   STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_LOOT_ROLL,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLootRoll                   );
    DEFINE_OPCODE_HANDLER(CMSG_MASTER_LOOT_ITEM,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMasterLootItemOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_DO_MASTER_LOOT_ROLL,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleDoMasterLootRollOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_SET_LOOT_SPECIALIZATION,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetLootSpecialization      );
    DEFINE_OPCODE_HANDLER(CMSG_SET_LOOT_METHOD,                                 STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleSetLootMethodOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_OPT_OUT_OF_LOOT,                                 STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleOptOutOfLootOpcode         );

    //////////////////////////////////////////////////////////////////////////
    /// Combat
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_ATTACKSTOP,                                      STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleAttackStopOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_ATTACKSWING,                                     STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleAttackSwingOpcode          );

    //////////////////////////////////////////////////////////////////////////
    /// Duel
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_DUEL_PROPOSED,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSendDuelRequest            );
    DEFINE_OPCODE_HANDLER(CMSG_DUEL_RESPONSE,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleDuelResponseOpcode         );

    //////////////////////////////////////////////////////////////////////////
    /// Spell
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_CAST_SPELL,                                      STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleCastSpellOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_CAST,                                     STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleCancelCastOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_USE_ITEM,                                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleUseItemOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_AURA,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleCancelAuraOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_AUTO_REPEAT_SPELL,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleCancelAutoRepeatSpellOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_CHANNELLING,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleCancelChanneling           );
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_GROWTH_AURA,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleCancelGrowthAuraOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_MOUNT_AURA,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleCancelMountAuraOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_QUEUED_SPELL,                             STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );

    //////////////////////////////////////////////////////////////////////////
    /// Cache
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_HOTFIX,                                  STATUS_AUTHED,      PROCESS_INPLACE,        &WorldSession::HandleRequestHotfix              );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_CREATURE,                                  STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleCreatureQueryOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_NPC_TEXT_QUERY,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleNpcTextQueryOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_NAME_QUERY,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleNameQueryOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_QUERY,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestQueryOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_POI_QUERY,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestPOIQuery              );
    DEFINE_OPCODE_HANDLER(CMSG_REALM_NAME_QUERY,                                STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleRealmQueryNameOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_GAMEOBJECT_QUERY,                                STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleGameObjectQueryOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_QUERY,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetitionQueryOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_INFO,                                STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleQueryGuildInfoOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_PAGE_TEXT_QUERY,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePageTextQueryOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_TEXT_QUERY,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleItemTextQuery              );

    //////////////////////////////////////////////////////////////////////////
    /// Interaction
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_LOGOUT_REQUEST,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLogoutRequestOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_LOGOUT_CANCEL,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLogoutCancelOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTION_BUTTON,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetActionButtonOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTIONBAR_TOGGLES,                           STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleSetActionBarToggles        );
    DEFINE_OPCODE_HANDLER(CMSG_SET_SELECTION,                                   STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleSetSelectionOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_SETSHEATHED,                                     STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleSetSheathedOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_GOSSIP_HELLO,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGossipHelloOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_GOSSIP_SELECT_OPTION,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGossipSelectOptionOpcode   );
    DEFINE_OPCODE_HANDLER(CMSG_TOGGLE_PVP,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTogglePvP                  );
    DEFINE_OPCODE_HANDLER(CMSG_TUTORIAL,                                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTutorial                   );
    DEFINE_OPCODE_HANDLER(CMSG_SET_SPECIALIZATION,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetSpecialization          );
    DEFINE_OPCODE_HANDLER(CMSG_SET_FACTION_INACTIVE,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetFactionInactiveOpcode   );
    DEFINE_OPCODE_HANDLER(CMSG_SET_WATCHED_FACTION,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetWatchedFactionOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_AREATRIGGER,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAreaTriggerOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_GAMEOBJECT_REPORT_USE,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGameobjectReportUse        );
    DEFINE_OPCODE_HANDLER(CMSG_GAMEOBJECT_USE,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGameObjectUseOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_SPELLCLICK,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSpellClick                 );
    DEFINE_OPCODE_HANDLER(CMSG_REPOP_REQUEST,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRepopRequestOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_RECLAIM_CORPSE,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleReclaimCorpseOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_CORPSE_LOCATION_FROM_CLIENT,               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleCorpseLocationFromClientQueryOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_CORPSE_TRANSPORT,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleCorpseTransportQueryOpcode );
    DEFINE_OPCODE_HANDLER(CMSG_RETURN_TO_GRAVEYARD,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleReturnToGraveyard          );
    DEFINE_OPCODE_HANDLER(CMSG_CLOSE_INTERACTION,                               STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_REFUND_INFO,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleItemRefundInfoRequest      );
    DEFINE_OPCODE_HANDLER(CMSG_FAR_SIGHT,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleFarSightOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_MOUNTSPECIAL_ANIM,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMountSpecialAnimOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_OPENING_CINEMATIC,                               STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_NEXT_CINEMATIC_CAMERA,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleNextCinematicCamera        );
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_CINEMATIC,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleCompleteCinematic          );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CEMETERY_LIST,                           STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleCemeteryListOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_TOTEM_DESTROYED,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTotemDestroyed             );
    DEFINE_OPCODE_HANDLER(CMSG_CONFIRM_RESPEC_WIPE,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTalentWipeConfirmOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_CANCEL_TRADE,                                    STATUS_LOGGEDIN_OR_RECENTLY_LOGGOUT, PROCESS_THREADUNSAFE, &WorldSession::HandleCancelTradeOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_CURRENCY,                              STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_GOLD,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetTradeGoldOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_SET_TRADE_ITEM,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetTradeItemOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_CLEAR_TRADE_ITEM,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleClearTradeItemOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_TRADE,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAcceptTradeOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_BUSY_TRADE,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBusyTradeOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_BEGIN_TRADE,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBeginTradeOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_IGNORE_TRADE,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleIgnoreTradeOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_INITIATE_TRADE,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleInitiateTradeOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_UNACCEPT_TRADE,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleUnacceptTradeOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_NEUTRAL_PLAYER_SELECT_FACTION,                   STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleSetFactionOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_INSPECT,                                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleInspectOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_INSPECT_HONOR_STATS,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleInspectHonorStatsOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_INSPECT_RATED_BG_STATS,                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleInspectRatedBGStatsOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_TIME_SYNC_RESP,                                  STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleTimeSyncResp               );
    DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_SKILL,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleUnlearnSkillOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_EMOTE,                                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleEmoteOpcode                );
    DEFINE_OPCODE_HANDLER(CMSG_ALTER_APPEARANCE,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAlterAppearance            );
    DEFINE_OPCODE_HANDLER(CMSG_SELF_RES,                                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSelfResOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_SEND_TEXT_EMOTE,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTextEmoteOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_READ_ITEM,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleReadItem                   );
    DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_MOVIE,                                  STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleCompleteMovieOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_SCENE_TRIGGER_EVENT,                             STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleSceneTriggerEventOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_SCENE_PLAYBACK_CANCELED,                         STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleSceneCancelOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_SUMMON_RESPONSE,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSummonResponseOpcode       );

    //////////////////////////////////////////////////////////////////////////
    /// Vehicles
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_PLAYER_VEHICLE_ENTER,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleEnterPlayerVehicle         );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_EXIT,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRequestVehicleExit         );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_NEXT_SEAT,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChangeSeatsOnControlledVehicle);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_PREV_SEAT,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChangeSeatsOnControlledVehicle);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_VEHICLE_SWITCH_SEAT,                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChangeSeatsOnControlledVehicle);
    DEFINE_OPCODE_HANDLER(CMSG_SET_VEHICLE_REC_ID_ACK,                          STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE,              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChangeSeatsOnControlledVehicle);
    DEFINE_OPCODE_HANDLER(CMSG_EJECT_PASSENGER,                                 STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleEjectPassenger             );
    DEFINE_OPCODE_HANDLER(CMSG_DISMISS_CONTROLLED_VEHICLE,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleDismissControlledVehicle   );

    //////////////////////////////////////////////////////////////////////////
    /// Vendors
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_LIST_INVENTORY,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleListInventoryOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_REPAIR_ITEM,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRepairItemOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_BUYBACK_ITEM,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBuybackItem                );
    DEFINE_OPCODE_HANDLER(CMSG_BUY_ITEM,                                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBuyItemOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_SELL_ITEM,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSellItemOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_ITEM_REFUND,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleItemRefund                 );

    //////////////////////////////////////////////////////////////////////////
    /// Taxi
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_SET_TAXI_BENCHMARK_MODE,                         STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleSetTaxiBenchmarkOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_ENABLE_TAXI_NODE,                                STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleTaxiQueryAvailableNodes    );
    DEFINE_OPCODE_HANDLER(CMSG_TAXI_HELLO,                                      STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleTaxiQueryAvailableNodes    );
    DEFINE_OPCODE_HANDLER(CMSG_ACTIVATE_TAXI,                                   STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleActivateTaxiOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_ACTIVATE_TAXI_EXPRESS,                           STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleActivateTaxiExpressOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_TAXI_NODE_STATUS_QUERY,                          STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleTaxiNodeStatusQueryOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_TAXI_REQUEST_EARLY_LANDING,                      STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleTaxiRequestEarlyLandingOpcode);

    //////////////////////////////////////////////////////////////////////////
    /// Quest
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_HELLO,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestgiverHelloOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_GIVER_STATUS_QUERY,                        STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleQuestgiverStatusQueryOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_STATUS_MULTIPLE_QUERY,                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestgiverStatusMultipleQuery);
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_QUERY_QUEST,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestgiverQueryQuestOpcode );
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_COMPLETE_QUEST,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestgiverCompleteQuest    );
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_CHOOSE_REWARD,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestgiverChooseRewardOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_ACCEPT_QUEST,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestgiverAcceptQuestOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_QUESTGIVER_REQUEST_REWARD,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestgiverRequestRewardOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_CONFIRM_ACCEPT,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestConfirmAccept         );
    DEFINE_OPCODE_HANDLER(CMSG_QUESTLOG_REMOVE_QUEST,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestLogRemoveQuest        );
    DEFINE_OPCODE_HANDLER(CMSG_QUEST_PUSH_RESULT,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQuestPushResult            );
    DEFINE_OPCODE_HANDLER(CMSG_PUSH_QUEST_TO_PARTY,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePushQuestToParty           );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_QUEST_COMPLETION_NPCS,                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQueryQuestCompletionNpcs   );

    //////////////////////////////////////////////////////////////////////////
    /// Account data
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_GET_UNDELETE_CHARACTER_COOLDOWN_STATUS,          STATUS_AUTHED,      PROCESS_THREADUNSAFE,   &WorldSession::HandleUndeleteCharacter          );

    //////////////////////////////////////////////////////////////////////////
    /// Chat
    //////////////////////////////////////////////////////////////////////////
    /// Addon chat
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_INSTANCE_CHAT,                STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::HandleAddonMessagechatOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_WHISPER,                      STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::HandleAddonMessagechatOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_GUILD,                        STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::HandleAddonMessagechatOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_OFFICER,                      STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::HandleAddonMessagechatOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_RAID,                         STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::HandleAddonMessagechatOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_ADDON_MESSAGE_PARTY,                        STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::HandleAddonMessagechatOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_ADDON_REGISTERED_PREFIXES,                       STATUS_NEVER,       PROCESS_THREADUNSAFE,   &WorldSession::HandleAddonRegisteredPrefixesOpcode);

    /// Chat
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_RAID_WARNING,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_PARTY,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_YELL,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_SAY,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_OFFICER,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_EMOTE,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_GUILD,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_RAID,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_AFK,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_CHANNEL,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_DND,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_CHAT_MESSAGE_WHISPER,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMessagechatOpcode          );

    /// Channel
    DEFINE_OPCODE_HANDLER(CMSG_JOIN_CHANNEL,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleJoinChannel                );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_ANNOUNCEMENTS,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelAnnouncements       );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_BAN,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelBan                 );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_DISPLAY_LIST,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelDisplayListQuery    );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_INVITE,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelInvite              );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_KICK,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelKick                );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_LIST,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelList                );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_MODERATOR,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelModerator           );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_MUTE,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelMute                );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_OWNER,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelOwner               );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_PASSWORD,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelPassword            );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_SET_OWNER,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelSetOwner            );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_SILENCE_ALL,                             STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_SILENCE_VOICE,                           STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNBAN,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelUnban               );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNMODERATOR,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelUnmoderator         );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNMUTE,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelUnmute              );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNSILENCE_ALL,                           STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_UNSILENCE_VOICE,                         STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_VOICE_OFF,                               STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_CHANNEL_VOICE_ON,                                STATUS_UNHANDLED,   PROCESS_THREADUNSAFE,   &WorldSession::HandleChannelVoiceOnOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_DECLINE_CHANNEL_INVITE,                          STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleChannelDeclineInvite       );
    DEFINE_OPCODE_HANDLER(CMSG_LEAVE_CHANNEL,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLeaveChannel               );
    DEFINE_OPCODE_HANDLER(CMSG_SET_ACTIVE_VOICE_CHANNEL,                        STATUS_UNHANDLED,   PROCESS_THREADUNSAFE,   &WorldSession::HandleSetActiveVoiceChannel      ); // STATUS_AUTHED

    //////////////////////////////////////////////////////////////////////////
    /// Movement
    //////////////////////////////////////////////////////////////////////////
    /// Start
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_FORWARD,                              STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_TURN_LEFT,                            STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_TURN_RIGHT,                           STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_BACKWARD,                             STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_STRAFE_LEFT,                          STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_STRAFE_RIGHT,                         STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_ASCEND,                               STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_DESCEND,                              STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_PITCH_UP,                             STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_PITCH_DOWN,                           STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_SWIM,                                 STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );

    /// Misc
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_JUMP,                                       STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FALL_LAND,                                  STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_WORLD_PORT_RESPONSE,                             STATUS_TRANSFER,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMoveWorldportAckOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TIME_SKIPPED,                               STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleMoveTimeSkippedOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SPLINE_DONE,                                STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMoveSplineDoneOpcode       );

    /// Update
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_HEARTBEAT,                                  STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_FACING,                                 STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_PITCH,                                  STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_RUN_MODE,                               STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_WALK_MODE,                              STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_TELEPORT_ACK,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMoveTeleportAck            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_CHNG_TRANSPORT,                             STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );

    /// Stop
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP,                                       STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_TURN,                                  STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_STRAFE,                                STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_SWIM,                                  STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_ASCEND,                                STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_PITCH,                                 STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );

    /// Ack
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_RUN_SPEED_CHEAT,                        STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_RUN_BACK_SPEED_CHEAT,                   STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_WALK_SPEED_CHEAT,                       STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_SWIM_SPEED_CHEAT,                       STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_SWIM_BACK_SPEED_CHEAT,                  STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_FLIGHT_SPEED_CHEAT,                     STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_FLIGHT_BACK_SPEED_CHEAT,                STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_TURN_SPEED_CHEAT,                       STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_PITCH_SPEED_CHEAT,                      STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_KNOCK_BACK_ACK,                             STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMoveKnockBackAck           );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_CAN_FLY_ACK,                            STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_FEATHER_FALL_ACK,                           STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_WATER_WALK_ACK,                             STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );
    DEFINE_OPCODE_HANDLER(CMSG_MOVE_HOVER_ACK,                                  STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandleMovementOpcodes            );

    //////////////////////////////////////////////////////////////////////////
    /// Group
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_PARTY_INVITE,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGroupInviteOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_PARTY_INVITE_RESPONSE,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGroupInviteResponseOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_LEAVE_GROUP,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLeaveGroupOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_PARTY_UNINVITE,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePartyUninviteOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_SET_PARTY_LEADER,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetPartyLeaderOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_SET_ROLE,                                        STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleSetRoleOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_MINIMAP_PING,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMinimapPingOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_RANDOM_ROLL,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRandomRollOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_UPDATE_RAID_TARGET,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleUpdateRaidTargetOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_CONVERT_RAID,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGroupRaidConvertOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_DO_READY_CHECK,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRaidLeaderReadyCheck       );
    DEFINE_OPCODE_HANDLER(CMSG_READY_CHECK_RESPONSE,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRaidConfirmReadyCheck      );
    DEFINE_OPCODE_HANDLER(CMSG_INITIATE_ROLE_POLL,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRolePollBegin              );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RAID_INFO,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRequestRaidInfoOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_SET_EVERYONE_IS_ASSISTANT,                       STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleGroupEveryoneIsAssistantOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_CLEAR_RAID_MARKER,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleClearRaidMarkerOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_ASSISTANT_LEADER,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGroupAssistantLeaderOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_ASSIGNMENT,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePartyAssignmentOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_CHANGE_SUB_GROUP,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGroupChangeSubGroupOpcode );
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_REQUEST_JOIN_UPDATES,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRequestJoinUpdates        );
    DEFINE_OPCODE_HANDLER(CMSG_GROUP_SWAP_SUB_GROUP,                            STATUS_UNHANDLED,   PROCESS_THREADUNSAFE,   &WorldSession::HandleGroupSwapSubGroupOpcode   );

    //////////////////////////////////////////////////////////////////////////
    /// Friend
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_SEND_CONTACT_LIST,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleContactListOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_ADD_FRIEND,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAddFriendOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_ADD_IGNORE,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAddIgnoreOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_SET_CONTACT_NOTES,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetContactNotesOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_DEL_FRIEND,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleDelFriendOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_DEL_IGNORE,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleDelIgnoreOpcode            );

    //////////////////////////////////////////////////////////////////////////
    /// Pet
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_PET_NAME_QUERY,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetNameQuery               );
    DEFINE_OPCODE_HANDLER(CMSG_PET_RENAME,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetRename                  );
    DEFINE_OPCODE_HANDLER(CMSG_PET_ABANDON,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetAbandon                 );
    DEFINE_OPCODE_HANDLER(CMSG_PET_ACTION,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetAction                  );
    DEFINE_OPCODE_HANDLER(CMSG_PET_CANCEL_AURA,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetCancelAuraOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_PET_CAST_SPELL,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetCastSpellOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_PET_SET_ACTION,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetSetAction               );
    DEFINE_OPCODE_HANDLER(CMSG_PET_STOP_ATTACK,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetStopAttack              );
    DEFINE_OPCODE_HANDLER(CMSG_LEARN_PET_SPECIALIZATION_GROUP,                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLearnPetSpecialization     );
    DEFINE_OPCODE_HANDLER(CMSG_LIST_STABLE_PETS,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleListStabledPetsOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_SET_PET_SLOT,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleStableSetPetSlot           );

    //////////////////////////////////////////////////////////////////////////
    /// Battle grounds
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBattlemasterJoinOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN_ARENA,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBattlemasterJoinArena          );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_JOIN_ARENA_SKIRMISH,                STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleBattlemasterJoinArenaSkirmish  );
    DEFINE_OPCODE_HANDLER(CMSG_JOIN_RATED_BATTLEGROUND,                         STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleBattlemasterJoinRated          );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_PORT,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBattleFieldPortOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_BATTLEFIELD_STATUS,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBattlefieldStatusOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_REQUEST_SCORE_DATA,                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBattleFieldRequestScoreData    );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PVP_REWARDS,                             STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleRequestPvpReward               );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PVP_OPTIONS_ENABLED,                     STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::HandleRequestPvpOptions              );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_COUNTDOWN_TIMER,                           STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                          );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CONQUEST_FORMULA_CONSTANTS,              STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleRequestConquestFormulaConstants);
    DEFINE_OPCODE_HANDLER(CMSG_LEAVE_BATTLEFIELD,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLeaveBattlefieldOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_SPIRIT_HEALER_ACTIVATE,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSpiritHealerActivateOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_AREA_SPIRIT_HEALER_QUERY,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAreaSpiritHealerQueryOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_AREA_SPIRIT_HEALER_QUEUE,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAreaSpiritHealerQueueOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MGR_QUEUE_REQUEST,                   STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleBfQueueRequest                 );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MGR_QUEUE_INVITE_RESPONSE,           STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleBfQueueInviteResponse          );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MGR_QUEUE_EXIT_REQUEST,              STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleBfExitQueueRequest             );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MGR_ENTRY_INVITE_RESPONSE,           STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleBfEntryInviteResponse          );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_LIST,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBattlefieldListOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_LEAVE,                               STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleBfExitRequest                  );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_RATED_BATTLEFIELD_INFO,                  STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleRequestRatedBgStats            );
    DEFINE_OPCODE_HANDLER(CMSG_START_WAR_GAME,                                  STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleStartWarGame                   );
    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_WARGAME_INVITE,                           STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleAcceptWarGameInvite            );

    //////////////////////////////////////////////////////////////////////////
    /// Guild
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_INVITE_BY_NAME,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildInviteByNameOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_GUILD_INVITE,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAcceptGuildInviteOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DECLINE_INVITATION,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildDeclineInvitationsOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_OFFICER_REMOVE_MEMBER,                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildOfficierRemoveMemberOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_GUILD_MASTER,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildSetGuildMasterOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_LEAVE,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildLeaveOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DELETE,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildDeleteOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_UPDATE_MOTD_TEXT,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildUpdateMOTDTextOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_UPDATE_INFO_TEXT,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildUpdateInfoTextOpcode  );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_GET_RANKS,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildGetRanksOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ADD_RANK,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildAddRankOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_DELETE_RANK,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildDeleteRankOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_RANK_PERMISSIONS,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildSetRankPermissionsOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SHIFT_RANK,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleShiftRanks                 );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_ASSIGN_MEMBER_RANK,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildAssignRankOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_GET_ROSTER,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildRosterOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_ACTIVATE,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankActivate          );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_BUY_TAB,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankBuyTab            );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_DEPOSIT_MONEY,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankDepositMoney      );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_LOG_QUERY,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankLogQuery          );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_REMAINING_WITHDRAW_MONEY_QUERY,       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankRemainingWithdrawMoneyQueryOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_QUERY_TAB,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankQueryTab          );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_TEXT_QUERY,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQueryGuildBankTextQuery    );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_SWAP_ITEMS,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankSwapItems         );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_UPDATE_TAB,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankUpdateTab         );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_WITHDRAW_MONEY,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildBankWithdrawMoney     );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_BANK_SET_TAB_TEXT,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetGuildBankTabText        );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_GET_ACHIEVEMENT_MEMBERS,                   STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_FOCUSED_ACHIEVEMENT,                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildAchievementProgressQuery);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_ACHIEVEMENT_TRACKING,                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_SET_MEMBER_NOTE,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildSetMemberNoteOpcode   );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_CHALLENGE_UPDATE_REQUEST,                  STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleGuildChallengeUpdateRequest);
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_GUILD_PARTY_STATE,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRequestGuildPartyState     );
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_GUILD_REWARDS_LIST,                      STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleRequestGuildRewardsListOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_REPLACE_GUILD_MASTER,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildMasterReplaceOpcode   );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_CHANGE_NAME_REQUEST,                       STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_PERMISSIONS_QUERY,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildPermissionsQueryOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_EVENT_LOG_QUERY,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildEventLogQueryOpcode   );
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_NEWS_UPDATE_STICKY,                        STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleGuildNewsUpdateStickyOpcode);
    DEFINE_OPCODE_HANDLER(CMSG_GUILD_QUERY_NEWS,                                STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleGuildQueryNewsOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_MEMBERS_FOR_RECIPE,                  STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_MEMBER_RECIPES,                      STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_RECIPES,                             STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleGuildRequestGuildRecipes   );
    DEFINE_OPCODE_HANDLER(CMSG_PLAYER_SAVE_GUILD_EMBLEM,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePlayerSaveGuildEmblemOpcode);

    /// Guild finding
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_ADD_RECRUIT,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildFinderAddRecruit      );
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_BROWSE,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildFinderBrowse          );
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_DECLINE_RECRUIT,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildFinderDeclineRecruit  );
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_GET_APPLICATIONS,                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildFinderGetApplications );
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_GET_RECRUITS,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildFinderGetRecruits     );
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_POST_REQUEST,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildFinderPostRequest     );
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_REMOVE_RECRUIT,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildFinderRemoveRecruit   );
    DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_SET_GUILD_POST,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGuildFinderSetGuildPost    );

    //////////////////////////////////////////////////////////////////////////
    /// Petition
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_OFFER_PETITION,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleOfferPetitionOpcode        );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_BUY,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetitionBuyOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_DECLINE,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetitionDeclineOpcode      );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_RENAME,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetitionRenameOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_SHOWLIST,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetitionShowListOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_SHOW_SIGNATURES,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetitionShowSignOpcode     );
    DEFINE_OPCODE_HANDLER(CMSG_PETITION_SIGN,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetitionSignOpcode         );
    DEFINE_OPCODE_HANDLER(CMSG_TURN_IN_PETITION,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTurnInPetitionOpcode       );

    //////////////////////////////////////////////////////////////////////////
    /// Battle pet opcodes
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_SET_ABILITY,                           STATUS_LOGGEDIN,    PROCESS_THREADSAFE,     &WorldSession::HandlePetBattleSetAbility        );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_RENAME,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleRename            );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_CAGE_PET,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleCagePet           );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_QUERY_NAME,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleQueryName         );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_REQUEST_WILD,                          STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleRequestWild       );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_REQUEST_PVP,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleRequestPvP        );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_JOIN_QUEUE,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleJoinQueue         );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_REQUEST_UPDATE,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleRequestUpdate     );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_CANCEL_REQUEST_PVP_MATCHMAKING,        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleCancelRequestPvPMatchmaking);
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_INPUT,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleInput             );
    DEFINE_OPCODE_HANDLER(CMSG_PETBATTLE_INPUT_NEW_FRONT_PET,                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandlePetBattleInputNewFrontPet  );
    DEFINE_OPCODE_HANDLER(CMSG_BATTLEPET_SET_BATTLESLOT,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleBattlePetSetBattleSlot     );
    DEFINE_OPCODE_HANDLER(CMSG_SUMMON_COMPANION,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSummonCompanion            );

    //////////////////////////////////////////////////////////////////////////
    /// Battle pay
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_BATTLE_PAY_GET_PURCHASE_LIST,                    STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );

    //////////////////////////////////////////////////////////////////////////
    /// LFG
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_DFGET_SYSTEM_INFO,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgLockInfoRequestOpcode   );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_GET_PLAYER_INFO,                             STATUS_NEVER,       PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_GET_STATUS,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgGetStatus               );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_JOIN,                                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgJoinOpcode              );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LEAVE,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgLeaveOpcode             );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_PROPOSAL_RESULT,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgProposalResultOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_SET_BOOT_VOTE,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgSetBootVoteOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_SET_COMMENT,                                 STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_DFSET_ROLES,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleDfSetRolesOpcode           );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_TELEPORT,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgTeleportOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_SEARCH_LFG_JOIN,                                 STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_SEARCH_LFG_LEAVE,                                STATUS_UNHANDLED,   PROCESS_INPLACE,        &WorldSession::Handle_NULL                      );
    DEFINE_OPCODE_HANDLER(CMSG_RESET_INSTANCES,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleResetInstancesOpcode       );

    //////////////////////////////////////////////////////////////////////////
    /// LFG LIST
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_LFG_LIST_BLACKLIST,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleRequestLfgListBlacklist    );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_JOIN,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgListJoin                );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_UPDATE_REQUEST,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgListUpdateRequest       );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_LEAVE,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgListLeave               );
    DEFINE_OPCODE_HANDLER(CMSG_LFG_LIST_SEARCH,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleLfgListSearch              );

    //////////////////////////////////////////////////////////////////////////
    /// Auction House
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_AUCTION_HELLO,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAuctionHelloOpcode         );

    //////////////////////////////////////////////////////////////////////////
    /// Trainers
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_TRAINER_LIST,                                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTrainerListOpcode          );
    DEFINE_OPCODE_HANDLER(CMSG_TRAINER_BUY_SPELL,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTrainerBuySpellOpcode      );

    //////////////////////////////////////////////////////////////////////////
    /// Void storage
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_VOID_STORAGE_QUERY,                              STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleVoidStorageQuery           );
    DEFINE_OPCODE_HANDLER(CMSG_VOID_STORAGE_TRANSFER,                           STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleVoidStorageTransfer        );
    DEFINE_OPCODE_HANDLER(CMSG_VOID_STORAGE_UNLOCK,                             STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleVoidStorageUnlock          );
    DEFINE_OPCODE_HANDLER(CMSG_VOID_SWAP_ITEM,                                  STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleVoidSwapItem               );

    //////////////////////////////////////////////////////////////////////////
    /// Transmogrification
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_TRANSMOGRIFY_ITEMS,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleTransmogrifyItems          );

    //////////////////////////////////////////////////////////////////////////
    /// Mail
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_GET_MAIL_LIST,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGetMailList                );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_CREATE_TEXT_ITEM,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMailCreateTextItem         );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_DELETE,                                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMailDelete                 );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_MARK_AS_READ,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMailMarkAsRead             );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_RETURN_TO_SENDER,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMailReturnToSender         );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_TAKE_ITEM,                                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMailTakeItem               );
    DEFINE_OPCODE_HANDLER(CMSG_MAIL_TAKE_MONEY,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleMailTakeMoney              );
    DEFINE_OPCODE_HANDLER(CMSG_SEND_MAIL,                                       STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSendMail                   );
    DEFINE_OPCODE_HANDLER(CMSG_QUERY_NEXT_MAIL_TIME,                            STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleQueryNextMailTime          );

    //////////////////////////////////////////////////////////////////////////
    /// Time
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_UITIME_REQUEST,                                  STATUS_LOGGEDIN,    PROCESS_INPLACE,        &WorldSession::HandleWorldStateUITimerUpdate    );

    //////////////////////////////////////////////////////////////////////////
    /// GameMasters
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_GMRESPONSE_RESOLVE,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGMResponseResolve          );
    DEFINE_OPCODE_HANDLER(CMSG_GMSURVEY_SUBMIT,                                 STATUS_UNHANDLED,   PROCESS_THREADUNSAFE,   &WorldSession::HandleGMSurveySubmit             );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_CREATE,                                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGMTicketCreateOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_DELETETICKET,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGMTicketDeleteOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_GETTICKET,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGMTicketGetTicketOpcode    );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_GET_CASE_STATUS,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGMTicketGetWebTicketOpcode );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_SYSTEMSTATUS,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGMTicketSystemStatusOpcode );
    DEFINE_OPCODE_HANDLER(CMSG_GMTICKET_UPDATETEXT,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleGMTicketUpdateOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_GM_REPORT_LAG,                                   STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleReportLag                  );
    DEFINE_OPCODE_HANDLER(CMSG_REPORT_BUG,                                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleReportBugOpcode            );
    DEFINE_OPCODE_HANDLER(CMSG_REPORT_SUGGESTION,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleReportSuggestionOpcode     );

    //////////////////////////////////////////////////////////////////////////
    /// ToyBox
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_ADD_NEW_TOY_TO_BOX,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleAddNewToyToBoxOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_SET_FAVORITE_TOY,                                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleSetFavoriteToyOpcode       );
    DEFINE_OPCODE_HANDLER(CMSG_USE_TOY,                                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE,   &WorldSession::HandleUseToyOpcode               );

    //////////////////////////////////////////////////////////////////////////
    /// Calendar
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_ADD_EVENT,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarAddEvent            );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_COMPLAIN,                               STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarComplain            );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_COPY_EVENT,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarCopyEvent           );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_INVITE,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventInvite         );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_MODERATOR_STATUS,                 STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventModeratorStatus);
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_REMOVE_INVITE,                    STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventRemoveInvite   );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_RSVP,                             STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventRsvp           );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_SIGNUP,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventSignup         );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_STATUS,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarEventStatus         );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET_CALENDAR,                           STATUS_LOGGEDIN,    PROCESS_THREADSAFE,   &WorldSession::HandleCalendarGetCalendar         );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET_EVENT,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarGetEvent            );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GET_NUM_PENDING,                        STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarGetNumPending       );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_GUILD_FILTER,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarGuildFilter         );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_REMOVE_EVENT,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarRemoveEvent         );
    DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_UPDATE_EVENT,                           STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleCalendarUpdateEvent         );

    //////////////////////////////////////////////////////////////////////////
    /// Challenges
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_LEADERS,                  STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleChallengeModeRequestLeaders );
    DEFINE_OPCODE_HANDLER(CMSG_GET_CHALLENGE_MODE_REWARDS,                      STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleGetChallengeModeRewards     );
    DEFINE_OPCODE_HANDLER(CMSG_CHALLENGE_MODE_REQUEST_MAP_STATS,                STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleChallengeModeRequestMapStats);

    //////////////////////////////////////////////////////////////////////////
    /// Challenges
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_BLACK_MARKET_OPEN,                              STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleBlackMarketHello            );
    DEFINE_OPCODE_HANDLER(CMSG_BLACK_MARKET_REQUEST_ITEMS,                     STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleBlackMarketRequestItems     );
    DEFINE_OPCODE_HANDLER(CMSG_BLACK_MARKET_PLACE_BID,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleBlackMarketBid              );

    //////////////////////////////////////////////////////////////////////////
    /// Twitter
    //////////////////////////////////////////////////////////////////////////
    DEFINE_OPCODE_HANDLER(CMSG_REQUEST_TWITTER_STATUS,                         STATUS_LOGGEDIN,    PROCESS_THREADUNSAFE, &WorldSession::HandleRequestTwitterStatus        );

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    //DEFINE_OPCODE_HANDLER(CMSG_ACCEPT_LEVEL_GRANT,                      STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAcceptGrantLevel          );
    //DEFINE_OPCODE_HANDLER(CMSG_ADD_VOICE_IGNORE,                        STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_2v2_STATS_REQUEST,                 STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_ACCEPT,                       STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamAcceptOpcode     );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_CREATE,                       STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamCreateOpcode     );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_DECLINE,                      STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamDeclineOpcode    );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_DISBAND,                      STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamDisbandOpcode    );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_INVITE,                       STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamInviteOpcode     );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_LEADER,                       STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamLeaderOpcode     );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_LEAVE,                        STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamLeaveOpcode      );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_QUERY,                        STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamQueryOpcode      );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_REMOVE,                       STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamRemoveOpcode     );
    //DEFINE_OPCODE_HANDLER(CMSG_ARENA_TEAM_ROSTER,                       STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleArenaTeamRosterOpcode     );
    //DEFINE_OPCODE_HANDLER(CMSG_AUTOEQUIP_ITEM_SLOT,                     STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleAutoEquipItemSlotOpcode   );
    //DEFINE_OPCODE_HANDLER(CMSG_CANCEL_TEMP_ENCHANTMENT,                 STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleCancelTempEnchantmentOpcode);
    //DEFINE_OPCODE_HANDLER(CMSG_CHANGEPLAYER_DIFFICULTY,                 STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHAT_FILTERED,                           STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_COMPLAIN,                                STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleComplainOpcode            );
    //DEFINE_OPCODE_HANDLER(CMSG_CONNECT_TO_FAILED,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DANCE_QUERY,                             STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEL_VOICE_IGNORE,                        STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DISMISS_CRITTER,                         STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleDismissCritter            );
    //DEFINE_OPCODE_HANDLER(CMSG_GRANT_LEVEL,                             STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleGrantLevel                );
    //DEFINE_OPCODE_HANDLER(CMSG_HEARTH_AND_RESURRECT,                    STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleHearthAndResurrect        );
    //DEFINE_OPCODE_HANDLER(CMSG_INSTANCE_LOCK_WARNING_RESPONSE,          STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_NOT_ACTIVE_MOVER,                   STATUS_UNHANDLED, PROCESS_THREADSAFE,   &WorldSession::HandleMoveNotActiveMover        );
    //DEFINE_OPCODE_HANDLER(CMSG_PLAYER_LOGOUT,                           STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandlePlayerLogoutOpcode        );
    //DEFINE_OPCODE_HANDLER(CMSG_PLAY_DANCE,                              STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PVP_LOG_DATA,                            STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandlePVPLogDataOpcode          );
    //DEFINE_OPCODE_HANDLER(CMSG_QUERY_BATTLEFIELD_STATE,                 STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_QUERY_TIME,                              STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleQueryTimeOpcode           );
    //DEFINE_OPCODE_HANDLER(CMSG_QUEST_NPC_QUERY,                         STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_RANDOMIZE_CHAR_NAME,                     STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleRandomizeCharNameOpcode   );
    //DEFINE_OPCODE_HANDLER(CMSG_REALM_SPLIT,                             STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleRealmSplitOpcode          );
    //DEFINE_OPCODE_HANDLER(CMSG_REDIRECTION_AUTH_PROOF,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_REPORT_PVP_AFK,                          STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleReportPvPAFK              );
    //DEFINE_OPCODE_HANDLER(CMSG_REQUEST_BATTLEPET_JOURNAL,               STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleRequestBattlePetJournal   );
    //DEFINE_OPCODE_HANDLER(CMSG_REQUEST_CATEGORY_COOLDOWNS,              STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleCategoryCooldownOpcode    );
    //DEFINE_OPCODE_HANDLER(CMSG_REQUEST_GM_TICKET,                       STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::HandleRequestGmTicket           );
    //DEFINE_OPCODE_HANDLER(CMSG_REQUEST_PARTY_MEMBER_STATS,              STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRequestPartyMemberStatsOpcode);
    //DEFINE_OPCODE_HANDLER(CMSG_RESET_CHALLENGE_MODE,                    STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleResetChallengeModeOpcode  );
    //DEFINE_OPCODE_HANDLER(CMSG_RESET_FACTION_CHEAT,                     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SEND_SOR_REQUEST_VIA_ADDRESS,            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SEND_SOR_REQUEST_VIA_BNET_ACCOUNT_ID,    STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_ALLOW_LOW_LEVEL_RAID2,               STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_CURRENCY_FLAGS,                      STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleChangeCurrencyFlags       );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_FACTION_ATWAR,                       STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSetFactionAtWar           );
    //DEFINE_OPCODE_HANDLER(CMSG_UNSET_FACTION_ATWAR,                     STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleUnSetFactionAtWar         );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_PREFERED_CEMETERY,                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_RELATIVE_POSITION,                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_SAVED_INSTANCE_EXTEND,               STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SHOW_ACCOUNT_ACHIEVEMENT,                STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleShowAccountAchievement    );
    //DEFINE_OPCODE_HANDLER(CMSG_STOP_DANCE,                              STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SUBMIT_BUG,                              STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SUBMIT_COMPLAIN,                         STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SUGGESTION_SUBMIT,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SUMMON_BATTLEPET_COMPANION,              STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSummonBattlePet           );
    //DEFINE_OPCODE_HANDLER(CMSG_SUSPEND_TOKEN,                           STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SYNC_DANCE,                              STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_TELEPORT_TO_UNIT,                        STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_TIME_ADJUSTMENT_RESPONSE,                STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_TIME_SYNC_RESP_FAILED,                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_SPECIALIZATION,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNREGISTER_ALL_ADDON_PREFIXES,           STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleUnregisterAddonPrefixesOpcode);
    //DEFINE_OPCODE_HANDLER(CMSG_UPDATE_MISSILE_TRAJECTORY,               STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleUpdateMissileTrajectory   );
    //DEFINE_OPCODE_HANDLER(CMSG_UPGRADE_ITEM,                            STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleUpgradeItemOpcode         );
    //DEFINE_OPCODE_HANDLER(CMSG_USED_FOLLOW,                             STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_VOICE_SESSION_ENABLE,                    STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::HandleVoiceSessionEnableOpcode  );
    //DEFINE_OPCODE_HANDLER(CMSG_WARDEN_DATA,                             STATUS_UNHANDLED, PROCESS_THREADUNSAFE, &WorldSession::HandleWardenDataOpcode          ); // STATUS_AUTHED
    //DEFINE_OPCODE_HANDLER(CMSG_WARGAME_ACCEPT,                          STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_WARGAME_START,                           STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_WHOIS,                                   STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleWhoisOpcode               );
    //DEFINE_OPCODE_HANDLER(CMSG_WORLD_TELEPORT,                          STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleWorldTeleportOpcode       );
    //DEFINE_OPCODE_HANDLER(CMSG_WRAP_ITEM,                               STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleWrapItemOpcode            );
    //DEFINE_OPCODE_HANDLER(CMSG_ZONEUPDATE,                              STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleZoneUpdateOpcode          );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_ALLOW_LOW_LEVEL_RAID1,               STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_ACTIVE_PVP_CHEAT,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_ADD_PVP_MEDAL_CHEAT,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_ADVANCE_SPAWN_TIME,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_AFK_MONITOR_INFO_CLEAR,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_AFK_MONITOR_INFO_REQUEST,                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_AUTH_SRP6_BEGIN,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_AUTH_SRP6_PROOF,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_AUTH_SRP6_RECODE,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_AUTOEQUIP_GROUND_ITEM,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_AUTOSTORE_GROUND_ITEM,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MANAGER_ADVANCE_STATE,       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_BATTLEFIELD_MANAGER_SET_NEXT_TRANSITION_TIME, STATUS_NEVER,PROCESS_INPLACE,     &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_BATTLEMASTER_HELLO,                      STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBattlemasterHelloOpcode   );
    //DEFINE_OPCODE_HANDLER(CMSG_BEASTMASTER,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_BOOTME,                                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_BOT_DETECTED,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_BOT_DETECTED2,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_BUY_ITEM_IN_SLOT,                        STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleBuyItemInSlotOpcode       );
    //DEFINE_OPCODE_HANDLER(CMSG_BUY_LOTTERY_TICKET_OBSOLETE,             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CALENDAR_EVENT_INVITE_NOTES,             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHANGE_GDF_ARENA_RATING,                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHANGE_PERSONAL_ARENA_RATING,            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHARACTER_POINT_CHEAT,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHEAT_DUMP_ITEMS_DEBUG_ONLY,             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHEAT_PLAYER_LOGIN,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHEAT_PLAYER_LOOKUP,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHEAT_SETMONEY,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHEAT_SET_ARENA_CURRENCY,                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHEAT_SET_HONOR_CURRENCY,                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CHECK_LOGIN_CRITERIA,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CLEAR_EXPLORATION,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CLEAR_HOLIDAY_BG_WIN_TIME,               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CLEAR_QUEST,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CLEAR_RANDOM_BG_WIN_TIME,                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CLEAR_SERVER_BUCK_DATA,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_COMPLETE_ACHIEVEMENT_CHEAT,              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CONTROLLER_EJECT_PASSENGER,              STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleEjectPassenger            );
    //DEFINE_OPCODE_HANDLER(CMSG_COOLDOWN_CHEAT,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CREATEGAMEOBJECT,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CREATEITEM,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_CREATEMONSTER,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DBLOOKUP,                                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEBUG_ACTIONS_START,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEBUG_ACTIONS_STOP,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEBUG_AISTATE,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEBUG_CHANGECELLZONE,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEBUG_LIST_TARGETS,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEBUG_PASSIVE_AURA,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEBUG_SERVER_GEO,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DECHARGE,                                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DELETE_DANCE,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DEL_PVP_MEDAL_CHEAT,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DESTROYMONSTER,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DESTROY_ITEMS,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DISABLE_PVP_CHEAT,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DROP_NEW_CONNECTION,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_DUMP_OBJECTS,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_ENABLE_DAMAGE_LOG,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_END_BATTLEFIELD_CHEAT,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_EXPIRE_RAID_INSTANCE,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_FLAG_QUEST,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_FLAG_QUEST_FINISH,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_FLOOD_GRACE_CHEAT,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_FORCEACTION,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_FORCEACTIONONOTHER,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_FORCEACTIONSHOW,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_FORCE_ANIM,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_FORCE_SAY_CHEAT,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GAMESPEED_SET,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GAMETIME_SET,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GETDEATHBINDZONE,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GHOST,                                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GMRESPONSE_CREATE_TICKET,                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_CHARACTER_RESTORE,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_CHARACTER_SAVE,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_CREATE_ITEM_TARGET,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_DESTROY_ONLINE_CORPSE,                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_FREEZE,                               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_GRANT_ACHIEVEMENT,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_INVIS,                                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_MOVECORPSE,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_NUKE,                                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_NUKE_ACCOUNT,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_NUKE_CHARACTER,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_REMOVE_ACHIEVEMENT,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_REQUEST_PLAYER_INFO,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_RESURRECT,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_REVEALTO,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_SET_CRITERIA_FOR_PLAYER,              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_SET_SECURITY_GROUP,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_SHOW_COMPLAINTS,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_SILENCE,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_SUMMONMOB,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_TEACH,                                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_UBERINVIS,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_UNSQUELCH,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_UNTEACH,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_UPDATE_TICKET_STATUS,                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_VISION,                               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GM_WHISPER,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GODMODE,                                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_GROUP_CANCEL,                            STATUS_LOGGEDIN,  PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_IGNORE_DIMINISHING_RETURNS_CHEAT,        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_IGNORE_KNOCKBACK_CHEAT,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_IGNORE_REQUIREMENTS_CHEAT,               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_INSTANCE_LOCK_RESPONSE,                  STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleInstanceLockResponse      );
    //DEFINE_OPCODE_HANDLER(CMSG_LEARN_DANCE_MOVE,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_LEARN_SPELL,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_LEVEL_CHEAT,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_LFG_SET_NEEDS,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_LF_GUILD_JOIN,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_LOAD_DANCES,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_LOTTERY_QUERY_OBSOLETE,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_LUA_USAGE,                               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MAELSTROM_GM_SENT_MAIL,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MAELSTROM_INVALIDATE_CACHE,              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MAELSTROM_RENAME_GUILD,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MAKEMONSTERATTACKGUID,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MEETINGSTONE_INFO,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_CHARACTER_CHEAT,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_CHARM_PORT_CHEAT,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_CHARM_TELEPORT_CHEAT,               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_ENABLE_SWIM_TO_FLY_TRANS_ACK,       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_FLY,                            STATUS_LOGGEDIN,  PROCESS_THREADSAFE,   &WorldSession::HandleMovementOpcodes           );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_RELATIVE_POSITION,              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_RUN_SPEED,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_SET_VEHICLE_REC_ID_ACK,             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_START_SWIM_CHEAT,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_STOP_SWIM_CHEAT,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_MOVE_TOGGLE_COLLISION_ACK,               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_NEW_SPELL_SLOT,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_NO_SPELL_VARIANCE,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PERFORM_ACTION_SET,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PETGODMODE,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PET_LEVEL_CHEAT,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PET_NAME_CACHE,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PET_UNLEARN,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PET_UNLEARN_TALENTS,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PLAYER_AI_CHEAT,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PLAYER_DIFFICULTY_CHANGE,                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PROFILEDATA_REQUEST,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_PVP_QUEUE_STATS_REQUEST,                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_QUERY_GUILD_MAX_XP,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_QUERY_OBJECT_POSITION,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_QUERY_OBJECT_ROTATION,                   STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_QUERY_SERVER_BUCK_DATA,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_QUERY_VEHICLE_STATUS,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_RECHARGE,                                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_REDIRECTION_FAILED,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_REFER_A_FRIEND,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_REMOVE_GLYPH,                            STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleRemoveGlyph               );
    //DEFINE_OPCODE_HANDLER(CMSG_REPLACE_ACCOUNT_DATA,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_RUN_SCRIPT,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SAVE_DANCE,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SAVE_PLAYER,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SEND_COMBAT_TRIGGER,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SEND_EVENT,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SEND_GENERAL_TRIGGER,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SEND_LOCAL_EVENT,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SERVERINFO,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SERVERTIME,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SERVER_BROADCAST,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SERVER_COMMAND,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SERVER_INFO_QUERY,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SETDEATHBINDPOINT,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_ACTIVE_TALENT_GROUP_OBSOLETE,        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_ARENA_MEMBER_SEASON_GAMES,           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_ARENA_MEMBER_WEEKLY_GAMES,           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_ARENA_TEAM_RATING_BY_INDEX,          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_ARENA_TEAM_SEASON_GAMES,             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_ARENA_TEAM_WEEKLY_GAMES,             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_BREATH,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_CHARACTER_MODEL,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_CRITERIA_CHEAT,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_DURABILITY_CHEAT,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_EXPLORATION,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_EXPLORATION_ALL,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_FACTION_CHEAT,                       STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleSetFactionCheat           );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_GLYPH,                               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_GLYPH_SLOT,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_GRANTABLE_LEVELS,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_LFG_COMMENT,                         STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::HandleLfgSetCommentOpcode       );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_PAID_SERVICE_CHEAT,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_PVP_RANK_CHEAT,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_PVP_TITLE,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_RUNE_COOLDOWN,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_RUNE_COUNT,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_SKILL_CHEAT,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_STAT_CHEAT,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_TITLE_SUFFIX,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SET_WORLDSTATE,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SKILL_BUY_RANK,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_SKILL_BUY_STEP,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_START_BATTLEFIELD_CHEAT,                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_START_QUEST,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_TARGET_CAST,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_TARGET_SCRIPT_CAST,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_TEST_DROP_RATE,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_TOGGLE_XP_GAIN,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNCLAIM_LICENSE,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNDRESSPLAYER,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNITANIMTIER_CHEAT,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_DANCE_MOVE,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_SPELL,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNLEARN_TALENTS,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNUSED5,                                 STATUS_LOGGEDIN,  PROCESS_THREADUNSAFE, &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_UNUSED6,                                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_USE_SKILL_CHEAT,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_VOICE_SET_TALKER_MUTED_REQUEST,          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_WEATHER_SPEED_CHEAT,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_XP_CHEAT,                                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );
    //DEFINE_OPCODE_HANDLER(CMSG_ZONE_MAP,                                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_NULL                     );

    // SMSG
    //DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_PROFILE,                         STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_ADJUST_SPLINE_DURATION,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_ADD_BATTLENET_FRIEND_RESPONSE,           STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_AREA_SHARE_MAPPINGS_RESPONSE,            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_AREA_SHARE_INFO_RESPONSE,                STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_BEASTMASTER_FAILED,                      STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_BONUS_ROLL_EMPTY,                        STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CHALLENGE_MODE_DELETE_LEADER_RESULT,     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CLEAR_ITEM_CHALLENGE_MODE_DATA,          STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CONSOLE_WRITE,                           STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_PROMOTION,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_PLAYER_CHOICE,                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_FORCE_OBJECT_RELINK,                     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GHOST,                                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GM_CHARACTER_SAVE_FAILURE,               STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GM_FREEZE_FAILURE,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GM_NAMED_POINTS,                         STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GM_RESURRECT_FAILURE,                    STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GM_REVEAL_TO,                            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GM_SILENCE,                              STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_IMMIGRANT_HOST_SEARCH_LOG,               STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_LIST_TARGETS,                            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_MOVE_CHARACTER_CHEAT_FAILURE,            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_MOVE_CHARACTER_CHEAT_SUCCESS,            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PENDING_RAID_LOCK,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_CHAT_RESTRICTED,              STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_DEBUG_QUEUE_DUMP_RESPONSE,    STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_FINAL_ROUND,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_FINISHED,                     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_FIRST_ROUND,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_QUEUE_PROPOSE_MATCH,          STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_QUEUE_STATUS,                 STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_ROUND_RESULT,                 STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_REPLACEMENTS_MADE,            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_REQUEST_FAILED,               STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PET_BATTLE_SLOT_UPDATE,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PLAYER_NOT_FOUND_FAILURE,                STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_REFRESH_COMPONENT,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_RESET_WEEKLY_CURRENCY,                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_EVENT,                      STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_FINISHED,        STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_FIRST_ROUND,     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_FINAL_ROUND,     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_ROUND_RESULT,    STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SCENE_OBJECT_PET_BATTLE_REMPLACEMENTS_MADE, STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SERVER_INFO_QUERY_RESPONSE,              STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SET_ITEM_CHALLENGE_MODE_DATA,            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SET_MELEE_ANIM_KIT,                      STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SET_MOVEMENT_ANIM_KIT,                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SET_PROMOTION_RESPONSE,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SET_SERVER_WOW_TIME,                     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SHOW_ZONES_CHEAT_RESULT,                 STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_UPDATE_EXPANSION_LEVEL,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PROFILE_DATA_RESPONSE,                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_ACCOUNT_INFO_RESPONSE,                   STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_DENIED,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_AREA_TRIGGER_MOVEMENT_UPDATE,            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_ARENA_ERROR,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_AVERAGE_ITEM_LEVEL_INFORM,               STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_BATTLEGROUND_INFO_THROTTLED,             STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_BATTLEPET_JOURNAL,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CAMERA_SHAKE,                            STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CHARACTER_LOGIN_FAILED,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CLEAR_BOSS_EMOTES,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_COMBAT_EVENT_FAILED,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_MAP_INFO,                    STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_PLAYER_INFO,                 STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_COMMENTATOR_STATE_CHANGED,               STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CUSTOM_LOAD_SCREEN,                      STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DAMAGE_CALC_LOG,                         STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DIFFERENT_INSTANCE_FROM_PARTY,           STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DISENCHANT_CREDIT,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DISPLAY_GAME_ERROR,                      STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DONT_AUTO_PUSH_SPELLS_TO_ACTION_BAR,     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DUMP_RIDE_TICKETS_RESPONSE,              STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_FAILED_PLAYER_CONDITION,                 STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_FLIGHT_SPLINE_SYNC,                      STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_FORCED_DEATH_UPDATE,                     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GAMEOBJECT_RESET_STATE,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GAME_SPEED_SET,                          STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GAME_TIME_SET,                           STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GAME_TIME_UPDATE,                        STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GAME_EVENT_DEBUG_LOG,                    STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GM_PLAYER_INFO,                          STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GOD_MODE,                                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GROUP_ACTION_THROTTLED,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_HEALTH_UPDATE,                           STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_INVALIDATE_PLAYER,                       STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_INVALID_PROMOTION_CODE,                  STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_ITEM_ADD_PASSIVE,                        STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_REMOVE_ITEM_PASSIVE,                     STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SEND_ITEM_PASSIVES,                      STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_MAP_OBJ_EVENTS,                          STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_MESSAGE_BOX,                             STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_MISSILE_CANCEL,                          STATUS_UNHANDLED, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_NOTIFY_MONEY,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_MOVE_SET_COMPOUND_STATE,                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_MOVE_UPDATE_COLLISION_HEIGHT,            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_ABORT_NEW_WORLD,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_OPEN_CONTAINER,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_OPEN_LFG_DUNGEON_FINDER,                 STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PAUSE_MIRROR_TIMER,                      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CHANGE_PLAYER_DIFFICULTY_RESULT,         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PLAYER_SKINNED,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PLAY_TIME_WARNING,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_PROPOSE_LEVEL_GRANT,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SUMMON_RAID_MEMBER_VALIDATE_FAILED,      STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GENERATE_RANDOM_CHARACTER_NAME_RESULT,   STATUS_AUTHED,    PROCESS_THREADUNSAFE, &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_REALM_SPLIT,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_REFER_A_FRIEND_EXPIRED,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_REFER_A_FRIEND_FAILURE,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_REPORT_PVP_PLAYER_AFK_RESULT,            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SERVER_TIME,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SERVER_INFO_RESPONSE,                    STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SERVER_PERF,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SET_DF_FAST_LAUNCH_RESULT,               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SET_PLAY_HOVER_ANIM,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SHOW_RATINGS,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SOR_START_EXPERIENCE_INCOMPLETE,         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_STREAMING_MOVIES,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SUMMON_CANCEL,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SUPPRESS_NPC_GREETINGS,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_TIME_ADJUSTMENT,                         STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_UPDATE_SERVER_PLAYER_POSITION,           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_WAIT_QUEUE_FINISH,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_WAIT_QUEUE_UPDATE,                       STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_WARGAME_REQUEST_SUCCESSFULLY_SENT_TO_OPPONENT, STATUS_NEVER, PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_CHEAT_PLAYER_LOOKUP,                     STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DBLOOKUP_RESULTS,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DEBUG_AISTATE,                           STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_DYNAMIC_DROP_ROLL_RESULT,                STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_FORCE_ACTION_SHOW_RESPONSE               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_FORCE_ANIM,                              STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_GHOSTEE_GONE,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_KICK_REASON,                             STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_RESET_RANGED_COMBAT_TIMER,               STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SCRIPT_MESSAGE,                          STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SERVER_BUCK_DATA,                        STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SERVER_BUCK_DATA_START,                  STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SHOW_MAILBOX,                            STATUS_NEVER,     PROCESS_INPLACE,      &WorldSession::Handle_ServerSide               );
    //DEFINE_OPCODE_HANDLER(SMSG_SET_QUEST_COMPLETED_BIT,                         STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    //DEFINE_OPCODE_HANDLER(SMSG_CLEAR_QUEST_COMPLETED_BITS,                      STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);
    //DEFINE_OPCODE_HANDLER(SMSG_CLEAR_QUEST_COMPLETED_BIT,                       STATUS_NEVER,         PROCESS_INPLACE,      &WorldSession::Handle_ServerSide);

#undef DEFINE_OPCODE_HANDLER
};
