/*
 * main.cpp
 *
 *  Created on: Nov 18, 2018
 *      Author: owner
 */

#include <iostream>
#include <fstream>
#include <unistd.h>

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <queue>
using namespace std;

void printGraph(unordered_map<string, vector<pair<string, int> > > &adj)
{
	for (auto i = adj.begin(); i != adj.end(); i++) {
		cout << i->first;
		for (auto y = (i->second).begin(); y != (i->second).end(); y++)
			cout << "-> " << y->first << y->second;
		printf("\n");
	}
}
void process_mem_usage(double& vm_usage, double& resident_set)
{
    vm_usage     = 0.0;
    resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}
//double vm, rss;
//   process_mem_usage(vm, rss);
//   cout << "VM: " << vm << "; RSS: " << rss << endl;

void addEdge(vector<pair<string, int> > &adj, string u, int weight){
	auto i = adj.begin();
	for(; i != adj.end(); i++){
		if(i-> first == u){
			i->second = (i->second < weight)? i->second: weight;
			break;
		}
	}
	pair<string, int> input (u, weight);
	if(i == adj.end())
		adj.push_back(input);
}

void insertion(string input, unordered_map<string, int> &hash, unordered_map<string, vector<pair<string, int> > > &adj, queue<string> &inputs){
	for(int i = 0; i < input.length()+1; i ++){
		for(int j = 0; j < 26; j++){
			char letter = 'a' + j;
			string insertion = input.substr(0,i) + letter + input.substr(i, input.length()-i);

			if(hash.find(insertion)!=hash.end()){
				// if input is found
				addEdge(adj[insertion], input, 3);
				addEdge(adj[input], insertion, 1);
				if(!hash[insertion])
					inputs.push(insertion);
				hash[insertion] = 1;
			}
		}
	}
}

void deletion(string input, unordered_map<string, int> &hash, unordered_map<string, vector<pair<string, int> > > &adj, queue<string> &inputs){
	for(int i = 0; i < input.length(); i ++){
		string temp = input;
		string deletion = temp.erase(i,1);
		if(hash.find(deletion) != hash.end()){
			addEdge(adj[input], deletion, 3);
			addEdge(adj[deletion], input, 1);
			if(!hash[deletion])
				inputs.push(deletion);
			hash[deletion] = 1;
		}
	}
}

void twinddle(string input, unordered_map<string, int> &hash, unordered_map<string, vector<pair<string, int> > > &adj, queue<string> &inputs){
	for(int i = 0; i < input.length(); i ++){
		for(int j = i+1; j < input.length(); j++){
			if(input[i] == input[j])
				continue;
			string twinddle = input.substr(0,i) + input[j] + input.substr(i+1, j-1-i) + input[i] + input.substr(j+1);
			if(hash.find(twinddle) != hash.end()){
				addEdge(adj[input], twinddle, 2);
				addEdge(adj[twinddle], input, 2);
				if(!hash[twinddle])
					inputs.push(twinddle);
				hash[twinddle] = 1;
			}
		}
	}

}

void reversal(string input, unordered_map<string, int> &hash, unordered_map<string, vector<pair<string, int> > > &adj, queue<string> &inputs){
	string reverse = input;
	int n = input.length();

	for (int i = 0; i < n / 2; i++)
		swap(reverse[i], reverse[n - i - 1]);

	if(hash.find(reverse) != hash.end()){
		addEdge(adj[input], reverse, n);
		addEdge(adj[reverse], input, n);
		if(!hash[reverse])
			inputs.push(reverse);
		hash[reverse] = 1;
	}
}

void constructGraph(string input, unordered_map<string, int> &hash, unordered_map<string, vector<pair<string, int> > > &adj, queue<string> &inputs){
	insertion(input, hash, adj, inputs);
	deletion(input, hash, adj, inputs);
	twinddle(input, hash, adj, inputs);
	reversal(input, hash, adj, inputs);
	inputs.pop();
}

int minEdgeBFS(unordered_map<string, vector<pair<string, int> > > &adj, string startingWord, string targetWord){
	if(adj.find(targetWord) == adj.end()){
		cout << "-1" << endl;
		return -1;
	}
	unordered_map<string, int> dist;
	unordered_map<string, vector<string> > order;
	unordered_map<string, bool> visited;
	queue<string> Q;

	for(auto i = adj.begin(); i != adj.end(); i++){
		dist[i->first] = -1; visited[i->first] = false;
	}

	dist[startingWord] = 0;
	visited[startingWord] = true;
	Q.push(startingWord);

	while(!Q.empty()){
		string word = Q.front(); Q.pop();
		for(auto i = adj[word].begin(); i != adj[word].end(); i++){
			if(visited[i->first])
				continue;
			dist[i->first] = dist[word] + i->second;
			order[i->first] = order[word];
			order[i->first].push_back(i->first);
			Q.push(i->first);
			visited[i->first] = true;
		}
	}

	cout << dist[targetWord];
	if(dist[targetWord] != -1){
		cout << '\t' << startingWord << '\t';
		for(auto i = order[targetWord].begin(); i != order[targetWord].end(); i++){
			cout << *i << '\t';
		}
	}

	cout << endl;
}


int main (){

	unordered_map<string, int> hash;
	unordered_map<string, vector<pair<string, int> > > adj;
	queue<string> inputs;

	ifstream inf("dict.txt");
	if(!inf)
		cout << "file open error" << endl;

	while(inf){
		string a;
		inf >> a;
		hash[a];
	}

	if(hash.find("")!= hash.end())
		hash.erase("");

	queue<string> standardInput;

	while(1){
		string startingWord; cin >> startingWord;
		string targetWord; cin >> targetWord;

		hash[startingWord] = 1;

		inputs.push(startingWord);
		while(!inputs.empty()){
			constructGraph(inputs.front(), hash, adj, inputs);
		}
		minEdgeBFS(adj, startingWord, targetWord);

	}
}
