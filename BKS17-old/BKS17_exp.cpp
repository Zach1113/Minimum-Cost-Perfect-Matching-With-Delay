#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <cfloat>
#include <random>
#include <utility> 
#include <functional>
#include <fstream>
using namespace std;

int nChoosek(int n, int k){

    if (k > n) return 0;
    if (k * 2 > n) /*return*/ k = n-k;  //remove the commented section
    if (k == 0) return 1;
 
    int result = n;
    for(int i = 2; i <= k; ++i) {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}


class Request {
    
public :
    // arrive time and location of each requests
    double atime;
    int location;

    // request ID
    int ID;

    // request is matched or not
    bool matched = false;

    // algorithm parameters
    double alpha = 2;
    double beta = 2;
    
    // a priority queue that stores the future matching time from this request to other requests
    // pq<matching time, targetID> 
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    // Constructor
    Request(double arrive_time, int loc, int id){
        atime = arrive_time;
        location = loc;
        ID = id;
    }

    // calculate the future matching time between this request and new arrived request
    void new_request_arrive(const Request &r){
        if (r.ID == ID) return;
        double budget_sufficiency_time = (abs(r.location - location) + (r.atime + atime) * alpha) / (2 * alpha);
        double budget_balance_time = (beta * r.atime - atime) / (beta - 1); 
        double matching_time = max(budget_sufficiency_time, budget_balance_time);
        pq.push(make_pair(matching_time, r.ID));
    }

    double get_min_matching_time(const vector<Request> &RQ){
        while(!pq.empty() && RQ[pq.top().second].matched){
            pq.pop();
        }
        if(pq.empty() || matched) return DBL_MAX;
        return pq.top().first;
    }

    // next match event occured, return target request ID
    int get_match_targetID(){
        auto tmp = pq.top();
        pq.pop();
        return tmp.second;
    }

};


class RequestGenerator {

public : 

    int min_location, max_location, number_of_requests;

    // <arrive_time, location>
    queue<pair<double, int>> q;

    // Constructor
    RequestGenerator(){}

    // generate requests
    void generateRandom(int min_loc, int max_loc, int num_of_req){

        min_location = min_loc;
        max_location = max_loc;
        number_of_requests = num_of_req;

        random_device rd;
        mt19937 gen(rd());

        double lambda = 0.1;
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

    void generateGreedy(){
        
        number_of_requests = 8;
        int length = 14;

        pair<double, int> arr[8] = {{0, 0}, {0, 2}, {0, 3}, {0, 5}, {0, 9}, {0, 11}, {0, 12}, {0, 14}};
        for(int i = 0; i < number_of_requests; ++i){
            q.push(arr[i]);
        }
    }

    void generateCustomized(){
        
        number_of_requests = 8;
        int length = 14;

        pair<double, int> arr[8] = {{0.0, 20}, {2.0, 5}, {3.0, 3}, {8.0, 15}, {15.0, 11}, {17.0, 21}, {18.0, 30}, {23.0, 19}};
        for(int i = 0; i < number_of_requests; ++i){
            q.push(arr[i]);
        }
    }

    // provide arrive time of next request
    double get_next_request_atime(){
        if(is_empty()) return DBL_MAX;
        return q.front().first;
    }

    pair<double, int> throw_next_request(){
        if(is_empty()) cout << "no more requests" << endl;
        auto tmp = q.front();
        q.pop();
        return tmp;
    }

    bool is_empty(){
        return q.empty();
    }
};

class MPMD{

public :

    vector<Request> RQ;
    bool request_event = true;
    int request_ID = 0;
    double totalCost = 0;
    double totalDistCost = 0;
    double totalWaitCost = 0;
    double time = 0;

    //constructor
    MPMD(){}

    // execute algorithm
    void execute(RequestGenerator &RG){
        
        int matched_requests = 0;
        while(matched_requests != RG.number_of_requests){

            int matchID, targetID;

            request_event = true;
            double next_event_time = RG.get_next_request_atime();

            //compare next arrival time with closest matching event, set boolean value
            for(int k = 0; k < RQ.size(); ++k){
                double next_matching_time = RQ[k].get_min_matching_time(RQ);
                if(next_matching_time < next_event_time){
                    request_event = false;
                    next_event_time = next_matching_time;
                    matchID = k;
                }
            }

            time = next_event_time;
            
            // request comes first
            if(request_event){
                auto tmp = RG.throw_next_request(); //tmp: <atime, location>
                Request new_request(tmp.first, tmp.second, request_ID);
                RQ.push_back(new_request);
                //cout << "Request " << new_request.ID << " arrived " << endl;
                //cout << "arrive time: " << new_request.atime << ", location: " << new_request.location << endl;
                
                // tell every already existed requests that a new request has arrived
                for(int k = 0; k < RQ.size(); ++k)
                    RQ[k].new_request_arrive(new_request);

                request_ID++;
            }

            // next matching happens first
            else{
                int targetID = RQ[matchID].get_match_targetID();
                RQ[matchID].matched = true;
                RQ[targetID].matched = true;

                double distCost = abs(RQ[matchID].location - RQ[targetID].location);
                double waitCost = (time - RQ[matchID].atime) + (time - RQ[targetID].atime);
                double edgeCost = distCost + waitCost;
                //cout << "match " << matchID << " and " << targetID << endl;
                //cout << "match time: " << time << ", cost: " << edgeCost <<endl;
                matched_requests += 2;
                
                totalCost += edgeCost;
                totalDistCost += distCost;
                totalWaitCost += waitCost;
            }
            //cout << "------------------------------------" << endl;
        }
        cout << "Alg Total Cost: " << totalCost << endl;
        cout << "Dist Cost: " << (totalDistCost / totalCost) * 100 << "%" << endl;
        cout << "Wait Cost: " << (totalWaitCost / totalCost) * 100 << "%" << endl;
    }
};

void outputRequestAsFile(RequestGenerator RG){
    
    vector<Request> RQ;
    int number_of_requests = RG.number_of_requests;
    int k = 2; // c(n, 2)

    for(int i = 0; i < number_of_requests; ++i){
        auto tmp = RG.throw_next_request(); //tmp: <atime, location>
        Request new_request(tmp.first, tmp.second, i);
        RQ.push_back(new_request);    
    }

    ofstream requestFile("requestFile.txt");
    
    // Write text to the file
    requestFile << number_of_requests << endl;
    requestFile << nChoosek(number_of_requests, k) << endl;
    
    for(int i = 0; i < number_of_requests - 1; ++i){
        for(int j = i + 1; j < number_of_requests; ++j){
            int cost = abs(RQ[i].location - RQ[j].location) + (RQ[j].atime - RQ[i].atime);
            requestFile << i << " " << j << " " << cost << endl;
        }
    }
    requestFile.close();
    //cout << "Text written to file successfully." << endl << endl;;
}

int main()
{   
    int min_location = 0, max_location = 100;
    int number_of_requests = 1000; //has to be an even number

    // generate all requests
    RequestGenerator RG;
    RG.generateRandom(min_location, max_location, number_of_requests);
    outputRequestAsFile(RG);

    MPMD alg;
    alg.execute(RG);

    return 0;
}