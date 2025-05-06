/*
 * Copyright (c) scott.cgi All Rights Reserved.
 *
 * This source code belongs to project Mojoc, which is a pure C Game Engine hosted on GitHub.
 * The Mojoc Game Engine is licensed under the MIT License, and will continue to be iterated with coding passion.
 *
 * License  : https://github.com/scottcgi/Mojoc/blob/master/LICENSE
 * GitHub   : https://github.com/scottcgi/Mojoc
 * CodeStyle: https://github.com/scottcgi/Mojoc/blob/master/Docs/CodeStyle.md
 *
 * Since    : 2013-7-7
 * Update   : 2019-2-12
 * Author   : scott.cgi
 */


#include <string.h>
#include "MojoC_SubMesh.h"
#include "MojoC_SkeletonSlot.h"


static inline void SetAttachmentToBone(SkeletonSlot* slot)
{
    SkeletonAttachmentData* attachmentData = slot->attachmentData;
    Drawable*               drawable;

    switch (attachmentData->type)
    {
        case SkeletonAttachmentDataType_Region:
        {
            SkeletonRegionAttachmentData* regionAttachmentData = attachmentData->childPtr;
            SubMesh*                      subMesh              = AArrayList_Get
                                                                 (
                                                                     AArrayList_GetPtr
                                                                     (
                                                                         slot->skeleton->meshList,
                                                                         regionAttachmentData->meshIndex,
                                                                         Mesh
                                                                     )
                                                                     ->childList,
                                                                     
                                                                     regionAttachmentData->subMeshIndex,
                                                                     SubMesh*
                                                                 );

            drawable         = subMesh->drawable;
            drawable->width  = regionAttachmentData->width;
            drawable->height = regionAttachmentData->height;

            ADrawable_SetParent   (drawable, slot->bone->drawable);
            ADrawable_SetPosition2(drawable, regionAttachmentData->x,      regionAttachmentData->y);
            ADrawable_SetRotationZ(drawable, regionAttachmentData->rotationZ);
            ADrawable_SetScale2   (drawable, regionAttachmentData->scaleX, regionAttachmentData->scaleY);

            break;
        }

        case SkeletonAttachmentDataType_Mesh:
        {
            SkeletonMeshAttachmentData* meshAttachmentData = attachmentData->childPtr;
            SubMesh*                    subMesh            = AArrayList_Get
                                                             (
                                                                 AArrayList_GetPtr
                                                                 (
                                                                     slot->skeleton->meshList,
                                                                     meshAttachmentData->meshIndex,
                                                                     Mesh
                                                                 )
                                                                 ->childList,
                                                                 
                                                                 meshAttachmentData->subMeshIndex,
                                                                 SubMesh*
                                                             );

            drawable         = subMesh->drawable;
            drawable->width  = meshAttachmentData->width;
            drawable->height = meshAttachmentData->height;
            ADrawable_SetParent(drawable, slot->bone->drawable);

            break;
        }

        case SkeletonAttachmentDataType_SkinnedMesh:
        {
            SkeletonSkinnedMeshAttachmentData* skinnedMeshAttachmentData = attachmentData->childPtr;
            SkeletonMeshAttachmentData*        meshAttachmentData        = skinnedMeshAttachmentData->meshAttachmentData;
            SubMesh*                           subMesh                   = AArrayList_Get
                                                                           (
                                                                             AArrayList_GetPtr
                                                                             (
                                                                                 slot->skeleton->meshList,
                                                                                 meshAttachmentData->meshIndex,
                                                                                 Mesh
                                                                             )
                                                                             ->childList,

                                                                             meshAttachmentData->subMeshIndex,
                                                                             SubMesh*
                                                                           );

            float* positions      = subMesh->positionArr->data;
            int*   bones          = skinnedMeshAttachmentData->boneArr->data;
            float* weights        = skinnedMeshAttachmentData->weightArr->data;
            float* weightVertices = skinnedMeshAttachmentData->weightVertexArr->data;

            for (int i = 0, w = 0, v = 0, p = 0; i < skinnedMeshAttachmentData->boneArr->length; p += 3)
            {
                float wx = 0.0f;
                float wy = 0.0f;
                int   k  = bones[i] + i + 1;
                
                ++i;

                for (; i < k; ++i, ++w, v += 3)
                {
                    int           boneIndex = bones[i];
                    SkeletonBone* bone      = AArray_GetPtr(slot->skeleton->boneArr, boneIndex, SkeletonBone);
                    float         weight    = weights[w];
                    Vector2       v2[1];

                    AMatrix->MultiplyMV2
                    (
                        bone->drawable->modelMatrix,
                        weightVertices[v],
                        weightVertices[v + 1],
                        v2
                    );

                    wx += v2->x * weight;
                    wy += v2->y * weight;
                }

                positions[p]     = wx;
                positions[p + 1] = wy;
            }

            drawable = subMesh->drawable;
            // we compute final world coordinate, so no parent
            ADrawable_SetParent(drawable, NULL);

            break;
        }

        default:
            return;
    }

    if (memcmp(drawable->color, slot->color, sizeof(Color)) != 0)
    {
        ADrawable_SetColor(drawable, slot->color);
    }

    if (ADrawable_CheckVisible(drawable) == false)
    {
        ADrawable_SetVisible(drawable);
    }
}


static void SetAttachmentData(SkeletonSlot* slot, SkeletonAttachmentData* attachmentData)
{
    if (slot->attachmentData == attachmentData)
    {
        return;
    }

    if (slot->attachmentData != NULL)
    {
        SubMesh* subMesh = ASkeleton->GetAttachmentSubMesh(slot->skeleton, slot->attachmentData);

        if (subMesh != NULL)
        {
            ADrawable_SetInvisible(subMesh->drawable);
        }
    }

    slot->attachmentData = attachmentData;
    *slot->color         = *slot->slotData->color;

    if (attachmentData != NULL)
    {
        SetAttachmentToBone(slot);
    }
}


static void SetToSetupPose(SkeletonSlot* slot)
{
    const char* attachmentName = slot->slotData->attachmentName;

    if (attachmentName != NULL)
    {
        SetAttachmentData(slot, ASkeleton->GetAttachmentData(slot->skeleton, slot->slotData->name, attachmentName));
    }
}


static void Init(SkeletonSlotData* slotData, Skeleton* skeleton, SkeletonSlot* outSlot)
{
    outSlot->slotData       = slotData;
    outSlot->skeleton       = skeleton;
    outSlot->attachmentData = NULL;
    outSlot->bone           = AArrayStrMap_Get(skeleton->boneMap, slotData->boneData->name, SkeletonBone*);

    SetToSetupPose(outSlot);
}


static SkeletonSlot* Create(SkeletonSlotData* slotData, Skeleton* skeleton)
{
    SkeletonSlot* slot = malloc(sizeof(SkeletonSlot));
    Init(slotData, skeleton, slot);

    return slot;
}


struct ASkeletonSlot ASkeletonSlot[1] =
{{
    Create,
    Init,
    SetAttachmentData,
    SetToSetupPose,
}};

