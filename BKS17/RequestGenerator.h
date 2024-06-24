#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <cfloat>
#include <random>
#include "Request.h"
//#include <utility> 
//#include <functional>
using namespace std;

class RequestGenerator {

public : 

    int min_location, max_location, number_of_requests;

    // <arrive_time, location>
    queue<pair<double, int>> q;

    // Constructor
    RequestGenerator();

    // generate requests
    void generateRandom(int min_loc, int max_loc, int num_of_req);

    void generateGreedy();

    void generateCustomized();

    // provide arrive time of next request
    double get_next_request_atime();

    pair<double, int> throw_next_request();

    bool is_empty();
};