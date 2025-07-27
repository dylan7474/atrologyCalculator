/**
 * @file main.c
 * @brief A console application that generates a personalized astrological forecast
 * and biorhythm chart based on a user's birth date and real-time NASA data.
 *
 * This program asks for the user's birth date, calculates their true Sun sign,
 * interprets the transiting houses and major aspects, and calculates their
 * current biorhythm cycles.
 *
 * Compilation:
 * gcc main.c -o nasa_forecast -lcurl -ljansson -lm
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include <math.h>
#include <time.h>

// --- Constants ---
#define AU_TO_KM 149597870.7
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define ORB_CONJ_OPP 8.0 // Orb of 8 degrees for Conjunction and Opposition
#define ORB_TRINE_SQR_SEX 6.0 // Orb of 6 degrees for Trine, Square, and Sextile

// --- ANSI Color Codes for Highlighting ---
#define COLOR_GREEN   "\x1b[32m" // For positive states
#define COLOR_RED     "\x1b[31m" // For negative states
#define COLOR_RESET   "\x1b[0m"

// Struct to hold the response data from a curl request.
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Struct to hold planetary data and astrological keywords.
struct Planet {
    const char *name;
    const char *id;
    double longitude;
    const char *keyword; // e.g., "energy", "love", "communication"
};

// --- Astrological Keywords ---
const char* planet_keywords[] = {
    "your identity and ego", "your emotions and security", "communication and thinking",
    "love and money", "energy and drive", "luck and expansion",
    "discipline and responsibility", "change and surprise", "dreams and intuition",
    "power and transformation"
};

const char* house_keywords[] = {
    "Self, Identity, and Appearance", "Money and Possessions", "Communication and Local Travel",
    "Home and Family", "Creativity and Romance", "Health and Daily Work",
    "Partnerships and Marriage", "Shared Resources and Transformation", "Philosophy and Long-Distance Travel",
    "Career and Public Reputation", "Friendships and Social Groups", "Spirituality and the Subconscious"
};

// Callback function for libcurl
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) return 0;
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

// Parses planetary data from NASA API response
int parse_planet_data(const char *json_text, double *longitude) {
    json_error_t error;
    json_t *root = json_loads(json_text, 0, &error);
    if (!root) return -1;
    json_t *result = json_object_get(root, "result");
    if (!json_is_string(result)) { json_decref(root); return -1; }
    const char *result_text = json_string_value(result);
    const char *data_start = strstr(result_text, "$$SOE");
    if (!data_start) { json_decref(root); return -1; }
    const char *x_ptr = strstr(data_start, "X =");
    const char *y_ptr = strstr(data_start, "Y =");
    if (!(x_ptr && y_ptr)) { json_decref(root); return -1; }
    double x_km, y_km;
    sscanf(x_ptr, "X =%lf", &x_km);
    sscanf(y_ptr, "Y =%lf", &y_km);
    
    double longitude_rad = atan2(y_km, x_km);
    *longitude = longitude_rad * (180.0 / M_PI);
    if (*longitude < 0) *longitude += 360;

    json_decref(root);
    return 0;
}

// Determines the zodiac sign index (0-11) from a longitude
int get_zodiac_index(double longitude_degrees) {
    return (int)floor(longitude_degrees / 30.0);
}

// Prints a single bar for the biorhythm chart
void print_biorhythm_bar(double value) {
    int bar_width = 20;
    int center = bar_width;
    int scaled_value = (int)(value / 100.0 * bar_width);

    printf("[");
    if (scaled_value >= 0) {
        for(int i=0; i<center; ++i) printf(" ");
        printf("|");
        for(int i=0; i<scaled_value; ++i) printf("+");
        for(int i=0; i<bar_width - scaled_value; ++i) printf(" ");
    } else {
        for(int i=0; i<center + scaled_value; ++i) printf(" ");
        for(int i=0; i<-scaled_value; ++i) printf("-");
        printf("|");
        for(int i=0; i<bar_width; ++i) printf(" ");
    }
    printf("]");
}

// Generates and prints the detailed forecast
void generate_forecast(struct Planet planets[], int num_planets, int sun_sign_idx) {
    const char* sun_sign_names[] = {"Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo", "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"};
    printf("\n--- Horoscope Forecast for %s ---\n", sun_sign_names[sun_sign_idx]);
    
    // --- House Transits Section ---
    printf("\n--- Planetary Transits by House ---\n");
    for (int i = 0; i < num_planets; i++) {
        int planet_sign_idx = get_zodiac_index(planets[i].longitude);
        // Calculate house number using Whole Sign House system
        int house_num = (planet_sign_idx - sun_sign_idx + 12) % 12 + 1;
        printf("- %s is transiting your %d%s House of %s, affecting %s.\n",
               planets[i].name,
               house_num,
               (house_num==1)?"st":(house_num==2)?"nd":(house_num==3)?"rd":"th",
               house_keywords[house_num - 1],
               planets[i].keyword);
    }

    // --- Major Aspects Section ---
    printf("\n--- Major Aspects to your Sun ---\n");
    double sun_sign_longitude = (sun_sign_idx * 30.0) + 15.0;
    int aspects_found = 0;
    for (int i = 0; i < num_planets; i++) {
        double angle_diff = fabs(planets[i].longitude - sun_sign_longitude);
        if (angle_diff > 180) angle_diff = 360 - angle_diff;

        const char* aspect_text = NULL;
        
        if (angle_diff <= ORB_CONJ_OPP) {
            aspect_text = "is in conjunction with your Sun, amplifying";
        } else if (fabs(angle_diff - 180) <= ORB_CONJ_OPP) {
            aspect_text = "opposes your Sun, creating tension with";
        } else if (fabs(angle_diff - 120) <= ORB_TRINE_SQR_SEX) {
            aspect_text = "forms a harmonious trine with your Sun, supporting";
        } else if (fabs(angle_diff - 90) <= ORB_TRINE_SQR_SEX) {
            aspect_text = "forms a challenging square with your Sun, creating friction with";
        } else if (fabs(angle_diff - 60) <= ORB_TRINE_SQR_SEX) {
            aspect_text = "forms a gentle sextile with your Sun, offering opportunities for";
        }

        if (aspect_text) {
            printf("- %s %s %s.\n", planets[i].name, aspect_text, planets[i].keyword);
            aspects_found = 1;
        }
    }

    if (!aspects_found) {
        printf("A quiet day. No major aspects are affecting your Sun sign today.\n");
    }
    printf("-------------------------------------\n");
}

// Generates a single, combined summary report
void generate_final_report(struct Planet planets[], int num_planets, int sun_sign_idx, int year, int month, int day) {
    int positive_aspects = 0;
    int negative_aspects = 0;
    const char* focus_house = NULL;

    double sun_sign_longitude = (sun_sign_idx * 30.0) + 15.0;

    for (int i = 0; i < num_planets; i++) {
        double angle_diff = fabs(planets[i].longitude - sun_sign_longitude);
        if (angle_diff > 180) angle_diff = 360 - angle_diff;

        if (fabs(angle_diff - 120) <= ORB_TRINE_SQR_SEX || fabs(angle_diff - 60) <= ORB_TRINE_SQR_SEX) {
            positive_aspects++;
        }
        if (fabs(angle_diff - 180) <= ORB_CONJ_OPP || fabs(angle_diff - 90) <= ORB_TRINE_SQR_SEX) {
            negative_aspects++;
        }
        if (strcmp(planets[i].name, "Sun") == 0) {
            int planet_sign_idx = get_zodiac_index(planets[i].longitude);
            int house_num = (planet_sign_idx - sun_sign_idx + 12) % 12;
            focus_house = house_keywords[house_num];
        }
    }

    // --- Biorhythm Calculation ---
    struct tm birth_tm = {0};
    birth_tm.tm_year = year - 1900;
    birth_tm.tm_mon = month - 1;
    birth_tm.tm_mday = day;
    time_t birth_t = mktime(&birth_tm);
    time_t now_t = time(NULL);
    double days_alive = difftime(now_t, birth_t) / (60 * 60 * 24);
    double physical = sin(2 * M_PI * days_alive / 23) * 100;
    double emotional = sin(2 * M_PI * days_alive / 28) * 100;
    double intellectual = sin(2 * M_PI * days_alive / 33) * 100;

    // --- Final Report Generation ---
    printf("\n--- Your Personal Forecast ---\n");
    
    // Biorhythm Chart
    printf("\nBiorhythms:\n");
    printf("Physical:     %4.0f%% ", physical);
    print_biorhythm_bar(physical);
    printf("\n");
    printf("Emotional:    %4.0f%% ", emotional);
    print_biorhythm_bar(emotional);
    printf("\n");
    printf("Intellectual: %4.0f%% ", intellectual);
    print_biorhythm_bar(intellectual);
    printf("\n\n");

    // Astrological Summary
    printf("Summary: ");
    if (positive_aspects > negative_aspects) {
        printf("Astrologically, today looks to be a positive day, with opportunities for growth and harmony. ");
    } else if (negative_aspects > positive_aspects) {
        printf("Astrologically, you may face some challenges today, requiring patience and careful thought. ");
    } else {
        printf("Astrologically, today brings a mix of opportunities and challenges, requiring balance. ");
    }
    if(focus_house) {
        printf("The main focus is on the area of %s. ", focus_house);
    }
    
    // Biorhythm Summary
    printf("\nFrom a biorhythm perspective: ");
    if (physical > 50) {
        printf(COLOR_GREEN "Physically, you should be feeling strong and energetic. " COLOR_RESET);
    } else if (physical < -50) {
        printf(COLOR_RED "Physically, you may feel low on energy. " COLOR_RESET);
    } else {
        printf("Physically, it's a relatively normal day. ");
    }

    if (emotional > 50) {
        printf(COLOR_GREEN "Emotionally, you're likely feeling positive and creative. " COLOR_RESET);
    } else if (emotional < -50) {
        printf(COLOR_RED "Emotionally, you may be feeling sensitive or withdrawn. " COLOR_RESET);
    } else {
        printf("Emotionally, things are on an even keel. ");
    }

    if (intellectual > 50) {
        printf(COLOR_GREEN "Intellectually, your mind is sharp and clear. " COLOR_RESET);
    } else if (intellectual < -50) {
        printf(COLOR_RED "Intellectually, it might be a good day for rest rather than complex tasks. " COLOR_RESET);
    } else {
         printf("Intellectually, your focus is stable. ");
    }
    
    printf("\n----------------------------\n");
}

int main(void) {
    struct Planet planets[] = {
        {"Sun", "10"}, {"Moon", "301"}, {"Mercury", "199"}, {"Venus", "299"},
        {"Mars", "499"}, {"Jupiter", "599"}, {"Saturn", "699"}, {"Uranus", "799"},
        {"Neptune", "899"}, {"Pluto", "999"}
    };
    int num_planets = sizeof(planets) / sizeof(planets[0]);
    for(int i=0; i<num_planets; ++i) planets[i].keyword = planet_keywords[i];

    // --- Get User Input for Birth Date ---
    int year, month, day;
    printf("Please enter your birth date.\n");
    printf("Year (e.g., 1990): ");
    scanf("%d", &year);
    printf("Month (1-12): ");
    scanf("%d", &month);
    printf("Day (1-31): ");
    scanf("%d", &day);

    if (year < 1900 || year > 2024 || month < 1 || month > 12 || day < 1 || day > 31) {
        printf("Invalid date. Exiting.\n");
        return 1;
    }

    // --- Calculate User's Sun Sign ---
    double earth_longitude_at_birth = 0.0;
    printf("\nCalculating your true Sun sign from NASA data...\n");

    char birth_date_str[11];
    snprintf(birth_date_str, sizeof(birth_date_str), "%04d-%02d-%02d", year, month, day);

    struct tm birth_tm = {0};
    birth_tm.tm_year = year - 1900;
    birth_tm.tm_mon = month - 1;
    birth_tm.tm_mday = day;
    time_t birth_t = mktime(&birth_tm);
    birth_t += 24 * 60 * 60;
    struct tm *next_day_tm = localtime(&birth_t);
    char next_day_str[11];
    strftime(next_day_str, sizeof(next_day_str), "%Y-%m-%d", next_day_tm);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();
    if (curl_handle) {
        struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };
        char url[512];
        snprintf(url, sizeof(url),
                 "https://ssd.jpl.nasa.gov/api/horizons.api?format=json&COMMAND='399'&OBJ_DATA='NO'&MAKE_EPHEM='YES'&EPHEM_TYPE='VECTORS'&CENTER='@sun'&START_TIME='%s'&STOP_TIME='%s'&STEP_SIZE='1d'&VEC_TABLE='1'",
                 birth_date_str, next_day_str);

        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl_handle, CURLOPT_URL, url);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        if (curl_easy_perform(curl_handle) == CURLE_OK) {
            if (parse_planet_data(chunk.memory, &earth_longitude_at_birth) != 0) {
                 printf("Error: Could not calculate Sun Sign. The NASA API might be temporarily unavailable or the date is invalid.\n");
                 curl_easy_cleanup(curl_handle);
                 free(chunk.memory);
                 curl_global_cleanup();
                 return 1;
            }
        } else {
            printf("Error: API call failed during Sun Sign calculation.\n");
            curl_easy_cleanup(curl_handle);
            free(chunk.memory);
            curl_global_cleanup();
            return 1;
        }
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);
    }
    
    double sun_longitude_at_birth = earth_longitude_at_birth + 180;
    if (sun_longitude_at_birth >= 360) sun_longitude_at_birth -= 360;
    int sun_sign_idx = get_zodiac_index(sun_longitude_at_birth);
    const char* sun_sign_names[] = {"Aries", "Taurus", "Gemini", "Cancer", "Leo", "Virgo", "Libra", "Scorpio", "Sagittarius", "Capricorn", "Aquarius", "Pisces"};
    printf("Your true Sun sign is %s.\n", sun_sign_names[sun_sign_idx]);


    // --- Fetch Current Planetary Data for Forecast ---
    printf("\nFetching today's planetary data from NASA...\n");

    time_t t_today = time(NULL);
    struct tm *tm_info = localtime(&t_today);
    char today_str[20], tomorrow_str[20];
    strftime(today_str, sizeof(today_str), "%Y-%m-%d", tm_info);
    t_today += 24 * 60 * 60;
    tm_info = localtime(&t_today);
    strftime(tomorrow_str, sizeof(tomorrow_str), "%Y-%m-%d", tm_info);

    for (int i = 0; i < num_planets; i++) {
        curl_handle = curl_easy_init();
        if (curl_handle) {
            struct MemoryStruct chunk = { .memory = malloc(1), .size = 0 };
            char url[512];
            snprintf(url, sizeof(url),
                     "https://ssd.jpl.nasa.gov/api/horizons.api?format=json&COMMAND='%s'&OBJ_DATA='NO'&MAKE_EPHEM='YES'&EPHEM_TYPE='VECTORS'&CENTER='@399'&START_TIME='%s'&STOP_TIME='%s'&STEP_SIZE='1d'&VEC_TABLE='1'",
                     planets[i].id, today_str, tomorrow_str);

            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(curl_handle, CURLOPT_URL, url);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
            if (curl_easy_perform(curl_handle) == CURLE_OK) {
                parse_planet_data(chunk.memory, &planets[i].longitude);
            }
            curl_easy_cleanup(curl_handle);
            free(chunk.memory);
        }
    }
    
    // --- Generate and Display Forecast and Biorhythms ---
    generate_forecast(planets, num_planets, sun_sign_idx);
    generate_final_report(planets, num_planets, sun_sign_idx, year, month, day);

    curl_global_cleanup();
    return 0;
}
