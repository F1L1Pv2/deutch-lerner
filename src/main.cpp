#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"


#include <string>
#include <iostream>
#include <random>
#include <time.h>

#include <Windows.h>
#include <cstdio>

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

using namespace std;

#define WIN_SOUND "audio/won.mp3"
#define PASS_SOUND "audio/pass.mp3"

// trim from start
inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

int levenshteinDist(string word1, string word2) {
    int size1 = word1.size();
    int size2 = word2.size();
    int verif[size1 + 1][size2 + 1]; // Verification matrix i.e. 2D array which will store the calculated distance.

    // If one of the words has zero length, the distance is equal to the size of the other word.
    if (size1 == 0)
        return size2;
    if (size2 == 0)
        return size1;

    // Sets the first row and the first column of the verification matrix with the numerical order from 0 to the length of each word.
    for (int i = 0; i <= size1; i++)
        verif[i][0] = i;
    for (int j = 0; j <= size2; j++)
        verif[0][j] = j;

    // Verification step / matrix filling.
    for (int i = 1; i <= size1; i++) {
        for (int j = 1; j <= size2; j++) {
            // Sets the modification cost.
            // 0 means no modification (i.e. equal letters) and 1 means that a modification is needed (i.e. unequal letters).
            int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;

            // Sets the current position of the matrix as the minimum value between a (deletion), b (insertion) and c (substitution).
            // a = the upper adjacent value plus 1: verif[i - 1][j] + 1
            // b = the left adjacent value plus 1: verif[i][j - 1] + 1
            // c = the upper left adjacent value plus the modification cost: verif[i - 1][j - 1] + cost
            verif[i][j] = min(
                min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                verif[i - 1][j - 1] + cost
            );
        }
    }

    // The last position of the matrix will contain the Levenshtein distance.
    return verif[size1][size2];
}


struct deutche{
    string firstForm;
    string secondForm;
    string polish;
    bool did = false;
};

deutche nauka[] = {
 {"finden "," fand"," znajdować"},
 {"kommen "," kam"," przychodzić"},
 {"sitzen "," sass"," siedzieć"},
 {"sprechen "," sprach"," mówić"},
 {"treffen "," traf"," spotykać"},
 {"einladen "," lud...ein"," zapraszać"},
 {"fahren "," fuhr "," jechać"},
 {"fliegen "," flog "," latać"},
 {"verlieren "," verlor"," zgubić"},
 {"ziehen "," zog "," przeprowadzac się"},
 {"anhalten "," hielt...an "," zatrzymać"},
 {"anrufen "," rief...an "," dzwonić"},
 {"aussteigen "," stieg...aus "," wysiadać"},
 {"schreiben "," schreib "," pisać"},
 {"gehen "," ging "," iść"},
 {"bringen "," brachte "," przynosić"},
 {"denken "," dachte "," myśleć"},
 {"werden "," wurde "," zostać"},
 {"wissen "," wusste "," wiedzieć"},
 {"verbringen "," verbrachte "," spędzać "},
 {"sich unterhalten ","sich unterhielt "," rozmawiać"},
 {"zukommen "," kam...zu "," podchodzić"},
 {"sehen "," sah "," widzieć"},
 {"essen "," ass "," jeść"},
 {"laufen "," lief "," biec"},
 {"lesen "," las "," czytać"},
 {"nehmen "," nahm "," brać"},
 {"schlafen "," schlief "," spać"},
 {"tragen "," trug "," nosić"},
 {"trinken "," trank "," pić"},
 {"gewinnen "," gewann "," wygrać "},
 {"sein "," war "," być"},
};


void init(){
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 1000);
    //setvbuf(stdin, nullptr, _IOFBF, 1000);

    srand(time(0));
}

bool check_all_did(){
    
    for(int i = 0; i < sizeof(nauka)/sizeof(deutche); i++){
        if(!nauka[i].did)
            return false;
    }

    return true;
}

ma_sound* sound;
ma_engine engine;

int get_index(){

    int index = rand()%(sizeof(nauka)/sizeof(deutche));


    if (check_all_did()){
        cout << "dziękuje za nauke używając mojego programu" << endl;

        ma_engine_play_sound(&engine,WIN_SOUND, NULL);

        Sleep(5500);

        ma_sound_uninit(&sound[0]);
        ma_sound_uninit(&sound[1]);
        ma_engine_uninit(&engine);
        exit(0);
    }

    while(nauka[index].did){
        index = rand()%(sizeof(nauka)/sizeof(deutche));
    }


    return index;
}


bool check_win(string a_f, string b_f){
    string a = trim(a_f);
    string b = trim(b_f);
    int result = levenshteinDist(a, b);

    if(result == 0){
        cout << "Poprawnie" << endl;
        return true;
    }

    if(result < 4){
        cout << "Prawie dobrze tutaj poprawna forma \"" << a << "\"" << endl;
       return false;
    }

    cout << "Nie udało się tutaj poprawna forma \"" << a << "\"" << endl << "\"";// << b << "\"" << endl << "levenshtein: " << result << endl;
    return false;
}


string get_input(string msg){
    cout << msg << endl << "> ";
    string out;
    //cin >> out;
    getline(cin, out);
    
    return out;
}

bool give_first_form(int index){
    return check_win(nauka[index].firstForm, get_input("Napisz pierwszą formę"));
}

bool give_second_form(int index){
    return check_win(nauka[index].secondForm, get_input("Napisz drugą formę"));
}


#define BIT(x) 1 << (x)
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))




int main(){

    ma_result result;
    sound = (ma_sound*)calloc(2,sizeof(ma_sound));
    result = ma_engine_init(NULL,&engine);
    if(result != MA_SUCCESS){
        cout << "failed to initialize sound engine" << endl;
        exit(1);
    }

    result = ma_sound_init_from_file(&engine, PASS_SOUND, 0, NULL, NULL, &sound[0]);
    if(result != MA_SUCCESS){
        cout << "failed to load sound " << PASS_SOUND << endl;
        exit(1);
    }
    result = ma_sound_init_from_file(&engine,WIN_SOUND, 0, NULL, NULL, &sound[1]);
    if(result != MA_SUCCESS){
        cout << "failed to load sound " << WIN_SOUND << endl;
        exit(1);
    }

    

    init();

    system("cls");

    while(true){
        int rand_index = get_index();
        
        cout << trim(nauka[rand_index].polish) << endl;

        bool firstForm = give_first_form(rand_index);
        if(!firstForm){
            Sleep(2000);
            goto next_thing;
        }
        bool secondForm = give_second_form(rand_index);

        nauka[rand_index].did = firstForm && secondForm;
        if(nauka[rand_index].did)
            ma_engine_play_sound(&engine, PASS_SOUND, NULL);
        Sleep(1000);
next_thing:
        system("cls");
        if(nauka[rand_index].did){
            cout << "Brawo udalo ci sie to nauczyc " << nauka[rand_index].polish << endl;
            cout << "---------------------------------------------------" << endl;
            
        }
    }

}