#pragma once

#include "wled.h"
#include "src/dependencies/json/ArduinoJson-v6.h"
#include <HTTPClient.h>
#include <math.h>
#include "presets.h"

#define MaxNumPossibleTrains 100

//Train line definitions
//Red Line Compile time defiitions 
#define RedLineNumStationsInLine (sizeof(RedLineTrack1StationSegments)/sizeof(RedLineTrack1StationSegments[1])) //counts number of stations we've defined above
#define Red_Num_LED_Domains (sizeof(RedLineLEDArray)/sizeof(RedLineLEDArray[0])) //counts the number of LED domains we've specified and makes all the LED number comply with that
#define Red_Num_LEDS RedLineStationLEDPosition[Red_Num_LED_Domains]//count max number of station LEDs and make this the number of LEDs we have (assumes every line ends with a station)
#define RedLineTrack1SegmentCount (sizeof(RedLineTrack1Segments)/sizeof(RedLineTrack1Segments[1])) //counts number of red line track 1 track segments
#define RedLineTrack2SegmentCount (sizeof(RedLineTrack2Segments)/sizeof(RedLineTrack2Segments[1])) //counts number of red line track 2 track segments
#define RedLineTrack1SegmentDomains (sizeof(RedLineTrack1Domains)/sizeof(RedLineTrack1Domains[1,1])) //counts the number of track 1 segment domains (including breaks and stations)
#define RedLineTrack2SegmentDomains (sizeof(RedLineTrack2Domains)/sizeof(RedLineTrack2Domains[1,1])) //and the same for track 2

//Blue line
#define BlueLineNumStationsInLine (sizeof(BlueLineTrack1StationSegments)/sizeof(BlueLineTrack1StationSegments[1])) //counts number of stations we've defined above
#define Blue_Num_LED_Domains (sizeof(BlueLineLEDArray)/sizeof(BlueLineLEDArray[1,1])) //counts the number of LED domains we've specified and makes all the LED number comply with that
#define Blue_Num_LEDS 297 //BlueLineStationLEDPosition[Blue_Num_LED_Domains]//count max number of station LEDs and make this the number of LEDs we have (assumes every line ends with a station)
#define BlueLineTrack1SegmentCount (sizeof(BlueLineTrack1Segments)/sizeof(BlueLineTrack1Segments[1])) //counts number of Blue line track 1 track segments
#define BlueLineTrack2SegmentCount (sizeof(BlueLineTrack2Segments)/sizeof(BlueLineTrack2Segments[1])) //counts number of Blue line track 2 track segments
#define BlueLineTrack1SegmentDomains (sizeof(BlueLineTrack1Domains)/sizeof(BlueLineTrack1Domains[1,1])) //counts the number of track 1 segment domains (including breaks and stations)
#define BlueLineTrack2SegmentDomains (sizeof(BlueLineTrack2Domains)/sizeof(BlueLineTrack2Domains[1,1])) //and the same for track 2

//Green Line
#define GreenLineNumStationsInLine (sizeof(GreenLineTrack1StationSegments)/sizeof(GreenLineTrack1StationSegments[1])) //counts number of stations we've defined above
#define Green_Num_LED_Domains (sizeof(GreenLineLEDArray)/sizeof(GreenLineLEDArray[1,1])) //counts the number of LED domains we've specified and makes all the LED number comply with that
#define Green_Num_LEDS GreenLineStationLEDPosition[Green_Num_LED_Domains]//count max number of station LEDs and make this the number of LEDs we have (assumes every line ends with a station)
#define GreenLineTrack1SegmentCount (sizeof(GreenLineTrack1Segments)/sizeof(GreenLineTrack1Segments[1])) //counts number of Green line track 1 track segments
#define GreenLineTrack2SegmentCount (sizeof(GreenLineTrack2Segments)/sizeof(GreenLineTrack2Segments[1])) //counts number of Green line track 2 track segments
#define GreenLineTrack1SegmentDomains (sizeof(GreenLineTrack1Domains)/sizeof(GreenLineTrack1Domains[1,1])) //counts the number of track 1 segment domains (including breaks and stations)
#define GreenLineTrack2SegmentDomains (sizeof(GreenLineTrack2Domains)/sizeof(GreenLineTrack2Domains[1,1])) //and the same for track 2

//Orange Line
#define OrangeLineNumStationsInLine (sizeof(OrangeLineTrack1StationSegments)/sizeof(OrangeLineTrack1StationSegments[1])) //counts number of stations we've defined above
#define Orange_Num_LED_Domains (sizeof(OrangeLineLEDArray)/sizeof(OrangeLineLEDArray[1,1])) //counts the number of LED domains we've specified and makes all the LED number comply with that
#define Orange_Num_LEDS OrangeLineStationLEDPosition[Orange_Num_LED_Domains]//count max number of station LEDs and make this the number of LEDs we have (assumes every line ends with a station)
#define OrangeLineTrack1SegmentCount (sizeof(OrangeLineTrack1Segments)/sizeof(OrangeLineTrack1Segments[1])) //counts number of Orange line track 1 track segments
#define OrangeLineTrack2SegmentCount (sizeof(OrangeLineTrack2Segments)/sizeof(OrangeLineTrack2Segments[1])) //counts number of Orange line track 2 track segments
#define OrangeLineTrack1SegmentDomains (sizeof(OrangeLineTrack1Domains)/sizeof(OrangeLineTrack1Domains[1,1])) //counts the number of track 1 segment domains (including breaks and stations)
#define OrangeLineTrack2SegmentDomains (sizeof(OrangeLineTrack2Domains)/sizeof(OrangeLineTrack2Domains[1,1])) //and the same for track 2

//Yellow Line
#define YellowLineNumStationsInLine (sizeof(YellowLineTrack1StationSegments)/sizeof(YellowLineTrack1StationSegments[1])) //counts number of stations we've defined above
#define Yellow_Num_LED_Domains (sizeof(YellowLineLEDArray)/sizeof(YellowLineLEDArray[1,1])) //counts the number of LED domains we've specified and makes all the LED number comply with that
#define Yellow_Num_LEDS YellowLineStationLEDPosition[Yellow_Num_LED_Domains]//count max number of station LEDs and make this the number of LEDs we have (assumes every line ends with a station)
#define YellowLineTrack1SegmentCount (sizeof(YellowLineTrack1Segments)/sizeof(YellowLineTrack1Segments[1])) //counts number of Yellow line track 1 track segments
#define YellowLineTrack2SegmentCount (sizeof(YellowLineTrack2Segments)/sizeof(YellowLineTrack2Segments[1])) //counts number of Yellow line track 2 track segments
#define YellowLineTrack1SegmentDomains (sizeof(YellowLineTrack1Domains)/sizeof(YellowLineTrack1Domains[1,1])) //counts the number of track 1 segment domains (including breaks and stations)
#define YellowLineTrack2SegmentDomains (sizeof(YellowLineTrack2Domains)/sizeof(YellowLineTrack2Domains[1,1])) //and the same for track 2

//Track Domains
//Red Line
uint16_t RedLineTrack1Domains [][2] = { {8,31},   {33,52},  {54,61},  {63,79},  {81,94},  {96,108}, {110,125},{127,132},{134,141},{143,153},{155,163},{165,178},{180,189},{191,202},{462,466},{468,476},{478,484},{486,495},{497,512},{514,526},{528,547},{549,570},{572,590},{592,610},{612,628},{630,651} };
uint16_t RedLineTrack2Domains [][2] ={  {211,231},{233,250},{252,259},{261,277},{279,293},{295,308},{310,325},{327,335},{337,345},{347,355},{357,362},{364,377},{379,388},{390,400},{662,666},{668,676},{678,685},{687,699},{701,716},{718,730},{732,756},{758,784},{786,808},{810,827},{829,845},{847,867} };
uint16_t RedLineTrack1Segments[] = {7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,501,502,503,504,505,506,507,508,509,510,511,512,513,514,515,516,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,540,541,542,543,544,545,546,547,548,549,550,551,552,553,554,555,556,557,558,559,560,561,562,563,564,565,566,567,568,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,585,586,587,588,589,590,591,592,593,594,595,596,597,598,599,600,601,602,603,604,605,606,607,608,609,610,611,612,613,614,615,616,617,618,619,620,621,622,623,624,625,626,627,628,629,630,631,632,633,634,635,636,637,638,639,640,641,642,643,644,645,646,647,648,649,650,651,652};
uint16_t RedLineTrack2Segments [] = {210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,661,662,663,664,665,666,667,668,669,670,671,672,673,674,675,676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,697,698,699,700,701,702,703,704,705,706,707,708,709,710,711,712,713,714,715,716,717,718,719,720,721,722,723,724,725,726,727,728,729,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,745,746,747,748,749,750,751,752,753,754,755,756,757,758,759,760,761,762,763,764,765,766,767,768,769,770,771,772,773,774,775,776,777,778,779,780,781,782,783,784,785,786,787,788,789,790,791,792,793,794,795,796,797,798,799,800,801,802,803,804,805,806,807,808,809,810,811,812,813,814,815,816,817,818,819,820,821,822,823,824,825,826,827,828,829,830,831,832,833,834,835,836,837,838,839,840,841,842,843,844,845,846,847,848,849,850,851,852,853,854,855,856,857,858,859,860,861,862,863,864,865,866,867,868};

uint16_t RedLineTrack1StationSegments[] = {  7, 32, 53, 62, 80, 95,109,126,133,142,154,164,179,190,203,467,477,485,496,513,527,548,571,591,611,629,652};  
uint16_t RedLineTrack2StationSegments[] = {210,232,251,260,278,294,309,326,336,346,356,363,378,389,661,667,677,686,700,717,731,757,785,809,828,846,868};

//Blue Line
uint16_t BlueLineTrack1Domains [][2] = { {2605,2623},{2635,2673},{966 , 968},{970 , 975},{ 977,1009},{1011,1023},{1025,1035},{1037,1051},{1053,1069},{1071,1091},{1093,1104},{1106,1116},{1118,1125},{1127,1134},{1378,1399},{1401,1405},{1407,1417},{1419,1423},{1425,1435},{1437,1442},{1444,1461},{2409,2419},{2421,2433},{2435,2448},{2450,2468},{2470,2486} };
uint16_t BlueLineTrack2Domains [][2] = { {2680,2708},{2710,2752},{1159,1161},{1163,1169},{1171,1203},{1205,1216},{1218,1229},{1231,1245},{1247,1264},{1266,1284},{1286,1297},{1299,1309},{1311,1322},{1324,1329},{1544,1567},{1569,1574},{1576,1589},{1591,1597},{1599,1609},{1611,1617},{1619,1635},{2423,2433},{2507,2520},{ 252,2536},{2538,2556},{2558,2573} };
uint16_t BlueLineTrack1Segments[] = {2604,2605,2606,2607,2608,2609,2610,2611,2612,2613,2614,2615,2616,2617,2618,2619,2620,2621,2622,2623,2624,2625,2626,2627,2628,2629,2630,2631,2632,2633,2634,2635,2636,2637,2638,2639,2640,2641,2642,2643,2644,2645,2646,2647,2648,2649,2650,2651,2652,2653,2654,2655,2656,2657,2658,2659,2660,2661,2662,2663,2664,2665,2666,2667,2668,2669,2670,2671,2672,2673,966,967,968,969,970,971,972,973,974,975,976,977,978,979,980,981,982,983,984,985,986,987,988,989,990,991,992,993,994,995,996,997,998,999,1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,1020,1021,1022,1023,1024,1025,1026,1027,1028,1029,1030,1031,1032,1033,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,1055,1056,1057,1058,1059,1060,1061,1062,1063,1064,1065,1066,1067,1068,1069,1070,1071,1072,1073,1074,1075,1076,1077,1078,1079,1080,1081,1082,1083,1084,1085,1086,1087,1088,1089,1090,1091,1092,1093,1094,1095,1096,1097,1098,1099,1100,1101,1102,1103,1104,1105,1106,1107,1108,1109,1110,1111,1112,1113,1114,1115,1116,1117,1118,1119,1120,1121,1122,1123,1124,1125,1126,1127,1128,1129,1130,1131,1132,1133,1134,1135,1378,1379,1380,1381,1382,1383,1384,1385,1386,1387,1388,1389,1390,1391,1392,1393,1394,1395,1396,1397,1398,1399,1400,1401,1402,1403,1404,1405,1406,1407,1408,1409,1410,1411,1412,1413,1414,1415,1416,1417,1418,1419,1420,1421,1422,1423,1424,1425,1426,1427,1428,1429,1430,1431,1432,1433,1434,1435,1436,1437,1438,1439,1440,1441,1442,1443,1444,1445,1446,1447,1448,1449,1450,1451,1452,1453,1454,1455,1456,1457,1458,1459,1460,1461,2409,2410,2411,2412,2413,2414,2415,2416,2417,2418,2419,2420,2421,2422,2423,2424,2425,2426,2427,2428,2429,2430,2431,2432,2433,2434,2435,2436,2437,2438,2439,2440,2441,2442,2443,2444,2445,2446,2447,2448,2449,2450,2451,2452,2453,2454,2455,2456,2457,2458,2459,2460,2461,2462,2463,2464,2465,2466,2467,2468,2469,2470,2471,2472,2473,2474,2475,2476,2477,2478,2479,2480,2481,2482,2483,2484,2485,2486,2487};
uint16_t BlueLineTrack2Segments [] = {2679,2680,2681,2682,2683,2684,2685,2686,2687,2688,2689,2690,2691,2692,2693,2694,2695,2696,2697,2698,2699,2700,2701,2702,2703,2704,2705,2706,2707,2708,2709,2710,2711,2712,2713,2714,2715,2716,2717,2718,2719,2720,2721,2722,2723,2724,2725,2726,2727,2728,2729,2730,2731,2732,2733,2734,2735,2736,2737,2738,2739,2740,2741,2742,2743,2744,2745,2746,2747,2748,2749,2750,2751,2752,1159,1160,1161,1162,1163,1164,1165,1166,1167,1168,1169,1170,1171,1172,1173,1174,1175,1176,1177,1178,1179,1180,1181,1182,1183,1184,1185,1186,1187,1188,1189,1190,1191,1192,1193,1194,1195,1196,1197,1198,1199,1200,1201,1202,1203,1204,1205,1206,1207,1208,1209,1210,1211,1212,1213,1214,1215,1216,1217,1218,1219,1220,1221,1222,1223,1224,1225,1226,1227,1228,1229,1230,1231,1232,1233,1234,1235,1236,1237,1238,1239,1240,1241,1242,1243,1244,1245,1246,1247,1248,1249,1250,1251,1252,1253,1254,1255,1256,1257,1258,1259,1260,1261,1262,1263,1264,1265,1266,1267,1268,1269,1270,1271,1272,1273,1274,1275,1276,1277,1278,1279,1280,1281,1282,1283,1284,1285,1286,1287,1288,1289,1290,1291,1292,1293,1294,1295,1296,1297,1298,1299,1300,1301,1302,1303,1304,1305,1306,1307,1308,1309,1310,1311,1312,1313,1314,1315,1316,1317,1318,1319,1320,1321,1322,1323,1324,1325,1326,1327,1328,1329,1330,1544,1545,1546,1547,1548,1549,1550,1551,1552,1553,1554,1555,1556,1557,1558,1559,1560,1561,1562,1563,1564,1565,1566,1567,1568,1569,1570,1571,1572,1573,1574,1575,1576,1577,1578,1579,1580,1581,1582,1583,1584,1585,1586,1587,1588,1589,1590,1591,1592,1593,1594,1595,1596,1597,1598,1599,1600,1601,1602,1603,1604,1605,1606,1607,1608,1609,1610,1611,1612,1613,1614,1615,1616,1617,1618,1619,1620,1621,1622,1623,1624,1625,1626,1627,1628,1629,1630,1631,1632,1633,1634,1635,2423,2424,2425,2426,2427,2428,2429,2430,2431,2432,2433,2506,2507,2508,2509,2510,2511,2512,2513,2514,2515,2516,2517,2518,2519,2520,2521,2522,2523,2524,2525,2526,2527,2528,2529,2530,2531,2532,2533,2534,2535,2536,2537,2538,2539,2540,2541,2542,2543,2544,2545,2546,2547,2548,2549,2550,2551,2552,2553,2554,2555,2556,2557,2558,2559,2560,2561,2562,2563,2564,2565,2566,2567,2568,2569,2570,2571,2572,2573,2574};

uint16_t BlueLineTrack1StationSegments[] = {2604,2634, 969, 976,1010,1024,1036,1052,1070,1092,1105,1117,1126,1135,1400,1406,1418,1424,1436,1443,2420,2434,2449,2469,2487};  
uint16_t BlueLineTrack2StationSegments[] = {2679,2709,1162,1170,1204,1217,1230,1246,1265,1285,1298,1310,1323,1330,1568,1575,1590,1598,1610,1618,1506,2521,2537,2557,2574};

//Green Line
uint16_t GreenLineTrack1Domains [][2] = { {2119,2135},{2137,2153},{2155,2169},{2171,2182},{2184,2198},{2200,2207},{2209,2218},{2220,2230},{2232,2240},{2242,2245},{1744,1752},{1754,1763},{1765,1772},{1774,1781},{1783,1795},{1797,1808},{1810,1832},{1834,1849},{1851,1870},{1872,1893} };
uint16_t GreenLineTrack2Domains [][2] = { {2256,2271},{2273,2290},{2292,2302},{2304,2316},{2318,2332},{2334,2341},{2343,2351},{2353,2363},{2365,2375},{2377,2380},{1900,1910},{1912,1922},{1924,1931},{1933,1941},{1943,1955},{1957,1970},{1972,1991},{1993,2008},{2010,2029},{2031,2054} };
uint16_t GreenLineTrack1Segments[] = {2118,2119,2120,2121,2122,2123,2124,2125,2126,2127,2128,2129,2130,2131,2132,2133,2134,2135,2136,2137,2138,2139,2140,2141,2142,2143,2144,2145,2146,2147,2148,2149,2150,2151,2152,2153,2154,2155,2156,2157,2158,2159,2160,2161,2162,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2173,2174,2175,2176,2177,2178,2179,2180,2181,2182,2183,2184,2185,2186,2187,2188,2189,2190,2191,2192,2193,2194,2195,2196,2197,2198,2199,2200,2201,2202,2203,2204,2205,2206,2207,2208,2209,2210,2211,2212,2213,2214,2215,2216,2217,2218,2219,2220,2221,2222,2223,2224,2225,2226,2227,2228,2229,2230,2231,2232,2233,2234,2235,2236,2237,2238,2239,2240,2241,2242,2243,2244,2245,2246,1744,1745,1746,1747,1748,1749,1750,1751,1752,1753,1754,1755,1756,1757,1758,1759,1760,1761,1762,1763,1764,1765,1766,1767,1768,1769,1770,1771,1772,1773,1774,1775,1776,1777,1778,1779,1780,1781,1782,1783,1784,1785,1786,1787,1788,1789,1790,1791,1792,1793,1794,1795,1796,1797,1798,1799,1800,1801,1802,1803,1804,1805,1806,1807,1808,1809,1810,1811,1812,1813,1814,1815,1816,1817,1818,1819,1820,1821,1822,1823,1824,1825,1826,1827,1828,1829,1830,1831,1832,1833,1834,1835,1836,1837,1838,1839,1840,1841,1842,1843,1844,1845,1846,1847,1848,1849,1850,1851,1852,1853,1854,1855,1856,1857,1858,1859,1860,1861,1862,1863,1864,1865,1866,1867,1868,1869,1870,1871,1872,1873,1874,1875,1876,1877,1878,1879,1880,1881,1882,1883,1884,1885,1886,1887,1888,1889,1890,1891,1892,1893,1894};
uint16_t GreenLineTrack2Segments [] = {2255,2256,2257,2258,2259,2260,2261,2262,2263,2264,2265,2266,2267,2268,2269,2270,2271,2272,2273,2274,2275,2276,2277,2278,2279,2280,2281,2282,2283,2284,2285,2286,2287,2288,2289,2290,2291,2292,2293,2294,2295,2296,2297,2298,2299,2300,2301,2302,2303,2304,2305,2306,2307,2308,2309,2310,2311,2312,2313,2314,2315,2316,2317,2318,2319,2320,2321,2322,2323,2324,2325,2326,2327,2328,2329,2330,2331,2332,2333,2334,2335,2336,2337,2338,2339,2340,2341,2342,2343,2344,2345,2346,2347,2348,2349,2350,2351,2352,2353,2354,2355,2356,2357,2358,2359,2360,2361,2362,2363,2364,2365,2366,2367,2368,2369,2370,2371,2372,2373,2374,2375,2376,2377,2378,2379,2380,1899,1900,1901,1902,1903,1904,1905,1906,1907,1908,1909,1910,1911,1912,1913,1914,1915,1916,1917,1918,1919,1920,1921,1922,1923,1924,1925,1926,1927,1928,1929,1930,1931,1932,1933,1934,1935,1936,1937,1938,1939,1940,1941,1942,1943,1944,1945,1946,1947,1948,1949,1950,1951,1952,1953,1954,1955,1956,1957,1958,1959,1960,1961,1962,1963,1964,1965,1966,1967,1968,1969,1970,1971,1972,1973,1974,1975,1976,1977,1978,1979,1980,1981,1982,1983,1984,1985,1986,1987,1988,1989,1990,1991,1992,1993,1994,1995,1996,1997,1998,1999,2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020,2021,2022,2023,2024,2025,2026,2027,2028,2029,2030,2031,2032,2033,2034,2035,2036,2037,2038,2039,2040,2041,2042,2043,2044,2045,2046,2047,2048,2049,2050,2051,2052,2053,2054,2055};

uint16_t GreenLineTrack1StationSegments[] = {2118,2136,2154,2170,2183,2199,2208,2219,2231,2241,2246,1753,1764,1773,1782,1796,1809,1833,1850,1871,1894};  
uint16_t GreenLineTrack2StationSegments[] = {2255,2272,2291,2303,2317,2333,2342,2352,2364,2376,1899,1911,1923,1932,1942,1956,1971,1992,2009,2030,2055};

//Orange Line
uint16_t OrangeLineTrack1Domains [][2] = { {2755,2795},{2797,2816},{2818,2843},{2845,2869},{2871,2885},{2887,2897},{2899,2910},{2912,2927},{1090,1104},{1106,1116},{1118,1125},{1127,1134},{1378,1399},{1401,1405},{1407,1417},{1419,1423},{1425,1435},{1437,1442},{1444,1474},{1476,1486},{1488,1499},{1501,1521},{1523,1541} };
uint16_t OrangeLineTrack2Domains [][2] = { {2934,2953},{2955,2975},{2977,3000},{3002,3022},{3024,3036},{3038,3047},{3049,3060},{3062,3076},{1283,1297},{1299,1309},{1311,1322},{1324,1329},{1544,1567},{1569,1574},{1576,1589},{1591,1597},{1599,1609},{1611,1617},{1619,1642},{1644,1656},{1658,1669},{1671,1691},{1693,1710} };
uint16_t OrangeLineTrack1Segments[] = {2774,2775,2776,2777,2778,2779,2780,2781,2782,2783,2784,2785,2786,2787,2788,2789,2790,2791,2792,2793,2794,2795,2796,2797,2798,2799,2800,2801,2802,2803,2804,2805,2806,2807,2808,2809,2810,2811,2812,2813,2814,2815,2816,2817,2818,2819,2820,2821,2822,2823,2824,2825,2826,2827,2828,2829,2830,2831,2832,2833,2834,2835,2836,2837,2838,2839,2840,2841,2842,2843,2844,2845,2846,2847,2848,2849,2850,2851,2852,2853,2854,2855,2856,2857,2858,2859,2860,2861,2862,2863,2864,2865,2866,2867,2868,2869,2870,2871,2872,2873,2874,2875,2876,2877,2878,2879,2880,2881,2882,2883,2884,2885,2886,2887,2888,2889,2890,2891,2892,2893,2894,2895,2896,2897,2898,2899,2900,2901,2902,2903,2904,2905,2906,2907,2908,2909,2910,2911,2912,2913,2914,2915,2916,2917,2918,2919,2920,2921,2922,2923,2924,2925,2926,2927,1090,1091,1092,1093,1094,1095,1096,1097,1098,1099,1100,1101,1102,1103,1104,1105,1106,1107,1108,1109,1110,1111,1112,1113,1114,1115,1116,1117,1118,1119,1120,1121,1122,1123,1124,1125,1126,1127,1128,1129,1130,1131,1132,1133,1134,1135,1378,1379,1380,1381,1382,1383,1384,1385,1386,1387,1388,1389,1390,1391,1392,1393,1394,1395,1396,1397,1398,1399,1400,1401,1402,1403,1404,1405,1406,1407,1408,1409,1410,1411,1412,1413,1414,1415,1416,1417,1418,1419,1420,1421,1422,1423,1424,1425,1426,1427,1428,1429,1430,1431,1432,1433,1434,1435,1436,1437,1438,1439,1440,1441,1442,1443,1444,1445,1446,1447,1448,1449,1450,1451,1452,1453,1454,1455,1456,1457,1458,1459,1460,1461,1462,1463,1464,1465,1466,1467,1468,1469,1470,1471,1472,1473,1474,1475,1476,1477,1478,1479,1480,1481,1482,1483,1484,1485,1486,1487,1488,1489,1490,1491,1492,1493,1494,1495,1496,1497,1498,1499,1500,1501,1502,1503,1504,1505,1506,1507,1508,1509,1510,1511,1512,1513,1514,1515,1516,1517,1518,1519,1520,1521,1522,1523,1524,1525,1526,1527,1528,1529,1530,1531,1532,1533,1534,1535,1536,1537,1538,1539,1540,1541,1542};
uint16_t OrangeLineTrack2Segments [] = {2933,2934,2935,2936,2937,2938,2939,2940,2941,2942,2943,2944,2945,2946,2947,2948,2949,2950,2951,2952,2953,2954,2955,2956,2957,2958,2959,2960,2961,2962,2963,2964,2965,2966,2967,2968,2969,2970,2971,2972,2973,2974,2975,2976,2977,2978,2979,2980,2981,2982,2983,2984,2985,2986,2987,2988,2989,2990,2991,2992,2993,2994,2995,2996,2997,2998,2999,3000,3001,3002,3003,3004,3005,3006,3007,3008,3009,3010,3011,3012,3013,3014,3015,3016,3017,3018,3019,3020,3021,3022,3023,3024,3025,3026,3027,3028,3029,3030,3031,3032,3033,3034,3035,3036,3037,3038,3039,3040,3041,3042,3043,3044,3045,3046,3047,3048,3049,3050,3051,3052,3053,3054,3055,3056,3057,3058,3059,3060,3061,3062,3063,3064,3065,3066,3067,3068,3069,3070,3071,3072,3073,3074,3075,3076,1283,1284,1285,1286,1287,1288,1289,1290,1291,1292,1293,1294,1295,1296,1297,1298,1299,1300,1301,1302,1303,1304,1305,1306,1307,1308,1309,1310,1311,1312,1313,1314,1315,1316,1317,1318,1319,1320,1321,1322,1323,1324,1325,1326,1327,1328,1329,1330,1544,1545,1546,1547,1548,1549,1550,1551,1552,1553,1554,1555,1556,1557,1558,1559,1560,1561,1562,1563,1564,1565,1566,1567,1568,1569,1570,1571,1572,1573,1574,1575,1576,1577,1578,1579,1580,1581,1582,1583,1584,1585,1586,1587,1588,1589,1590,1591,1592,1593,1594,1595,1596,1597,1598,1599,1600,1601,1602,1603,1604,1605,1606,1607,1608,1609,1610,1611,1612,1613,1614,1615,1616,1617,1618,1619,1620,1621,1622,1623,1624,1625,1626,1627,1628,1629,1630,1631,1632,1633,1634,1635,1636,1637,1638,1639,1640,1641,1642,1643,1644,1645,1646,1647,1648,1649,1650,1651,1652,1653,1654,1655,1656,1657,1658,1659,1660,1661,1662,1663,1664,1665,1666,1667,1668,1669,1670,1671,1672,1673,1674,1675,1676,1677,1678,1679,1680,1681,1682,1683,1684,1685,1686,1687,1688,1689,1690,1691,1692,1693,1694,1695,1696,1697,1698,1699,1700,1701,1702,1703,1704,1705,1706,1707,1708,1709,1710,1711};

uint16_t OrangeLineTrack1StationSegments[] = {2774,2796,2817,2844,2870,2886,2898,2911,1092,1105,1117,1126,1135,1400,1406,1418,1424,1436,1443,1475,1487,1500,1522,1542};  
uint16_t OrangeLineTrack2StationSegments[] = {2933,2954,2976,3001,3023,3037,3048,3061,1285,1298,1310,1323,1330,1568,1575,1590,1598,1610,1618,1643,1657,1670,1692,1711};

//Yellow Line
uint16_t YellowLineTrack1Domains [][2] = { { 945, 954},{ 956, 968},{ 970, 975},{ 977,1009},{1011,1023},{1025,1035},{1037,1051},{1053,1054},{3124,3145},{2362,2363},{2365,2375},{2377,2380},{1900,1910},{1912,1922},{1924,1931},{1933,1941},{1943,1955},{1957,1970} };
uint16_t YellowLineTrack2Domains [][2] = { {1138,1147},{1149,1161},{1163,1169},{1171,1204},{1205,1216},{1218,1229},{1231,1245},{1247,1248},{3105,3123},{2229,2230},{2232,2240},{2242,2245},{1744,1752},{1754,1763},{1765,1772},{1774,1781},{1783,1795},{1797,1808} };
uint16_t YellowLineTrack1Segments[] = {944,945,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,974,975,976,977,978,979,980,981,982,983,984,985,986,987,988,989,990,991,992,993,994,995,996,997,998,999,1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,1020,1021,1022,1023,1024,1025,1026,1027,1028,1029,1030,1031,1032,1033,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1052,1053,1054,3124,3125,3126,3127,3128,3129,3130,3131,3132,3133,3134,3135,3136,3137,3138,3139,3140,3141,3142,3143,3144,3145,2362,2363,2364,2365,2366,2367,2368,2369,2370,2371,2372,2373,2374,2375,2376,2377,2378,2379,2380,1899,1900,1901,1902,1903,1904,1905,1906,1907,1908,1909,1910,1911,1912,1913,1914,1915,1916,1917,1918,1919,1920,1921,1922,1923,1924,1925,1926,1927,1928,1929,1930,1931,1932,1933,1934,1935,1936,1937,1938,1939,1940,1941,1942,1943,1944,1945,1946,1947,1948,1949,1950,1951,1952,1953,1954,1955,1956,1957,1958,1959,1960,1961,1962,1963,1964,1965,1966,1967,1968,1969,1970,1971};
uint16_t YellowLineTrack2Segments [] = {1137,1138,1139,1140,1141,1142,1143,1144,1145,1146,1147,1148,1149,1150,1151,1152,1153,1154,1155,1156,1157,1158,1159,1160,1161,1162,1163,1164,1165,1166,1167,1168,1169,1170,1171,1172,1173,1174,1175,1176,1177,1178,1179,1180,1181,1182,1183,1184,1185,1186,1187,1188,1189,1190,1191,1192,1193,1194,1195,1196,1197,1198,1199,1200,1201,1202,1203,1204,1205,1206,1207,1208,1209,1210,1211,1212,1213,1214,1215,1216,1217,1218,1219,1220,1221,1222,1223,1224,1225,1226,1227,1228,1229,1230,1231,1232,1233,1234,1235,1236,1237,1238,1239,1240,1241,1242,1243,1244,1245,1246,1247,1248,3105,3106,3107,3108,3109,3110,3111,3112,3113,3114,3115,3116,3117,3118,3119,3120,3121,3122,3123,2229,2230,2231,2232,2233,2234,2235,2236,2237,2238,2239,2240,2241,2242,2243,2244,2245,2246,1744,1745,1746,1747,1748,1749,1750,1751,1752,1753,1754,1755,1756,1757,1758,1759,1760,1761,1762,1763,1764,1765,1766,1767,1768,1769,1770,1771,1772,1773,1774,1775,1776,1777,1778,1779,1780,1781,1782,1783,1784,1785,1786,1787,1788,1789,1790,1791,1792,1793,1794,1795,1796,1797,1798,1799,1800,1801,1802,1803,1804,1805,1806,1807,1808,1809};

uint16_t YellowLineTrack1StationSegments[] = { 944, 955, 969, 976,1010,1024,1036,1052,2364,2376,1899,1911,1923,1932,1942,1956,1071};  
uint16_t YellowLineTrack2StationSegments[] = {1137,1148,1162,1170,1204,1217,1230,1246,2231,2241,2246,1753,1764,1773,1782,1796,1789};

//Red LED Arrays
uint16_t RedLineStationLEDPosition[] = {0,11,22,33,44,56,65,73,79,87,94,99,104,109,138,149,155,165,175,182,192,205,228,242,250,258,266}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
uint16_t RedLineLEDArray [][2] = { {1,10},{12,21},{23,32},{34,43},{45,55},{57,64},{66,72},{74,78},{80,86},{88,93},{95,98},{100,103},{105,108},{110,137},{139,148},{150,154},{156,164},{166,174},{176,181},{183,191},{193,204},{206,227},{229,241},{243,249},{251,257},{259,265} };

//Blue LED Arrays
uint16_t BlueLineStationLEDPosition[] = {0,23,56,62,72,82,92,101,121,149,168,178,188,195,200,206,224,230,236,245,251,256,267,278,284,290,297}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
uint16_t BlueLineLEDArray [][2] = { {1,22},{24,55},{57,62},{63,71},{73,81},{83,91},{93,100},{102,120},{122,148},{150,167},{169,177},{179,187},{189,194},{196,199},{201,205},{207,223},{225,229},{231,235},{237,244},{246,250},{252,255},{257,266},{268,277},{279,283},{285,289},{291,296} };

//Green LED Arrays
uint16_t GreenLineStationLEDPosition[] = {0,8,15,22,32,37,48,53,73,80,86,93,104,115,124,134,152,158,166,173,180}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
uint16_t GreenLineLEDArray [][2] = { {1,7},{9,14},{16,21},{23,31},{33,36},{38,47},{49,52},{54,72},{74,79},{81,85},{87,92},{94,103},{105,114},{116,123},{125,133},{135,151,},{153,157,},{159,165},{167,172},{174,179} };

//Orange LED Arrays
uint16_t OrangeLineStationLEDPosition[] = {0,6,12,18,24,30,36,42,52,78,88,99,112,117,122,132,139,145,150,157,163,177,184,192,200,208}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
uint16_t OrangeLineLEDArray [][2] = { {1,5},{7,11},{13,17},{19,23},{25,29},{31,35},{37,41},{43,51},{53,77},{79,87},{89,98},{100,112},{113,116},{118,121},{123,132},{134,138},{140,144},{146,149},{151,156},{158,164},{166,176},{178,183},{185,191},{193,199},{201,207} };

//Yellow LED Arrays
uint16_t YellowLineStationLEDPosition[] = {0,8,13,18,29,42,47,56,100,107,113,120,129,135,152,170,184}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
uint16_t YellowLineLEDArray [][2] = { {1,7},{9,12},{14,17},{19,28},{30,41},{43,46},{48,55},{57,99},{101,112},{114,119},{121,128},{130,134},{136,151},{153,169},{171,183} };

//train position array fields
int TrainPositions_TrainId[MaxNumPossibleTrains];
int TrainPositions_TrainNumber[MaxNumPossibleTrains];
int TrainPositions_CarCount[MaxNumPossibleTrains];
int TrainPositions_DirectionNum[MaxNumPossibleTrains];
int TrainPositions_CircuitId[MaxNumPossibleTrains];
String TrainPositions_DestinationStationCode[MaxNumPossibleTrains]; 
String TrainPositions_LineCode[MaxNumPossibleTrains]; 
long TrainPositions_SecondsAtLocation[MaxNumPossibleTrains];
String TrainPositions_ServiceType[MaxNumPossibleTrains];

//train math variables
int MaxNumTrains = 0; 
int NormalTrainCount = 0;
int RedTrainCount = 0;

// test fake train data variable 
uint8_t UseFakeJsonData = 2;
String SimulatedTrains;

uint16_t TempTrain1PositionCounter = 0;
uint16_t TempTrain2PositionCounter = 399;

uint16_t RedTrack1PositionCounter = 0;
uint16_t RedTrack2PositionCounter = RedLineTrack2SegmentCount;

uint16_t BlueTrack1PositionCounter = 0;
uint16_t BlueTrack2PositionCounter = 297;

uint16_t GreenTrack1PositionCounter = 0;
uint16_t GreenTrack2PositionCounter = GreenLineTrack2SegmentDomains;

uint16_t OrangeTrack1PositionCounter = 0;
uint16_t OrangeTrack2PositionCounter = OrangeLineTrack2SegmentDomains;

uint16_t YellowTrack1PositionCounter = 0;
uint16_t YellowTrack2PositionCounter = YellowLineTrack2SegmentDomains;


/*
 * ASL's usermod test
 * Usermods allow you to add own functionality to WLED more easily
 * See: https://github.com/Aircoookie/WLED/wiki/Add-own-functionality
 * 
 * This is an example for a v2 usermod.
 * v2 usermods are class inheritance based and can (but don't have to) implement more functions, each of them is shown in this example.
 * Multiple v2 usermods can be added to one compilation easily.
 * 
 * Creating a usermod:
 * This file serves as an example. If you want to create a usermod, it is recommended to use usermod_v2_empty.h from the usermods folder as a template.
 * Please remember to rename the class and file to a descriptive name.
 * You may also use multiple .h and .cpp files.
 * 
 * Using a usermod:
 * 1. Copy the usermod into the sketch folder (same folder as wled00.ino)
 * 2. Register the usermod by adding #include "usermod_filename.h" in the top and registerUsermod(new MyUsermodClass()) in the bottom of usermods_list.cpp
 */

//class name. Use something descriptive and leave the ": public Usermod" part :)
class usermod_v2_ASL : public Usermod {
  private:
    //Private class members. You can declare variables and functions only accessible to your usermod here
    uint32_t state_seg_0_col_0_0 = 0;
    //String state_bri = "0";

    //Max number of trains




    // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    bool testBool = false;
    unsigned long testULong = 42424242;
    float testFloat = 42.42;
    String devString = "0";
    String testString = "0";
    String ServerAddressString = "https://api.wmata.com/TrainPositions/TrainPositions?contentType=json";
    String apiKeyString = "44c0d03c967442abad897ca70efd5639";
    int ServerPollIntervalSeconds = 10;
    int LEDRefreshIntervalms = 1000;
    WS2812FX::Segment &seg = strip.getSegment(0);
    uint32_t steps = 0;

    //values unchanged from dc metro code
    unsigned long RunCycles = 0; 

    //Red Line Variables
    
    uint32_t RedLineStrip[];
    uint32_t RedLineFrameTarget[];

    //red line color definitions 

    //Loop timing variables
    unsigned long lastTime = 0;
    int WMATATimeDelay = 7000; //delay for the task polling WMATA API (data updates every 7-10 seconds, clients are allowed to post GET requests at 10hz or 50k call/day (every 1.7s)
    const int FakeGETTimeDelay = 1000; //the timer to use when we're using fake data
    int LEDRefreshTime = 1; //timing for the led updating task. This gets updated by math



    //LED strip variables
    int MaxLEDDelta = 0;
    int AllLEDSBrightness = 255;


    // These config variables have defaults set inside readFromConfig()
    int testInt;
    long testLong;
    int8_t testPins[2];

  public:
    //Functions called by WLED
    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
      Serial.println("setting up.....");
      // if (!readObjectFromFile("/presets.json", nullptr, &doc)) {

        
      // }
      applyPreset(1);

      //releaseJSONBufferLock();
      
      //JsonDocument::add(TrainPlotPreset);
      //deserializeJson(doc, TrainPlotPreset);
      //JsonObject obj = doc.as<JsonObject>();
      //JsonObject& JsonPreset = jsonBuffer.parseObject(TrainPlotPreset);
      //Serial.println("Hello from my usermod!");
        // if(!WLED_FS.exists("/presets.json")) {
        // Serial.println("Writing to EEPROM");
        // const char filename[] = "/presets.json";
        // uint16_t index = 1;
        // JsonObject presets = JSONpreset;
        // writeObjectToFileUsingId(filename, index, &doc);
        // File f = WLED_FS.open("/presets.json", "w"); //open the file in write mode
        //   f.print(JSONpreset);
        //   f.close();
        // }
        // serializeJsonPretty(doc, Serial);
        // writeObjectToFileUsingId("/presets.json", 0, &doc);

        DevLEDAddress[0] = 1;
        DevLEDAddress[1] = 4;
        DevLEDAddress[2] = 6;
        DevLEDAddress[3] = 23;
        DevLEDAddress[4] = 14;

        TrackRed = BLACK;
    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
      //Serial.println("Connected to WiFi!");
        if (millis() - lastTime > 1000) {
        Serial.println("I'm alive! in connected loop");
        Serial.println(DevLEDAddress[4]);
        lastTime = millis();
      }
    }


    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     * 
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     * 
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop() {
     //Serial.println("Connected to WiFi!");
      if (millis() - lastTime > 2500) {
        Serial.println("I'm alive! in main loop");
        //readFromConfig();
        //Serial.println(ServerAddressString);
        GetWMATAdata();
        Serial.println(steps);
      Serial.println("Applying preset 1");
        steps++;
        // DynamicJsonDocument doc(16384); //JSON doc size, see JSON arduino assistant for better info
        // HTTPClient http; //establish the HTTPclient object
        // String payload;
        // String serverPath = ServerAddressString; //these are defined in globals  
        // int16_t httpResponseCode = 0;
        // http.begin(serverPath); 
        // //httpResponseCode = http.GET(); //send the GET Request
        // if (httpResponseCode > 0) {
        //   Serial.print("HTTP Response code: ");
        //   Serial.println(httpResponseCode);
        //   }
        // Serial.println(httpResponseCode);
        // payload = http.getString(); //write the Wmata response to a String object
        // http.end(); //free memory now and clean up 
        // Serial.println(payload);
        //int color = WS2812FX::color_from_palette(0, false, true, 0);
        //Serial.println(color);
        //uint32_t test = strip.getMainSegment().colors;
        //Serial.println(test);
        
       // strip.setPixelColor()
       //2trainsim(String LineCode, uint16_t Track1SegmentCount, uint16_t Track2SegmentCount, uint16_t Track1Segments[], uint16_t Track2Segments[]) 
        String JsonPremable = R"=====({"TrainPositions":[)=====";
        String JsonEpilogue = R"=====(]
        })=====";
        SimulatedTrains = JsonPremable //+ 
        //TrainSim("RD", RedLineTrack1SegmentCount, RedLineTrack2SegmentCount, RedLineTrack1Segments, RedLineTrack2Segments) + "," + 
        //TrainSim("BL", BlueLineTrack1SegmentCount, BlueLineTrack2SegmentCount, BlueLineTrack1Segments, BlueLineTrack2Segments) + "," + 
        //TrainSim("GR", GreenLineTrack1SegmentCount, GreenLineTrack2SegmentCount, GreenLineTrack1Segments, GreenLineTrack2Segments) + "," + 
        //TrainSim("OR", OrangeLineTrack1SegmentCount, OrangeLineTrack2SegmentCount, OrangeLineTrack1Segments, OrangeLineTrack2Segments) + "," + 
        //TrainSim("YL", YellowLineTrack1SegmentCount, YellowLineTrack2SegmentCount, YellowLineTrack1Segments, YellowLineTrack2Segments) 
        + JsonEpilogue;
        //Serial.println(SimulatedTrains);

        PlotLEDStations(RedTargetFrame, RedLineStationLEDPosition, RedLineNumStationsInLine, StationRed, TrackRed, Red_Num_LEDS);
        PlotLEDTrainPositions(RedTargetFrame, "RD", RedLineTrack1Domains, RedLineTrack1Segments, RedLineTrack1StationSegments, RedLineNumStationsInLine, RedLineLEDArray, RedLineStationLEDPosition, Red_Num_LED_Domains, TrainRed);
        PlotLEDTrainPositions(RedTargetFrame, "RD", RedLineTrack2Domains, RedLineTrack2Segments, RedLineTrack2StationSegments, RedLineNumStationsInLine, RedLineLEDArray, RedLineStationLEDPosition, Red_Num_LED_Domains, TrainRed);
       

        PlotLEDStations(BlueTargetFrame, BlueLineStationLEDPosition, BlueLineNumStationsInLine, StationBlue, TrackBlue, Blue_Num_LEDS);
        PlotLEDTrainPositions(BlueTargetFrame, "BL", BlueLineTrack1Domains, BlueLineTrack1Segments, BlueLineTrack1StationSegments, BlueLineNumStationsInLine, BlueLineLEDArray, BlueLineStationLEDPosition, Blue_Num_LED_Domains, TrainBlue);
        PlotLEDTrainPositions(BlueTargetFrame, "BL", BlueLineTrack2Domains, BlueLineTrack2Segments, BlueLineTrack2StationSegments, BlueLineNumStationsInLine, BlueLineLEDArray, BlueLineStationLEDPosition, Blue_Num_LED_Domains, TrainBlue);
       
        PlotLEDStations(GreenTargetFrame, GreenLineStationLEDPosition, GreenLineNumStationsInLine, StationGreen, TrackGreen, Green_Num_LEDS);
        PlotLEDTrainPositions(GreenTargetFrame, "GR", GreenLineTrack1Domains, GreenLineTrack1Segments, GreenLineTrack1StationSegments, GreenLineNumStationsInLine, GreenLineLEDArray, GreenLineStationLEDPosition, Green_Num_LED_Domains, TrainGreen);
        PlotLEDTrainPositions(GreenTargetFrame, "GR", GreenLineTrack2Domains, GreenLineTrack2Segments, GreenLineTrack2StationSegments, GreenLineNumStationsInLine, GreenLineLEDArray, GreenLineStationLEDPosition, Green_Num_LED_Domains, TrainGreen);
       
        PlotLEDStations(OrangeTargetFrame, OrangeLineStationLEDPosition, OrangeLineNumStationsInLine, StationOrange, TrackOrange, Orange_Num_LEDS);
        PlotLEDTrainPositions(OrangeTargetFrame, "OR", OrangeLineTrack1Domains, OrangeLineTrack1Segments, OrangeLineTrack1StationSegments, OrangeLineNumStationsInLine, OrangeLineLEDArray, OrangeLineStationLEDPosition, Orange_Num_LED_Domains, TrainOrange);
        PlotLEDTrainPositions(OrangeTargetFrame, "OR", OrangeLineTrack2Domains, OrangeLineTrack2Segments, OrangeLineTrack2StationSegments, OrangeLineNumStationsInLine, OrangeLineLEDArray, OrangeLineStationLEDPosition, Orange_Num_LED_Domains, TrainOrange);
       
        PlotLEDStations(YellowTargetFrame, YellowLineStationLEDPosition, YellowLineNumStationsInLine, StationYellow, TrackYellow, Yellow_Num_LEDS);
        PlotLEDTrainPositions(YellowTargetFrame, "YL", YellowLineTrack1Domains, YellowLineTrack1Segments, YellowLineTrack1StationSegments, YellowLineNumStationsInLine, YellowLineLEDArray, YellowLineStationLEDPosition, Yellow_Num_LED_Domains, TrainYellow);
        PlotLEDTrainPositions(YellowTargetFrame, "YL", YellowLineTrack2Domains, YellowLineTrack2Segments, YellowLineTrack2StationSegments, YellowLineNumStationsInLine, YellowLineLEDArray, YellowLineStationLEDPosition, Yellow_Num_LED_Domains, TrainYellow);
       
        // Serial.print("train red = ");
        // Serial.println(TrainRed);
        // Serial.print("station red = ");
        // Serial.println(StationRed);
        // Serial.print("track red = ");
        // Serial.println(TrackRed);
        // for(int i=0; i<Red_Num_LEDS; i++){
        //   Serial.print(i);
        //   Serial.print(" ");
        //   Serial.print(RedLineFrameTarget[i]);
        // }
        lastTime = millis();

      }
      // if (millis() - lastTime > 1000) {
      //   //Serial.println("I'm alive!");
      //   lastTime = millis();
      // }
    }


    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    /*
    void addToJsonInfo(JsonObject& root)
    {
      int reading = 20;
      //this code adds "u":{"Light":[20," lux"]} to the info object
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      JsonArray lightArr = user.createNestedArray("Light"); //name
      lightArr.add(reading); //value
      lightArr.add(" lux"); //unit
    }
    */


    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("ASL-usermod-v2");
      top["testString"] = String ("written from addtojson state");
      top["DevString"] = String ("written from addtojson state");
      Serial.println("added to json state");
      //root["user0"] = userVar0;
    }


    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root)
    {
      userVar0 = root["user0"] | userVar0; //if "user0" key exists in JSON, update, else keep old value
      // JsonObject& state = root["state"];
      // JsonObject& state_seg = state["seg"];
      //JsonObject& state_seg_0_col = state_seg_0["col"];
      //JsonObject& state_seg_0_col_0 = state_seg_0_col[0];
      //JsonObject& state = root["state"];
      uint32_t state_bri = root["transition"];
      Serial.println(state_bri);
      //state_seg_0_col_0_0 = root["state"];//["seg"]["0"]["col"]["0"]; // 213
      //int state_seg_0_col_0_1 = state_seg_0_col_0[1]; // 255
      //int state_seg_0_col_0_2 = state_seg_0_col_0[2]; // 107
      if (root["bri"] == 255) Serial.println(F("Don't burn down your garage!"));
      uint32_t bri = root["bri"];
      Serial.println(bri);
    }


    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     * 
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     * 
     * addToConfig() will make your settings editable through the Usermod Settings page automatically.
     *
     * Usermod Settings Overview:
     * - Numeric values are treated as floats in the browsernh    .
     *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C float
     *     before being returned to the Usermod.  The float data type has only 6-7 decimal digits of precision, and
     *     doubles are not supported, numbers will be rounded to the nearest float value when being parsed.
     *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
     *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
     *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
     *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
     *     used in the Usermod when reading the value from ArduinoJson.
     * - Pin values can be treated differently from an integer value by using the key name "pin"
     *   - "pin" can contain a single or array of integer values
     *   - On the Usermod Settings page there is simple checpoking for pin conflicts and warnings for special pins
     *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
     *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
     *
     * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
     * 
     * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.  
     * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
     * 
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("ASL-usermod-v2");
      top["great"] = userVar0; //save these vars persistently whenever settings are saved
      top["testBool"] = testBool;
      top["testInt"] = testInt;
      top["testLong"] = testLong;
      top["testULong"] = testULong;
      top["testFloat"] = testFloat;
      top["testString"] = String ("written from addconfig");
      top["DevString"] = String ("written from addconfig");
      top["ServerAddressString"] = ServerAddressString;
      top["apiKeyString"] = apiKeyString;
      top["ServerPollIntervalSeconds"] = ServerPollIntervalSeconds;
      top["LEDRefreshIntervalms"] = LEDRefreshIntervalms;
      JsonArray pinArray = top.createNestedArray("pin");
      pinArray.add(testPins[0]);
      pinArray.add(testPins[1]); 
    }


    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     * 
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     * 
     * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
     * 
     * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
     * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
     * 
     * This function is guaranteed to be called on boot, but could also be called every time settings are updated
     */
    bool readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)

      JsonObject top = root["ASL-usermod-v2"];

      bool configComplete = !top.isNull();

      configComplete &= getJsonValue(top["great"], userVar0);
      configComplete &= getJsonValue(top["testBool"], testBool);
      configComplete &= getJsonValue(top["testULong"], testULong);
      configComplete &= getJsonValue(top["testFloat"], testFloat);
      configComplete &= getJsonValue(top["testString"], testString);
      configComplete &= getJsonValue(top["devString"], devString);

      // A 3-argument getJsonValue() assigns the 3rd argument as a default value if the Json value is missing
      configComplete &= getJsonValue(top["testInt"], testInt, 42);  
      configComplete &= getJsonValue(top["testLong"], testLong, -42424242);
      configComplete &= getJsonValue(top["testString"], testString, "set from 3 arg");
      configComplete &= getJsonValue(top["DevString"], devString, "set from 3 arg");
      //configComplete &= getJsonValue(top["pin"][0], testPins[0], 26);
      //configComplete &= getJsonValue(top["pin"][1], testPins[1], 27);
      configComplete &= getJsonValue(top["ServerAddressString"], ServerAddressString);
      configComplete &= getJsonValue(top["apiKeyString"], apiKeyString);

      return configComplete;
    }


    /*
     * handleOverlayDraw() is called just before every show() (LED strip update frame) after effects have set the colors.
     * Use this to blank out some LEDs or set them to a different color regardless of the set effect mode.
     * Commonly used for custom clocks (Cronixie, 7 segment)
     */
    void handleOverlayDraw()
    {
      // #define SEGCOLOR(x)      _colors_t[x]
      // for(int i=0; i < 50; i++){
      //     strip.setPixelColor(i, SEGCOLOR(0)); // set the first pixel to black
      // }
      //strip.setPixelColor(steps, color_from_palette(1, true, false, 0, 255));

    }

   
    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_EXAMPLE;
    }

   //More methods can be added in the future, this example will then be extended.
   //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
   void GetWMATAdata()
   {
     DynamicJsonDocument doc(16384); //JSON doc size, see JSON arduino assistant for better info
     HTTPClient http; //establish the HTTPclient object
     String payload;
     String serverPath = ServerAddressString + "&api_key=" + apiKeyString; //these are defined in globals  
     int16_t httpResponseCode = 0;
     http.begin(serverPath); 
     httpResponseCode = http.GET(); //send the GET Request
    //  if (httpResponseCode > 0) {
    //     Serial.print("HTTP Response code: ");
    //     Serial.println(httpResponseCode);
    //   }
    //payload = SimulatedTrains;
      payload = http.getString(); //write the Wmata response to a String object
      http.end(); //free memory now and clean up 
     //Serial.println(payload);
     //Serial.print("State JSON ");
     //Serial.println(state_bri);
      deserializeJson(doc, payload);
    JsonArray TrainPositions = doc["TrainPositions"]; //make a TrainsPositions nested JSON object
    MaxNumTrains = (TrainPositions.size()); //change train positions to maxnumtrains global.    
    for (int i = 0; i <= (MaxNumTrains); i++) { // loop to park the JSON data into global arrays
        JsonObject workingTrainPosition = TrainPositions[i];
        TrainPositions_TrainId[i] = workingTrainPosition["TrainId"]; // "020"
        TrainPositions_TrainNumber[i] = workingTrainPosition["TrainNumber"]; // "713"
        TrainPositions_CarCount[i] = workingTrainPosition["CarCount"]; // 8
        TrainPositions_DirectionNum[i] = workingTrainPosition["DirectionNum"]; // 2
        TrainPositions_CircuitId[i] = workingTrainPosition["CircuitId"]; // 2012
        const char *DestCodeBuf = workingTrainPosition["DestinationStationCode"];
        String DestCodeBufString((char*)DestCodeBuf);
        TrainPositions_DestinationStationCode[i] = DestCodeBufString;
        const char *LineCodeBuf = workingTrainPosition["LineCode"];
        String LineCodeBufString((char*)LineCodeBuf);
        TrainPositions_LineCode[i] = LineCodeBufString;
        TrainPositions_SecondsAtLocation[i] = workingTrainPosition["SecondsAtLocation"]; // 
        const char* ServiceTypeBuf = workingTrainPosition["ServiceType"];
        String ServiceTypeString((char*)ServiceTypeBuf);
        TrainPositions_ServiceType[i] = ServiceTypeString;
        // Serial.print("in retreival :");
        //Serial.println(TrainPositions_TrainId[i]);
        }
   }

  //function to plot the stations (specifically) within the lines. The stations don't move.
  void PlotLEDStations(uint32_t LEDArrayToPlot[], uint16_t StationArray[], uint16_t NumStations, uint32_t StationColor, uint32_t TrackColor, uint32_t NumLEDs)
  { 
    //strip.fill(BLACK);
    //LEDArrayToPlot->fill(TrackColor); //write the whole allray to very weak red
    for(int i = 0; i < NumLEDs; i++) {
      LEDArrayToPlot[i] = BLACK;
      //strip.setPixelColor(i,BLACK);
    }
    for(int i = 0; i < (NumStations); i++) {
      uint16_t StationArrayIndex = StationArray[i];
      LEDArrayToPlot[StationArrayIndex] = StationColor; //write the resulting stations to the array
      //StationArray[i] = StationColor;
      //Serial.println((String)"plotting stations " + StationArrayIndex);
    }
  }


  //function to plot the trains into the LED array, modularized to accept various line colors and output arrays
  void PlotLEDTrainPositions(uint32_t LEDArrayToPlot[], String LineCode, uint16_t TrackSegmentArray[][2], uint16_t TrackSegmentList[], uint16_t StationSegmentArray[], uint8_t NumStationsInLine, uint16_t LEDTrackArray[][2], uint16_t LEDStationArray[], uint16_t NumLEDSegmentDomains, uint32_t TrainColor)
  {
    int NumNormalTrains = 0;
    for (int i = 0; i < MaxNumTrains; i++) { //run this for the max number of trains
      int8_t StationPlotted = 0;
      if(TrainPositions_ServiceType[i] == "Normal" && TrainPositions_LineCode[i] == LineCode){
          NumNormalTrains ++;
          for(int x = 0; x < NumStationsInLine; x++){ //run this loop for the total number of stations we have, to cycle through the station array
            if(TrainPositions_CircuitId[i] == StationSegmentArray[x]) { 
              LEDArrayToPlot[(LEDStationArray[x])] = TrainColor; //set the LED to high brightness so that we know theres a train there
              Serial.println((String)"Train ID " + TrainPositions_TrainId[i] + " is at Station Segment " + StationSegmentArray[x] + " Direction: " + TrainPositions_DirectionNum[i] + " Mapped to " + LineCode + " LED " + LEDStationArray[x]); 
              StationPlotted = 1;
            }else 
            if(TrainPositions_CircuitId[i] == (StationSegmentArray[x]+1) || TrainPositions_CircuitId[i] == (StationSegmentArray[x]-1)) { //if the train is + or - 1 from the station it snaps it to the station. WMATA has documented that the sensor before the platform sometimes trips
              LEDArrayToPlot[(LEDStationArray[x])] = TrainColor;
              Serial.println((String)"Train ID " + TrainPositions_TrainId[i] + " is at Station Segment " + StationSegmentArray[x] + " Direction: " + TrainPositions_DirectionNum[i] + " Snapped to " + LineCode + " LED " + LEDStationArray[x]);
              StationPlotted = 1;
            }
          }
          if(StationPlotted == 0){ //don't plot the segment if we already plotted it as a station
            for(int y = 0; y < (NumLEDSegmentDomains); y++) { //cycle through each domain and check to see if it's the domain we want (this is -1 since we're directly counting the number of LEDs and we want the number of domains between them)
              if(TrainPositions_CircuitId[i] >= TrackSegmentArray[y][0] && TrainPositions_CircuitId[i] <= TrackSegmentArray[y][1]){  //we should now have found the correct domain range
                int MappedLED = mapRound(TrainPositions_CircuitId[i], TrackSegmentArray[y][0], TrackSegmentArray[y][1], LEDTrackArray[y][0], LEDTrackArray[y][1]);
                LEDArrayToPlot[MappedLED] = TrainColor;
                Serial.println((String)"Train ID " + TrainPositions_TrainId[i] + " at circuit " + TrainPositions_CircuitId[i] + " Direction: " + TrainPositions_DirectionNum[i] + " Mapped to " + LineCode + " LED " + MappedLED);
              }
            }
          }
        } 
      }
  }

  //Fuction to do rounded domain mapping
  int mapRound(int x, int in_min, int in_max, int out_min, int out_max)
  {
    return (x - in_min) * (out_max - out_min+1) / (in_max - in_min+1) + out_min;
  }

  //function to determine train counts, which are the used to iterate in loops
  void TrainStats()
  {
    NormalTrainCount = 0;
    RedTrainCount = 0;
    for (int i = 0; i <= MaxNumTrains; i++) {
      if (TrainPositions_ServiceType[i] == "Normal"){
      //Serial.println((String)"Train " + i + ", ID: " + TrainPositions_TrainId[i] + " Position : " + TrainPositions_CircuitId[i]);
      NormalTrainCount++;
      }
      if (TrainPositions_LineCode[i] == "RD"){
        RedTrainCount ++;
      }
    }
  }


  String TrainSim(String LineCode, uint16_t Track1SegmentCount, uint16_t Track2SegmentCount, uint16_t Track1Segments[], uint16_t Track2Segments[]) { //function thats modular enough to allow line specification for testing. This function does NOT duplicate the representative size of a payload from WMATA
    String DirectionCode = "\"Synthetic\""; //use a null station code. We don't even parse it anyway, but it will allow us to see if data is synthetic
    uint8_t Train1Direction = 2;
    uint8_t Train2Direction = 1;
    uint16_t Track1PositionCounter = 0;
    uint16_t Track2PositionCounter = 0;
    uint8_t TempTrain1IDNumber = 0;
    uint8_t TempTrain2IDNumber = 0;

      if(LineCode == "RD") {
          Track1PositionCounter = RedTrack1PositionCounter;
          Track2PositionCounter = RedTrack2PositionCounter;
          TempTrain1IDNumber = 10;
          TempTrain2IDNumber = 11;
        } else if (LineCode == "BL") {
          Track1PositionCounter = BlueTrack1PositionCounter;
          Track2PositionCounter = BlueTrack2PositionCounter;
          TempTrain1IDNumber = 20;
          TempTrain2IDNumber = 21;
        } else if (LineCode == "GR") {
          Track1PositionCounter = GreenTrack1PositionCounter;
          Track2PositionCounter = GreenTrack2PositionCounter;
          TempTrain1IDNumber = 30;
          TempTrain2IDNumber = 31;
        } else if (LineCode == "OR") {
          Track1PositionCounter = OrangeTrack1PositionCounter;
          Track2PositionCounter = OrangeTrack2PositionCounter;
          TempTrain1IDNumber = 40;
          TempTrain2IDNumber = 41;
        } else if (LineCode == "YL") {
          Track1PositionCounter = YellowTrack1PositionCounter;
          Track2PositionCounter = YellowTrack2PositionCounter;
          TempTrain1IDNumber = 50;
          TempTrain2IDNumber = 51;
      }

      String JsonFormattingPart1 = R"=====({
      "TrainId": ")=====";

      String JsonFormattingPart2 = R"=====(",
         "TrainNumber": "000",
         "CarCount": 0,
         "DirectionNum": )=====";
      
      String JsonFormattingPart3 = R"=====(,
         "CircuitId": )=====";

      String JsonFormattingPart4 = R"=====(,
         "DestinationStationCode": "Synthetic",
         "LineCode":")=====";

      String JsonFormattingPart5 = R"=====(",
         "SecondsAtLocation": 5,
         "ServiceType": "Normal"
         })=====";

      if(UseFakeJsonData == 2) {
        if(Track1PositionCounter <= Track1SegmentCount){
          Track1PositionCounter ++;
        }
        if(Track1PositionCounter >= Track1SegmentCount){
          Track1PositionCounter = 0;
        }

        if(Track2PositionCounter >= 0){
          Track2PositionCounter --;
        }
        if(Track2PositionCounter < 0){
          Track2PositionCounter = Track2SegmentCount;
        }

        if(LineCode == "RD") {
          RedTrack1PositionCounter = Track1PositionCounter;
          RedTrack2PositionCounter = Track2PositionCounter;
        } else if (LineCode == "BL") {
          BlueTrack1PositionCounter = Track1PositionCounter;
          BlueTrack2PositionCounter = Track2PositionCounter;
        } else if (LineCode == "GR") {
          GreenTrack1PositionCounter = Track1PositionCounter;
          GreenTrack2PositionCounter = Track2PositionCounter;
        } else if (LineCode == "OR") {
          OrangeTrack1PositionCounter = Track1PositionCounter;
          OrangeTrack2PositionCounter = Track2PositionCounter;
        } else if (LineCode == "YL") {
          YellowTrack1PositionCounter = Track1PositionCounter;
          YellowTrack2PositionCounter = Track2PositionCounter;
        }
      }

    // Serial.println((String)"TempTrain1PositionCounter = " + TempTrain1PositionCounter + " Array Value = " + RedLineTrack1Segments[TempTrain1PositionCounter]);
    // Serial.println((String)"TempTrain2PositionCounter = " + TempTrain2PositionCounter + " Array Value = " + RedLineTrack2Segments[TempTrain2PositionCounter]);

    String FakeJsonBody = JsonFormattingPart1 + TempTrain1IDNumber + JsonFormattingPart2 + Train1Direction + JsonFormattingPart3 + Track1Segments[Track1PositionCounter] + JsonFormattingPart4 + LineCode + JsonFormattingPart5 +
    ","+
    JsonFormattingPart1 + TempTrain2IDNumber + JsonFormattingPart2 + Train2Direction + JsonFormattingPart3 + Track2Segments[Track2PositionCounter] + JsonFormattingPart4 + LineCode + JsonFormattingPart5;

    //Serial.println(FakeDataJson);
    return FakeJsonBody;
  }

  String UseFakeData() { //function to allow us to create test JSON data rather than having to get it from WMATA. Below is a representative weekend train load
    if(UseFakeJsonData == 2) { //this implements two moving trains worth of data
      String FakeDataJSONpart1 = R"=====({"TrainPositions":[{
        "TrainId": "001",
        "TrainNumber": "001",
        "CarCount": 8,
        "DirectionNum": )=====";
      String FakeDataJSONpart2 = R"=====(,
        "CircuitId": )=====";
      String FakeDataJSONpart3 = R"=====(,
        "DestinationStationCode": )=====";
      String FakeDataJSONpart4 = R"=====(,
        "LineCode": "RD",
        "SecondsAtLocation": 1,
        "ServiceType": "Normal"
      }, {
        "TrainId": "002",
        "TrainNumber": "703",
        "CarCount": 8,
        "DirectionNum": )====="; //2,
      String FakeDataJSONpart5 = R"=====(,
        "CircuitId": )=====";
      String FakeDataJSONpart6 = R"=====(,
        "DestinationStationCode": )=====";
      String FakeDataJSONpart7 = R"=====(,
        "LineCode": "RD",
        "SecondsAtLocation": 1,
        "ServiceType": "Normal"
      }, {
        "TrainId": "024",
        "TrainNumber": "506",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 1773,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 29,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "035",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 2545,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "036",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 2548,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "046",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 2551,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "093",
        "TrainNumber": "000",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 2581,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 275,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "104",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 1057,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "108",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 1072,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "109",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 1063,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "110",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 1264,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "111",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 1259,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "129",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 2601,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "138",
        "TrainNumber": "714",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 344,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 2,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "150",
        "TrainNumber": "701",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 2363,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 47,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "153",
        "TrainNumber": "792",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 1758,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "229",
        "TrainNumber": "845",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 715,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 4,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "233",
        "TrainNumber": "824",
        "CarCount": 6,
        "DirectionNum": 1,
        "CircuitId": 22,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 11,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "247",
        "TrainNumber": "851",
        "CarCount": 6,
        "DirectionNum": 1,
        "CircuitId": 3271,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 2,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "249",
        "TrainNumber": "000",
        "CarCount": 6,
        "DirectionNum": 1,
        "CircuitId": 2933,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 13,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "253",
        "TrainNumber": "000",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 660,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 10,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "255",
        "TrainNumber": "708",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 3191,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 5,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "277",
        "TrainNumber": "706",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 2851,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 733,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "405",
        "TrainNumber": "PM37",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 997,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "415",
        "TrainNumber": "T006",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 993,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "419",
        "TrainNumber": "PM35",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 1181,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "426",
        "TrainNumber": "12220",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 987,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "444",
        "TrainNumber": "000",
        "CarCount": 6,
        "DirectionNum": 1,
        "CircuitId": 2494,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "445",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 984,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "448",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 1187,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "452",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 2634,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "471",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 2536,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "472",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 2542,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "473",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 2543,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "474",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 1280,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "475",
        "TrainNumber": "000",
        "CarCount": 0,
        "DirectionNum": 1,
        "CircuitId": 1087,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 1832,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "499",
        "TrainNumber": "710",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 3030,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 37,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "502",
        "TrainNumber": "706",
        "CarCount": 2,
        "DirectionNum": 1,
        "CircuitId": 513,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 43,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "510",
        "TrainNumber": "000",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 2580,
        "DestinationStationCode": null,
        "LineCode": null,
        "SecondsAtLocation": 457,
        "ServiceType": "Unknown"
      }, {
        "TrainId": "486",
        "TrainNumber": "592",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 2254,
        "DestinationStationCode": "F11",
        "LineCode": "GR",
        "SecondsAtLocation": 922,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "495",
        "TrainNumber": "501",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 2317,
        "DestinationStationCode": "F11",
        "LineCode": "GR",
        "SecondsAtLocation": 13,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "017",
        "TrainNumber": "905",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 1443,
        "DestinationStationCode": "D13",
        "LineCode": "OR",
        "SecondsAtLocation": 11,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "004",
        "TrainNumber": "992",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 3019,
        "DestinationStationCode": "K08",
        "LineCode": "OR",
        "SecondsAtLocation": 2,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "494",
        "TrainNumber": "110",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 6,
        "DestinationStationCode": "A15",
        "LineCode": "RD",
        "SecondsAtLocation": 214,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "506",
        "TrainNumber": "192",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 331,
        "DestinationStationCode": "A15",
        "LineCode": "RD",
        "SecondsAtLocation": 0,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "491",
        "TrainNumber": "191",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 572,
        "DestinationStationCode": "B11",
        "LineCode": "RD",
        "SecondsAtLocation": 36,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "490",
        "TrainNumber": "691",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 2412,
        "DestinationStationCode": "G05",
        "LineCode": "SV",
        "SecondsAtLocation": 10,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "131",
        "TrainNumber": "792",
        "CarCount": 8,
        "DirectionNum": 2,
        "CircuitId": 3421,
        "DestinationStationCode": "N06",
        "LineCode": "SV",
        "SecondsAtLocation": 91,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "273",
        "TrainNumber": "332",
        "CarCount": 0,
        "DirectionNum": 2,
        "CircuitId": 1176,
        "DestinationStationCode": "C15",
        "LineCode": "YL",
        "SecondsAtLocation": 561,
        "ServiceType": "NoPassengers"
      }, {
        "TrainId": "154",
        "TrainNumber": "391",
        "CarCount": 8,
        "DirectionNum": 1,
        "CircuitId": 1895,
        "DestinationStationCode": "E10",
        "LineCode": "YL",
        "SecondsAtLocation": 195,
        "ServiceType": "NoPassengers"
      }]
      })=====";
      
      uint8_t TempTrain1DirectionNum = 2; //southbound from Shady Grove
      uint8_t TempTrain2DirectionNum = 1; //southbound from Glenmont
      String TempTrain1DesitinationCode = "\"B11\""; //Glenmont
      String TempTrain2DesitinationCode = "\"A15\""; //Shady Grove

      String FakeDataJSON = FakeDataJSONpart1 + TempTrain1DirectionNum + FakeDataJSONpart2 + RedLineTrack1Segments[TempTrain1PositionCounter] + FakeDataJSONpart3 + TempTrain1DesitinationCode + FakeDataJSONpart4 + TempTrain2DirectionNum + FakeDataJSONpart5 + RedLineTrack2Segments[TempTrain2PositionCounter] + FakeDataJSONpart6 + TempTrain2DesitinationCode + FakeDataJSONpart7;

      Serial.println((String)"TempTrain1PositionCounter = " + TempTrain1PositionCounter + " Array Value = " + RedLineTrack1Segments[TempTrain1PositionCounter]);
      Serial.println((String)"TempTrain2PositionCounter = " + TempTrain2PositionCounter + " Array Value = " + RedLineTrack2Segments[TempTrain2PositionCounter]);

      if(TempTrain1PositionCounter <= RedLineTrack1SegmentCount){
        TempTrain1PositionCounter ++;
      }
      if(TempTrain1PositionCounter >= RedLineTrack1SegmentCount){
        TempTrain1PositionCounter = 0;
      }

      if(TempTrain2PositionCounter >= 0){
        TempTrain2PositionCounter --;
      }
      if(TempTrain2PositionCounter < 0){
        TempTrain2PositionCounter = 399;
      }

      return FakeDataJSON;
      }
  }

};