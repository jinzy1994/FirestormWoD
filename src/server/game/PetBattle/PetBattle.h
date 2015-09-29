/*
* Copyright (C) 2012-2014 JadeCore <http://www.pandashan.com/>
* Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
* Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
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

#pragma once
#ifndef __PETBATTLE_HEADER__
#define __PETBATTLE_HEADER__

#include "Common.h"
#include "Player.h"
#include <ace/Singleton.h>
#include "Player.h"

class Field;

#define MAX_PETBATTLE_SLOTS 3
#define MAX_PETBATTLE_TEAM 2
#define MAX_PETBATTLE_ABILITIES 3
#define MAX_PETBATTLE_ABILITY_TURN 10

#define NUM_BATTLEPET_PETTYPES 10

#define PETBATTLE_NULL_ID -1
#define PETBATTLE_NULL_SLOT -1
#define PETBATTLE_UPDATE_INTERVAL 300
#define PETBATTLE_DELETE_INTERVAL (1 * 30 * IN_MILLISECONDS)

#define PETBATTLE_TEAM_1 0
#define PETBATTLE_TEAM_2 1
#define PETBATTLE_PVE_TEAM_ID 1

#define BATTLEPET_MAX_LEVEL 25

enum PetBattleType
{
    PETBATTLE_TYPE_PVE,
    PETBATTLE_TYPE_PVP_DUEL,
    PETBATTLE_TYPE_PVP_MATCHMAKING
};

enum ePetBattleStatus
{
    PETBATTLE_STATUS_CREATION,
    PETBATTLE_STATUS_RUNNING,
    PETBATTLE_STATUS_FINISHED,
    PETBATTLE_STATUS_PENDING_DELETE
};

enum ePetBattleQualities
{
    BATTLEPET_QUALITY_POOR      = 0,
    BATTLEPET_QUALITY_COMMON    = 1,
    BATTLEPET_QUALITY_UNCOMMON  = 2,
    BATTLEPET_QUALITY_RARE      = 3,
    BATTLEPET_QUALITY_EPIC      = 4,
    BATTLEPET_QUALITY_LEGENDARY = 5
};

enum eBattlePetTypes
{
    BATTLEPET_PETTYPE_HUMANOID      = 0,
    BATTLEPET_PETTYPE_DRAGONKIN     = 1,
    BATTLEPET_PETTYPE_FLYING        = 2,
    BATTLEPET_PETTYPE_UNDEAD        = 3,
    BATTLEPET_PETTYPE_CRITTER       = 4,
    BATTLEPET_PETTYPE_MAGIC         = 5,
    BATTLEPET_PETTYPE_ELEMENTAL     = 6,
    BATTLEPET_PETTYPE_BEAST         = 7,
    BATTLEPET_PETTYPE_AQUATIC       = 8,
    BATTLEPET_PETTYPE_MECHANICAL    = 9
};

enum eBattlePetFlags
{
    BATTLEPET_FLAG_FAVORITE         = 0x01,
    PETBATTLE_FLAG_CAPTURED         = 0x01,
    BATTLEPET_FLAG_ABILITY_1_SECOND = 0x10,
    BATTLEPET_FLAG_ABILITY_2_SECOND = 0x20,
    BATTLEPET_FLAG_ABILITY_3_SECOND = 0x40
};

enum eBattlePetRequests
{
    PETBATTLE_REQUEST_FAILED                            = 0,
    PETBATTLE_REQUEST_NOT_HERE                          = 1,
    PETBATTLE_REQUEST_NOT_DURING_FLYING                 = 2,
    PETBATTLE_REQUEST_GROUND_NOT_ENOUGHT_SMOOTH         = 3,
    PETBATTLE_REQUEST_AREA_NOT_CLEAR                    = 4,
    PETBATTLE_REQUEST_ALREADY_IN_COMBAT                 = 5,
    PETBATTLE_REQUEST_DEATH                             = 6,
    PETBATTLE_REQUEST_SEATED                            = 7,
    PETBATTLE_REQUEST_NOT_VALID_TARGET                  = 8,
    PETBATTLE_REQUEST_TOO_FAR                           = 9,
    PETBATTLE_REQUEST_INVALID_TARGET                    = 10,
    PETBATTLE_REQUEST_NEED_TO_BE_TRAINER                = 11,
    PETBATTLE_REQUEST_DECLINED                          = 12,
    PETBATTLE_REQUEST_ALREADY_IN_PETBATTLE              = 13,
    PETBATTLE_REQUEST_NEED_PET_IN_SLOTS                 = 14,
    PETBATTLE_REQUEST_PET_ALL_DEAD                      = 15,
    PETBATTLE_REQUEST_NEED_AT_LEAST_1_PET_IN_SLOT       = 16,
    PETBATTLE_REQUEST_CODEX_LOCKED_BY_AN_ANOTHER_USER   = 17,
    PETBATTLE_REQUEST_TARGET_IN_A_BATTLEPET             = 18
};

enum BattlePetState
{
    BATTLEPET_STATE_Internal_InitialHealth          = 3,
    BATTLEPET_STATE_Internal_InitialLevel           = 17,
    BATTLEPET_STATE_Internal_CaptureBoost           = 90,
    BATTLEPET_STATE_Internal_EffectSucceeded        = 91,
    BATTLEPET_STATE_Internal_HealthBeforeInstakill  = 145,

    BATTLEPET_STATE_Is_Dead                         =   1,
    BATTLEPET_STATE_maxHealthBonus                  =   2,
    BATTLEPET_STATE_Stat_Kharma                     =   4,
    BATTLEPET_STATE_Stat_Power                      =  18,
    BATTLEPET_STATE_Stat_Stamina                    =  19,
    BATTLEPET_STATE_Stat_Speed                      =  20,
    BATTLEPET_STATE_Mechanic_IsPoisoned             =  21,
    BATTLEPET_STATE_Mechanic_IsStunned              =  22,
    BATTLEPET_STATE_Mod_DamageDealtPercent          =  23,
    BATTLEPET_STATE_Mod_DamageTakenPercent          =  24,
    BATTLEPET_STATE_Mod_SpeedPercent                =  25,
    BATTLEPET_STATE_Ramping_DamageID                =  26,
    BATTLEPET_STATE_Ramping_DamageUses              =  27,
    BATTLEPET_STATE_Condition_WasDamagedThisTurn    =  28,
    BATTLEPET_STATE_untargettable                   =  29,
    BATTLEPET_STATE_Mechanic_IsUnderground          =  30,
    BATTLEPET_STATE_Last_HitTaken                   =  31,
    BATTLEPET_STATE_Last_HitDealt                   =  32,
    BATTLEPET_STATE_Mechanic_IsFlying               =  33,
    BATTLEPET_STATE_Mechanic_IsBurning              =  34,
    BATTLEPET_STATE_turnLock                        =  35,
    BATTLEPET_STATE_swapOutLock                     =  36,
    BATTLEPET_STATE_Stat_CritChance                 =  40,
    BATTLEPET_STATE_Stat_Accuracy                   =  41,
    BATTLEPET_STATE_Passive_Critter                 =  42,
    BATTLEPET_STATE_Passive_Beast                   =  43,
    BATTLEPET_STATE_Passive_Humanoid                =  44,
    BATTLEPET_STATE_Passive_Flying                  =  45,
    BATTLEPET_STATE_Passive_Dragon                  =  46,
    BATTLEPET_STATE_Passive_Elemental               =  47,
    BATTLEPET_STATE_Passive_Mechanical              =  48,
    BATTLEPET_STATE_Passive_Magic                   =  49,
    BATTLEPET_STATE_Passive_Undead                  =  50,
    BATTLEPET_STATE_Passive_Aquatic                 =  51,
    BATTLEPET_STATE_Mechanic_IsChilled              =  52,
    BATTLEPET_STATE_Weather_BurntEarth              =  53,
    BATTLEPET_STATE_Weather_ArcaneStorm             =  54,
    BATTLEPET_STATE_Weather_Moonlight               =  55,
    BATTLEPET_STATE_Weather_Darkness                =  56,
    BATTLEPET_STATE_Weather_Sandstorm               =  57,
    BATTLEPET_STATE_Weather_Blizzard                =  58,
    BATTLEPET_STATE_Weather_Mud                     =  59,
    BATTLEPET_STATE_Weather_Rain                    =  60,
    BATTLEPET_STATE_Weather_Sunlight                =  61,
    BATTLEPET_STATE_Weather_LightningStorm          =  62,
    BATTLEPET_STATE_Weather_Windy                   =  63,
    BATTLEPET_STATE_Mechanic_IsWebbed               =  64,
    BATTLEPET_STATE_Mod_HealingDealtPercent         =  65,
    BATTLEPET_STATE_Mod_HealingTakenPercent         =  66,
    BATTLEPET_STATE_Mechanic_IsInvisible            =  67,
    BATTLEPET_STATE_unkillable                      =  68,
    BATTLEPET_STATE_Mechanic_IsObject               =  69,
    BATTLEPET_STATE_Special_Plant                   =  70,
    BATTLEPET_STATE_Add_FlatDamageTaken             =  71,
    BATTLEPET_STATE_Add_FlatDamageDealt             =  72,
    BATTLEPET_STATE_Stat_Dodge                      =  73,
    BATTLEPET_STATE_Special_BlockedAttackCount      =  74,
    BATTLEPET_STATE_Special_ObjectRedirectionAuraID =  75,
    BATTLEPET_STATE_Mechanic_IsBleeding             =  77,
    BATTLEPET_STATE_Stat_Gender                     =  78,
    BATTLEPET_STATE_Mechanic_IsBlind                =  82,
    BATTLEPET_STATE_Cosmetic_Stealthed              =  84,
    BATTLEPET_STATE_Cosmetic_WaterBubbled           =  85,
    BATTLEPET_STATE_Mod_PetTypeDamageDealtPercent   =  87,
    BATTLEPET_STATE_Mod_PetTypeDamageTakenPercent   =  88,
    BATTLEPET_STATE_Mod_PetType_ID                  =  89,
    BATTLEPET_STATE_Special_IsCockroach             =  93,
    BATTLEPET_STATE_swapInLock                      =  98,
    BATTLEPET_STATE_Mod_MaxHealthPercent            =  99,
    BATTLEPET_STATE_Clone_Active                    = 100,
    BATTLEPET_STATE_Clone_PBOID                     = 101,
    BATTLEPET_STATE_Clone_PetAbility1               = 102,
    BATTLEPET_STATE_Clone_PetAbility2               = 103,
    BATTLEPET_STATE_Clone_PetAbility3               = 104,
    BATTLEPET_STATE_Clone_Health                    = 105,
    BATTLEPET_STATE_Clone_MaxHealth                 = 106,
    BATTLEPET_STATE_Clone_LastAbilityID             = 107,
    BATTLEPET_STATE_Clone_LastAbilityTurn           = 108,
    BATTLEPET_STATE_Special_IsCharging              = 113,
    BATTLEPET_STATE_Special_IsRecovering            = 114,
    BATTLEPET_STATE_Clone_CloneAbilityID            = 117,
    BATTLEPET_STATE_Clone_CloneAuraID               = 118,
    BATTLEPET_STATE_DarkSimulacrum_AbilityID        = 119,
    BATTLEPET_STATE_Special_ConsumedCorpse          = 120,
    BATTLEPET_STATE_Ramping_PBOID                   = 121,
    BATTLEPET_STATE_reflecting                      = 122,
    BATTLEPET_STATE_Special_BlockedFriendlyMode     = 123,
    BATTLEPET_STATE_Special_TypeOverride            = 124,
    BATTLEPET_STATE_Mechanic_IsWall                 = 126,
    BATTLEPET_STATE_Condition_DidDamageThisRound    = 127,
    BATTLEPET_STATE_Cosmetic_FlyTier                = 128,
    BATTLEPET_STATE_Cosmetic_FetishMask             = 129,
    BATTLEPET_STATE_Mechanic_IsBomb                 = 136,
    BATTLEPET_STATE_Special_IsCleansing             = 141,
    BATTLEPET_STATE_Cosmetic_Bigglesworth           = 144,
    BATTLEPET_STATE_resilient                       = 149,
    BATTLEPET_STATE_Passive_Elite                   = 153,
    BATTLEPET_STATE_Cosmetic_Chaos                  = 158,
    BATTLEPET_STATE_Passive_Boss                    = 162,
    NUM_BATTLEPET_STATES
};

enum PetBattleCastTriggerFlag
{
    PETBATTLE_CAST_TRIGGER_NONE             = 0x00,
    PETBATTLE_CAST_TRIGGER_IGNORE_COOLDOWN  = 0x01,

    PETBATTLE_CAST_TRIGGER_ALL              = 0xFFFFFFFF
};

enum PetBattleCastResult
{
    PETBATTLE_CAST_OK,
    PETBATTLE_CAST_INVALID_ID,
    PETBATTLE_CAST_INTERNAL_ERROR
};

enum PetBattleResult
{
    PETBATTLE_RESULT_WON,
    PETBATTLE_RESULT_LOOSE,
    PETBATTLE_RESULT_ABANDON
};

enum ePetBattleRounds
{
    PETBATTLE_ROUND_RUNNING,
    PETBATTLE_ROUND_FINISHED
};

enum PetBattleRoundResult
{
    PETBATTLE_ROUND_RESULT_NONE             = 0,
    PETBATTLE_ROUND_RESULT_NORMAL           = 2,
    PETBATTLE_ROUND_RESULT_CATCH_OR_KILL    = 3
};

/// Extracted from file FrameXML/SharedPetBattleTemplates.lua
enum ePetBattleAbilities
{
    PETBATTLE_ABILITY_TURN0_PROC_ON_APPLY           = 0,
    PETBATTLE_ABILITY_TURN0_PROC_ON_DAMAGE_TAKEN    = 1,        /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_DAMAGE_DEALT    = 2,        /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_HEAL_TAKEN      = 3,        /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_HEAL_DEALT      = 4,        /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_AURA_REMOVED    = 5,        /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_ROUND_START     = 6,        /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_ROUND_END       = 7,        /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_TURN            = 8,
    PETBATTLE_ABILITY_TURN0_PROC_ON_ABILITY         = 9,        /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_SWAP_IN         = 10,       /// implemented
    PETBATTLE_ABILITY_TURN0_PROC_ON_SWAP_OUT        = 11,       /// implemented

    PETBATTLE_ABILITY_TURN0_PROC_ON_NONE = 0xFF      /// Custom value
};

class BattlePet
{
    public:
        typedef std::shared_ptr<BattlePet> Ptr;

    public:
        /// Load
        void Load(Field* p_Fields);
        /// Load
        void CloneFrom(BattlePet::Ptr & p_BattlePet);
        /// Save
        void Save();

        /// When a player earn a pet it will insert into his account
        void AddToPlayer(Player* p_Player);

        /// Update pet stats
        void UpdateStats();

    public:
        uint32          AccountID;                              ///< Owner account ID
        uint64          JournalID;                              ///< As companion (db/journal id)
        int32           Slot;                                   ///< Battle slot
        std::string     Name;                                   ///< Name
        uint32          NameTimeStamp;                          ///< Name timestamp
        uint32          Species;                                ///< Species ID
        uint32          DisplayModelID;                         ///< Display id (no real usage, client can deduce it for some species)
        uint32          Breed;                                  ///< Breed quality (factor for some states)
        uint32          Quality;                                ///< Pet quality (factor for some states)
        uint32          Abilities[MAX_PETBATTLE_ABILITIES];     ///< Available abilities
        int32           Health;                                 ///< Current health
        uint32          Level;                                  ///< Pet level
        uint32          XP;                                     ///< Pet XP
        uint32          Flags;                                  ///< Flags
        int32           InfoPower;                              ///< Info power (need UpdateStats calls)
        int32           InfoMaxHealth;                          ///< Info max health (need UpdateStats calls) 
        int32           InfoSpeed;                              ///< Info speed (need UpdateStats calls)
        int32           InfoGender;                             ///< Info gender (need UpdateStats calls)

};

class PetBattle;

/// Extended BattlePet class for pet battles
class BattlePetInstance : public BattlePet
{
    public:
        typedef std::shared_ptr<BattlePetInstance> Ptr;

    public:
        /// Constructor
        BattlePetInstance();

        /// Is alive ?
        bool IsAlive();
        bool CanAttack();

        /// Get stats
        int32 GetMaxHealth();
        /// Get pet speed
        int32 GetSpeed();
        /// Get max xp for current level
        uint32 GetMaxXPForCurrentLevel();
        /// Get xp earn
        uint32 GetXPEarn(uint32 p_TargetPetID);

        /// Update original instance
        void UpdateOriginalInstance();

    public:
        uint32      TeamID;                                 ///< Team ID
        uint32      ID;                                     ///< Rel id for battle (0 - 1 - 2 - 3 - 4 - 5)

        int32       Cooldowns[MAX_PETBATTLE_ABILITIES];     ///< Pet cool downs
        int32       States[NUM_BATTLEPET_STATES];           ///< Pet states

        PetBattle * PetBattleInstance;                      ///< Pet battle instance helper

        uint32      OldLevel;

        BattlePet::Ptr  OriginalBattlePet;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Pet battle event type
enum PetBattleEventType
{
    PETBATTLE_EVENT_UPDATE_TRIGGER          = 0,    // 0 on 5.4.7
    PETBATTLE_EVENT_UPDATE_NPC_EMOTE        = 1,    // 7 on 5.4.7
    PETBATTLE_EVENT_UPDATE_FRONTPET         = 2,    // 6 on 5.4.7
    PETBATTLE_EVENT_UPDATE_HEALTH           = 3,    // 5 on 5.4.7
    PETBATTLE_EVENT_UPDATE_SPEED            = 4,    // 4 on 5.4.7
    PETBATTLE_EVENT_UPDATE_ABILITY_CHANGE   = 5,    // 2 on 5.4.7
    PETBATTLE_EVENT_UPDATE_STATE            = 6,    // 3 on 5.4.7
    PETBATTLE_EVENT_UPDATE_BUFF             = 7,    // 1 on 5.4.7
};

/// Pet battle event
struct PetBattleEventUpdate
{
    /// Constructor
    PetBattleEventUpdate();

    uint32  UpdateType;     ///< Update type
    int8    TargetPetID;    ///< Target pet id

    /// Update data here
    union
    {
        int32 Health;
        int32 MaxHealth;
        int32 TriggerAbilityId;
        int32 Speed;

        /// Buff update data
        struct
        {
            uint32  ID;                     ///< Aura display slot
            uint32  AbilityID;              ///< Aura ability ID
            int32   Duration;               ///< Remaining duration
            uint32  Turn;                   ///< Aura turn
        } Buff;

        /// State npc emote
        struct
        {
            uint32  ID;                     ///< State ID
            int32   Value;                  ///< State value
        } State;

        /// State npc emote
        struct
        {
            uint32  BroadcastTextID;        ///< State ID
        } NpcEmote;
    };
};

typedef std::list<PetBattleEventUpdate> PetBattleEventUpdateList;

/// Event types
enum ePetBattleEvents
{
    PETBATTLE_EVENT_SET_HEALTH              = 0,
    PETBATTLE_EVENT_BUFF_APPLY              = 1,
    PETBATTLE_EVENT_BUFF_CANCEL             = 2,
    PETBATTLE_EVENT_BUFF_CHANGE             = 3,
    PETBATTLE_EVENT_PET_SWAP                = 4,
    PETBATTLE_EVENT_CATCH                   = 5,
    PETBATTLE_EVENT_SET_STATE               = 6,
    PETBATTLE_EVENT_SET_MAX_HEALTH          = 7,
    PETBATTLE_EVENT_SET_SPEED               = 8,
    PETBATTLE_EVENT_SET_POWER               = 9,
    PETBATTLE_EVENT_TRIGGER_ABILITY         = 10,
    PETBATTLE_EVENT_ABILITY_CHANGE          = 11,
    PETBATTLE_EVENT_NPC_EMOTE               = 12,
    PETBATTLE_EVENT_AURA_PROCESSING_BEGIN   = 13,
    PETBATTLE_EVENT_AURA_PROCESSING_END     = 14
};

/// Event flags
enum ePetBattleEventFlags
{
    PETBATTLE_EVENT_FLAG_SKIP_TURN  = 0x0001,
    PETBATTLE_EVENT_FLAG_MISS       = 0x0002,
    PETBATTLE_EVENT_FLAG_CRITICAL   = 0x0004,
    PETBATTLE_EVENT_FLAG_BLOCKED    = 0x0008,
    PETBATTLE_EVENT_FLAG_DODGE      = 0x0010,
    PETBATTLE_EVENT_FLAG_HEAL       = 0x0020,
    PETBATTLE_EVENT_FLAG_REFLECT    = 0x0080,
    PETBATTLE_EVENT_FLAG_ABSORB     = 0x0100,
    PETBATTLE_EVENT_FLAG_IMMUNE     = 0x0200,
    PETBATTLE_EVENT_FLAG_STRONG     = 0x0400,
    PETBATTLE_EVENT_FLAG_WEAK       = 0x0800,
    PETBATTLE_EVENT_FLAG_UNK_KILL   = 0x1000,

    PETBATTLE_EVENT_FLAG_PERIODIC   = 0x00010000 // Not exist in client, flags field is actually on 16 bits
};

/// PetBattleEvent for client sync
struct PetBattleEvent
{
    /// Constructor
    PetBattleEvent(uint32 p_EventType = 0, int32 p_SourcePetID = PETBATTLE_NULL_ID, uint32 p_Flags = 0, uint32 p_AbilityEffectID = 0, uint32 p_RoundTurn = 0, uint32 p_BuffTurn = 0, uint32 p_StackDepth = 0);

    /// Make an health update
    PetBattleEvent& UpdateHealth(int8 p_TargetPetID, int32 p_Health);
    /// Make an state update
    PetBattleEvent& UpdateState(int8 p_TargetPetID, uint32 p_StateID, int32 p_Value);
    /// Make an front pet change
    PetBattleEvent& UpdateFrontPet(int8 p_NewFrontPet = PETBATTLE_NULL_ID);
    /// Make an buff update
    PetBattleEvent& UpdateBuff(int8 p_TargetPetID, uint32 p_ID, uint32 p_AbilityID, int32 p_Duration, uint32 p_Turn);
    /// Make an speed update
    PetBattleEvent& UpdateSpeed(int8 p_TargetPetID, int32 p_Speed);
    /// Make an trigger
    PetBattleEvent& Trigger(int8 p_TargetPetID, uint32 p_AbilityId);

    uint32 EventType;               ///< Type of event (PETBATTLE_EVENT_SET_HEALTH, PETBATTLE_EVENT_BUFF_APPLY, PETBATTLE_EVENT_BUFF_CANCEL,PETBATTLE_EVENT_BUFF_CHANGE, PETBATTLE_EVENT_PET_SWAP, ...)
    uint32 Flags;                   ///< Event flags (PETBATTLE_EVENT_FLAG_SKIP_TURN,PETBATTLE_EVENT_FLAG_MISS, PETBATTLE_EVENT_FLAG_CRITICAL, PETBATTLE_EVENT_FLAG_BLOCKED, ...)
    int32  SourcePetID;             ///< Caster pet id
    uint32 AbilityEffectID;         ///< Id of an ability effect (used for client animation)
    uint32 BuffTurn;                ///< Buff rel turn count/id
    uint32 RoundTurn;               ///< Turn in round turn see  PetBattle::RoundTurn (used for order sync)
    uint32 StackDepth;              ///< unk

    PetBattleEventUpdateList Updates;   ///< Event updates, client support more than 1 update pet event, but never seen more than 1 update per event on retails
};

typedef std::list<PetBattleEvent> PetBattleEventList;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// Pet battle request
struct PetBattleRequest
{
    /// Requester Guid & request id are the same
    union
    {
        uint64 RequesterGuid;
        uint64 RequestID;
    };

    uint64 OpponentGuid;                        ///< Opponent guid (player or wild pet guid)
    PetBattleType RequestType;                  ///< Battle request type (PETBATTLE_TYPE_PVE / PETBATTLE_TYPE_PVP_DUEL / PETBATTLE_TYPE_PVP_MATCHMAKING)
    float PetBattleCenterPosition[3];           ///< Pet battlefield center position
    float TeamPosition[MAX_PETBATTLE_TEAM][3];  ///< Teams position
    float BattleFacing;                         ///< unk
    uint32 LocationResult;                      ///< unk
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// Pet aura
class PetBattleAura
{
    public:
        /// Apply
        void Apply(PetBattle* p_Battle);
        /// Remove
        void Remove(PetBattle* p_Battle);

        /// Process all aura effects
        void Process(PetBattle* p_Battle);
        /// Make aura expire
        void Expire(PetBattle* p_Battle);

    public:
        uint32  AbilityID;              ///< Ability ID
        uint32  TriggerId;              /// Ability effect id
        uint32  CasterPetID;            ///< Caster pet id
        uint32  TargetPetID;            ///< Target pet id
        int32   Turn;                   ///< Turn (increment every aura process)
        uint32  ID;                     ///< Slot (client side)
        int32   Duration;               ///< Remaining duration (client sinc)
        int32   MaxDuration;            ///< Max aura duration
        bool    Expired;                ///< Aura is expired ?

};
typedef std::list<PetBattleAura*> PetBattleAuraList;

/// Team flags 1
enum PetBattleTeamInputFlags
{
    PETBATTLE_TEAM_INPUT_FLAG_LOCK_ABILITIES_1  = 0x01,
    PETBATTLE_TEAM_INPUT_FLAG_LOCK_ABILITIES_2  = 0x02,
    PETBATTLE_TEAM_INPUT_FLAG_LOCK_PET_SWAP     = 0x04,
    PETBATTLE_TEAM_INPUT_FLAG_SELECT_NEW_PET    = 0x08,
};

/// Team flags 2
enum PetBattleTeamCatchFlags
{
    PETBATTLE_TEAM_CATCH_FLAG_ENABLE_TRAP           = 0x01,
    PETBATTLE_TEAM_CATCH_FLAG_NEED_LVL3_PET         = 0x02,
    PETBATTLE_TEAM_CATCH_FLAG_TOO_MUCH_HP           = 0x04,
    PETBATTLE_TEAM_CATCH_FLAG_ONE_CATCH_PER_FIGHT   = 0x08,
};

/// Pet battle team
class PetBattleTeam
{
    public:
        /// Update team
        bool Update();

        /// Do casts for front pet
        void DoCasts(uint32 p_Turn0ProcCond = PETBATTLE_ABILITY_TURN0_PROC_ON_NONE);

        /// Has pending multi turn cast
        bool HasPendingMultiTurnCast();

        /// Can cast any ability
        bool CanCastAny();
        /// Can swap
        bool CanSwap(int8 p_ReplacementPet = PETBATTLE_NULL_ID);
        /// Can catch opponent pet
        uint8 CanCatchOpponentTeamFrontPet();

        /// Get team flags 1
        uint32 GetTeamInputFlags();
        /// Get team flags 2
        uint32 GetTeamTrapFlags();
        /// Get available pet for combat
        std::vector<uint32> GetAvailablesPets();

        /// Get catch ability ID
        uint32 GetCatchAbilityID();

    public:
        uint64 OwnerGuid;                                       ///< Team owner guid
        uint64 PlayerGuid;                                      ///< Team player owner guid

        PetBattle * PetBattleInstance;                          ///< Pet battle instance

        BattlePetInstance::Ptr TeamPets[MAX_PETBATTLE_SLOTS];   ///< Team pets
        uint32 TeamPetCount;                                    ///< Team pet count

        std::map<uint32, uint32> CapturedSpeciesCount;          ///< Captured species count

        uint32 ActivePetID;                                     ///< Team active pet

        uint32 ActiveAbilityId;
        uint32 activeAbilityTurn;
        uint32 activeAbilityTurnMax;

        int8 CapturedPet;                                       ///< Captured pet id

        bool Ready;                                             ///< Team is ready to process next round
};

/// Pet battle instance
class PetBattle
{
    public:
        /// Constructor
        PetBattle();
        /// Destructor
        ~PetBattle();

        /// Add pet to the battle
        void AddPet(uint32 p_TeamID, std::shared_ptr<BattlePetInstance> p_Pet);

        /// Start the battle
        void Begin();
        /// Proceed round battle
        void ProceedRound();
        /// Finish the battle
        void Finish(uint32 p_WinnerTeamID, bool p_Aborted);

        /// Update the pet battle
        void Update(uint32 p_TimeDiff);
        
        /// Swap pet
        void SwapPet(uint32 p_TeamID, uint32 p_NewFrontPetID, bool p_Initial = false);
        
        /// Check can cast
        bool CanCast(uint32 p_TeamID, uint32 p_AbilityID);
        /// Prepare cast
        void PrepareCast(uint32 p_TeamID, uint32 p_AbilityID);
        /// Cast an ability
        PetBattleCastResult Cast(uint32 p_CasterPetID, uint32 p_AbilityID, uint32 p_Turn, uint32 p_Turn0ProcCondition, uint32 p_TriggerFlag);

        /// Add aura
        bool AddAura(uint32 p_CasterPetID, uint32 p_TargetPetID, uint32 p_AbilityID, int32 p_Duration, int32 p_MaxAllowed, uint32 p_FromAbilityEffectID, uint32 p_Flags);
        /// Set pet state
        void SetPetState(uint32 p_SourcePetID, uint32 p_TargetPetID, uint32 p_FromAbilityEffectID, uint32 p_State, int32 p_Value, bool p_FromCapture = false, uint32 p_Flags = 0);
        /// kill
        void Kill(int8 p_Killer, int8 p_Target, uint32 p_KillerAbibilityEffectID, bool p_FromCapture = false, uint32 p_Flags = 0);
        /// Catch
        void Catch(int8 p_Catcher, int8 p_CatchedTarget, uint32 p_FromAbilityEffectID);

        /// Get first attacking team
        uint32 GetFirstAttackingTeam();

        /// Get forfeit health penalty pct
        int32 GetForfeitHealthPenalityPct();

    public:
        uint32 ID;                                                              ///< Battle global unique ID
        PetBattleType BattleType;                                               ///< Battle type (PETBATTLE_TYPE_PVE / PETBATTLE_TYPE_PVP_DUEL / PETBATTLE_TYPE_PVP_MATCHMAKING)
        uint32 Turn;                                                            ///< Battle current turn id
        PetBattleResult CombatResult;                                           ///< Combat result (PETBATTLE_RESULT_WON, PETBATTLE_RESULT_LOOSE, PETBATTLE_RESULT_ABANDON)

        uint32 BattleStatus;                                                    ///< PETBATTLE_STATUS_CREATION / PETBATTLE_STATUS_RUNNING / PETBATTLE_STATUS_FINISHED

        uint32 RoundStatus;                                                     ///< Current round status (PETBATTLE_ROUND_RUNNING / PETBATTLE_ROUND_FINISHED)
        uint32 RoundTurn;                                                       ///< Current round turn for spells cast (independant of PetBattle::Turn)
        PetBattleRoundResult RoundResult;                                       ///< Current round result
        uint32 RoundFirstTeamCasting;                                           ///< Team id who has the priority in ProceedRound (base on active pets speed)
        PetBattleEventList RoundEvents;                                         ///< Current round event queue (for client update)
        std::vector<uint32> RoundDeadPets;                                      ///< Pets who died during this round
        std::vector<std::pair<uint32, uint32>> RoundPetSpeedUpdate;             ///< Round pet speed update <petid, abilityeffectid>

        PetBattleTeam * Teams[MAX_PETBATTLE_TEAM];                              ///< Battle teams
        std::shared_ptr<BattlePetInstance> Pets[MAX_PETBATTLE_TEAM * MAX_PETBATTLE_SLOTS];      ///< All pets involved in the battle
        uint32 TotalPetCount;                                                   ///< Battle total pet count

        PetBattleAuraList PetAuras;                                             ///< Current battle pets auras
        uint32 WeatherAbilityId;                                                ///< Only one weather at a time on battle

        int32 WinnerTeamId;
        std::map<uint8, bool> FightedPets;
        int8 CatchedPetId;

    private:
        IntervalTimer m_UpdateTimer;

};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/// Pet battle system main class (singleton)
class PetBattleSystem
{
    /// ACE_Singleton class constructor/destructor access
    friend class ACE_Singleton<PetBattleSystem, ACE_Null_Mutex>;

    private:
        /// Constructor
        PetBattleSystem();
        /// Destructor
        ~PetBattleSystem();

    public:
        /// Create a new battle with an unique auto incremented ID
        PetBattle* CreateBattle();
        /// Create a new pet battle request (actually we use requested guid (player guid) as request id)
        PetBattleRequest* CreateRequest(uint64 p_RequesterGuid);

        /// Get a battle by his unique id
        PetBattle* GetBattle(uint64 p_BattleID);
        /// Get a request by his requested guid (player guid)
        PetBattleRequest* GetRequest(uint64 p_RequesterGuid);

        /// Remove an battle and delete it
        void RemoveBattle(uint64 p_BattleID);
        /// Remove an request and delete it
        void RemoveRequest(uint64 p_RequesterGuid);

        /// Update the whole pet battle system (request and battles)
        void Update(uint32 p_TimeDiff);

        /// Forfeit an battle
        void ForfeitBattle(uint64 p_BattleID, uint64 p_ForfeiterGuid);

    private:
        uint32                              m_MaxPetBattleID;       ///< Global battle unique id
        std::map<uint64, PetBattle*>        m_PetBattles;          ///< All running battles
        std::map<uint64, PetBattleRequest*> m_Requests;             ///< All pending battles request

        IntervalTimer                               m_DeleteUpdateTimer;        ///< Deletion queue update timer
        std::queue<std::pair<uint64, PetBattle*>>   m_PetBbattlesDeleteQueue;   ///< Deletion queue
};

/// Pet battle system class singleton init
#define sPetBattleSystem (ACE_Singleton<PetBattleSystem, ACE_Null_Mutex>::instance())

#endif
