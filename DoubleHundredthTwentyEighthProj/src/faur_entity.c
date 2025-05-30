/*
    Copyright 2016 Alex Margarit <alex@alxm.org>
    This file is part of Faur, a C video game framework.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3,
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "faur_entity.v.h"
#include <faur.v.h>

#if F_CONFIG_ECS
static FListIntr g_lists[F_LIST__NUM]; // FListIntr<FEntity*>
static unsigned g_activeNumPermanent; // Number of always-active entities

unsigned f_entity__num; // Number of entities this frame
unsigned f_entity__numActive; // Number of active entities this frame

bool f_entity__bulkFreeInProgress; // Set to prevent using freed entities

static inline bool canDelete(const FEntity* Entity)
{
    return Entity->references == 0
            && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED);
}

static inline bool listIsIn(const FEntity* Entity, FEntityList List)
{
    return Entity->uniqueList == List;
}

static inline void listAddTo(FEntity* Entity, FEntityList List)
{
    f_listintr_addLast(&g_lists[List], Entity);
    Entity->uniqueList = List;
}

static inline void listMoveTo(FEntity* Entity, FEntityList List)
{
    f_listintr_removeNode(&Entity->node);
    listAddTo(Entity, List);
}

void f_entity__init(void)
{
    for(int i = F_LIST__NUM; i--; ) {
        f_listintr_init(&g_lists[i], FEntity, node);
    }
}

void f_entity__uninit(void)
{
    f_entity__bulkFreeInProgress = true;

    for(int i = F_LIST__NUM; i--; ) {
        f_listintr_clearEx(&g_lists[i], (FCallFree*)f_entity__free);
    }
}

void f_entity__tick(void)
{
    f_entity__numActive = g_activeNumPermanent;

    f_entity__flushFromSystems();

    // Check what systems the new entities match
    F_LISTINTR_ITERATE(&g_lists[F_LIST__NEW], FEntity*, e) {
        for(unsigned s = F_CONFIG_ECS_SYS_NUM; s--; ) {
            const FSystem* system = f_system__array[s];

            if(F_ECS__BITS_TEST(
                e->componentBits, system->runtime->componentBits)) {

                if(system->onlyActiveEntities) {
                    f_list_addLast(e->matchingSystemsActive, (FSystem*)system);
                } else {
                    f_list_addLast(e->matchingSystemsRest, (FSystem*)system);
                }
            }
        }

        listAddTo(e, F_LIST__RESTORE);
    }

    // Add entities to the systems they match
    F_LISTINTR_ITERATE(&g_lists[F_LIST__RESTORE], FEntity*, e) {
        #if F_CONFIG_DEBUG
            if(f_list_sizeIsEmpty(e->matchingSystemsActive)
                && f_list_sizeIsEmpty(e->matchingSystemsRest)) {

                f_out__warning(
                    "Entity %s was not matched to any systems", e->id);
            }
        #endif

        if(!F_FLAGS_TEST_ANY(e->flags, F_ENTITY__ACTIVE_REMOVED)) {
            F_LIST_ITERATE(e->matchingSystemsActive, const FSystem*, system) {
                f_list_addLast(e->systemNodesActive,
                               f_list_addLast(system->runtime->entities, e));
            }
        }

        F_LIST_ITERATE(e->matchingSystemsRest, const FSystem*, system) {
            f_list_addLast(e->systemNodesEither,
                           f_list_addLast(system->runtime->entities, e));
        }

        listAddTo(e, F_LIST__DEFAULT);
    }

    f_listintr_clear(&g_lists[F_LIST__NEW]);
    f_listintr_clear(&g_lists[F_LIST__RESTORE]);
    f_listintr_clearEx(&g_lists[F_LIST__FREE], (FCallFree*)f_entity__free);
}

void f_entity__flushFromSystems(void)
{
    F_LISTINTR_ITERATE(&g_lists[F_LIST__FLUSH], FEntity*, e) {
        if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(e->flags, F_ENTITY__DEBUG)) {
            f_out__info("%s removed from all systems", e->id);
        }

        f_list_clearEx(e->systemNodesActive, (FCallFree*)f_list_removeNode);
        f_list_clearEx(e->systemNodesEither, (FCallFree*)f_list_removeNode);

        listAddTo(e, canDelete(e) ? F_LIST__FREE : F_LIST__DEFAULT);
    }

    f_listintr_clear(&g_lists[F_LIST__FLUSH]);
}

void f_entity__flushFromSystemsActive(FEntity* Entity)
{
    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("%s removed from active-only systems", Entity->id);
    }

    F_FLAGS_SET(Entity->flags, F_ENTITY__ACTIVE_REMOVED);
    f_list_clearEx(Entity->systemNodesActive, (FCallFree*)f_list_removeNode);

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVE_INACTIVE)) {
        f_entity_removedSet(Entity);
    }
}

FEntity* f_entity_new(const char* Id)
{
    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_new(%s): Free in progress", Id);
    }

    FEntity* e = f_pool__alloc(F_POOL__ENTITY);

    listAddTo(e, F_LIST__NEW);

    e->id = "FEntity";
    e->matchingSystemsActive = f_list_new();
    e->matchingSystemsRest = f_list_new();
    e->systemNodesActive = f_list_new();
    e->systemNodesEither = f_list_new();
    e->componentBits = F_ECS__BITS_NEW();
    e->lastActive = f_fps_ticksGet() - 1;

    if(f__collection) {
        e->collectionList = f__collection;
        f_listintr_addLast(f__collection, e);
    } else {
        f_listintr_nodeInit(&e->collectionNode);
    }

    if(Id) {
        e->id = f_str_dup(Id);
        F_FLAGS_SET(e->flags, F_ENTITY__ALLOC_STRING_ID);
    }

    f_entity__num++;

    return e;
}

void f_entity__free(FEntity* Entity)
{
    if(Entity == NULL) {
        return;
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity__free(%s)", Entity->id);
    }

    f_listintr_removeNode(&Entity->node);
    f_listintr_removeNode(&Entity->collectionNode);
    f_list_free(Entity->matchingSystemsActive);
    f_list_free(Entity->matchingSystemsRest);
    f_list_freeEx(Entity->systemNodesActive, (FCallFree*)f_list_removeNode);
    f_list_freeEx(Entity->systemNodesEither, (FCallFree*)f_list_removeNode);

    for(unsigned c = F_CONFIG_ECS_COM_NUM; c--; ) {
        f_component__instanceFree(Entity->componentsTable[c]);
    }

    if(Entity->parent) {
        f_entity_refDec(Entity->parent);
    }

    F_ECS__BITS_FREE(Entity->componentBits);

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ALLOC_STRING_ID)) {
        f_mem_free(Entity->id);
    }

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ACTIVE_PERMANENT)) {
        g_activeNumPermanent--;
        f_entity__numActive--;
    }

    f_pool_release(Entity);

    f_entity__num--;
}

void f_entity_debugSet(FEntity* Entity, bool DebugOn)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_debugSet: Free in progress");
    }

    if(F_CONFIG_DEBUG) {
        if(DebugOn) {
            F_FLAGS_SET(Entity->flags, F_ENTITY__DEBUG);
        } else {
            F_FLAGS_CLEAR(Entity->flags, F_ENTITY__DEBUG);
        }

        f_out__info("f_entity_debugSet(%s, %d)", Entity->id, DebugOn);
    }
}

const char* f_entity_idGet(const FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_idGet: Free in progress");
    }

    return Entity->id;
}

FEntity* f_entity_parentGet(const FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_parentGet: Free in progress");
    }

    return Entity->parent;
}

void f_entity_parentSet(FEntity* Entity, FEntity* Parent)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_parentSet: Free in progress");
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_parentSet(%s, %s)",
                    Entity->id,
                    Parent ? Parent->id : "NULL");
    }

    if(Parent && Parent->collectionList != Entity->collectionList) {
        F__FATAL("f_entity_parentSet(%s, %s): Different collections",
                 Entity->id,
                 Parent->id);
    }

    if(Entity->parent) {
        f_entity_refDec(Entity->parent);
    }

    Entity->parent = Parent;

    if(Parent) {
        f_entity_refInc(Parent);
    }
}

bool f_entity_parentHas(const FEntity* Child, const FEntity* PotentialParent)
{
    F__CHECK(Child != NULL);
    F__CHECK(PotentialParent != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_parentHas: Free in progress");
    }

    for(FEntity* p = Child->parent; p != NULL; p = p->parent) {
        if(p == PotentialParent) {
            return true;
        }
    }

    return false;
}

void f_entity_refInc(FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_refInc: Free in progress");
    }

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {
        F__FATAL("f_entity_refInc(%s): Entity is removed", Entity->id);
    }

    if(Entity->references == INT_MAX) {
        F__FATAL("f_entity_refInc(%s): Count too high", Entity->id);
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_refInc(%s) %d->%d",
                    Entity->id,
                    Entity->references,
                    Entity->references + 1);
    }

    Entity->references++;
}

void f_entity_refDec(FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(f_entity__bulkFreeInProgress) {
        // The entity could have already been freed despite any outstanding
        // references. This is the only FEntity API that may be called by
        // components' FCallComponentInstanceFree callbacks.
        return;
    }

    if(Entity->references == 0) {
        F__FATAL("f_entity_refDec(%s): Count too low", Entity->id);
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_refDec(%s) %d->%d",
                    Entity->id,
                    Entity->references,
                    Entity->references - 1);
    }

    Entity->references--;

    if(canDelete(Entity)) {
        listMoveTo(Entity, F_LIST__FLUSH);
    }
}

bool f_entity_removedGet(const FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_removedGet: Free in progress");
    }

    return F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED);
}

void f_entity_removedSet(FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_removedSet: Free in progress");
    }

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {
        #if F_CONFIG_DEBUG
            f_out__warning(
                "f_entity_removedSet(%s): Entity is removed", Entity->id);
        #endif

        return;
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_removedSet(%s)", Entity->id);
    }

    F_FLAGS_SET(Entity->flags, F_ENTITY__REMOVED);
    listMoveTo(Entity, F_LIST__FLUSH);
}

bool f_entity_activeGet(const FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_activeGet: Free in progress");
    }

    return F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ACTIVE_PERMANENT)
        || Entity->lastActive == f_fps_ticksGet();
}

void f_entity_activeSet(FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_activeSet: Free in progress");
    }

    if(Entity->muteCount > 0
        || F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {

        return;
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_activeSet(%s)", Entity->id);
    }

    if(!F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ACTIVE_PERMANENT)) {
        f_entity__numActive++;
    }

    Entity->lastActive = f_fps_ticksGet();

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ACTIVE_REMOVED)) {
        F_FLAGS_CLEAR(Entity->flags, F_ENTITY__ACTIVE_REMOVED);

        // Add entity back to active-only systems
        F_LIST_ITERATE(Entity->matchingSystemsActive, const FSystem*, system) {
            f_list_addLast(Entity->systemNodesActive,
                           f_list_addLast(system->runtime->entities, Entity));
        }
    }
}

void f_entity_activeSetRemove(FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_activeSetRemove: Free in progress");
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_activeSetRemove(%s)", Entity->id);
    }

    F_FLAGS_SET(Entity->flags, F_ENTITY__REMOVE_INACTIVE);
}

void f_entity_activeSetPermanent(FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_activeSetPermanent: Free in progress");
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_activeSetPermanent(%s)", Entity->id);
    }

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__ACTIVE_PERMANENT)) {
        #if F_CONFIG_DEBUG
            f_out__warning(
                "f_entity_activeSetPermanent(%s): Already set", Entity->id);
        #endif

        return;
    }

    F_FLAGS_SET(Entity->flags, F_ENTITY__ACTIVE_PERMANENT);

    g_activeNumPermanent++;
    f_entity__numActive++;
}

void* f_entity_componentAdd(FEntity* Entity, const FComponent* Component)
{
    F__CHECK(Entity != NULL);
    F__CHECK(Component != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_componentAdd: Free in progress");
    }

    if(!listIsIn(Entity, F_LIST__NEW)) {
        F__FATAL("f_entity_componentAdd(%s, %s): Too late",
                 Entity->id,
                 Component->stringId);
    }

    if(Entity->componentsTable[Component->runtime->bitId] != NULL) {
        F__FATAL("f_entity_componentAdd(%s, %s): Already added",
                 Entity->id,
                 Component->stringId);
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_componentAdd(%s, %s)",
                    Entity->id,
                    Component->stringId);
    }

    FComponentInstance* c = f_component__instanceNew(Component, Entity);

    Entity->componentsTable[Component->runtime->bitId] = c;
    F_ECS__BITS_SET(Entity->componentBits, Component->runtime->bitId);

    return c->buffer;
}

bool f_entity_componentHas(const FEntity* Entity, const FComponent* Component)
{
    F__CHECK(Entity != NULL);
    F__CHECK(Component != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_componentHas: Free in progress");
    }

    return Entity->componentsTable[Component->runtime->bitId] != NULL;
}

void* f_entity_componentGet(const FEntity* Entity, const FComponent* Component)
{
    F__CHECK(Entity != NULL);
    F__CHECK(Component != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_componentGet: Free in progress");
    }

    FComponentInstance* instance =
        Entity->componentsTable[Component->runtime->bitId];

    return instance ? instance->buffer : NULL;
}

void* f_entity_componentReq(const FEntity* Entity, const FComponent* Component)
{
    F__CHECK(Entity != NULL);
    F__CHECK(Component != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_componentReq: Free in progress");
    }

    FComponentInstance* instance =
        Entity->componentsTable[Component->runtime->bitId];

    if(instance == NULL) {
        F__FATAL("f_entity_componentReq(%s, %s): Missing component",
                 Entity->id,
                 Component->stringId);
    }

    return instance->buffer;
}

bool f_entity_muteGet(const FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_muteGet: Free in progress");
    }

    return Entity->muteCount > 0;
}

void f_entity_muteInc(FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_muteInc: Free in progress");
    }

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {
        #if F_CONFIG_DEBUG
            f_out__warning(
                "f_entity_muteInc(%s): Entity is removed", Entity->id);
        #endif

        return;
    }

    if(Entity->muteCount == INT_MAX) {
        F__FATAL("f_entity_muteInc(%s): Count too high", Entity->id);
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_muteInc(%s) %d->%d",
                    Entity->id,
                    Entity->muteCount,
                    Entity->muteCount + 1);
    }

    if(Entity->muteCount++ == 0) {
        listMoveTo(Entity, F_LIST__FLUSH);
    }
}

void f_entity_muteDec(FEntity* Entity)
{
    F__CHECK(Entity != NULL);

    if(F_CONFIG_DEBUG && f_entity__bulkFreeInProgress) {
        F__FATAL("f_entity_muteDec: Free in progress");
    }

    if(F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__REMOVED)) {
        #if F_CONFIG_DEBUG
            f_out__warning(
                "f_entity_muteDec(%s): Entity is removed", Entity->id);
        #endif

        return;
    }

    if(Entity->muteCount == 0) {
        F__FATAL("f_entity_muteDec(%s): Count too low", Entity->id);
    }

    if(F_CONFIG_DEBUG && F_FLAGS_TEST_ANY(Entity->flags, F_ENTITY__DEBUG)) {
        f_out__info("f_entity_muteDec(%s) %d->%d",
                    Entity->id,
                    Entity->muteCount,
                    Entity->muteCount - 1);
    }

    if(--Entity->muteCount == 0) {
        if(!f_list_sizeIsEmpty(Entity->matchingSystemsActive)
            || !f_list_sizeIsEmpty(Entity->matchingSystemsRest)) {

            if(listIsIn(Entity, F_LIST__FLUSH)) {
                // Entity was muted and unmuted before it left systems
                listMoveTo(Entity, F_LIST__DEFAULT);
            } else {
                // To be added back to matched systems
                listMoveTo(Entity, F_LIST__RESTORE);
            }
        } else {
            // Entity has not been matched to systems yet, treat it as new
            listMoveTo(Entity, F_LIST__NEW);
        }
    }
}
#endif // F_CONFIG_ECS
