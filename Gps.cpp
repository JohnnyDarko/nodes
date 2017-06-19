#include <iostream>
#include <string.h>
#include <time.h> 
#include "Gps.h"

// drivers
#include "SerialReader.h"

SerialReader serial;

Gps::Gps()
{

}

int Gps::update()
{
    int update_flag = 0;
    //gps_data_cluster tmp_cluster;
    int res = serial.init(GPS_PORT, GPS_BAUD_RATE);

    
    if(res == 0)
    {
        struct timespec  t_last = data.TOA;
        
        // TODO: include timeout maybe
        while(update_flag != UPDATE_SENTENCE_FLAGS) // 3 = 0x0000000000000011 all flags set
        {
            memset( nmea_sentence, '\0', sizeof(char)*SENTENCE_MAX_LEN );
            serial.read_line(nmea_sentence );
            
            if(validate_checksum(nmea_sentence))
            {
                get_sentence_talker(nmea_sentence, sentence_talker_id);
                
                if(strcmp(sentence_talker_id, "$GNGGA")==0)
                {
                    memccpy(GNGGA_sentence, nmea_sentence, '\0', 100);
                    update_flag |= (1 << 0);
                }

                else if(strcmp(sentence_talker_id, "$GNVTG")==0)
                {
                    memccpy(GNVTG_sentence, nmea_sentence, '\0', 100);
                    update_flag |= (1 << 1);
                }
            }
        }
        
        if(update_flag == UPDATE_SENTENCE_FLAGS)
        {
            update_data_cluster();
            res = 0;
        }
        
        if(t_last.tv_nsec == data.TOA.tv_nsec && t_last.tv_sec == data.TOA.tv_sec)
        {
            res = -1;
        }
    }

    return res;
}

int Gps::update_data_cluster()
{
    int res = -1;
    int update_flag = 0;
    struct timespec  t_now  = {0,0};
    char sentence_part[DATUM_LEN]={0};
    while (data.cluster_guard.test_and_set()) {} ; //  sit and spin
    // GNGGA
    get_sentence_part(GNGGA_sentence,sentence_part, 2);
    if(sentence_part != NULL)
    {
        data.utc_time = atof(sentence_part);
        update_flag |= (1 << 0);
    }
    get_sentence_part(GNGGA_sentence,sentence_part, 3);
    if(sentence_part != NULL)
    {
        data.latitude = atof(sentence_part);
        update_flag |= (1 << 1);
    }
    get_sentence_part(GNGGA_sentence,sentence_part, 5);
    if(sentence_part != NULL)
    {
        data.longitude = atof(sentence_part);
        update_flag |= (1 << 2);
    }
    get_sentence_part(GNGGA_sentence,sentence_part, 7);
    if(sentence_part != NULL)
    {
        data.fix_quality = atoi(sentence_part);
        update_flag |= (1 << 3);
    }
    get_sentence_part(GNGGA_sentence,sentence_part, 8);
    if(sentence_part != NULL)
    {
        data.num_sats = atoi(sentence_part);
        update_flag |= (1 << 4);
    }
    get_sentence_part(GNGGA_sentence,sentence_part, 9);
    if(sentence_part != NULL)
    {
        data.hdop = atof(sentence_part);
        update_flag |= (1 << 5);
    }
    get_sentence_part(GNGGA_sentence,sentence_part, 10);
    if(sentence_part != NULL)
    {
        data.altitude = atof(sentence_part);
        update_flag |= (1 << 6);
    }
    //        // GNVTG
    get_sentence_part(GNVTG_sentence,sentence_part, 10);
    if(sentence_part != NULL)
    {
        data.nav_mode[0] = sentence_part[0];
        data.nav_mode[1] = '\0';
        update_flag |= (1 << 7);
    }
    get_sentence_part(GNVTG_sentence,sentence_part, 4);
    if(sentence_part != NULL)
    {
        data.cog_m = atof(sentence_part);
        update_flag |= (1 << 8);
    }
    get_sentence_part(GNVTG_sentence,sentence_part, 8);
    if(sentence_part != NULL)
    {
        data.sog_kph = atof(sentence_part);
        data.sog_mps = (data.sog_kph*1000)/3600;
        update_flag |= (1 << 9);
    }

    // final
    clock_gettime(CLOCK_MONOTONIC, &t_now);
    update_flag |= (1 << 10);

    data.TOA = t_now;
    update_flag |= (1 << 11);
        
    data.cluster_guard.clear();
    if(update_flag == UPDATE_DATA_FLAGS) // 4095 = 0x0000111111111111 - all flags set
    {
        res = 0;
    }

    return res;
}

bool Gps::validate_checksum(const char* sentence)
{
    bool ret = false;
    int i = 0;
    int calc_sum = 0;
    char sentence_checksum[2] = {'\0'};
    int sentence_sum = 0;
    int c = 0;
    for (calc_sum = 0, i = 0; i < SENTENCE_MAX_LEN; i++)
    {
        if(sentence[i])
        {
            c = static_cast<uint8_t>(sentence[i]);
            if (c == '*') 
            {
                for(int j=1;j<3; j++)
                {
                   sentence_checksum[j-1] = sentence[i+j];
                }
                break;
            }
            if (c != '$')
            {
                calc_sum ^= c;
            }
        }

    }
    sentence_checksum[2] = '\0';
    sentence_sum = strtol(sentence_checksum, NULL, 16);
    if(calc_sum == sentence_sum)
    {
        ret = true;
    } 
    return ret;
}

void Gps::get_sentence_talker(const char* sentence, char* talker_id)
{
    memset( talker_id, '\0', sizeof(char)*TALKER_ID_LEN );
    memccpy(talker_id, sentence, '\0', TALKER_ID_LEN-1);
    talker_id[TALKER_ID_LEN] = '\0';
}

void Gps::get_sentence_part(const char* sentence,char* sentence_part,  int part_num)
{
    memset(sentence_part,'\0', sizeof(char)*DATUM_LEN);
    int comma_count = 0;
    int part_count = 0;
    
    for(int i = 0; i< SENTENCE_MAX_LEN; i++)
    {
        if(sentence[i]==',')
        {
            comma_count++;
        }
        else
        {
            if(sentence[i]=='*')
            {
                sentence_part[part_count] = '\0';
                break;
            }
            if(comma_count==(part_num-1))
            {
                sentence_part[part_count] = sentence[i];
                part_count++;
            }
            else if(comma_count==part_num)
            {
                sentence_part[part_count] = '\0';
                break;
            }
        }

    }
}
