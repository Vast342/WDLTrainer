// probably excessive includes
#include <iostream>
#include <cstdint>
#include <cassert>
#include <string_view>
#include <array>
#include <bit>
#include <ranges>
#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <chrono>
#include <fstream>
#include <memory>
#include <charconv>

struct DataPoint {
    int moveNumber;
    int score;
    double winRate;
    DataPoint(int m, int s, double w) {
        moveNumber = m;
        score = s;
        winRate = w;
    }
};

std::string pgnDir;
std::vector<DataPoint> win_rate_data;

// splits a string into segments based on the seperator
// borrowed from Clarity
std::vector<std::string> split(const std::string string, const char seperator) {
    std::stringstream stream(string);
    std::string segment;
    std::vector<std::string> list;

    // every time that it can get a segment
    while(std::getline(stream, segment, seperator)) {
        // add it to the vector
        list.push_back(segment);
    }
    
    return list;
}

// initial values borrowed from stockfish
std::array<double, 8> params = {-1.719, 12.448, -12.855, 331.883, -3.001, 22.505, -51.253, 93.209};

// function to get horizontal shift parameter
int get_logistic_a(int move_count) {
    return ((params[0] * move_count / 32 + params[1]) * move_count / 32 + params[2]) * move_count / 32 + params[3];
}

// function to get curve aggressiveness parameter
int get_logistic_b(int move_count) {
    return ((params[4] * move_count / 32 + params[5]) * move_count / 32 + params[6]) * move_count / 32 + params[7];
}

// calculates the win percentage based on the above parameters (we love logistdic curves in this household)
double calculate_winpercent(int score, int move_count) {
    return 1 / (1 + exp(-(score - get_logistic_a(move_count))) + get_logistic_b(move_count));
}

// calculates the ratio of wins and draws and losses
std::array<double, 3> calculate_wdl(int score, int move_count) {
    const double win_ratio = calculate_winpercent(score, move_count);
    const double loss_ratio = calculate_winpercent(-score, move_count);
    const double draw_ratio = 1 - win_ratio - loss_ratio;
    return {win_ratio, draw_ratio, loss_ratio};
}

// round number to nearest multiple of 5
int round(int n) 
{ 
    // Smaller multiple 
    int a = (n / 5) * 5; 
      
    // Larger multiple 
    int b = a + 5; 
  
    // Return of closest of two 
    return (n - a > b - n)? b : a; 
} 

struct Case
{
    int moveCount, score;
    double result;
};

void convert_data() {
    std::chrono::steady_clock::time_point beginTime = std::chrono::steady_clock::now();
    std::ifstream pgnStream(pgnDir);
    int lineCount = 0;
    std::vector<Case> cases;
    std::string line;
    while (std::getline(pgnStream, line))
    {
        // I can't use case because it's reserved
        Case f;
        auto res = std::from_chars(line.c_str(), line.c_str() + line.length(), f.moveCount);
        res = std::from_chars(res.ptr + 1, line.c_str() + line.length(), f.score);
        std::from_chars(res.ptr + 1, line.c_str() + line.length(), f.result);
        cases.push_back(f);
        lineCount++;
    }
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beginTime).count();
    std::cout << "File loaded in " << elapsedTime << " ms, total of " << lineCount << " lines" << std::endl;
    beginTime = std::chrono::steady_clock::now();
    struct
    {
        bool operator()(Case a, Case b) const {
            if (a.moveCount < b.moveCount)
                return true;
            if (a.moveCount > b.moveCount)
                return false;
            // a.first == b.first, compare second
            return a.score < b.score;
         }
    }
    customLessThan;
    std::sort(cases.begin(), cases.end(), customLessThan);

    elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beginTime).count();
    std::cout << "Sorted cases in " << elapsedTime << " ms" << std::endl;
    beginTime = std::chrono::steady_clock::now();
    // convert each data point to a winrate based on move count and score
    int currentMove = 0;
    int currentScore = -10000;
    int currentScoreInstances = 0;
    int currentScoreWins = 0;
    int number = 0;
    for(Case entry : cases) {
        number++;
        //std::cout << "entry number " << number << " current move: " << currentMove << " current score: " << currentScore << std::endl;
        if(currentMove != entry.moveCount) {
            currentMove = entry.moveCount;
            currentScore = -10000;
            currentScoreInstances = 0;
            currentScoreWins = 0;
        }
        if(currentScore != entry.score) {
            if(currentScore != -10000) {
                win_rate_data.push_back(DataPoint(currentMove, currentScore, double(currentScoreWins) / double(currentScoreInstances)));
            }
            currentScore = entry.score;
        }
        currentScoreInstances++;
        if(entry.result == 1) {
            currentScoreWins++;
        }
    }
    elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - beginTime).count();
    std::cout << "Converted into data points in " << elapsedTime << " ms" << std::endl;
    beginTime = std::chrono::steady_clock::now();
}

double calculate_error() {
    double dataPoints = 0;
    double totalError = 0;
    for(DataPoint point : win_rate_data) {
        totalError += pow((point.winRate - calculate_winpercent(point.score, point.moveNumber)), 2);
        dataPoints++;
    }
    return totalError / dataPoints;
}

void train_stuff() {
    std::cout << "initial error: " << calculate_error() << std::endl;
    std::cout << "what would you like the learning rate to start out as?" << std::endl;
    double lr;
    std::cin >> lr;
    std::cout << "how many iterations would you like?" << std::endl;
    int iterations;
    std::cin >> iterations;
    for(int i = 0; i < iterations; i++) {
        // stuff to do each iteration
    }
    std::cout << "this is totally me training stuff" << std::endl;
    std::cout << "final error: " << calculate_error() << std::endl;
}

void output_params() {
    std::cout << "here are your parameters:" << std::endl;
    std::cout << "lmao sike i didn't do ANYTHING yet" << std::endl;
}

int main() {
    std::cout << "where is the pgn file?" << std::endl;
    std::cin >> pgnDir;
    convert_data();
    train_stuff();
    output_params();
    std::cout << "cool, bye" << std::endl;
    return 0;
}