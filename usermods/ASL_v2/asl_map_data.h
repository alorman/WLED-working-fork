#pragma once
#include <Arduino.h>

/*
 * ASL WMATA metro map — static track/LED mapping data.
 *
 * All tables are constexpr so they are placed in flash, not RAM.
 * Terminology:
 *  - "circuit" / "segment": WMATA track circuit IDs
 *  - "domain": a contiguous run of circuits between two stations, as {first,last}
 *  - "LED array": physical LED index ranges corresponding to each domain
 * Sim timing (seconds per domain and per station) lives in asl_timing_data.h,
 * generated from timing/segment_times.csv.
 *
 * Track / direction conventions (not documented in the original source;
 * derived from the circuit numbering, see timing/README.md):
 *  - Every table for both tracks is written in the same geographic order:
 *    index 0 is the LED-0 end of the line. Domain y of Track1 and domain y of
 *    Track2 are the same stretch of map, and {first,last} of a domain maps to
 *    the {first,last} LED of LEDArray[y].
 *  - Track1 trains travel from index 0 upward (LED 0 -> end of strip);
 *    Track2 trains travel the other way. The sim relies on this.
 *  - Track1 direction per line:
 *      Red    Shady Grove -> Glenmont
 *      Blue   Franconia-Springfield -> Downtown Largo   (no Potomac Yard on this hardware)
 *      Green  Branch Avenue -> Greenbelt               (opposite of the timing sheet)
 *      Orange Vienna -> New Carrollton
 *      Yellow Huntington -> Fort Totten                (old route; no Potomac Yard)
 *  - The Track1/Track2 labels on shared track are not fully consistent:
 *    north of L'Enfant Plaza, Yellow Track1 (northbound) uses the circuits of
 *    Green Track2 (southbound). Harmless for rendering, because a circuit is
 *    looked up on both tracks, but one of the two lines has its labels swapped.
 */

//Track Domains
//Red Line
// Track1 = Shady Grove -> Glenmont. Circuits 7-203 are the 15 stations from Shady
// Grove to Metro Center, then the numbering jumps to the Gallery Place - Glenmont side.
static constexpr uint16_t RedLineTrack1Domains [][2] = { {  8, 31},{ 33, 52},{ 54, 61},{ 63, 79},{ 81, 94},{ 96,108},{110,125},{127,132},{134,141},{143,153},{155,163},{165,178},{180,189},{191,202},{462,466},{468,476},{478,484},{486,495},{497,512},{514,526},{528,547},{549,570},{572,590},{592,610},{612,628},{630,651} };
static constexpr uint16_t RedLineTrack2Domains [][2] = {  {211,231},{233,250},{252,259},{261,277},{279,293},{295,308},{310,325},{327,335},{337,345},{347,355},{357,362},{364,377},{379,388},{390,400},{662,666},{668,676},{678,685},{687,699},{701,716},{718,730},{732,756},{758,784},{786,808},{810,827},{829,845},{847,867} };

static constexpr uint16_t RedLineTrack1StationSegments[] = {  7, 32, 53, 62, 80, 95,109,126,133,142,154,164,179,190,203,467,477,485,496,513,527,548,571,591,611,629,652};  
static constexpr uint16_t RedLineTrack2StationSegments[] = {210,232,251,260,278,294,309,326,336,346,356,363,378,389,661,667,677,686,700,717,731,757,785,809,828,846,868};

//Blue Line
// Track1 = Franconia-Springfield -> Downtown Largo. The first two domains (26xx) are
// Franconia -> Van Dorn -> King St. Potomac Yard (opened 2023) is not on this hardware;
// Braddock Rd -> Reagan National is one domain.
static constexpr uint16_t BlueLineTrack1Domains [][2] = { {2605,2633},{2635,2673},{967 , 975},{ 977,1009},{1011,1023},{1025,1035},{1037,1051},{1053,1069},{1071,1091},{1093,1104},{1106,1116},{1118,1125},{1127,1134},{1378,1383},{1385,1392},{1394,1399},{1401,1405},{1407,1417},{1419,1423},{1425,1435},{1437,1460},{2409,2419},{2421,2433},{2435,2448},{2450,2468},{2470,2486} };
static constexpr uint16_t BlueLineTrack2Domains [][2] = { {2680,2708},{2710,2752},{1160,1169},{1171,1203},{1205,1216},{1218,1229},{1231,1245},{1247,1264},{1266,1284},{1286,1297},{1299,1309},{1311,1322},{1324,1329},{1544,1548},{1550,1558},{1560,1567},{1569,1574},{1576,1589},{1591,1597},{1599,1609},{1611,1634},{2495,2505},{2507,2520},{2522,2536},{2538,2556},{2558,2573} };

static constexpr uint16_t BlueLineTrack1StationSegments[] = {2604,2634, 966, 976,1010,1024,1036,1052,1070,1092,1105,1117,1126,1135,1384,1393,1400,1406,1418,1424,1436,1461,2420,2434,2449,2469,2487};  
static constexpr uint16_t BlueLineTrack2StationSegments[] = {2679,2709,1159,1170,1204,1217,1230,1246,1265,1285,1298,1310,1323,1330,1549,1559,1568,1575,1590,1598,1610,1635,2506,2521,2537,2557,2574};

//Green Line
// Track1 = Branch Avenue -> Greenbelt (index 0 = Branch Ave). This is the opposite
// of the direction the timing sheet was ridden; timing/gen_timing.py reverses it.
// Evidence: Yellow's station circuits 11-16 equal Green Track2's 11-16, which only
// lines up station-by-station (Mt Vernon Sq ... Fort Totten) with Branch Ave at 0,
// and domain 9 (4 circuits) is then Archives -> Gallery Place, the shortest hop.
static constexpr uint16_t GreenLineTrack1Domains [][2] = { {2119,2135},{2137,2153},{2155,2169},{2171,2182},{2184,2198},{2200,2207},{2209,2218},{2220,2230},{2232,2240},{2242,2245},{1744,1752},{1754,1763},{1765,1772},{1774,1781},{1783,1795},{1797,1808},{1810,1832},{1834,1849},{1851,1870},{1872,1893} };
static constexpr uint16_t GreenLineTrack2Domains [][2] = { {2256,2271},{2273,2290},{2292,2302},{2304,2316},{2318,2332},{2334,2341},{2343,2351},{2353,2363},{2365,2375},{2377,2380},{1900,1910},{1912,1922},{1924,1931},{1933,1941},{1943,1955},{1957,1970},{1972,1991},{1993,2008},{2010,2029},{2031,2054} };

static constexpr uint16_t GreenLineTrack1StationSegments[] = {2118,2136,2154,2170,2183,2199,2208,2219,2231,2241,2246,1753,1764,1773,1782,1796,1809,1833,1850,1871,1894};  
static constexpr uint16_t GreenLineTrack2StationSegments[] = {2255,2272,2291,2303,2317,2333,2342,2352,2364,2376,1899,1911,1923,1932,1942,1956,1971,1992,2009,2030,2055};

//Orange Line
// Track1 = Vienna -> New Carrollton. The first 8 domains (27xx-29xx) are Vienna -> Rosslyn.
// Station list fixes (checked against the domain gaps and the Blue line's shared stations):
//  - Track1 was missing Federal Triangle (1384) and Smithsonian (1393); Track2 was
//    missing the same two stations (1549, 1559). Without them those two stations had
//    no station circuit, every Orange station from L'Enfant Plaza on was off by two
//    in the station -> LED lookup, and the last two station dots were never drawn.
//  - Track1 domain {1383,1378} was written backwards (Blue has {1378,1383}); the
//    range test could never match, so live trains there had no LED position.
// Still unverified (left as-is):
//  - Rosslyn: Orange Track1 uses station circuit 1089, Blue uses 1092.
//  - Stadium-Armory: Orange Track1 uses 1443 after domain {1437,1442}; Blue uses 1461
//    after domain {1437,1460}, which runs through 1443.
//  - Vienna: Track1 station circuit 2774 lies inside domain {2755,2795}; a train
//    passing circuits 2773-2775 snaps back onto the Vienna dot for a moment.
//  - OrangeLineStationLEDPosition overlaps OrangeLineLEDArray at three stations
//    (every other line leaves a one-LED gap for each station): Metro Center 112 is
//    the last LED of {100,112}, L'Enfant Plaza 132 the last of {123,132}, and
//    Stadium-Armory 163 sits inside {158,164} while LED 165 is unused. Trains step
//    back about one LED arriving at Stadium-Armory.
static constexpr uint16_t OrangeLineTrack1Domains [][2] = { {2755,2795},{2797,2816},{2818,2843},{2845,2869},{2871,2885},{2887,2897},{2899,2910},{2912,2927},{1090,1104},{1106,1116},{1118,1125},{1127,1134},{1378,1383},{1385,1392},{1394,1399},{1401,1405},{1407,1417},{1419,1423},{1425,1435},{1437,1442},{1444,1474},{1476,1486},{1488,1499},{1501,1521},{1523,1541} };
static constexpr uint16_t OrangeLineTrack2Domains [][2] = { {2934,2953},{2955,2975},{2977,3000},{3002,3022},{3024,3036},{3038,3047},{3049,3060},{3062,3075},{1283,1297},{1299,1309},{1311,1322},{1324,1329},{1544,1548},{1550,1558},{1560,1567},{1569,1574},{1576,1589},{1591,1597},{1599,1609},{1611,1617},{1619,1642},{1644,1656},{1658,1669},{1671,1691},{1693,1710} };

static constexpr uint16_t OrangeLineTrack1StationSegments[] = {2774,2796,2817,2844,2870,2886,2898,2911,1089,1105,1117,1126,1135,1384,1393,1400,1406,1418,1424,1436,1443,1475,1487,1500,1522,1542};  
static constexpr uint16_t OrangeLineTrack2StationSegments[] = {2933,2954,2976,3001,3023,3037,3048,3061,1282,1298,1310,1323,1330,1549,1559,1568,1575,1590,1598,1610,1618,1643,1657,1670,1692,1711};

//Yellow Line
// Track1 = Huntington -> Fort Totten (the old route north of Mt Vernon Sq; the north
// end shares Green's circuits). Potomac Yard is not on this hardware.
// Unverified (left as-is) - station circuits that lie inside a domain, so passing
// trains briefly snap onto the station dot:
//  - Track1 King St-Old Town = 969, inside {967,975}; the domain gap and Blue's
//    King St both say 966.
//  - Track2 Pentagon = 1246, inside {1231,1247}; Blue Track2 has {1231,1245}.
//  - Track2 L'Enfant Plaza = 2231, inside {2230,2240}; Green Track1 has {2232,2240}.
static constexpr uint16_t YellowLineTrack1Domains [][2] = { { 945, 954},{ 956, 965},{ 967, 975},{ 977,1009},{1011,1023},{1025,1035},{1037,1053},{3124,3145},{2363,2375},{2377,2380},{1900,1910},{1912,1922},{1924,1931},{1933,1941},{1943,1955},{1957,1970} };
static constexpr uint16_t YellowLineTrack2Domains [][2] = { {1138,1147},{1149,1161},{1163,1169},{1171,1203},{1205,1216},{1218,1229},{1231,1247},{3105,3123},{2230,2240},{2242,2245},{1744,1752},{1754,1763},{1765,1772},{1774,1781},{1783,1795},{1797,1808} };

static constexpr uint16_t YellowLineTrack1StationSegments[] = { 944, 955, 969, 976,1010,1024,1036,1054,2362,2376,1899,1911,1923,1932,1942,1956,1971};  
static constexpr uint16_t YellowLineTrack2StationSegments[] = {1137,1148,1162,1170,1204,1217,1230,1246,2231,2241,2246,1753,1764,1773,1782,1796,1809};

//LED ARRAYS (in real-world space)
//Red LED Arrays
static constexpr uint16_t RedLineStationLEDPosition[] = {0,11,22,33,44,56,65,73,79,87,94,99,104,109,138,149,155,165,175,182,192,205,228,242,250,258,266}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
static constexpr uint16_t RedLineLEDArray [][2] = { {1,10},{12,21},{23,32},{34,43},{45,55},{57,64},{66,72},{74,78},{80,86},{88,93},{95,98},{100,103},{105,108},{110,137},{139,148},{150,154},{156,164},{166,174},{176,181},{183,191},{193,204},{206,227},{229,241},{243,249},{251,257},{259,265} };

//Blue LED Arrays
static constexpr uint16_t BlueLineStationLEDPosition[] = {0,23,56,62,72,82,92,101,121,149,168,178,188,195,200,206,224,230,236,245,251,256,267,278,284,290,297}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
static constexpr uint16_t BlueLineLEDArray [][2] = { {1,22},{24,55},{57,61},{63,71},{73,81},{83,91},{93,100},{102,120},{122,148},{150,167},{169,177},{179,187},{189,194},{196,199},{201,205},{207,223},{225,229},{231,235},{237,244},{246,250},{252,255},{257,266},{268,277},{279,283},{285,289},{291,296} };

//Green LED Arrays
static constexpr uint16_t GreenLineStationLEDPosition[] = {0,8,15,22,32,37,48,53,73,80,86,93,104,115,124,134,152,158,166,173,180}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
static constexpr uint16_t GreenLineLEDArray [][2] = { {1,7},{9,14},{16,21},{23,31},{33,36},{38,47},{49,52},{54,72},{74,79},{81,85},{87,92},{94,103},{105,114},{116,123},{125,133},{135,151,},{153,157,},{159,165},{167,172},{174,179} };

//Orange LED Arrays
static constexpr uint16_t OrangeLineStationLEDPosition[] = {0,6,12,18,24,30,36,42,52,78,88,99,112,117,122,132,139,145,150,157,163,177,184,192,200,208}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
static constexpr uint16_t OrangeLineLEDArray [][2] = { {1,5},{7,11},{13,17},{19,23},{25,29},{31,35},{37,41},{43,51},{53,77},{79,87},{89,98},{100,112},{113,116},{118,121},{123,132},{134,138},{140,144},{146,149},{151,156},{158,164},{166,176},{178,183},{185,191},{193,199},{201,207} };

//Yellow LED Arrays
static constexpr uint16_t YellowLineStationLEDPosition[] = {0,8,13,18,29,42,47,56,100,107,113,120,129,135,152,170,184}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
static constexpr uint16_t YellowLineLEDArray [][2] = { {1,7},{9,12},{14,17},{19,28},{30,41},{43,46},{48,55},{57,99},{101,106},{108,112},{114,119},{121,128},{130,134},{136,151},{153,169},{171,183} };

// ---- Derived counts (all compile-time) ----
#define RedLineNumStationsInLine    (sizeof(RedLineTrack1StationSegments)/sizeof(RedLineTrack1StationSegments[0]))
#define Red_Num_LED_Domains         (sizeof(RedLineLEDArray)/sizeof(RedLineLEDArray[0]))
#define Red_Num_LEDS                (RedLineStationLEDPosition[Red_Num_LED_Domains])

#define BlueLineNumStationsInLine   (sizeof(BlueLineTrack1StationSegments)/sizeof(BlueLineTrack1StationSegments[0]))
#define Blue_Num_LED_Domains        (sizeof(BlueLineLEDArray)/sizeof(BlueLineLEDArray[0]))
#define Blue_Num_LEDS               (BlueLineStationLEDPosition[Blue_Num_LED_Domains])

#define GreenLineNumStationsInLine  (sizeof(GreenLineTrack1StationSegments)/sizeof(GreenLineTrack1StationSegments[0]))
#define Green_Num_LED_Domains       (sizeof(GreenLineLEDArray)/sizeof(GreenLineLEDArray[0]))
#define Green_Num_LEDS              (GreenLineStationLEDPosition[Green_Num_LED_Domains])

#define OrangeLineNumStationsInLine (sizeof(OrangeLineTrack1StationSegments)/sizeof(OrangeLineTrack1StationSegments[0]))
#define Orange_Num_LED_Domains      (sizeof(OrangeLineLEDArray)/sizeof(OrangeLineLEDArray[0]))
#define Orange_Num_LEDS             (OrangeLineStationLEDPosition[Orange_Num_LED_Domains])

#define YellowLineNumStationsInLine (sizeof(YellowLineTrack1StationSegments)/sizeof(YellowLineTrack1StationSegments[0]))
#define Yellow_Num_LED_Domains      (sizeof(YellowLineLEDArray)/sizeof(YellowLineLEDArray[0]))
#define Yellow_Num_LEDS             (YellowLineStationLEDPosition[Yellow_Num_LED_Domains])

// ---- table consistency checks ----
// The sim and the renderer index stations and domains in parallel across all
// of these tables (station k sits between domain k-1 and domain k), so every
// line needs: one domain per LED range on both tracks, and one station circuit
// per station LED on both tracks. A mismatch shifts trains onto the wrong
// station dots (this is how the Orange line's two missing stations showed up).
#define ASL_COUNT(a) (sizeof(a) / sizeof((a)[0]))
#define ASL_CHECK_LINE(L) \
  static_assert(ASL_COUNT(L##LineTrack1Domains) == L##_Num_LED_Domains, #L ": Track1 domain count != LED range count"); \
  static_assert(ASL_COUNT(L##LineTrack2Domains) == L##_Num_LED_Domains, #L ": Track2 domain count != LED range count"); \
  static_assert(ASL_COUNT(L##LineTrack1StationSegments) == ASL_COUNT(L##LineStationLEDPosition), #L ": Track1 station count != station LED count"); \
  static_assert(ASL_COUNT(L##LineTrack2StationSegments) == ASL_COUNT(L##LineStationLEDPosition), #L ": Track2 station count != station LED count"); \
  static_assert(ASL_COUNT(L##LineStationLEDPosition) == L##_Num_LED_Domains + 1, #L ": need exactly one more station than domains")
ASL_CHECK_LINE(Red);
ASL_CHECK_LINE(Blue);
ASL_CHECK_LINE(Green);
ASL_CHECK_LINE(Orange);
ASL_CHECK_LINE(Yellow);
#undef ASL_CHECK_LINE
