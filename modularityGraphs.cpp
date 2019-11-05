/*
Modularity-based Graph Generator.
Version 0.0
Authors:
  - Jesús Giráldez-Cru (UGR)

Based on: Jesús Giráldez-Cru, Jordi Levy: Generating SAT Instances with Community Structure. Artificial Intelligence 238: 119-134 (2016).

Contact: jgiraldez@ugr.es

    Copyright (C) 2019 J. Giráldez-Cru, J. Levy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <getopt.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <stdio.h>

using namespace std;

const int k = 2;

const int nodes_defvalue = 1000;
const int edges_defvalue = 4000;
const int comm_defvalue = 10;
const double Q_defvalue = 0.8;
const int seed_defvalue = 0;

	
int nodes = nodes_defvalue;			// number of nodes
int edges = edges_defvalue;			// number of edges
double Q = Q_defvalue;				// modularity
int communities = comm_defvalue;	// number of communities
int seed = seed_defvalue;

double P; 							// probability


void printUsage(char* app){
	cerr << "  Usage: " << app << " [options]" << endl;
	cerr << "    -n <int>   :  number of nodes (" << nodes_defvalue << ")" << endl;
	cerr << "    -m <int>   :  number of edges (" << edges_defvalue << ")" << endl;
	cerr << "    -c <int>   :  number of communities (" << comm_defvalue << ")" << endl;
	cerr << "    -Q <float> :  modularity: Q (" << Q_defvalue << ")" << endl;
	cerr << "    -s <int>   :  seed (" << seed_defvalue << ")" << endl;
	cerr << "    -o <string>:  output file (stdout)" << endl;
	cerr << "  Restrictions:" << endl;
	cerr << "    1. c must be greater than 1" << endl;
	cerr << "    2. Q must be in the interval (0,1)" << endl;
	exit(-1);
}

void parseArgs(int argc, char **argv){
	int opt;
	while((opt=getopt(argc, argv, "n:m:c:Q:s:?h")) != -1){
		switch(opt){
			case 'n':
				nodes = atoi(optarg);
				break;
			case 'm':
				edges = atoi(optarg);
				break;
			case 'c':
				communities = atoi(optarg);
				if(communities <= 1 ){
					cerr << "WARNING: c must be greater than 1" << endl;
					cerr << "  c changed to " << comm_defvalue << " (default value)" << endl;
					communities = comm_defvalue;
				}
				break;
			case 'Q':
				Q = atof(optarg);
				if(Q <= 0 || Q >= 1){
					cerr << "WARNING: Q must be in the interval (0,1)" << endl;
					cerr << "  Q changed to " << Q_defvalue << " (default value)" << endl;
					Q = Q_defvalue;
				}
				break;
			case 's':
				seed = atoi(optarg);
				break;
			case 'h':
			case '?':
				printUsage(argv[0]);
				break;
			default:
				cerr << "ERROR: Incorrect argument: " << optarg << endl;
				printUsage(argv[0]);
		}
	}
}

void computeProbability(){
	P = Q + 1/(double)communities;	
}

void computeN2C(vector<int> &n2c){
	int rn;
	double rd;
	
	rn = rand();
	rd = ((double)rn) / (RAND_MAX);
	
	if(rd <= P){ // All nodes in the same community
		rn = rand();
		for(int i=0; i<k; i++)
			n2c[i] = rn % communities;
		
	}
	else { // All nodes in distict communities
		for(int i=0; i<k; i++){
			bool used=false;
			do{
				used=false;
				rn = rand();
				for(int j=0; j<i && !used; j++){
					if(n2c[j] ==  rn%communities)
						used=true;
				}
			}while(used);
			n2c[i] = rn%communities;
		}
	}
}

void computeEdge(vector<int> &n2c, vector<int> &edge){
	int rn;
	for(int j=0; j<k; j++){
		// Random node in the community
		int var;
		bool repeated = false;
		do{
			repeated = false;
			rn = rand();
			var = rn % (n2c[j]*nodes/communities - (n2c[j]+1)*nodes/communities) + n2c[j]*nodes/communities + 1;
			for(int l=0; l<j && !repeated; l++){
				if(abs(edge[l]) == var){
					repeated = true;
				}
			}
		}while(repeated);
		
		edge[j] = var;
	}
}



int main(int argc, char **argv){
	
	FILE *fout;
	
	// Parse arguments
	parseArgs(argc, argv);
	
	// Compute the probability P, according to #comm and Q
	computeProbability();
	
	// Print header
	cout << "Source;Target;Type;Weight" << endl;	

	
	int rn;			// random number
	double rd;		// random double between 0 and 1
	srand (seed);
	
	// Iterate for each edges
	for(int i=0; i<edges; i++){
		
		// n2c is the community of each node of the edge
		vector<int> n2c(k,0);
		computeN2C(n2c);
		
		// Compute the edge
		vector<int> edge(k);
		computeEdge(n2c, edge);
		
		// Print the edge
		for(int j=0; j<k; j++){
			cout << edge[j] << ";";
		}
		cout << "Undirected;1" << endl;
				
	}
	
	
}