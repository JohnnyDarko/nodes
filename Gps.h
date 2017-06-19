// This code is under MIT licence
// you can find the complete file in project-root / LICENSE.txt
#ifndef N_GPS
#define N_GPS

#include <atomic>
#include <time.h>


#define GPS_BAUD_RATE 38400
#define GPS_PORT "/dev/serial0"
//#define GPS_PORT "nmea.txt"

    struct gps_data_cluster
    {
        friend class Gps;
        timespec TOA = {0, 0};        // Time of Aquisition (when cluster was updated)
        int  fix_quality = 0;    //GGA - 0:no fix, 1:valid fix (sps), 2:valid fix differential
        double utc_time = 0;     //GGA - UTC current time from satellite
        double latitude = 0;     //GGA
        double longitude = 0;    //GGA
        double altitude = 0.0;   //GGA
        float hdop = 0;          //GGA - Horizontal dilution of precision
        int   num_sats = 0;      //GGA
        char  nav_mode[2] = {'\0', '\0'};   //GVT (A)utonomous, (D)GPS, 
                                 //    (E)stimated dead recogning
        double cog_m = 0;        //GVT - Course over ground (M)agnetic (degrees)
        float sog_kph = 0;       //GVT - Speed over ground Kilometers per hour
        float sog_mps = 0;       //CALC - speed over ground meters per second
        std::atomic_flag cluster_guard = ATOMIC_FLAG_INIT;
    };

class Gps {

    public:
        Gps();
        int update();
        gps_data_cluster data;

#ifndef UNIT_TEST
    private:
#endif
        int update_data_cluster();
        char GNGGA_sentence[100];
        char GNVTG_sentence[100];
        char GNRMC_sentence[100];
        static const int UPDATE_SENTENCE_FLAGS = 3; // 3 = 0x0000000000000011
        static const int UPDATE_DATA_FLAGS = 4095; // 4095 = 0x0000111111111111 - all flags set
        static const int SENTENCE_MAX_LEN = 100;
        static const int DATUM_LEN = 20;
        static const int TALKER_ID_LEN = 7;
        char nmea_sentence[SENTENCE_MAX_LEN] = {0};
        char sentence_talker_id[TALKER_ID_LEN] = {0};
        bool is_valid_sentence(char * );
        bool validate_checksum(const char *);
        void get_sentence_talker(const char*, char*);
        void get_sentence_part(const char* ,char* ,  int);
};

#endif  // N_GPS


//        struct GNRMC_cluster
//        {                               //(1) is the tag name $GNRMC
//            double UTC = 0.0;           //(2) fix UTC time - hhmmss.sss
//            char status = '\0';         //(3) fix (A)ctive, (V)oid
//            double latitude = 0.0;      //(4) latitude degrees 4807.038 ddmm.mmmm
//            char ns_indicator = '\0';   //(5) lat indicator (N)orth (S)outh
//            double longitude = 0.0;     //(6) longitude degress 4807.038 ddmm.mmmm
//            char ew_indicator = '\0';   //(7) lon indicator (E)ast (W)est
//            double sog_knots = 0.0;     //(8) speed over ground Knot
//            double cog = 0.0;           //(9) course over ground degrees
//            int date = 0;               //(10) date ddmmyy
//            double mag_var = 0.0;       //(11) magnetic variation 
//            char mag_indicator = '\0';  //(12) magnetic variation (E)ast (W)est
//            char mode = '\0';           //(13) mode (N)ot valid (A)utonamous 
//                                        //(14) (D)ifferential (E)stimated dead recogning
//        };
//        struct GNGGA_cluster
//        {                               //(1) is the tag name $GNGGA
//            double UTC = 0.0;           //(2) fix UTC time - hhmmss.sss
//            double latitude = 0.0;      //(3) latitude degrees 4807.038 ddmm.mmmm
//            char ns_indicator = '\0';   //(4) lat indicator (N)orth (S)outh
//            double longitude = 0.0;     //(5) longitude degress 4807.038 ddmm.mmmm
//            char ew_indicator = '\0';   //(6_ lon indicator (E)ast (W)est
//            int fix_indicator = 0;      //(7) 0: position fix unavailable
//                                        //    1: valid position fix, SPS mode
//                                        //    2: valid position fix, differential GPS mode
//            int num_sats = 0;           //(8) Number of satellites in use, (00 ~ 24) 
//            double hdop = 0.0;          //(9) Horizontal dilution of precision, (00.0 ~ 99.9) 
//            double altitude = 0.0;      //(10) Altitude Mean sea level altitude (-9999.9 ~ 17999.9) in meter 
//            char alt_units = '\0';      //(11) Altitude Units M meters
//            double geoid_sep = 0.0;     //(12) Geoid Separation -34.2 meters Geoid-to-ellipsoid separation.
//                                        //(13) Ellipsoid altitude = MSL Altitude + Geoid Separation
//            char geoid_sep_units = '\0';//(14) Separation units (meters)
//            int age_of_diff_corr = 0;   //(15) age of differential correction in seconds
//            int dgps_station_id = 0;    //(16) Differential reference station ID, 0000 ~ 1023
//                                        //(17) NULL when DGPS not used
//        };
//        struct GNVTG_cluster
//        {                               //(1) is the tag name $GNVTG
//            double course_true = 0.0;   //(2) course degrees true heading
//            char ref_true = '\0';       //(3) Reference (M)agnetic
//            double course_mag = 0.0;    //(4) course degrees magnetic heading
//            char ref_mag = '\0';        //(5) Reference (M)agnetic
//            double sog_knots = 0.0;     //(6) speed over ground Kn/h
//            char sog_kn_unit = '\0';    //(7) sog units knots  (N)
//            double sog_kilo = 0.0;      //(8) speed over ground Km/h
//            char sog_km_unit = '\0';    //(9) sog units kilometers (K)
//            char mode = '\0';           //(10) mode (A)utonomous, (D)GPS, 
//                                        //    (E)stimated dead recogning
//        };

//        struct GNGSA_cluster
//        {                                //(1) is the tag name $GNGSA
//            char mode_1 = '\0';          //(2) Mode 1 (M)anual 2d/3d (A)utomatic 2d/3d
//            int mode_2 = 0;              //(3) Mode 2 (1)no fix (3)2d <4sats (3)3d >3sats
//            int gps_sat_id[32] = {0};    //(4) gps satelites
//            int glonass_sat_id[32] = {0};//(5) glonass satelites
//            double pdop = 0.0;           //(6) Position Dilution of Precision
//            double hdop = 0.0;           //(7) Horizontal Dilution of Precision
//            double vdop = 0.0;           //(8) Vertical Dilution of Precision
//            // only 12 satelites will be included per GSA sentence, so consecutive
//            // gsa messages  will be continuance.
//        };