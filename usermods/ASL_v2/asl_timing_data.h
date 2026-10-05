#pragma once
// GENERATED FILE - do not edit by hand.
// Source: timing/segment_times.csv (stopwatch rides, see timing/README.md)
// Regenerate: python usermods/ASL_v2/timing/gen_timing.py
//
// Per line, in Track1 order (see asl_map_data.h for track direction notes):
//   <Line>LineTravelS[d] = seconds from doors closed at station d to doors
//                          open at station d+1 (one entry per domain)
//   <Line>LineDwellS[s]  = seconds doors open at station s
// 0 = no measurement: the sim uses the "Fallback Station Dwell" setting for
// dwell and the legacy 3 s/circuit placeholder speed for travel.
// Track2 runs the same timeline in reverse.

#include "asl_map_data.h"

// Red Line, Track1 order: Shady Grove -> Glenmont
static constexpr uint16_t RedLineTravelS[] = {
   192,  // Shady Grove -> Rockville
   146,  // Rockville -> Twinbrook
   104,  // Twinbrook -> North Bethesda
   118,  // North Bethesda -> Grosvenor-Strathmore
   165,  // Grosvenor-Strathmore -> Medical Center
    96,  // Medical Center -> Bethesda
   130,  // Bethesda -> Friendship Heights
    80,  // Friendship Heights -> Tenleytown-AU
   118,  // Tenleytown-AU -> Van Ness-UDC
    73,  // Van Ness-UDC -> Cleveland Park
    80,  // Cleveland Park -> Woodley Park-Zoo
   104,  // Woodley Park-Zoo -> Dupont Circle
    64,  // Dupont Circle -> Farragut North
    98,  // Farragut North -> Metro Center
     0,  // Metro Center -> Gallery Place-Chinatown  // no data: placeholder speed
    57,  // Gallery Place-Chinatown -> Judiciary Square
    86,  // Judiciary Square -> Union Station
    88,  // Union Station -> NoMa-Gallaudet U
   160,  // NoMa-Gallaudet U -> Rhode Island Ave-Brentwood
    83,  // Rhode Island Ave-Brentwood -> Brookland-CUA
   110,  // Brookland-CUA -> Fort Totten
   136,  // Fort Totten -> Takoma
   114,  // Takoma -> Silver Spring
   154,  // Silver Spring -> Forest Glen
   141,  // Forest Glen -> Wheaton
   150,  // Wheaton -> Glenmont
};
static constexpr uint16_t RedLineDwellS[] = {
     0,  // Shady Grove  // no data: fallback dwell setting
    24,  // Rockville
    26,  // Twinbrook
    32,  // North Bethesda
    31,  // Grosvenor-Strathmore
    26,  // Medical Center
    21,  // Bethesda
    24,  // Friendship Heights
    24,  // Tenleytown-AU
    22,  // Van Ness-UDC
    26,  // Cleveland Park
    26,  // Woodley Park-Zoo
    27,  // Dupont Circle
    25,  // Farragut North
    33,  // Metro Center
    34,  // Gallery Place-Chinatown
     0,  // Judiciary Square  // no data: fallback dwell setting
    40,  // Union Station
    31,  // NoMa-Gallaudet U
    26,  // Rhode Island Ave-Brentwood
     0,  // Brookland-CUA  // no data: fallback dwell setting
    34,  // Fort Totten
    31,  // Takoma
    28,  // Silver Spring
    28,  // Forest Glen
    36,  // Wheaton
     0,  // Glenmont  // no data: fallback dwell setting
};
static_assert(sizeof(RedLineTravelS) / sizeof(RedLineTravelS[0]) == Red_Num_LED_Domains,
              "Red: travel table must have one entry per domain - regenerate with timing/gen_timing.py");
static_assert(sizeof(RedLineDwellS) / sizeof(RedLineDwellS[0]) == RedLineNumStationsInLine,
              "Red: dwell table must have one entry per station - regenerate with timing/gen_timing.py");

// Blue Line, Track1 order: Franconia-Springfield -> Downtown Largo
static constexpr uint16_t BlueLineTravelS[] = {
   282,  // Franconia-Springfield -> Van Dorn Street
   358,  // Van Dorn Street -> King St-Old Town
    77,  // King St-Old Town -> Braddock Road
   335,  // Braddock Road -> Reagan National Airport
    71,  // Reagan National Airport -> Crystal City
    82,  // Crystal City -> Pentagon City
   108,  // Pentagon City -> Pentagon
   166,  // Pentagon -> Arlington Cemetery
   100,  // Arlington Cemetery -> Rosslyn
   137,  // Rosslyn -> Foggy Bottom-GWU
    72,  // Foggy Bottom-GWU -> Farragut West
    57,  // Farragut West -> McPherson Square
    70,  // McPherson Square -> Metro Center
    52,  // Metro Center -> Federal Triangle
    73,  // Federal Triangle -> Smithsonian
    64,  // Smithsonian -> L'Enfant Plaza
    51,  // L'Enfant Plaza -> Federal Center SW
    68,  // Federal Center SW -> Capitol South
    66,  // Capitol South -> Eastern Market
    77,  // Eastern Market -> Potomac Ave
   140,  // Potomac Ave -> Stadium-Armory
   209,  // Stadium-Armory -> Benning Road
   130,  // Benning Road -> Capitol Heights
    96,  // Capitol Heights -> Addison Road-Seat Pleasant
   125,  // Addison Road-Seat Pleasant -> Morgan Boulevard
   149,  // Morgan Boulevard -> Downtown Largo
};
static constexpr uint16_t BlueLineDwellS[] = {
     0,  // Franconia-Springfield  // no data: fallback dwell setting
    34,  // Van Dorn Street
    28,  // King St-Old Town
    24,  // Braddock Road
    44,  // Reagan National Airport
    17,  // Crystal City
    23,  // Pentagon City
    17,  // Pentagon
    21,  // Arlington Cemetery
    26,  // Rosslyn
    28,  // Foggy Bottom-GWU
    25,  // Farragut West
    29,  // McPherson Square
    33,  // Metro Center
    35,  // Federal Triangle
    28,  // Smithsonian
    26,  // L'Enfant Plaza
    20,  // Federal Center SW
    35,  // Capitol South
    30,  // Eastern Market
    30,  // Potomac Ave
    27,  // Stadium-Armory
    26,  // Benning Road
    26,  // Capitol Heights
    28,  // Addison Road-Seat Pleasant
    34,  // Morgan Boulevard
     0,  // Downtown Largo  // no data: fallback dwell setting
};
static_assert(sizeof(BlueLineTravelS) / sizeof(BlueLineTravelS[0]) == Blue_Num_LED_Domains,
              "Blue: travel table must have one entry per domain - regenerate with timing/gen_timing.py");
static_assert(sizeof(BlueLineDwellS) / sizeof(BlueLineDwellS[0]) == BlueLineNumStationsInLine,
              "Blue: dwell table must have one entry per station - regenerate with timing/gen_timing.py");

// Green Line, Track1 order: Branch Avenue -> Greenbelt
static constexpr uint16_t GreenLineTravelS[] = {
   198,  // Branch Avenue -> Suitland
   148,  // Suitland -> Naylor Road
   126,  // Naylor Road -> Southern Avenue
   114,  // Southern Avenue -> Congress Heights
   120,  // Congress Heights -> Anacostia
   113,  // Anacostia -> Navy Yard-Ballpark
    68,  // Navy Yard-Ballpark -> Waterfront
    92,  // Waterfront -> L'Enfant Plaza
    67,  // L'Enfant Plaza -> Archives-Navy Memorial
    52,  // Archives-Navy Memorial -> Gallery Place-Chinatown
    63,  // Gallery Place-Chinatown -> Mt Vernon Sq-7th St
    59,  // Mt Vernon Sq-7th St -> Shaw-Howard U
    62,  // Shaw-Howard U -> U Street/Cardozo
    98,  // U Street/Cardozo -> Columbia Heights
   117,  // Columbia Heights -> Georgia Ave-Petworth
   140,  // Georgia Ave-Petworth -> Fort Totten
   196,  // Fort Totten -> West Hyattsville
   128,  // West Hyattsville -> Hyattsville Crossing
   180,  // Hyattsville Crossing -> College Park-U of Md
   232,  // College Park-U of Md -> Greenbelt
};
static constexpr uint16_t GreenLineDwellS[] = {
     0,  // Branch Avenue  // no data: fallback dwell setting
    39,  // Suitland
    40,  // Naylor Road
    34,  // Southern Avenue
    27,  // Congress Heights
    32,  // Anacostia
    32,  // Navy Yard-Ballpark
    28,  // Waterfront
    26,  // L'Enfant Plaza
    27,  // Archives-Navy Memorial
    34,  // Gallery Place-Chinatown
    26,  // Mt Vernon Sq-7th St
    29,  // Shaw-Howard U
    38,  // U Street/Cardozo
    42,  // Columbia Heights
    39,  // Georgia Ave-Petworth
    34,  // Fort Totten
    28,  // West Hyattsville
    28,  // Hyattsville Crossing
    36,  // College Park-U of Md
     0,  // Greenbelt  // no data: fallback dwell setting
};
static_assert(sizeof(GreenLineTravelS) / sizeof(GreenLineTravelS[0]) == Green_Num_LED_Domains,
              "Green: travel table must have one entry per domain - regenerate with timing/gen_timing.py");
static_assert(sizeof(GreenLineDwellS) / sizeof(GreenLineDwellS[0]) == GreenLineNumStationsInLine,
              "Green: dwell table must have one entry per station - regenerate with timing/gen_timing.py");

// Orange Line, Track1 order: Vienna/Fairfax-GMU -> New Carrollton
static constexpr uint16_t OrangeLineTravelS[] = {
   236,  // Vienna/Fairfax-GMU -> Dunn Loring-Merrifield
   225,  // Dunn Loring-Merrifield -> West Falls Church
   226,  // West Falls Church -> East Falls Church
   234,  // East Falls Church -> Ballston-MU
    77,  // Ballston-MU -> Virginia Square-GMU
    62,  // Virginia Square-GMU -> Clarendon
    65,  // Clarendon -> Court House
   132,  // Court House -> Rosslyn
   137,  // Rosslyn -> Foggy Bottom-GWU
    72,  // Foggy Bottom-GWU -> Farragut West
    57,  // Farragut West -> McPherson Square
    70,  // McPherson Square -> Metro Center
    52,  // Metro Center -> Federal Triangle
    73,  // Federal Triangle -> Smithsonian
    64,  // Smithsonian -> L'Enfant Plaza
    51,  // L'Enfant Plaza -> Federal Center SW
    68,  // Federal Center SW -> Capitol South
    66,  // Capitol South -> Eastern Market
    77,  // Eastern Market -> Potomac Ave
   140,  // Potomac Ave -> Stadium-Armory
   263,  // Stadium-Armory -> Minnesota Ave
    99,  // Minnesota Ave -> Deanwood
   122,  // Deanwood -> Cheverly
   149,  // Cheverly -> Landover
   134,  // Landover -> New Carrollton
};
static constexpr uint16_t OrangeLineDwellS[] = {
     0,  // Vienna/Fairfax-GMU  // no data: fallback dwell setting
    37,  // Dunn Loring-Merrifield
    35,  // West Falls Church
    28,  // East Falls Church
    24,  // Ballston-MU
    26,  // Virginia Square-GMU
    22,  // Clarendon
    28,  // Court House
    26,  // Rosslyn
    28,  // Foggy Bottom-GWU
    25,  // Farragut West
    29,  // McPherson Square
    33,  // Metro Center
    35,  // Federal Triangle
    28,  // Smithsonian
    26,  // L'Enfant Plaza
    20,  // Federal Center SW
    35,  // Capitol South
    30,  // Eastern Market
    30,  // Potomac Ave
    27,  // Stadium-Armory
    23,  // Minnesota Ave
    28,  // Deanwood
    24,  // Cheverly
    24,  // Landover
     0,  // New Carrollton  // no data: fallback dwell setting
};
static_assert(sizeof(OrangeLineTravelS) / sizeof(OrangeLineTravelS[0]) == Orange_Num_LED_Domains,
              "Orange: travel table must have one entry per domain - regenerate with timing/gen_timing.py");
static_assert(sizeof(OrangeLineDwellS) / sizeof(OrangeLineDwellS[0]) == OrangeLineNumStationsInLine,
              "Orange: dwell table must have one entry per station - regenerate with timing/gen_timing.py");

// Yellow Line, Track1 order: Huntington -> Fort Totten
static constexpr uint16_t YellowLineTravelS[] = {
     0,  // Huntington -> Eisenhower Avenue  // no data: placeholder speed
     0,  // Eisenhower Avenue -> King St-Old Town  // no data: placeholder speed
    77,  // King St-Old Town -> Braddock Road
   335,  // Braddock Road -> Reagan National Airport
    71,  // Reagan National Airport -> Crystal City
    82,  // Crystal City -> Pentagon City
   108,  // Pentagon City -> Pentagon
   278,  // Pentagon -> L'Enfant Plaza
    67,  // L'Enfant Plaza -> Archives-Navy Memorial
    52,  // Archives-Navy Memorial -> Gallery Place-Chinatown
    63,  // Gallery Place-Chinatown -> Mt Vernon Sq-7th St
    59,  // Mt Vernon Sq-7th St -> Shaw-Howard U
    62,  // Shaw-Howard U -> U Street/Cardozo
    98,  // U Street/Cardozo -> Columbia Heights
   117,  // Columbia Heights -> Georgia Ave-Petworth
   140,  // Georgia Ave-Petworth -> Fort Totten
};
static constexpr uint16_t YellowLineDwellS[] = {
     0,  // Huntington  // no data: fallback dwell setting
     0,  // Eisenhower Avenue  // no data: fallback dwell setting
    28,  // King St-Old Town
    24,  // Braddock Road
    44,  // Reagan National Airport
    17,  // Crystal City
    23,  // Pentagon City
    17,  // Pentagon
    26,  // L'Enfant Plaza
    27,  // Archives-Navy Memorial
    34,  // Gallery Place-Chinatown
    26,  // Mt Vernon Sq-7th St
    29,  // Shaw-Howard U
    38,  // U Street/Cardozo
    42,  // Columbia Heights
    39,  // Georgia Ave-Petworth
    34,  // Fort Totten
};
static_assert(sizeof(YellowLineTravelS) / sizeof(YellowLineTravelS[0]) == Yellow_Num_LED_Domains,
              "Yellow: travel table must have one entry per domain - regenerate with timing/gen_timing.py");
static_assert(sizeof(YellowLineDwellS) / sizeof(YellowLineDwellS[0]) == YellowLineNumStationsInLine,
              "Yellow: dwell table must have one entry per station - regenerate with timing/gen_timing.py");

