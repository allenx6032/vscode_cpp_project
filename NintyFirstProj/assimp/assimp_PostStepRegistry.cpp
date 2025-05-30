/*
---------------------------------------------------------------------------
Open Asset Import Library (assimp)
---------------------------------------------------------------------------

Copyright (c) 2006-2016, assimp team

All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
---------------------------------------------------------------------------
*/

/** @file ImporterRegistry.cpp

Central registry for all postprocessing steps available. Do not edit this file
directly (unless you are adding new steps), instead use the
corresponding preprocessor flag to selectively disable steps.
*/

#include "assimp_ProcessHelper.h"

#ifndef ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS
#   include "assimp_CalcTangentsProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_JOINVERTICES_PROCESS
#   include "assimp_JoinVerticesProcess.h"
#endif
#if !(defined ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS && defined ASSIMP_BUILD_NO_FLIPUVS_PROCESS && defined ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS)
#   include "assimp_ConvertToLHProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_TRIANGULATE_PROCESS
#   include "assimp_TriangulateProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS
#   include "assimp_GenFaceNormalsProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS
#   include "assimp_GenVertexNormalsProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_REMOVEVC_PROCESS
#   include "assimp_RemoveVCProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS
#   include "assimp_SplitLargeMeshes.h"
#endif
#ifndef ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS
#   include "assimp_PretransformVertices.h"
#endif
#ifndef ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS
#   include "assimp_LimitBoneWeightsProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_VALIDATEDS_PROCESS
#   include "assimp_ValidateDataStructure.h"
#endif
#ifndef ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS
#   include "assimp_ImproveCacheLocality.h"
#endif
#ifndef ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS
#   include "assimp_FixNormalsStep.h"
#endif
#ifndef ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS
#   include "assimp_RemoveRedundantMaterials.h"
#endif
#ifndef ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS
#   include "assimp_FindInvalidDataProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS
#   include "assimp_FindDegenerates.h"
#endif
#ifndef ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS
#   include "assimp_SortByPTypeProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS
#   include "assimp_ComputeUVMappingProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS
#   include "assimp_TextureTransform.h"
#endif
#ifndef ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS
#   include "assimp_FindInstancesProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS
#   include "assimp_OptimizeMeshes.h"
#endif
#ifndef ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS
#   include "assimp_OptimizeGraph.h"
#endif
#ifndef ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS
#   include "assimp_SplitByBoneCountProcess.h"
#endif
#ifndef ASSIMP_BUILD_NO_DEBONE_PROCESS
#   include "assimp_DeboneProcess.h"
#endif

namespace Assimp {

// ------------------------------------------------------------------------------------------------
void GetPostProcessingStepInstanceList(std::vector< BaseProcess* >& out)
{
    // ----------------------------------------------------------------------------
    // Add an instance of each post processing step here in the order
    // of sequence it is executed. Steps that are added here are not
    // validated - as RegisterPPStep() does - all dependencies must be given.
    // ----------------------------------------------------------------------------
    out.reserve(25);
#if (!defined ASSIMP_BUILD_NO_MAKELEFTHANDED_PROCESS)
    out.push_back( new MakeLeftHandedProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_FLIPUVS_PROCESS)
    out.push_back( new FlipUVsProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_FLIPWINDINGORDER_PROCESS)
    out.push_back( new FlipWindingOrderProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_REMOVEVC_PROCESS)
    out.push_back( new RemoveVCProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS)
    out.push_back( new RemoveRedundantMatsProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_FINDINSTANCES_PROCESS)
    out.push_back( new FindInstancesProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS)
    out.push_back( new OptimizeGraphProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS)
    out.push_back( new FindDegeneratesProcess());
#endif
#ifndef ASSIMP_BUILD_NO_GENUVCOORDS_PROCESS
    out.push_back( new ComputeUVMappingProcess());
#endif
#ifndef ASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS
    out.push_back( new TextureTransformStep());
#endif
#if (!defined ASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS)
    out.push_back( new PretransformVertices());
#endif
#if (!defined ASSIMP_BUILD_NO_TRIANGULATE_PROCESS)
    out.push_back( new TriangulateProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS)
    out.push_back( new SortByPTypeProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS)
    out.push_back( new FindInvalidDataProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS)
    out.push_back( new OptimizeMeshesProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS)
    out.push_back( new FixInfacingNormalsProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS)
    out.push_back( new SplitByBoneCountProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS)
    out.push_back( new SplitLargeMeshesProcess_Triangle());
#endif
#if (!defined ASSIMP_BUILD_NO_GENFACENORMALS_PROCESS)
    out.push_back( new GenFaceNormalsProcess());
#endif

    // .........................................................................
    // DON'T change the order of these five ..
    // XXX this is actually a design weakness that dates back to the time
    // when Importer would maintain the postprocessing step list exclusively.
    // Now that others access it too, we need a better solution.
    out.push_back( new ComputeSpatialSortProcess());
    // .........................................................................

#if (!defined ASSIMP_BUILD_NO_GENVERTEXNORMALS_PROCESS)
    out.push_back( new GenVertexNormalsProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_CALCTANGENTS_PROCESS)
    out.push_back( new CalcTangentsProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_JOINVERTICES_PROCESS)
    out.push_back( new JoinVerticesProcess());
#endif

    // .........................................................................
    out.push_back( new DestroySpatialSortProcess());
    // .........................................................................

#if (!defined ASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS)
    out.push_back( new SplitLargeMeshesProcess_Vertex());
#endif
#if (!defined ASSIMP_BUILD_NO_DEBONE_PROCESS)
    out.push_back( new DeboneProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS)
    out.push_back( new LimitBoneWeightsProcess());
#endif
#if (!defined ASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS)
    out.push_back( new ImproveCacheLocalityProcess());
#endif
}

}
