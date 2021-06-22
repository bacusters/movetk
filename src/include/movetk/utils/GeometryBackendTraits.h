/*
 * Copyright (C) 2018-2020 HERE Europe B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 * License-Filename: LICENSE
 */

//
// Created by Mitra, Aniket on 29/01/2019.
//

// TODO: remove
#define CGAL_BACKEND_ENABLED 0

#if CGAL_BACKEND_ENABLED
#include "movetk/geom/CGALTraits.h"
#else
#include "movetk/geom/BoostGeometryTraits.h"
#endif

#include "movetk/geom/GeometryInterface.h"
#include "movetk/metric/Norm.h"
#include "movetk/utils/Iterators.h"
#include "movetk/ds/FreeSpaceDiagram.h"
#include "movetk/TuplePrinter.h"

struct boost_backend_tag;
struct cgal_backend_tag;

template<typename Tag>
struct BackendTraits
{
    
};
template<>
struct BackendTraits<boost_backend_tag>
{
    template<typename NT, std::size_t Dimensions>
    using backend_type = movetk_support::BoostGeometryTraits<NT, Dimensions>;

    template<typename Backend>
    using wrapper_geometry = typename Backend::Wrapper_Boost_Geometry;

    // Boost only supports 2 dimensions
    template<std::size_t Dimensions>
    static constexpr bool SUPPORTS_DIMENSIONS() { return Dimensions == 2; }

    
};
template<typename NT, std::size_t Dimensions, typename Tag>
struct DeduceKernel
{
    using BackendTraits_ = BackendTraits<Tag>;
    using Backend = typename BackendTraits_::template backend_type<NT, Dimensions>;
    using WrapGeom = typename BackendTraits_::template wrapper_geometry<Backend>;
    using Kernel = movetk_core::MovetkGeometryKernel<WrapGeom>;
};

#if CGAL_BACKEND_ENABLED
template<>
struct BackendTraits<cgal_backend_tag>
{
    template<typename NT, std::size_t Dimensions>
    using backend_type = movetk_support::CGALTraits<NT, Dimensions>;
    template<typename Backend>
    using wrapper_geometry = typename Backend::Wrapper_CGAL_Geometry;
    template<std::size_t Dimensions>
    static constexpr bool SUPPORTS_DIMENSIONS = true;
};
#endif

struct GeometryKernel
{

#if CGAL_BACKEND_ENABLED
    //==============================
    // Define the Number Type
    // For the CGAL backend,
    // One can choose from the
    // following number types
    typedef long double NT;
    //typedef CGAL::Mpzf NT;
    //typedef CGAL::Gmpfr NT;
    //typedef CGAL::Gmpq NT;
    //==============================

    //==============================
    // Define the Dimensions
    // It is possible to choose
    // a higher dimension
    const static size_t dimensions = 2;
    //const size_t dimensions = 7;
    //==============================

    //==============================
    // Define the Geometry Backend
    typedef movetk_support::CGALTraits<NT, dimensions> CGAL_GeometryBackend;
    //Using the Geometry Backend define the Movetk Geometry Kernel
    typedef movetk_core::MovetkGeometryKernel<
        typename CGAL_GeometryBackend::Wrapper_CGAL_Geometry>
        MovetkGeometryKernel;
#else
    //==============================
    // Define the Number Type
    // For the Boost Backend
    using NT = long double;
    //==============================

    //==============================
    // Define the Dimensions
    // It is possible to choose
    // a higher dimension
    // Note: Boost Geometry Adapter supports geometry in two
    // dimensions only
    static constexpr size_t dimensions = 2;
    //==============================

    //==============================
    // Define the Geometry Backend
    using Boost_Geometry_Backend = movetk_support::BoostGeometryTraits<NT, dimensions>;
    //Using the Geometry Backend define the Movetk Geometry Kernel
    using MovetkGeometryKernel = movetk_core::MovetkGeometryKernel<Boost_Geometry_Backend::Wrapper_Boost_Geometry>;
    //==============================
#endif

    using Norm = movetk_support::FiniteNorm<MovetkGeometryKernel, 2>;

    using SphSegIntersectionTraits = movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
                                            movetk_core::sphere_segment_intersection_tag>;

    typedef movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
                                            movetk_core::sphere_sphere_intersection_tag>
        SphSphIntersectionTraits;

    typedef movetk_support::FreeSpaceCellTraits<SphSegIntersectionTraits> FreeSpaceCellTraits;
    typedef movetk_support::FreeSpaceCell<FreeSpaceCellTraits> FreeSpaceCell;
    typedef movetk_support::FreeSpaceDiagramTraits<FreeSpaceCell> FreeSpaceDiagramTraits;
    typedef movetk_support::FreeSpaceDiagram<FreeSpaceDiagramTraits> FreeSpaceDiagram;
};

template<typename NT_, std::size_t Dimensions_, typename BackendTag>
struct VerifyKernelSupport
{
    // Verify that the chosen backend supports the given dimensions.
    static_assert(BackendTraits<BackendTag>::template SUPPORTS_DIMENSIONS<Dimensions_>());
};

template<typename NT_, std::size_t Dimensions_, typename BackendTag>
struct GeometryKernel2 :
    VerifyKernelSupport<NT_,Dimensions_, BackendTag>,
    DeduceKernel<NT_, Dimensions_, BackendTag>::Kernel
{
    //==============================
    // Define the Number Type
    // For the Boost Backend
    using NT = NT_;
    //==============================

    static constexpr size_t dimensions = Dimensions_;

    //==============================
    // Define the Geometry Backend
    using Backend = typename BackendTraits<BackendTag>::template backend_type<NT, dimensions>;
    //Using the Geometry Backend define the Movetk Geometry Kernel
    using MovetkGeometryKernel = movetk_core::MovetkGeometryKernel<typename BackendTraits<BackendTag>::template wrapper_geometry<Backend>>;
    //==============================

    using Norm = movetk_support::FiniteNorm<MovetkGeometryKernel, 2>;

    using SphSegIntersectionTraits = movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm,
        movetk_core::sphere_segment_intersection_tag>;

    using SphSphIntersectionTraits = movetk_core::IntersectionTraits<MovetkGeometryKernel, Norm, 
        movetk_core::sphere_sphere_intersection_tag>;

    using FreeSpaceCellTraits = movetk_support::FreeSpaceCellTraits<SphSegIntersectionTraits>;
    using FreeSpaceCell = movetk_support::FreeSpaceCell<FreeSpaceCellTraits>;
    using FreeSpaceDiagramTraits = movetk_support::FreeSpaceDiagramTraits<FreeSpaceCell>;
    using FreeSpaceDiagram = movetk_support::FreeSpaceDiagram<FreeSpaceDiagramTraits>;
};

template<typename NT, std::size_t Dimensions>
using MoveTkBoostKernel = GeometryKernel2<NT, Dimensions, boost_backend_tag>;

// Predefine a kernel
using MoveTkBoostKernel_2_ld = MoveTkBoostKernel<long double, 2>;