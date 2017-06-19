#include "test_main.hpp"

#include "Gps.h"
#include <cstring>
#include <cmath>

// for writing mock file
#include <fstream>  
#include <iostream>  
#include <string>

#define SOFTWARE_IN_THE_LOOP 3
#define HARDWARE_IN_THE_LOOP 4



TEST_CASE( "Test Gps functions, calculations, and getters", "Gps" ) 
{
#ifdef SIL
    int test_type = SOFTWARE_IN_THE_LOOP;
#else
    int test_type = HARDWARE_IN_THE_LOOP;
#endif
    
    // create nmea.txt for mock nmea sentence
    std::ofstream file_writer("nmea.txt"); // out file stream
    file_writer << "$GNRMC,071342.60,A,3650.13719,N,09201.24908,W,0.055,,290517,,,D*74\r\n";
    file_writer << "$GNRMC,071342.60,A,3650.13719,N,09201.24908,W,0.055,,290517,,,D*75\r\n";
    file_writer << "$GNGGA,071342.60,3650.13719,N,09201.24908,W,2,12,1.02,304.2,M,-29.7,M,,0000*72\r\n";
    file_writer << "$GNVTG,,T,,M,0.011,N,0.020,K,D*3A\r\n";
    file_writer << "$GNRMC,071342.60,A,3650.13719,N,09201.24908,W,0.055,,290517,,,D*74\r\n";

    file_writer.close();

    Gps gps;
    const char* sentence_good = "$GNRMC,071342.60,A,3650.13719,N,09201.24908,W,0.055,,290517,,,D*74";
    const char* sentence_bad = "$GNRMC,071342.60,A,3650.13719,N,09201.24908,W,0.055,,290517,,,D*75";
    
    SECTION( "Test validate checksum pass")
    {
        REQUIRE(gps.validate_checksum(sentence_good)==true);
    }
    
    SECTION( "Test validate checksum fail")
    {
        REQUIRE(gps.validate_checksum(sentence_bad)==false);
    }
    
    SECTION( "Test get sentence talker")
    {
        
        gps.get_sentence_talker(sentence_good, gps.sentence_talker_id);
        REQUIRE(strcmp(gps.sentence_talker_id, "$GNRMC")==0);
    }
    
    SECTION( "Test get sentence part")
    { 
        char sentence_part[gps.DATUM_LEN]={0};
        float target_num = 0;
        char target_char[gps.DATUM_LEN]={'W',0};
        gps.get_sentence_part(sentence_good, sentence_part,2);
        target_num = atof(sentence_part);
        REQUIRE(target_num - 071342.60 < .002); // or whatever the system epsilon is
        
        gps.get_sentence_part(sentence_good, sentence_part,7);
        REQUIRE(strcmp(sentence_part, target_char)==0); // or whatever the system epsilon is
    }
    
    SECTION( "Test update data cluster" )
    {
        int res = -1;
        const char* sentence_1 = "$GNGGA,071342.60,3650.13719,N,09201.24908,W,2,12,1.02,304.2,M,-29.7,M,,0000*72";
        const char* sentence_2 = "$GNVTG,,T,,M,0.011,N,0.020,K,D*3A";
        memccpy(gps.GNGGA_sentence, sentence_1, '\0', 100);
        memccpy(gps.GNVTG_sentence, sentence_1, '\0', 100);
        res = gps.update_data_cluster();
        
        REQUIRE(res ==0);
    }
    
    SECTION( "Test Gps Update")
    {
        int time_check;
        int res = 0;
        const char* target_char = "D";
        time_check = gps.data.utc_time;
        res = gps.update();
        
        REQUIRE(time_check != gps.data.utc_time);
        
        switch(test_type)
        {
            case SOFTWARE_IN_THE_LOOP:
            {
                REQUIRE(res == 0);
                REQUIRE(std::abs(gps.data.utc_time - 71342.60) < 0.002);
                REQUIRE(std::abs(gps.data.latitude - 3650.13719) < 0.002);
                REQUIRE(std::abs(gps.data.longitude - 9201.24908) < 0.002);
                REQUIRE(gps.data.fix_quality == 2);
                REQUIRE(gps.data.num_sats == 12);
                REQUIRE(std::abs(gps.data.hdop - 1.02) < 0.002);
                REQUIRE(std::abs(gps.data.altitude - 304.2) < 0.002);
                REQUIRE(strcmp(gps.data.nav_mode, target_char)==0);
                REQUIRE(gps.data.cog_m == 0);
                REQUIRE(std::abs(gps.data.sog_kph - 0.020) < 0.002);
                break;
            }
            case HARDWARE_IN_THE_LOOP:
            {
                REQUIRE(res == 0);
                REQUIRE(gps.data.utc_time > 0);
                REQUIRE(gps.data.latitude > 0);
                REQUIRE(gps.data.longitude > 0);
                REQUIRE(gps.data.fix_quality > 0);
                REQUIRE(gps.data.num_sats > 0);
                REQUIRE(gps.data.hdop > 0);
                REQUIRE(gps.data.altitude > 0);
                REQUIRE(gps.data.nav_mode > 0);
                break;
            }
        }
    }
}