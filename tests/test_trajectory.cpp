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


#include <array>
#include "catch2/catch.hpp"

#include <iostream>
#include <string>
using std::string;
#include <tuple>
using std::tuple;
#include <vector>
using std::vector;

#include "movetk/ds/ColumnarTrajectory.h"
#include "movetk/ds/TabularTrajectory.h"
#include "movetk/io/TuplePrinter.h"

#include "movetk/geom/BoostGeometryTraits.h"
#include "movetk/geom/BoostGeometryWrapper.h"
#include "movetk/geom/GeometryInterface.h"

TEST_CASE( "TabularTrajectory", "[tabulartrajectory]" ) {

    using ProbePoint = std::tuple<string, int, float>;
    ProbePoint p1 = {"abc", 1, 5.4};
    ProbePoint p2 = {"def", 2, 4.5};
    std::vector<ProbePoint> data = {p1, p2};

    TabularTrajectory<string, int, float> t { data };

    std::cout << "tabular trj: \n" << t;
//    for (auto x: t.get_x()) {
    for (auto x: t.get<2>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;


    vector<float> col4 {443.1, 441.3};
    auto t2 = concat_field(t, col4);

    std::cout << "concat'ed trj2: " << t2;
    for (auto x: t2.get<3>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

}

TEST_CASE("TabularTrajectory construction with iterator range", "[construct_traj_with_iterator_range]") {
    using ProbePoint = std::tuple<string, int, float>;
    ProbePoint p1 = {"abc", 1, 5.4};
    ProbePoint p2 = {"def", 7, 4.5};
    ProbePoint p3 = {"ghi", 8, 3.2};
    ProbePoint p4 = {"jkl", 10, 2.1};

    std::vector<ProbePoint> data = {p1, p2, p3, p4};

    using iterator = typename  std::vector<ProbePoint>::iterator;

    TabularTrajectory<string, int, float> t { data };

    TabularTrajectory<string, int, float> t2(std::next(t.begin()), std::next(t.begin(), 3));

    std::cout << "tabular trj2: \n" << t2;

    REQUIRE( t2.get<0>().front() == "def" );
    REQUIRE( t2.get<0>().back() == "ghi" );
}

TEST_CASE( "TabularTrajectory field iterator", "[tabulartrajectory_field_iterator]" ) {

    using ProbePoint = std::tuple<string, int, float>;
    ProbePoint p1 = {"abc", 1, 5.4};
    ProbePoint p2 = {"def", 2, 4.5};
    std::vector<ProbePoint> data = {p1, p2};

    using iterator = typename  std::vector<ProbePoint>::iterator;

    TabularTrajectory<string, int, float> t { data };

    /*int i = 0;
    std::cout << "tabular trj first col: ";
    for (auto it = t.begin<0>(); it != t.end<0>(); ++it) {
        std::cout << *it << " ";
        ++i;
    }*/
    const auto i = std::distance(t.begin<0>(), t.end<0>());

    auto expected = {"abc", "def"};
    REQUIRE( std::equal(t.begin<0>(), t.end<0>(), std::begin(expected)) );
    REQUIRE( expected.size() == i );

    auto first = t.begin<0>();
    auto last = t.end<0>();
    REQUIRE( std::distance(first, last) == 2);
}

TEST_CASE( "Update field values of TabularTrajectory", "[update_tabulartrajectory]" ) {

    using ProbePoint = std::tuple<string, int, float>;
    ProbePoint p1 = {"abc", 1, 5.4};
    ProbePoint p2 = {"def", 2, 4.5};
    std::vector<ProbePoint> data = {p1, p2};

    TabularTrajectory<string, int, float> t { data };

    std::cout << "tabular trj: \n" << t;
//    for (auto x: t.get_x()) {
    for (auto x: t.get<2>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;


    vector<float> new_col2 {443.1, 441.3};
    t.update_field<2>(new_col2);

    std::cout << "updated trj: " << t;
    for (auto x: t.get<2>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    REQUIRE(t.get<2>()[0] == Approx(443.1));
    REQUIRE(t.get<2>()[1] == Approx(441.3));
}


TEST_CASE( "ColumnarTrajectory", "[columnartrajectory]" ) {

    vector<string> col0 {"abc", "ghi", "aaa"};
    vector<int> col1 {5, 4, 3};
    vector<float> col2 {33.13, 11.30, 21.20};
    vector<float> col3 {3.1, 1.3, 2.2};

    tuple<vector<string>, vector<int>, vector<float>, vector<float>> data = std::make_tuple(col0, col1, col2, col3);

//    ColumnarTrajectory<2, 3, 0, 1, string, int, float, float> t{ data };
    ColumnarTrajectory<string, int, float, float> t{ data };

    std::cout << "trj: ";
//    for (auto x: t.get_x()) {
    for (auto x: t.get<2>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;


    vector<float> new_col3 {443.1, 441.3, 442.2};
    t.update_field<2>(new_col3);

    std::cout << "updated trj: ";
    for (auto x: t.get<2>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    REQUIRE( t.get<2>()[0] == Approx(443.1) );
    REQUIRE( t.get<2>()[1] == Approx(441.3) );
    REQUIRE( t.get<2>()[2] == Approx(442.2) );

}

TEST_CASE( "Update field values of ColumnarTrajectory", "[update_columnartrajectory]" ) {

    vector<string> col0 {"abc", "ghi", "aaa"};
    vector<int> col1 {5, 4, 3};
    vector<float> col2 {33.13, 11.30, 21.20};
    vector<float> col3 {3.1, 1.3, 2.2};

    tuple<vector<string>, vector<int>, vector<float>, vector<float>> data = std::make_tuple(col0, col1, col2, col3);

//    ColumnarTrajectory<2, 3, 0, 1, string, int, float, float> t{ data };
    ColumnarTrajectory<string, int, float, float> t{ data };

    std::cout << "trj: ";
//    for (auto x: t.get_x()) {
    for (auto x: t.get<2>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;


    vector<float> col4 {443.1, 441.3, 442.2};
    auto t2 = concat_field(t, col4);

    std::cout << "trj2: ";
    for (auto x: t2.get<4>()) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

}

TEST_CASE( "insert points to trajectory", "[insert_points_to_trajectory]" ) {

    using ProbePoint = std::tuple<string, int, float>;
    ProbePoint p1 = {"abc", 1, 5.4};
    ProbePoint p2 = {"def", 7, 4.5};
    std::vector<ProbePoint> data = {p1, p2};

    TabularTrajectory<string, int, float> t { data };

    std::cout << "tabular trj: \n" << t;


    SECTION("insert middle/range end violated") {
        ProbePoint p3 = {"ghi", 3, 3.2};
        ProbePoint p4 = {"jkl", 8, 2.1};
        std::vector<ProbePoint> data2 = {p3, p4};
        TabularTrajectory<string, int, float> t2 { data2 };

        auto pos_it = std::begin(t)+1;
        REQUIRE_THROWS(t.insert<1>(pos_it, std::begin(t2), std::end(t2)));
    }

    SECTION("insert middle/range begin violated") {
        ProbePoint p3_1 = {"ghi", 0, 3.2};
        ProbePoint p4_1 = {"jkl", 5, 2.1};
        std::vector<ProbePoint> data2_1 = {p3_1, p4_1};
        TabularTrajectory<string, int, float> t2_1 { data2_1 };

        auto pos_it = std::begin(t)+1;
        REQUIRE_THROWS(t.insert<1>(pos_it, std::begin(t2_1), std::end(t2_1)));
    }

    SECTION("insert middle/correct range") {
        ProbePoint p5 = {"mno", 5, 1.2};
        ProbePoint p6 = {"prs", 6, 1.1};
        std::vector<ProbePoint> data3 = {p5, p6};
        TabularTrajectory<string, int, float> t3 { data3 };

        auto pos_it = std::begin(t)+1;
        t.insert<1>(pos_it, std::begin(t3), std::end(t3));
        std::cout << "after insert trj: " << t;
        std::cout << "t3: " << t3;

        REQUIRE(t.size()==4);
        REQUIRE(std::get<0>(*(t.begin()+1))=="mno");
    }

    SECTION("prepend") {

        ProbePoint p0 = {"xyz", 0, -5.4};
        std::vector<ProbePoint> data0 = {p0};
        TabularTrajectory<string, int, float> t0 { data0 };

        t.insert<1>(t.begin(), std::begin(t0), std::end(t0));

        std::cout << "after prepend trj: " << t;
        std::cout << "t0: " << t0;

        REQUIRE(t.size()==3);
        REQUIRE(std::get<0>(*(t.begin()))=="xyz");
    }

    SECTION("append") {

        ProbePoint p7 = {"tuv", 10, 50.4};
        std::vector<ProbePoint> data7 = {p7};
        TabularTrajectory<string, int, float> t7 { data7 };

        t.insert<1>(std::end(t), std::begin(t7), std::end(t7));

        std::cout << "after append trj: " << t;
        std::cout << "t7: " << t7;

        REQUIRE(t.size()==3);
        REQUIRE(std::get<0>(*(t.rbegin()))=="tuv");
    }
}

TEST_CASE( "trajectory lookup by offset", "[lookupbyoffset]" ) {

    //==============================
    // Define the Number Type
    typedef double NT;
    // Define the Geometry Backend
    typedef movetk_support::BoostGeometryTraits<NT, 3> Boost_GeometryBackend;
    // Using the Geometry Backend define the Movetk Geometry Kernel
    typedef movetk_core::MovetkGeometryKernel<
            typename Boost_GeometryBackend::Wrapper_Boost_Geometry> MovetkGeometryKernel;
    //==============================

    // Phoenix_downtown bottom-left as origin
    double lat0 = 33.439361, lon0 = -112.084793; // origin
    using ProbePoint = std::tuple<double, double, std::string>;
    std::vector<ProbePoint> data { ProbePoint {lat0, lon0, "0"} };

    std::vector<std::pair<double, double>> true_xys {{20, 0}, {20, 10}, {-10, -30}, {-10, 0}, {0, 0}};
    std::vector<double> true_offsets {0, 20, 30, 80, 110, 120};  // length of each segment: {20, 10, 50, 30, 10};

    int i = 1;
    for (auto & [x, y] : true_xys) {
        double lat1, lon1;
        destination_exact(lat0, lon0, x, y, lat1, lon1);
        data.push_back(ProbePoint{lat1, lon1, std::to_string(i)});
        i++;
    }

    TabularTrajectory<double, double, std::string> t { data };
    std::cout << "tabular trj: \n" << t;

    auto offset_fn = t.lookup_offset_fn<MovetkGeometryKernel, 0, 1>();

    std::array<std::string, 8> expected_labels {"1", "2", "4", "3", "5", "-1", "0", "2"};
    i = 0;
    for (auto offset: {10, 25, 90, 35, 115, 130, -10, 20}) {
        double a, b;
        auto traj_it = offset_fn(offset, a, b);
        if (traj_it == std::end(t)) {
            std::cout << "Offset " << offset << " is beyond trajectory's end.\n";
            REQUIRE( expected_labels[i] == "-1" );
        }
        else if (traj_it == std::begin(t)) {
            std::cout << "Offset " << offset << " is beyond trajectory's start.\n";
            REQUIRE( expected_labels[i] == "0" );
        }
        else {
            std::string label = std::get<2>(*traj_it);
            std::cout << "discrete point at offset: " << label << '\n';
            std::cout << a << " <= " << offset << " <= " << b << "\n";
            REQUIRE( expected_labels[i] == label );
            REQUIRE( a <= offset );
            REQUIRE( b >= offset );
        }
        ++i;
    }

    SECTION("Reverse lookup") {
        auto res_opt = offset_fn.reverse_lookup(t.begin() + 3);
        REQUIRE( res_opt );
        REQUIRE( *res_opt == Approx(80.).epsilon(0.00001) );
    }
}