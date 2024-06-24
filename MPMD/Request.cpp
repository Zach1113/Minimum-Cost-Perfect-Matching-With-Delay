#include "Request.h"

Request::Request(double arrive_time, int loc, int id)
    : atime(arrive_time), location(loc), ID(id) {}

    // calculate the future matching time between this request and new arrived request
void Request::new_request_arrive(const Request &r){
    if (r.ID == ID) return;
    double budget_sufficiency_time = (abs(r.location - location) + (r.atime + atime) * alpha) / (2 * alpha);
    double budget_balance_time = (beta * r.atime - atime) / (beta - 1); 
    double matching_time = max(budget_sufficiency_time, budget_balance_time);
    pq.push(make_pair(matching_time, r.ID));
}

double Request::get_min_matching_time(const vector<Request> &RQ){
    while(!pq.empty() && RQ[pq.top().second].matched){
        pq.pop();
    }
    if(pq.empty() || matched) return DBL_MAX;
    return pq.top().first;
}

// next match event occured, return target request ID
int Request::get_match_targetID(){
    auto tmp = pq.top();
    pq.pop();
    return tmp.second;
}