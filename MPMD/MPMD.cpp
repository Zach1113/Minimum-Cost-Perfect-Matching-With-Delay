// g++ -O3 MPMD.cpp BinaryHeap.cpp Matching.cpp Graph.cpp Request.cpp RequestGenerator.cpp -o mpmd
// ./mpmd

#include "Matching.h"
#include "Graph.h"
#include "Request.h"
#include "RequestGenerator.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

vector<Request> offlineQ;

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

class OPT{

public: 

    void execute(){

        Graph G;
        vector<double> cost;
        
        //Read the graph
        pair< Graph, vector<double> > p = ReadWeightedGraph(offlineQ);
        G = p.first;
        cost = p.second;

        //Create a Matching instance passing the graph
        Matching M(G);

        //Pass the costs to solve the problem
        pair< list<int>, double > solution = M.SolveMinimumCostPerfectMatching(cost);

        list<int> matching = solution.first;
        double obj = solution.second;
        double totalDistCost = 0;

        cout << "OPT" << endl;
        cout << "Cost: " << obj << endl;

        for(list<int>::iterator it = matching.begin(); it != matching.end(); it++)
        {
            pair<int, int> e = G.GetEdge(*it);
            int r1 = e.first, r2 = e.second;
            totalDistCost += abs(offlineQ[r1].location - offlineQ[r2].location);
        }
        cout << "Dist Cost: " << totalDistCost << endl; 
        cout << "Wait Cost: " << obj - totalDistCost << endl;
    }

    pair< Graph, vector<double> > ReadWeightedGraph(vector<Request> Q){

        int n = Q.size(); // vertices
        int m = nChoosek(n, 2); // edges
        
        Graph G(n);
        vector<double> cost(m);
        for(int u = 0; u < n - 1; ++u){
            for(int v = u + 1; v < n; ++v){
                double c = abs(Q[u].location - Q[v].location) + (Q[v].atime - Q[u].atime);
                G.AddEdge(u, v);
                cost[G.GetEdgeIndex(u, v)] = c;
            }
        }

        return make_pair(G, cost);
    }

};

class BKS17{

public :

    vector<Request> RQ;

    bool request_event = true;
    int request_ID = 0;
    double totalCost = 0;
    double totalDistCost = 0;
    double totalWaitCost = 0;
    double time = 0;

    //constructor
    BKS17(){}

    // execute algorithm
    void execute(RequestGenerator RG){
        
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
                //offlineQ.push_back(new_request);
                // tell every already existed requests that a new request has arrived
                for(int k = 0; k < RQ.size(); ++k){
                    RQ[k].new_request_arrive_BKS17(new_request);
                }
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
                matched_requests += 2;
                
                totalCost += edgeCost;
                totalDistCost += distCost;
                totalWaitCost += waitCost;
            }
        }
        cout << "BKS17" << endl;
        cout << "Cost: " << totalCost << endl;
        cout << "Dist Cost: " << totalDistCost << endl;
        cout << "Wait Cost: " << totalWaitCost << endl << endl;
    }
};

class AJF20{

public :

    vector<Request> RQ;

    bool request_event = true;
    int request_ID = 0;
    double totalCost = 0;
    double totalDistCost = 0;
    double totalWaitCost = 0;
    double time = 0;

    //constructor
    AJF20(){}

    // execute algorithm
    void execute(RequestGenerator RG){
        
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
                // tell every already existed requests that a new request has arrived
                for(int k = 0; k < RQ.size(); ++k){
                    RQ[k].new_request_arrive_AJF20(new_request);
                }
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
                matched_requests += 2;
                
                totalCost += edgeCost; 
                totalDistCost += distCost;
                totalWaitCost += waitCost;
            }
        }
        cout << "AJF20" << endl;
        cout << "Cost: " << totalCost << endl;
        cout << "Dist Cost: " << totalDistCost << endl;
        cout << "Wait Cost: " << totalWaitCost << endl << endl;
    }
};

class MPMD{

public :

    vector<Request> RQ;
   
    vector<int> tmp;
    priority_queue<pair<double, pair<int, int>>, vector<pair<double, pair<int, int>>>, greater<pair<double, pair<int, int>>>> pq;

    bool request_event = true;
    int request_ID = 0;
    double totalCost = 0;
    double totalDistCost = 0;
    double totalWaitCost = 0;
    double time = 0;

    int matched_requests = 0;
    int matchID, targetID;

    //constructor
    MPMD(){}

    pair< Graph, vector<double> > ReadWeightedGraph(vector<Request> Q)
    {
        tmp.clear();
        int n = 0; // vertices
        for(int i = 0; i < Q.size(); ++i){
            if(!Q[i].matched){
                ++n;
                tmp.push_back(i);
            }
        }

        int m = nChoosek(n, 2); // edges
        
        Graph G(n);
        vector<double> cost(m);
        for(int u = 0; u < tmp.size() - 1; ++u){
            for(int v = u + 1; v < tmp.size(); ++v){
                double c = abs(Q[tmp[u]].location - Q[tmp[v]].location) + (Q[tmp[v]].atime - Q[tmp[u]].atime);
                G.AddEdge(u, v);
                cost[G.GetEdgeIndex(u, v)] = c;
            }
        }

        return make_pair(G, cost);
    }

    void MinimumCostPerfectMatching(vector<Request> Q)
    {
        Graph G;
        vector<double> cost;
        
        //Read the graph
        pair< Graph, vector<double> > p = ReadWeightedGraph(Q);
        G = p.first;
        cost = p.second;

        //Create a Matching instance passing the graph
        Matching M(G);

        //Pass the costs to solve the problem
        pair< list<int>, double > solution = M.SolveMinimumCostPerfectMatching(cost);

        list<int> matching = solution.first;

        for(list<int>::iterator it = matching.begin(); it != matching.end(); it++)
        {
            pair<int, int> e = G.GetEdge(*it);
            int r1 = tmp[e.first], r2 = tmp[e.second];
            double matching_time = (abs(Q[r1].location - Q[r2].location) + 2 * (Q[r1].atime + Q[r2].atime)) / 4;
            pq.push(make_pair(matching_time, make_pair(r1, r2)));
        }
    }

    // execute algorithm
    void execute(RequestGenerator RG){

        while(matched_requests != RG.number_of_requests){         

            request_event = true;
            double next_event_time = RG.get_next_request_atime();

            //compare next arrival time with closest matching event, set boolean value
            if(!pq.empty()){
                double next_matching_time = pq.top().first;
                if(next_matching_time < next_event_time){
                    request_event = false;
                    next_event_time = next_matching_time;
                }
            }

            time = next_event_time;
            
            // request comes first
            if(request_event){
                
                auto tmp = RG.throw_next_request(); //tmp: <atime, location>
                Request new_request(tmp.first, tmp.second, request_ID);
                //cout << "Request event: " << request_ID << endl;
                RQ.push_back(new_request);
                offlineQ.push_back(new_request);

                // Do offline alg if even request arrives
                if(request_ID % 2 == 1){
                    
                    // clear pq before executing offline
                    while (!pq.empty()) pq.pop();
                    MinimumCostPerfectMatching(RQ);
                    
                    // Need to check if the new matching result causes any immediate matching
                    while(!pq.empty() && pq.top().first <= time){
                       match();
                    }
                }
                request_ID++;
            }

            // next matching happens first
            else {
                match();
            }
        }
        cout << "MPMD" << endl;
        cout << "Cost: " << totalCost << endl;
        cout << "Dist Cost: " << totalDistCost << endl;
        cout << "Wait Cost: " << totalWaitCost << endl << endl;
    }

    void match(){
        
        matchID = pq.top().second.first;
        targetID = pq.top().second.second;
        pq.pop();
        
        RQ[matchID].matched = true;
        RQ[targetID].matched = true;
        //cout << "match " << matchID << " and " << targetID << endl;

        double distCost = abs(RQ[matchID].location - RQ[targetID].location);
        double waitCost = (time - RQ[matchID].atime) + (time - RQ[targetID].atime);
        double edgeCost = distCost + waitCost;
        matched_requests += 2;
        
        totalCost += edgeCost;
        totalDistCost += distCost;
        totalWaitCost += waitCost;
    }
};

int main()
{
	int min_location = 0, max_location = 100;
    int number_of_requests = 1000; //has to be an even number

    // generate all requests
    RequestGenerator RG;
	//RG.generateGreedy();
    //RG.generateCustomized();
    RG.generateRandom(min_location, max_location, number_of_requests);

    MPMD alg1;
    alg1.execute(RG);

    BKS17 alg2;
    alg2.execute(RG);

    AJF20 alg3;
    alg3.execute(RG);

	OPT alg4;
    alg4.execute();
    
	return 0;
}