#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <cfloat>
//#include <random>
//#include <utility> 
//#include <functional>
using namespace std;

class Request {
    
public :
    // arrive time and location of each requests
    double atime;
    int location;

    // request ID
    int ID;

    // request is matched or not
    bool matched = false;

    // BKS17 parameters
    double alpha = 0.5;
    double beta = 2;
    
    // BKS17
    // a priority queue that stores the future matching time from this request to other requests
    // pq<matching time, targetID> 
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    // Constructor
    Request(double arrive_time, int loc, int id);

    // BKS17
    // calculate the future matching time between this request and new arrived request
    void new_request_arrive(const Request &r);

    double get_min_matching_time(const vector<Request> &RQ);

    // next match event occured, return target request ID
    int get_match_targetID();

};