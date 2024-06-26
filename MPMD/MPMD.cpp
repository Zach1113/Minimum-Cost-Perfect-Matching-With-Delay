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

vector<Request> RQ;

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

void outputRequestAsFile(RequestGenerator RG, string filename){
    
    vector<Request> RQ;
    int number_of_requests = RG.number_of_requests;
    int k = 2; // c(n, 2)

    for(int i = 0; i < number_of_requests; ++i){
        auto tmp = RG.throw_next_request(); //tmp: <atime, location>
        Request new_request(tmp.first, tmp.second, i);
        RQ.push_back(new_request);    
    }

    ofstream requestFile(filename);
    
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
}

pair< Graph, vector<double> > ReadWeightedGraph(string filename)
{
	//Please see Graph.h for a description of the interface

	ifstream file;
	file.open(filename.c_str());

	string s;
	getline(file, s);
	stringstream ss(s);
	int n;
	ss >> n;
	getline(file, s);
	ss.str(s);
	ss.clear();
	int m;
	ss >> m;

	Graph G(n);
	vector<double> cost(m);
	for(int i = 0; i < m; i++)
	{
		getline(file, s);
		ss.str(s);
		ss.clear();
		int u, v;
		double c;
		ss >> u >> v >> c;

		G.AddEdge(u, v);
		cost[G.GetEdgeIndex(u, v)] = c;
	}

	file.close();
	return make_pair(G, cost);
}

void MinimumCostPerfectMatchingExample(string filename)
{
	Graph G;
	vector<double> cost;
	
	//Read the graph
	pair< Graph, vector<double> > p = ReadWeightedGraph(filename);
	G = p.first;
	cost = p.second;

	//Create a Matching instance passing the graph
	Matching M(G);

	//Pass the costs to solve the problem
	pair< list<int>, double > solution = M.SolveMinimumCostPerfectMatching(cost);

	list<int> matching = solution.first;
	double obj = solution.second;
	double totalDistCost = 0;

	cout << "Opt Cost: " << obj << endl;

	for(list<int>::iterator it = matching.begin(); it != matching.end(); it++)
	{
		pair<int, int> e = G.GetEdge(*it);
		int r1 = e.first, r2 = e.second;
		totalDistCost += abs(RQ[r1].location - RQ[r2].location);
	}
	cout << "Dist Cost: " << totalDistCost << endl; 
	cout << "Wait Cost: " << obj - totalDistCost << endl;
}

class MPMD{

public :

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
                matched_requests += 2;
                
                totalCost += edgeCost;
                totalDistCost += distCost;
                totalWaitCost += waitCost;
            }
        }
        cout << "Alg Cost: " << totalCost << endl;
        cout << "Dist Cost: " << totalDistCost << endl;
        cout << "Wait Cost: " << totalWaitCost << endl << endl;
    }
};

int main(int argc, char* argv[])
{
	int min_location = 0, max_location = 100;
    int number_of_requests = 10; //has to be an even number

    // generate all requests
    RequestGenerator RG;
	//RG.generateGreedy();
    RG.generateRandom(min_location, max_location, number_of_requests);
	outputRequestAsFile(RG, "requestFile.txt");

    MPMD alg;
    alg.execute(RG);

	string filename = "requestFile.txt";

	int i = 1;
	while(i < argc)
	{
		string a(argv[i]);
		i++;
	}

	try{
		MinimumCostPerfectMatchingExample(filename);
	}
	catch(const char * msg){
		cout << msg << endl;
		return 1;
	}

	return 0;
}