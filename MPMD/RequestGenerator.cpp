#include "RequestGenerator.h"

RequestGenerator::RequestGenerator(){}

// generate requests
void RequestGenerator::generateRandom(int min_loc, int max_loc, int num_of_req){

    min_location = min_loc;
    max_location = max_loc;
    number_of_requests = num_of_req;

    random_device rd;
    mt19937 gen(rd());

    double lambda = 1;
    exponential_distribution<> exp_dis(lambda);
    uniform_int_distribution<> uni_dis(min_location, max_location);

    double time = 0, location;
    for(int i = 0; i < number_of_requests; ++i){
        if(i == 0){
            q.push({time, uni_dis(gen)});
            continue;
        }

        time += exp_dis(gen);
        location = uni_dis(gen);
        q.push({time, location});
    }

}

void RequestGenerator::generateGreedy(){
    
    number_of_requests = 8;
    int length = 14;

    pair<double, int> arr[8] = {{0, 0}, {0, 2}, {0, 3}, {0, 5}, {0, 9}, {0, 11}, {0, 12}, {0, 14}};
    for(int i = 0; i < number_of_requests; ++i){
        q.push(arr[i]);
    }
}

void RequestGenerator::generateCustomized(){
    
    number_of_requests = 8;
    int length = 14;

    pair<double, int> arr[8] = {{0.0, 20}, {2.0, 5}, {3.0, 3}, {8.0, 15}, {15.0, 11}, {17.0, 21}, {18.0, 30}, {23.0, 19}};
    for(int i = 0; i < number_of_requests; ++i){
        q.push(arr[i]);
    }
}

// provide arrive time of next request
double RequestGenerator::get_next_request_atime(){
    if(is_empty()) return DBL_MAX;
    return q.front().first;
}

pair<double, int> RequestGenerator::throw_next_request(){
    if(is_empty()) cout << "no more requests" << endl;
    auto tmp = q.front();
    q.pop();
    return tmp;
}

bool RequestGenerator::is_empty(){
    return q.empty();
}