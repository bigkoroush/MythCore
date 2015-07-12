/*
 * Copyright (C) 2008 - 2011 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 - 2014 Myth Project <http://mythprojectnetwork.blogspot.com/>
 *
 * Myth Project's source is based on the Trinity Project source, you can find the
 * link to that easily in Trinity Copyrights. Myth Project is a private community.
 * To get access, you either have to donate or pass a developer test.
 * You may not share Myth Project's sources! For personal use only.
 */

#include "ScriptPCH.h"
#include "trial_of_the_crusader.h"

enum eYells
{
    SAY_STAGE_0_01            = -1649070,
    SAY_STAGE_0_02            = -1649071,
    SAY_STAGE_0_03a           = -1649072,
    SAY_STAGE_0_03h           = -1649073,
    SAY_STAGE_0_04            = -1649074,
    SAY_STAGE_0_05            = -1649075,
    SAY_STAGE_0_06            = -1649076,
    SAY_STAGE_0_WIPE          = -1649077,
    SAY_STAGE_1_01            = -1649080,
    SAY_STAGE_1_02            = -1649081,
    SAY_STAGE_1_03            = -1649082,
    SAY_STAGE_1_04            = -1649083,
    SAY_STAGE_1_05            = -1649030, //INTRO Jaraxxus
    SAY_STAGE_1_06            = -1649084,
    SAY_STAGE_1_07            = -1649086,
    SAY_STAGE_1_08            = -1649087,
    SAY_STAGE_1_09            = -1649088,
    SAY_STAGE_1_10            = -1649089,
    SAY_STAGE_1_11            = -1649090,
    SAY_STAGE_2_01            = -1649091,
    SAY_STAGE_2_02a           = -1649092,
    SAY_STAGE_2_02h           = -1649093,
    SAY_STAGE_2_03            = -1649094,
    SAY_STAGE_2_04a           = -1649095,
    SAY_STAGE_2_04h           = -1649096,
    SAY_STAGE_2_05a           = -1649097,
    SAY_STAGE_2_05h           = -1649098,
    SAY_STAGE_2_06            = -1649099,
    SAY_STAGE_3_01            = -1649100,
    SAY_STAGE_3_02            = -1649101,
    SAY_STAGE_3_03a           = -1649102,
    SAY_STAGE_3_03h           = -1649103,
    SAY_STAGE_4_01            = -1649104,
    SAY_STAGE_4_02            = -1649105,
    SAY_STAGE_4_03            = -1649106,
    SAY_STAGE_4_04            = -1649107,
    SAY_STAGE_4_05            = -1649108,
    SAY_STAGE_4_06            = -1649109,
    SAY_STAGE_4_07            = -1649110,
};

struct _Messages
{
    eAnnouncerMessages msgnum;
    uint32 id;
    bool state;
    uint32 encounter;
};

static _Messages _GossipMessage[]=
{
    {MSG_BEASTS, GOSSIP_ACTION_INFO_DEF+1, false, TYPE_BEASTS},
    {MSG_JARAXXUS, GOSSIP_ACTION_INFO_DEF+2, false, TYPE_JARAXXUS},
    {MSG_CRUSADERS, GOSSIP_ACTION_INFO_DEF+3, false, TYPE_CRUSADERS},
    {MSG_VALKIRIES, GOSSIP_ACTION_INFO_DEF+4, false, TYPE_VALKIRIES},
    {MSG_LICH_KING, GOSSIP_ACTION_INFO_DEF+5, false, TYPE_ANUBARAK},
    {MSG_ANUBARAK, GOSSIP_ACTION_INFO_DEF+6, true, TYPE_ANUBARAK}
};

enum
{
    NUM_MESSAGES = 6,
};

class npc_announcer_toc10 : public CreatureScript
{
public:
    npc_announcer_toc10() : CreatureScript("npc_announcer_toc10") { }

    struct npc_announcer_toc10AI : public ScriptedAI
    {
        npc_announcer_toc10AI(Creature* pCreature): ScriptedAI(pCreature)
        {
            pInstance = pCreature->GetInstanceScript();
        }

        InstanceScript* pInstance;

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            if(Creature* pAlly = GetClosestCreatureWithEntry(me, NPC_THRALL, 300.0f))
                pAlly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            if(Creature* pAlly = GetClosestCreatureWithEntry(me, NPC_PROUDMOORE, 300.0f))
                pAlly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void AttackStart(Unit* /*pWho*/) { }
    };

    bool OnGossipHello(Player* pPlayer, Creature* pCreature)
    {
        InstanceScript* instanceScript = pCreature->GetInstanceScript();
        if(!instanceScript)
            return true;

        char const* _message = "We are ready!";

        if(pPlayer->isInCombat() || instanceScript->IsEncounterInProgress() || instanceScript->GetData(TYPE_EVENT))
            return true;

        uint8 i = 0;
        for(; i < NUM_MESSAGES; ++i)
        {
            if((!_GossipMessage[i].state && instanceScript->GetData(_GossipMessage[i].encounter) != DONE)
                || (_GossipMessage[i].state && instanceScript->GetData(_GossipMessage[i].encounter) == DONE))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN, _GossipMessage[i].id);
                break;
            }
        }

        pPlayer->SEND_GOSSIP_MENU(_GossipMessage[i].msgnum, pCreature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 /*sender*/, uint32 action)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        pPlayer->CLOSE_GOSSIP_MENU();
        InstanceScript* instanceScript = pCreature->GetInstanceScript();
        if(!instanceScript)
            return true;

        switch(action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                if(instanceScript->GetData(TYPE_BEASTS) != DONE)
                {
                    Creature* pTemp;
                    pTemp = Unit::GetCreature(*pPlayer, instanceScript->GetData64(NPC_GORMOK));
                    if(pTemp)
                        pTemp->DespawnOrUnsummon();

                    pTemp = Unit::GetCreature(*pPlayer, instanceScript->GetData64(NPC_ICEHOWL));
                    if(pTemp)
                        pTemp->DespawnOrUnsummon();

                    pTemp = Unit::GetCreature(*pPlayer, instanceScript->GetData64(NPC_DREADSCALE));
                    if(pTemp)
                        pTemp->DespawnOrUnsummon();

                    pTemp = Unit::GetCreature(*pPlayer, instanceScript->GetData64(NPC_ACIDMAW));
                    if(pTemp)
                        pTemp->DespawnOrUnsummon();

                    instanceScript->SetData(TYPE_EVENT, 110);
                    instanceScript->SetData(TYPE_NORTHREND_BEASTS, NOT_STARTED);
                    instanceScript->SetData(TYPE_BEASTS, NOT_STARTED);
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                if(Creature* pIcehowl = Unit::GetCreature(*pPlayer, instanceScript->GetData64(NPC_ICEHOWL)))
                    pIcehowl->DespawnOrUnsummon();

                if(Creature* pJaraxxus = Unit::GetCreature(*pPlayer, instanceScript->GetData64(NPC_JARAXXUS)))
                {
                    pJaraxxus->RemoveAurasDueToSpell(SPELL_JARAXXUS_CHAINS);
                    pJaraxxus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pJaraxxus->SetReactState(REACT_AGGRESSIVE);
                    pJaraxxus->SetInCombatWithZone();
                } else if(instanceScript->GetData(TYPE_JARAXXUS) != DONE) {
                    instanceScript->SetData(TYPE_EVENT, 1010);
                    instanceScript->SetData(TYPE_JARAXXUS, NOT_STARTED);
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                if(Creature* jaraxxus = Unit::GetCreature(*pPlayer, instanceScript->GetData64(NPC_JARAXXUS)))
                    jaraxxus->DespawnOrUnsummon();

                if(instanceScript->GetData(TYPE_CRUSADERS) != DONE)
                {
                    if(pPlayer->GetTeam() == ALLIANCE)
                        instanceScript->SetData(TYPE_EVENT, 3000);
                    else
                        instanceScript->SetData(TYPE_EVENT, 3001);
                    instanceScript->SetData(TYPE_CRUSADERS, NOT_STARTED);
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                if(instanceScript->GetData(TYPE_VALKIRIES) != DONE)
                {
                    instanceScript->SetData(TYPE_EVENT, 4000);
                    instanceScript->SetData(TYPE_VALKIRIES, NOT_STARTED);
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
            {
                if(instanceScript->GetData(TYPE_LICH_KING) != DONE && !pPlayer->isGameMaster())
                    return true;

                if(GameObject* floor = GameObject::GetGameObject(*pPlayer, instanceScript->GetData64(GO_ARGENT_COLISEUM_FLOOR)))
                    floor->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);

                pCreature->CastSpell(pCreature, 69016, false);

                Creature* pAnubArak = Unit::GetCreature(*pCreature, instanceScript->GetData64(NPC_ANUBARAK));
                if(!pAnubArak || !pAnubArak->isAlive())
                    pAnubArak = pCreature->SummonCreature(NPC_ANUBARAK, AnubarakLoc[0].GetPositionX(), AnubarakLoc[0].GetPositionY(), AnubarakLoc[0].GetPositionZ(), 3, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);

                instanceScript->SetData(TYPE_ANUBARAK, NOT_STARTED);
                pCreature->SetVisible(false);
                break;
            }
        }

        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        return true;
    }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_announcer_toc10AI(pCreature);
    }
};

class boss_lich_king_toc : public CreatureScript
{
public:
    boss_lich_king_toc() : CreatureScript("boss_lich_king_toc") { }

    struct boss_lich_king_tocAI : public ScriptedAI
    {
        boss_lich_king_tocAI(Creature* pCreature): ScriptedAI(pCreature)
        {
            pInstance = pCreature->GetInstanceScript();
        }

        InstanceScript* pInstance;
        uint32 m_uiUpdateTimer;

        void Reset()
        {
            m_uiUpdateTimer = 0;
            me->SetReactState(REACT_PASSIVE);
            if(Creature* summoned = me->SummonCreature(NPC_TRIGGER, ToCCommonLoc[2].GetPositionX(), ToCCommonLoc[2].GetPositionY(), ToCCommonLoc[2].GetPositionZ(), 5, TEMPSUMMON_TIMED_DESPAWN, 60000))
            {
                summoned->CastSpell(summoned, 51807, false);
                summoned->SetDisplayId(11686);
            }
            if(pInstance) pInstance->SetData(TYPE_LICH_KING, IN_PROGRESS);
            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
        }

        void MovementInform(uint32 uiType, uint32 uiId)
        {
            if(uiType != POINT_MOTION_TYPE)
                return;
            switch(uiId)
            {
                case 0:
                    pInstance->SetData(TYPE_EVENT, 5030);
                    break;
                case 1:
                    pInstance->SetData(TYPE_EVENT, 5050);
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if(!pInstance) return;
            if(pInstance->GetData(TYPE_EVENT_NPC) != NPC_LICH_KING_1) return;

            m_uiUpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
            if(m_uiUpdateTimer <= diff)
            {
                switch(pInstance->GetData(TYPE_EVENT))
                {
                    case 5010:
                        DoScriptText(SAY_STAGE_4_02, me);
                        m_uiUpdateTimer = 3000;
                        me->GetMotionMaster()->MovePoint(0, LichKingLoc[0]);
                        pInstance->SetData(TYPE_EVENT, 5020);
                        break;
                    case 5030:
                        DoScriptText(SAY_STAGE_4_04, me);
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                        m_uiUpdateTimer = 10000;
                        pInstance->SetData(TYPE_EVENT, 5040);
                        break;
                    case 5040:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                        me->GetMotionMaster()->MovePoint(1, LichKingLoc[1]);
                        m_uiUpdateTimer = 1000;
                        pInstance->SetData(TYPE_EVENT, 0);
                        break;
                    case 5050:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 5060);
                        break;
                    case 5060:
                        DoScriptText(SAY_STAGE_4_05, me);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                        m_uiUpdateTimer = 2500;
                        pInstance->SetData(TYPE_EVENT, 5070);
                        break;
                    case 5070:
                        me->CastSpell(me, 68198, false);
                        m_uiUpdateTimer = 1500;
                        pInstance->SetData(TYPE_EVENT, 5080);
                        break;
                    case 5080:
                        if(GameObject* pGoFloor = pInstance->instance->GetGameObject(pInstance->GetData64(GO_ARGENT_COLISEUM_FLOOR)))
                            pGoFloor->SetDestructibleState(GO_DESTRUCTIBLE_DESTROYED);
                        me->CastSpell(me, 69016, false);
                        if(pInstance)
                        {
                            pInstance->SetData(TYPE_LICH_KING, DONE);
                            Creature* pTemp = Unit::GetCreature(*me, pInstance->GetData64(NPC_ANUBARAK));
                            if(!pTemp || !pTemp->isAlive())
                                pTemp = me->SummonCreature(NPC_ANUBARAK, AnubarakLoc[0].GetPositionX(), AnubarakLoc[0].GetPositionY(), AnubarakLoc[0].GetPositionZ(), 3, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);

                            pInstance->SetData(TYPE_EVENT, 0);
                        }
                        me->DespawnOrUnsummon();
                        m_uiUpdateTimer = 20000;
                        break;
                }
            } else m_uiUpdateTimer -= diff;
            pInstance->SetData(TYPE_EVENT_TIMER, m_uiUpdateTimer);
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_lich_king_tocAI(pCreature);
    }
};

class npc_fizzlebang_toc : public CreatureScript
{
public:
    npc_fizzlebang_toc() : CreatureScript("npc_fizzlebang_toc") { }

    struct npc_fizzlebang_tocAI : public ScriptedAI
    {
        npc_fizzlebang_tocAI(Creature* pCreature): ScriptedAI(pCreature), Summons(me)
        {
            pInstance = (InstanceScript*)me->GetInstanceScript();
        }

        InstanceScript* pInstance;
        SummonList Summons;
        uint32 m_uiUpdateTimer;
        uint64 m_uiPortalGUID;
        uint64 m_uiTriggerGUID;

        void JustDied(Unit* killer)
        {
            DoScriptText(SAY_STAGE_1_06, me, killer);
            pInstance->SetData(TYPE_EVENT, 1180);
            if(Creature* pTemp = Unit::GetCreature(*me, pInstance->GetData64(NPC_JARAXXUS)))
            {
                pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pTemp->SetReactState(REACT_AGGRESSIVE);
                pTemp->SetInCombatWithZone();
            }
        }

        void Reset()
        {
            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
            m_uiPortalGUID = 0;
            me->GetMotionMaster()->MovePoint(1, ToCCommonLoc[10].GetPositionX(), ToCCommonLoc[10].GetPositionY()-60, ToCCommonLoc[10].GetPositionZ());
        }

        void MovementInform(uint32 uiType, uint32 uiId)
        {
            if(uiType != POINT_MOTION_TYPE) return;

            switch(uiId)
            {
                case 1:
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    if(pInstance)
                    {
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        pInstance->SetData(TYPE_EVENT, 1120);
                        pInstance->SetData(TYPE_EVENT_TIMER, 1000);
                    }
                    break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            Summons.Summon(summoned);
        }

        void UpdateAI(const uint32 diff)
        {
            if(!pInstance) return;

            if(pInstance->GetData(TYPE_EVENT_NPC) != NPC_FIZZLEBANG) return;

            m_uiUpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
            if(m_uiUpdateTimer <= diff)
            {
                switch(pInstance->GetData(TYPE_EVENT))
                {
                    case 1110:
                        if(Creature* pIcehowl = Unit::GetCreature(*me, pInstance->GetData64(NPC_ICEHOWL)))
                            pIcehowl->DespawnOrUnsummon();
                        pInstance->SetData(TYPE_EVENT, 1120);
                        m_uiUpdateTimer = 4000;
                        break;
                    case 1120:
                        DoScriptText(SAY_STAGE_1_02, me);
                        pInstance->SetData(TYPE_EVENT, 1130);
                        m_uiUpdateTimer = 12000;
                        break;
                    case 1130:
                        me->GetMotionMaster()->MovementExpired();
                        DoScriptText(SAY_STAGE_1_03, me);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_SPELL_CAST_OMNI);
                        if(Unit* pTrigger =  me->SummonCreature(NPC_TRIGGER, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 4.69494f, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            m_uiTriggerGUID = pTrigger->GetGUID();
                            pTrigger->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.0f);
                            pTrigger->SetDisplayId(22862);
                            pTrigger->CastSpell(pTrigger, SPELL_WILFRED_PORTAL, false);
                        }
                        pInstance->SetData(TYPE_EVENT, 1132);
                        m_uiUpdateTimer = 4000;
                        break;
                    case 1132:
                        me->GetMotionMaster()->MovementExpired();
                        pInstance->SetData(TYPE_EVENT, 1134);
                        m_uiUpdateTimer = 4000;
                        break;
                    case 1134:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_SPELL_CAST_OMNI);
                        if(Creature* pPortal = me->SummonCreature(NPC_WILFRED_PORTAL, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 4.71239f, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            pPortal->SetReactState(REACT_PASSIVE);
                            pPortal->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.0f);
                            pPortal->CastSpell(pPortal, SPELL_WILFRED_PORTAL, false);
                            m_uiPortalGUID = pPortal->GetGUID();
                        }
                        m_uiUpdateTimer = 4000;
                        pInstance->SetData(TYPE_EVENT, 1135);
                        break;
                    case 1135:
                        pInstance->SetData(TYPE_EVENT, 1140);
                        m_uiUpdateTimer = 3000;
                        break;
                    case 1140:
                        DoScriptText(SAY_STAGE_1_04, me);
                        if(Creature* pTemp = me->SummonCreature(NPC_JARAXXUS, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 5.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME))
                        {
                            pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pTemp->SetReactState(REACT_PASSIVE);
                            pTemp->GetMotionMaster()->MovePoint(0, ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY()-10, ToCCommonLoc[1].GetPositionZ());
                        }
                        pInstance->SetData(TYPE_EVENT, 1142);
                        m_uiUpdateTimer = 5000;
                        break;
                    case 1142:
                        if(Creature* pTemp = Unit::GetCreature(*me, pInstance->GetData64(NPC_JARAXXUS)))
                            pTemp->SetTarget(me->GetGUID());
                        if(Creature* pTrigger = Unit::GetCreature(*me, m_uiTriggerGUID))
                            pTrigger->DespawnOrUnsummon();
                        if(Creature* pPortal = Unit::GetCreature(*me, m_uiPortalGUID))
                            pPortal->DespawnOrUnsummon();
                        pInstance->SetData(TYPE_EVENT, 1144);
                        m_uiUpdateTimer = 10000;
                        break;
                    case 1144:
                        if(Creature* pTemp = Unit::GetCreature(*me, pInstance->GetData64(NPC_JARAXXUS)))
                            DoScriptText(SAY_STAGE_1_05, pTemp);
                        pInstance->SetData(TYPE_EVENT, 1150);
                        m_uiUpdateTimer = 5000;
                        break;
                    case 1150:
                        if(Creature* pTemp = Unit::GetCreature(*me, pInstance->GetData64(NPC_JARAXXUS)))
                        {
                            //1-shot Fizzlebang
                            pTemp->CastSpell(me, 67888, false);
                            me->SetInCombatWith(pTemp);
                            pTemp->AddThreat(me, 1000.0f);
                            pTemp->AI()->AttackStart(me);
                        }
                        pInstance->SetData(TYPE_EVENT, 1160);
                        m_uiUpdateTimer = 3000;
                        break;
                }
            } else m_uiUpdateTimer -= diff;
            pInstance->SetData(TYPE_EVENT_TIMER, m_uiUpdateTimer);
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_fizzlebang_tocAI(pCreature);
    }
};

class npc_tirion_toc : public CreatureScript
{
public:
    npc_tirion_toc() : CreatureScript("npc_tirion_toc") { }

    struct npc_tirion_tocAI : public ScriptedAI
    {
        npc_tirion_tocAI(Creature* pCreature): ScriptedAI(pCreature)
        {
            pInstance = (InstanceScript*)me->GetInstanceScript();
        }

        InstanceScript* pInstance;
        uint32 m_uiUpdateTimer;

        void Reset() { }

        void AttackStart(Unit* /*pWho*/) { }

        void UpdateAI(const uint32 diff)
        {
            if(!pInstance) return;
            if(pInstance->GetData(TYPE_EVENT_NPC) != NPC_TIRION) return;

            m_uiUpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
            if(m_uiUpdateTimer <= diff)
            {
                switch(pInstance->GetData(TYPE_EVENT))
                {
                    case 110:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                        DoScriptText(SAY_STAGE_0_01, me);
                        m_uiUpdateTimer = 22000;
                        pInstance->SetData(TYPE_EVENT, 120);
                        break;
                    case 140:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                        DoScriptText(SAY_STAGE_0_02, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 150);
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        break;
                    case 150:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                        if(pInstance->GetData(TYPE_BEASTS) != DONE)
                        {
                            me->SummonCreature(NPC_GORMOK, ToCCommonLoc[10].GetPositionX(), ToCCommonLoc[10].GetPositionY(), ToCCommonLoc[10].GetPositionZ(), 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
                            if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_GORMOK)))
                            {
                                pTemp->GetMotionMaster()->MovePoint(0, ToCCommonLoc[5].GetPositionX(), ToCCommonLoc[5].GetPositionY(), ToCCommonLoc[5].GetPositionZ());
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                                pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                pTemp->SetReactState(REACT_PASSIVE);
                            }
                        }
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 155);
                        break;
                    case 155:
                        if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_GORMOK)))
                        {
                            pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            pTemp->SetReactState(REACT_AGGRESSIVE);
                            pTemp->SetInCombatWithZone();
                        }
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        pInstance->SetData(TYPE_BEASTS, IN_PROGRESS);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 160);
                        break;
                    case 200:
                        DoScriptText(SAY_STAGE_0_04, me);
                        m_uiUpdateTimer = 8000;
                        pInstance->SetData(TYPE_EVENT, 205);
                        break;
                    case 205:
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 210);
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        break;
                    case 210:
                        if(pInstance->GetData(TYPE_BEASTS) != DONE)
                        {
                            me->SummonCreature(NPC_DREADSCALE, ToCCommonLoc[3].GetPositionX(), ToCCommonLoc[3].GetPositionY(), ToCCommonLoc[3].GetPositionZ(), 5, TEMPSUMMON_MANUAL_DESPAWN);
                            me->SummonCreature(NPC_ACIDMAW, ToCCommonLoc[4].GetPositionX(), ToCCommonLoc[4].GetPositionY(), ToCCommonLoc[4].GetPositionZ(), 5, TEMPSUMMON_MANUAL_DESPAWN);
                            if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_DREADSCALE)))
                            {
                                pTemp->GetMotionMaster()->MovePoint(0, ToCCommonLoc[8].GetPositionX(), ToCCommonLoc[8].GetPositionY(), ToCCommonLoc[8].GetPositionZ());
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                                pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                pTemp->SetReactState(REACT_PASSIVE);
                            }
                            if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_ACIDMAW)))
                            {
                                pTemp->GetMotionMaster()->MovePoint(0, ToCCommonLoc[9].GetPositionX(), ToCCommonLoc[9].GetPositionY(), ToCCommonLoc[9].GetPositionZ());
                                pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                                pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                pTemp->SetReactState(REACT_PASSIVE);
                            }
                        }
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 220);
                        break;
                    case 220:
                        if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_DREADSCALE)))
                        {
                            pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            pTemp->SetReactState(REACT_AGGRESSIVE);
                            pTemp->SetInCombatWithZone();
                        }
                        if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_ACIDMAW)))
                        {
                            pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            pTemp->SetReactState(REACT_AGGRESSIVE);
                            pTemp->SetInCombatWithZone();
                        }
                        pInstance->SetData(TYPE_EVENT, 230);
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        break;
                    case 300:
                        DoScriptText(SAY_STAGE_0_05, me);
                        m_uiUpdateTimer = 8000;
                        pInstance->SetData(TYPE_EVENT, 305);
                        break;
                    case 305:
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 310);
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        break;
                    case 310:
                        if(pInstance->GetData(TYPE_BEASTS) != DONE)
                        {
                            me->SummonCreature(NPC_ICEHOWL, ToCCommonLoc[10].GetPositionX(), ToCCommonLoc[10].GetPositionY(), ToCCommonLoc[10].GetPositionZ(), 5, TEMPSUMMON_DEAD_DESPAWN);
                            if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_ICEHOWL)))
                            {
                                pTemp->GetMotionMaster()->MovePoint(0, ToCCommonLoc[5].GetPositionX(), ToCCommonLoc[5].GetPositionY(), ToCCommonLoc[5].GetPositionZ());
                                pTemp->SetInCombatWithZone();
                            }
                        }
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 315);
                        break;
                    case 315:
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        pInstance->SetData(TYPE_EVENT, 320);
                        break;
                    case 400:
                        DoScriptText(SAY_STAGE_0_06, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 0);
                        break;
                    case 666:
                        DoScriptText(SAY_STAGE_0_WIPE, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 0);
                        break;
                    case 1010:
                        DoScriptText(SAY_STAGE_1_01, me);
                        m_uiUpdateTimer = 7000;
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        me->SummonCreature(NPC_FIZZLEBANG, ToCCommonLoc[10].GetPositionX(), ToCCommonLoc[10].GetPositionY(), ToCCommonLoc[10].GetPositionZ(), 2, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                        pInstance->SetData(TYPE_EVENT, 0);
                        break;
                    case 1180:
                        DoScriptText(SAY_STAGE_1_07, me);
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 0);
                        break;
                    case 2000:
                        DoScriptText(SAY_STAGE_1_08, me);
                        m_uiUpdateTimer = 18000;
                        pInstance->SetData(TYPE_EVENT, 2010);
                        break;
                    case 2030:
                        DoScriptText(SAY_STAGE_1_11, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 0);
                        break;
                    case 3000:
                        DoScriptText(SAY_STAGE_2_01, me);
                        m_uiUpdateTimer = 12000;
                        pInstance->SetData(TYPE_EVENT, 3050);
                        break;
                    case 3001:
                        DoScriptText(SAY_STAGE_2_01, me);
                        m_uiUpdateTimer = 12000;
                        pInstance->SetData(TYPE_EVENT, 3051);
                        break;
                    case 3060:
                        DoScriptText(SAY_STAGE_2_03, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 3070);
                        break;
                    case 3061:
                        DoScriptText(SAY_STAGE_2_03, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 3071);
                        break;
                    //Summoning crusaders
                    case 3091:
                        me->SummonCreature(NPC_CHAMPIONS_CONTROLLER, ToCCommonLoc[1]);
                        if(Creature* pChampionController = Unit::GetCreature((*me), pInstance->GetData64(NPC_CHAMPIONS_CONTROLLER)))
                            pChampionController->AI()->SetData(0, HORDE);
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 3092);
                        break;
                    //Summoning crusaders
                    case 3090:
                        me->SummonCreature(NPC_CHAMPIONS_CONTROLLER, ToCCommonLoc[1]);
                        if(Creature* pChampionController = Unit::GetCreature((*me), pInstance->GetData64(NPC_CHAMPIONS_CONTROLLER)))
                            pChampionController->AI()->SetData(0, ALLIANCE);
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 3092);
                        break;
                    case 3092:
                        if(Creature* pChampionController = Unit::GetCreature((*me), pInstance->GetData64(NPC_CHAMPIONS_CONTROLLER)))
                            pChampionController->AI()->SetData(1, NOT_STARTED);
                        pInstance->SetData(TYPE_EVENT, 3095);
                        break;
                    //Crusaders battle end
                    case 3100:
                        DoScriptText(SAY_STAGE_2_06, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 0);
                        break;
                    case 4000:
                        DoScriptText(SAY_STAGE_3_01, me);
                        m_uiUpdateTimer = 13000;
                        pInstance->SetData(TYPE_EVENT, 4010);
                        break;
                    case 4010:
                        DoScriptText(SAY_STAGE_3_02, me);
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 4015);
                        break;
                    case 4015:
                        me->SummonCreature(NPC_LIGHTBANE, ToCCommonLoc[3].GetPositionX(), ToCCommonLoc[3].GetPositionY(), ToCCommonLoc[3].GetPositionZ(), 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                        if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_LIGHTBANE)))
                        {
                            pTemp->GetMotionMaster()->MovePoint(0, ToCCommonLoc[6].GetPositionX(), ToCCommonLoc[6].GetPositionY(), ToCCommonLoc[6].GetPositionZ());
                            pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->SetReactState(REACT_PASSIVE);
                        }
                        me->SummonCreature(NPC_DARKBANE, ToCCommonLoc[4].GetPositionX(), ToCCommonLoc[4].GetPositionY(), ToCCommonLoc[4].GetPositionZ(), 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                        if(Creature* pTemp = Unit::GetCreature((*me), pInstance->GetData64(NPC_DARKBANE)))
                        {
                            pTemp->GetMotionMaster()->MovePoint(0, ToCCommonLoc[7].GetPositionX(), ToCCommonLoc[7].GetPositionY(), ToCCommonLoc[7].GetPositionZ());
                            pTemp->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->SetReactState(REACT_PASSIVE);
                        }
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 4016);
                        break;
                    case 4016:
                        pInstance->SetData(TYPE_EVENT, 4017);
                        pInstance->DoUseDoorOrButton(pInstance->GetData64(GO_MAIN_GATE_DOOR));
                        break;
                    case 4040:
                        m_uiUpdateTimer = 60000;
                        pInstance->SetData(TYPE_EVENT, 5000);
                        break;
                    case 5000:
                        DoScriptText(SAY_STAGE_4_01, me);
                        m_uiUpdateTimer = 10000;
                        pInstance->SetData(TYPE_EVENT, 5005);
                        break;
                    case 5005:
                        m_uiUpdateTimer = 8000;
                        pInstance->SetData(TYPE_EVENT, 5010);
                        me->SummonCreature(NPC_LICH_KING_1, ToCCommonLoc[2].GetPositionX(), ToCCommonLoc[2].GetPositionY(), ToCCommonLoc[2].GetPositionZ(), 5);
                        break;
                    case 5020:
                        DoScriptText(SAY_STAGE_4_03, me);
                        m_uiUpdateTimer = 1000;
                        pInstance->SetData(TYPE_EVENT, 0);
                        break;
                    case 6000:
                        me->NearTeleportTo(AnubarakLoc[0].GetPositionX(), AnubarakLoc[0].GetPositionY(), AnubarakLoc[0].GetPositionZ(), 4.0f);
                        m_uiUpdateTimer = 20000;
                        pInstance->SetData(TYPE_EVENT, 6005);
                        break;
                    case 6005:
                        DoScriptText(SAY_STAGE_4_06, me);
                        m_uiUpdateTimer = 20000;
                        pInstance->SetData(TYPE_EVENT, 6010);
                        break;
                    case 6010:
                        if(IsHeroic())
                        {
                            DoScriptText(SAY_STAGE_4_07, me);
                            m_uiUpdateTimer = 60000;
                            pInstance->SetData(TYPE_ANUBARAK, SPECIAL);
                            pInstance->SetData(TYPE_EVENT, 6020);
                        } else pInstance->SetData(TYPE_EVENT, 6030);
                        break;
                    case 6020:
                        me->DespawnOrUnsummon();
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 6030);
                        break;
                }
            } else m_uiUpdateTimer -= diff;
            pInstance->SetData(TYPE_EVENT_TIMER, m_uiUpdateTimer);
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_tirion_tocAI(pCreature);
    }
};

class npc_garrosh_toc : public CreatureScript
{
public:
    npc_garrosh_toc() : CreatureScript("npc_garrosh_toc") { }

    struct npc_garrosh_tocAI : public ScriptedAI
    {
        npc_garrosh_tocAI(Creature* pCreature): ScriptedAI(pCreature)
        {
            pInstance = (InstanceScript*)me->GetInstanceScript();
        }

        InstanceScript* pInstance;
        uint32 m_uiUpdateTimer;

        void Reset() { }

        void AttackStart(Unit* /*pWho*/) { }

        void UpdateAI(const uint32 diff)
        {
            if(!pInstance) return;
            if(pInstance->GetData(TYPE_EVENT_NPC) != NPC_GARROSH) return;

            m_uiUpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
            if(m_uiUpdateTimer <= diff)
            {
                switch(pInstance->GetData(TYPE_EVENT))
                {
                    case 130:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                        DoScriptText(SAY_STAGE_0_03h, me);
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 132);
                        break;
                    case 132:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 140);
                        break;
                    case 2010:
                        DoScriptText(SAY_STAGE_1_09, me);
                        m_uiUpdateTimer = 9000;
                        pInstance->SetData(TYPE_EVENT, 2020);
                        break;
                    case 3050:
                        DoScriptText(SAY_STAGE_2_02h, me);
                        m_uiUpdateTimer = 15000;
                        pInstance->SetData(TYPE_EVENT, 3060);
                        break;
                    case 3070:
                        DoScriptText(SAY_STAGE_2_04h, me);
                        m_uiUpdateTimer = 6000;
                        pInstance->SetData(TYPE_EVENT, 3080);
                        break;
                    case 3081:
                        DoScriptText(SAY_STAGE_2_05h, me);
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 3091);
                        break;
                    case 4030:
                        DoScriptText(SAY_STAGE_3_03h, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 4040);
                        break;
                }
            } else m_uiUpdateTimer -= diff;
            pInstance->SetData(TYPE_EVENT_TIMER, m_uiUpdateTimer);
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_garrosh_tocAI(pCreature);
    }
};

class npc_varian_toc : public CreatureScript
{
public:
    npc_varian_toc() : CreatureScript("npc_varian_toc") { }

    struct npc_varian_tocAI : public ScriptedAI
    {
        npc_varian_tocAI(Creature* pCreature): ScriptedAI(pCreature)
        {
            pInstance = (InstanceScript*)me->GetInstanceScript();
        }

        InstanceScript* pInstance;
        uint32 m_uiUpdateTimer;

        void Reset() { }

        void AttackStart(Unit* /*pWho*/) { }

        void UpdateAI(const uint32 diff)
        {
            if(!pInstance) return;
            if(pInstance->GetData(TYPE_EVENT_NPC) != NPC_VARIAN) return;

            m_uiUpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
            if(m_uiUpdateTimer <= diff)
            {
                switch(pInstance->GetData(TYPE_EVENT))
                {
                    case 120:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                        DoScriptText(SAY_STAGE_0_03a, me);
                        m_uiUpdateTimer = 2000;
                        pInstance->SetData(TYPE_EVENT, 122);
                        break;
                    case 122:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 130);
                        break;
                    case 2020:
                        DoScriptText(SAY_STAGE_1_10, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 2030);
                        break;
                    case 3051:
                        DoScriptText(SAY_STAGE_2_02a, me);
                        m_uiUpdateTimer = 10000;
                        pInstance->SetData(TYPE_EVENT, 3061);
                        break;
                    case 3071:
                        DoScriptText(SAY_STAGE_2_04a, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 3081);
                        break;
                    case 3080:
                        DoScriptText(SAY_STAGE_2_05a, me);
                        m_uiUpdateTimer = 3000;
                        pInstance->SetData(TYPE_EVENT, 3090);
                        break;
                    case 4020:
                        DoScriptText(SAY_STAGE_3_03a, me);
                        m_uiUpdateTimer = 5000;
                        pInstance->SetData(TYPE_EVENT, 4040);
                        break;
                }
            } else m_uiUpdateTimer -= diff;
            pInstance->SetData(TYPE_EVENT_TIMER, m_uiUpdateTimer);
        }
    };

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new npc_varian_tocAI(pCreature);
    }
};

void AddSC_trial_of_the_crusader()
{
    new npc_announcer_toc10;
    new boss_lich_king_toc;
    new npc_fizzlebang_toc;
    new npc_tirion_toc;
    new npc_garrosh_toc;
    new npc_varian_toc;
}