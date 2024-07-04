#include "Graph.h"

Graph::Graph(int n, const list< pair<int, int> > & edges):
	n(n),
	m(edges.size()),
	adjMat(n, vector<bool>(n, false)),
	adjList(n),
	edges(),
	edgeIndex(n, vector<int>(n, -1))
{
	for(list< pair<int, int> >::const_iterator it = edges.begin(); it != edges.end(); it++)
	{
		int u = (*it).first;
		int v = (*it).second;

		AddEdge(u, v);
	}
}

pair<int, int> Graph::GetEdge(int e) const
{
	if(e > (int)edges.size())
		cout << "GetEdge: edge does not exist" << endl;

	return edges[e];
}

int Graph::GetEdgeIndex(int u, int v) const
{
	if( u > n or
		v > n )
		cout << "GetEdgeIndex1: vertex does not exist" << endl;

	if(edgeIndex[u][v] == -1)
		cout << "GetEdgeIndex2: edge does not exist" << endl;

	return edgeIndex[u][v];
}

void Graph::AddVertex()
{
	for(int i = 0; i < n; i++)
	{
		adjMat[i].push_back(false);
		edgeIndex[i].push_back(-1);
	}
	n++;
	adjMat.push_back( vector<bool>(n, false) );
	edgeIndex.push_back( vector<int>(n, -1) );
	adjList.push_back( list<int>() );
}

void Graph::AddEdge(int u, int v)
{
	if( u > n or
		v > n )
		cout << "AddEdge: vertex does not exist. " << "u: " << u << ", v: " << v << ", n: " << n << endl;

	if(adjMat[u][v]) return;

	adjMat[u][v] = adjMat[v][u] = true;
	adjList[u].push_back(v);
	adjList[v].push_back(u);

	edges.push_back(pair<int, int>(u, v));
	edgeIndex[u][v] = edgeIndex[v][u] = m++;
}

const list<int> & Graph::AdjList(int v) const
{
	if(v > n)
		cout << "AdjList: vertex does not exist";

	return adjList[v];
}

const vector< vector<bool> > & Graph::AdjMat() const
{
	return adjMat;
}

