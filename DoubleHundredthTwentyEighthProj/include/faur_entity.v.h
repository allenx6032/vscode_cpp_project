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

#ifndef F_INC_ECS_ENTITY_V_H
#define F_INC_ECS_ENTITY_V_H

#include "faur_entity.p.h"

#include "faur_list.v.h"
#include "faur_collection.v.h"
#include "faur_component.v.h"
#include "faur_system.v.h"

#define F_ENTITY__ACTIVE_REMOVED F_FLAGS_BIT(0) // kicked by active-only system
#define F_ENTITY__ACTIVE_PERMANENT F_FLAGS_BIT(1) // always report as active
#define F_ENTITY__DEBUG F_FLAGS_BIT(2) // print debug messages for this entity
#define F_ENTITY__REMOVED F_FLAGS_BIT(3) // marked for removal, may have refs
#define F_ENTITY__REMOVE_INACTIVE F_FLAGS_BIT(4) // mark for removal if kicked
#define F_ENTITY__ALLOC_STRING_ID F_FLAGS_BIT(5) // free string ID if set

typedef enum {
    F_LIST__DEFAULT, // no pending changes
    F_LIST__NEW, // new entities that aren't in any systems yet
    F_LIST__RESTORE, // entities matched to systems, to be added to them
    F_LIST__FLUSH, // muted or removed entities, to be flushed from systems
    F_LIST__FREE, // entities to be freed at the end of current frame
    F_LIST__NUM
} FEntityList;

struct FEntity {
    char* id; // specified name for debugging
    FEntity* parent; // manually associated parent entity
    FListIntrNode node; // list node in one of FEntityList
    FEntityList uniqueList; // bucket list this entity is in
    FListIntrNode collectionNode; // collection list node
    const FCollection* collectionList; // collection backpointer
    FList* matchingSystemsActive; // FList<const FSystem*>
    FList* matchingSystemsRest; // FList<const FSystem*>
    FList* systemNodesActive; // FList<FListNode*> in active-only FSystem lists
    FList* systemNodesEither; // FList<FListNode*> in rest FSystem lists
    F__EcsBitfield componentBits; // each component's bit ID is set
    unsigned lastActive; // frame when f_entity_activeSet was last called
    int references; // if >0, then the entity lingers in the removed limbo list
    int muteCount; // if >0, then the entity isn't picked up by any systems
    unsigned flags; // various properties
    #if F_CONFIG_ECS_COM_NUM > 0
        FComponentInstance* componentsTable[F_CONFIG_ECS_COM_NUM]; // Data/NULL
    #endif
};

extern unsigned f_entity__num;
extern unsigned f_entity__numActive;

extern bool f_entity__bulkFreeInProgress;

extern void f_entity__init(void);
extern void f_entity__uninit(void);

extern void f_entity__tick(void);
extern void f_entity__flushFromSystems(void);

extern void f_entity__free(FEntity* Entity);

extern void f_entity__flushFromSystemsActive(FEntity* Entity);

#endif // F_INC_ECS_ENTITY_V_H
