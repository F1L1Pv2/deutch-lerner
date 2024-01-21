#define _CRT_SECURE_NO_WARNINGS

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
#include <vector>

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

#define WIN_SOUND "audio/won.mp3"
#define PASS_SOUND "audio/pass.mp3"

// trim from start
// inline std::string &ltrim(std::string &s) {
//     s.erase(s.begin(), std::find_if(s.begin(), s.end(),
//             std::not1(std::ptr_fun<int, int>(std::isspace))));
//     return s;
// }

// // trim from end
// inline std::string &rtrim(std::string &s) {
//     s.erase(std::find_if(s.rbegin(), s.rend(),
//             std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
//     return s;
// }

// // trim from both ends
// inline std::string &trim(std::string &s) {
//     return ltrim(rtrim(s));
// }

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
    vector<string> forms;
    bool did = false;
};

vector<deutche> nauka;
vector<string> forms;


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
    
    for(int i = 0; i < nauka.size(); i++){
        if(!nauka[i].did)
            return false;
    }

    return true;
}

ma_sound* sound;
ma_engine engine;

int get_index(){

    int index = rand()%nauka.size();


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
        index = rand()%nauka.size();
    }


    return index;
}


bool check_win(string a_f, string b_f){
    string a = a_f;
    string b = b_f;
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
    cin >> out;
    return out;
}

string get_form(string form_name){
    string msg = "Napisz ";
    msg += form_name;
    return get_input(msg);
}

bool give_form(int index, int form_index){
    return check_win(nauka.at(index).forms.at(form_index), get_form(forms.at(form_index)));
}


#define BIT(x) 1 << (x)
#define KB(x) ((unsigned long long)1024 * x)
#define MB(x) ((unsigned long long)1024 * KB(x))
#define GB(x) ((unsigned long long)1024 * MB(x))


// for string delimiter
std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void parse_file(string filename, vector<deutche>& parsed_file){
    
    ifstream file(filename);

    string line;

    size_t pos = 0;
    string delimiter = ",";
    string token;
    size_t max_forms = -1;
    size_t line_index = 1;
    while(getline(file,line)){
        if(line.length()>0){
            vector<string> tokens = split(line, delimiter);
            if(max_forms == -1){
                max_forms = tokens.size();
            }
            if(tokens.size() > max_forms){
                cerr << "Line " << line_index<<": has more forms than maximum" << max_forms << "specified at the first line"<<endl;
                exit(1);
            }

            deutche learn;

            learn.forms = tokens;
            parsed_file.push_back(learn);

            line_index += 1;
        }
    }

}

namespace fs = std::filesystem;

int main(){

    init();

    std::string path = "presets";
    size_t i = 0;
    cout << "Wybierz preset: "<<endl;
    vector<string> Paths;
    for (const auto & entry : fs::directory_iterator(path)){
        cout << i << ": "<< entry.path().filename().generic_string() << std::endl;
        
        Paths.push_back(entry.path().generic_string());

        i++;
    }

    cout << "> ";

    string choice;
    cin >> choice;

    int choice_int;

    try{
        choice_int = stoi(choice); 
    }catch(exception &err){
        cerr<<"Podano nieprawidłową wartość" << endl;
        exit(1);
    }

    if(choice_int > Paths.size() || choice_int<0){
        cerr<<"nie isntieje taki plik"<<endl;
        exit(1);
    }

    parse_file(Paths.at(choice_int), nauka);

    forms = nauka[0].forms;

    nauka.erase(nauka.begin());


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

    


    system("cls");

    while(true){
        int rand_index = get_index();

        

        cout << nauka.at(rand_index).forms.at(0) << endl;

       nauka[rand_index].did = true;

       for(size_t i = 1; i<nauka.at(rand_index).forms.size(); i++){
            nauka[rand_index].did = give_form(rand_index,i);
        
            if(!nauka.at(rand_index).did){
                Sleep(2000);
                break;
            }
       }

        if(nauka.at(rand_index).did){
            ma_engine_play_sound(&engine, PASS_SOUND, NULL);
            Sleep(1000);
        }

        system("cls");
        if(nauka.at(rand_index).did){
            cout << "Brawo udalo ci sie to nauczyc \"" << nauka.at(rand_index).forms.at(0) << "\"" << endl;
            cout << "---------------------------------------------------" << endl;
            
        }
    }

}