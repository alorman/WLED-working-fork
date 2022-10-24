#pragma once

#include "wled.h"
#include "src/dependencies/json/ArduinoJson-v6.h"
#include <HTTPClient.h>
#include <math.h>

    //Red Line Compile time defiitions 
    #define RedLineNumStationsInLine (sizeof(RedLineTrack1StationSegments)/sizeof(RedLineTrack1StationSegments[1])) //counts number of stations we've defined above
    #define Red_Num_LED_Domains (sizeof(RedLineLEDArray)/sizeof(RedLineLEDArray[0])) //counts the number of LED domains we've specified and makes all the LED number comply with that
    #define Red_Num_LEDS RedLineStationLEDPosition[Red_Num_LED_Domains]//count max number of station LEDs and make this the number of LEDs we have (assumes every line ends with a station)
    #define RedLineTrack1SegmentCount (sizeof(RedLineTrack1Segments)/sizeof(RedLineTrack1Segments[1])) //counts number of red line track 1 track segments
    #define RedLineTrack2SegmentCount (sizeof(RedLineTrack2Segments)/sizeof(RedLineTrack2Segments[1])) //counts number of red line track 2 track segments
    #define RedLineTrack1SegmentDomains (sizeof(RedLineTrack1Domains)/sizeof(RedLineTrack1Domains[1,1])) //counts the number of track 1 segment domains (including breaks and stations)
    #define RedLineTrack2SegmentDomains (sizeof(RedLineTrack2Domains)/sizeof(RedLineTrack2Domains[1,1])) //and the same for track 2
    #define MaxNumPossibleTrains 100
    uint16_t RedLineTrack1Domains [][2] = { {8,31},{33,52},{54,61},{63,79},{81,94},{96,108},{110,125},{127,132},{134,141},{143,153},{155,163},{165,178},{180,189},{191,202},{462,466},{468,476},{478,484},{486,495},{497,512},{514,526},{528,547},{549,570},{572,590},{592,610},{612,628},{630,651} };
    uint16_t RedLineTrack2Domains [][2] ={ {211,231},{233,250},{252,259},{261,277},{279,293},{295,308},{310,325},{327,335},{337,345},{347,355},{357,362},{364,377},{379,388},{390,400},{662,666},{668,676},{678,685},{687,699},{701,716},{718,730},{732,756},{758,784},{786,808},{810,827},{829,845},{847,867} };
    uint16_t RedLineTrack1Segments[] = {7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,462,463,464,465,466,467,468,469,470,471,472,473,474,475,476,477,478,479,480,481,482,483,484,485,486,487,488,489,490,491,492,493,494,495,496,497,498,499,500,501,502,503,504,505,506,507,508,509,510,511,512,513,514,515,516,517,518,519,520,521,522,523,524,525,526,527,528,529,530,531,532,533,534,535,536,537,538,539,540,541,542,543,544,545,546,547,548,549,550,551,552,553,554,555,556,557,558,559,560,561,562,563,564,565,566,567,568,569,570,571,572,573,574,575,576,577,578,579,580,581,582,583,584,585,586,587,588,589,590,591,592,593,594,595,596,597,598,599,600,601,602,603,604,605,606,607,608,609,610,611,612,613,614,615,616,617,618,619,620,621,622,623,624,625,626,627,628,629,630,631,632,633,634,635,636,637,638,639,640,641,642,643,644,645,646,647,648,649,650,651,652};
    uint16_t RedLineTrack2Segments [] = {210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,256,257,258,259,260,261,262,263,264,265,266,267,268,269,270,271,272,273,274,275,276,277,278,279,280,281,282,283,284,285,286,287,288,289,290,291,292,293,294,295,296,297,298,299,300,301,302,303,304,305,306,307,308,309,310,311,312,313,314,315,316,317,318,319,320,321,322,323,324,325,326,327,328,329,330,331,332,333,334,335,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,353,354,355,356,357,358,359,360,361,362,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,397,398,399,400,661,662,663,664,665,666,667,668,669,670,671,672,673,674,675,676,677,678,679,680,681,682,683,684,685,686,687,688,689,690,691,692,693,694,695,696,697,698,699,700,701,702,703,704,705,706,707,708,709,710,711,712,713,714,715,716,717,718,719,720,721,722,723,724,725,726,727,728,729,730,731,732,733,734,735,736,737,738,739,740,741,742,743,744,745,746,747,748,749,750,751,752,753,754,755,756,757,758,759,760,761,762,763,764,765,766,767,768,769,770,771,772,773,774,775,776,777,778,779,780,781,782,783,784,785,786,787,788,789,790,791,792,793,794,795,796,797,798,799,800,801,802,803,804,805,806,807,808,809,810,811,812,813,814,815,816,817,818,819,820,821,822,823,824,825,826,827,828,829,830,831,832,833,834,835,836,837,838,839,840,841,842,843,844,845,846,847,848,849,850,851,852,853,854,855,856,857,858,859,860,861,862,863,864,865,866,867,868};

    uint16_t RedLineTrack1StationSegments[] = {7,32,53,62,80,95,109,126,133,142,154,164,179,190,203,467,477,485,496,513,527,548,571,591,611,629,652};  
    uint16_t RedLineTrack2StationSegments[] = {210,232,251,260,278,294,309,326,336,346,356,363,378,389,661,667,677,686,700,717,731,757,785,809,828,846,868};

    //Red LED Globals
    uint16_t RedLineStationLEDPosition[] = {1,7,13,19,25,31,36,41,44,47,50,52,54,57,66,68,70,73,77,81,86,92,103,110,120,125}; //hard coded position of each station within sequential numbering of LEDS (this can be less than the total number of stations if you want (for some odd reason))
    uint16_t RedLineLEDArray [][2] = { {2,6},{8,12},{14,18},{20,24},{26,30},{32,35},{37,40},{41,43},{45,46},{48,49},{51,51},{53,53},{55,56},{58,65},{67,67},{69,69},{71,72},{74,76},{78,80},{82,85},{87,91},{93,102},{104,109},{111,119},{121,124} };

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
      //Serial.println("Hello from my usermod!");
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
      if (millis() - lastTime > 1000) {
        Serial.println("I'm alive! in main loop");
        //readFromConfig();
        //Serial.println(ServerAddressString);
        //GetWMATAdata();
        Serial.println(steps);
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
        PlotLEDStations(TargetFrame, RedLineStationLEDPosition, RedLineNumStationsInLine, StationRed, TrackRed, Red_Num_LEDS);
        //PlotLEDTrainPositions(TargetFrame, "RD", RedLineTrack1Domains, RedLineTrack1Segments, RedLineTrack1StationSegments, RedLineNumStationsInLine, RedLineLEDArray, RedLineStationLEDPosition,Red_Num_LED_Domains, StationRed, TrainRed);
        Serial.print("train red = ");
        Serial.println(TrainRed);
        Serial.print("station red = ");
        Serial.println(StationRed);
        Serial.print("trac red = ");
        Serial.println(TrackRed);
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
     * - Numeric values are treated as floats in the browser.
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
     *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
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
     if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }
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
        }
   }

  //function to plot the stations (specifically) within the lines. The stations don't move.
  void PlotLEDStations(uint32_t LEDArrayToPlot[], uint16_t StationArray[], uint16_t NumStations, uint32_t StationColor, uint32_t TrackColor, uint32_t NumLEDs)
  { 
    //strip.fill(BLACKa);
    //LEDArrayToPlot->fill(TrackColor); //write the whole allray to very weak red
    for(int i = 0; i < NumLEDs; i++) {
      LEDArrayToPlot[i] = BLACK;
      //strip.setPixelColor(i,BLACK);
    }
    for(int i = 0; i < (NumStations-1); i++) {
      uint16_t StationArrayIndex = StationArray[i];
      LEDArrayToPlot[StationArrayIndex] = StationColor; //write the resulting stations to the array
      //StationArray[i] = StationColor;
      Serial.println(StationArrayIndex);
    }
  }


  //function to plot the trains into the LED array, modularized to accept various line colors and output arrays
  void PlotLEDTrainPositions(uint32_t LEDArrayToPlot[], String LineCode, uint16_t TrackSegmentArray[][2], uint16_t TrackSegmentList[], uint16_t StationSegmentArray[], uint8_t NumStationsInLine, uint16_t LEDTrackArray[][2], uint16_t LEDStationArray[], uint16_t NumLEDSegmentDomains, uint32_t StationColor, uint32_t TrainColor)
  {
    int NumNormalTrains = 0;
    Serial.println("plotting trians");
    for (int i = 0; i < MaxNumTrains; i++) { //run this for the max number of trains
      int8_t StationPlotted = 0;
      if(TrainPositions_ServiceType[i] == "Normal" && TrainPositions_LineCode[i] == LineCode){
          NumNormalTrains ++;
          for(int x = 0; x < NumStationsInLine; x++){ //run this loop for the total number of stations we have, to cycle through the station array
            if(TrainPositions_CircuitId[i] == StationSegmentArray[x]) { 
              TargetFrame[x] = TrainColor; //set the LED to high brightness so that we know theres a train there
              Serial.println((String)"Train ID " + TrainPositions_TrainId[i] + " is at Station Segment " + StationSegmentArray[x] + " Direction: " + TrainPositions_DirectionNum[i] + " Mapped to " + LineCode + " LED " + LEDStationArray[x]); 
              StationPlotted = 1;
            }else if(TrainPositions_CircuitId[i] == (StationSegmentArray[x]+1) || TrainPositions_CircuitId[i] == (StationSegmentArray[x]-1)) { //additional loop for aide in snapping segments to stations
              TargetFrame[x] = TrainColor;
              Serial.println((String)"Train ID " + TrainPositions_TrainId[i] + " is at Station Segment " + StationSegmentArray[x] + " Direction: " + TrainPositions_DirectionNum[i] + " Snapped to " + LineCode + " LED " + LEDStationArray[x]);
              StationPlotted = 1;
            }
          }
          if(StationPlotted == 0){
            for(int y = 0; y < (NumLEDSegmentDomains - 1); y++) { //cycle through each domain and check to see if it's the domain we want (this is -1 since we're directly counting the number of LEDs and we want the number of domains between them)
              if(TrainPositions_CircuitId[i] >= TrackSegmentArray[y][0] && TrainPositions_CircuitId[i] <= TrackSegmentArray[y][1]){  //we should now have found the correct domain range
                int MappedLED = mapRound(TrainPositions_CircuitId[i], TrackSegmentArray[y][0], TrackSegmentArray[y][1], LEDTrackArray[y][0], LEDTrackArray[y][1]);
                TargetFrame[MappedLED] = TrainColor;
                //Serial.println((String)"Train ID " + TrainPositions_TrainId[i] + " at circuit " + TrainPositions_CircuitId[i] + " Direction: " + TrainPositions_DirectionNum[i] + " Mapped to " + LineCode + " LED " + MappedLED);
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
      if (TrainPositions_LineCode[i] == "SV"){
        RedTrainCount ++;
      }
    }
  }

};