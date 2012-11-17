/*
 * This file contains the main function.
 * This function calls the function for parsing input file and building graph data structures.
 * Then it calls some functions for generating statistics
 * The output file is stored in .out file
 * 
 */

#include "netlist.h"
#include <iostream>
#include <vector>
#include <list>
#include <cstring>
#include <fstream>

using namespace std;
void print(class circuit &);
void print_slack(class circuit &c);

int main(int argc, char **argv){
	class circuit c;
	list<u32> slist;		/* List containing topologically sorted gates */
	u32 max_at;				/* Maximum arrival time */
	
	if(argc != 3){
		cout<<"Error: Please specify an input and an output file.\n";
		return 1;
	}

	/* Parse the input file and generate the graph data structures */
	if(c.parse_input(argv[1]) != 0){
		cerr<<"Error while Parsing input \n";
		return 1;
	}


	/* Opening the output file for storing result */
	ofstream fout(argv[2]);
	if(!fout.is_open()){
		cerr<<"Can't open output file for writing\n";
		return -1;
	}
	
	/* Calculate the fan out for all the gates in the netlist */
	c.update_fan_out();
	
	
	/* Calculate the number of gate types that drive the same type gate, and the fanin counterpart */
	//c.update_same_faninout();
	
	/* Sort the ciruit using topological sort */ 
	if(c.topo_sort(slist) != 0){
		cout<<"Topo Sort failed "<<endl;
		fout.close();
		return(-1);
	}

	//cout<<"Topological sorting "<<endl;
	//for(list<u32>::const_iterator i = slist.begin(); i != slist.end(); i++)		cout<<*i<<endl;

	/* Update the arrival time */
	c.update_arrival_time(slist, max_at);

	
	/* Update the slack */
	c.update_slack(slist, max_at);

	fout<<max_at<<endl;			/* maximum delay over all nodes */
	
	/* Generating the index of adjacency list entries for INPUT gates */
	u32 gtype = c.get_gate_type("INPUT");
	fout<<c.gate_type[gtype]<<" ";
	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++)
		if(i->type==gtype) {
			fout<<i->id<<" ";
		}
	fout<<endl;

	/* Generating the index of adjacency list entries for OUTPUT gates */
	gtype=c.get_gate_type("OUTPUT");
	fout<<c.gate_type[gtype]<<" ";
	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++)
		if(i->type==gtype){
			fout<<i->id<<" ";
		}
	fout<<endl;



	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++)
		fout<<i->id<<" "<<i->a_time<<" "<<i->slack<<"\n";

	fout<<endl;
	
	//print(c);

	/* Clean up */
	fout.close();
	cout<<"Statistics Generated, check output file\n";
	
	return 0;
}
void print_slack(class circuit &c){
	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++)
		cout<<i->id<<" "<<i->a_time<<" "<<i->slack<<endl;

}
void print(class circuit &c){
	cout<<"Number of gates: "<<c.num_gates<<endl;
	cout<<"Number of nets: "<<c.num_nets<<endl;
	cout<<"Type of Gates\n";
	for(int i=0; i<NUM_TYPE_GATES; i++){
		cout<<c.get_gate_name(i)<<" : "<<c.gate_type[i]<<endl;
	}
#if 1
	cout<<"\nNets List\n";
	for(vector<struct net>::const_iterator i = c.net_list.begin(); i != c.net_list.end(); i++){
		cout<<"\n  "<<c.get_edge_name(i->id)<<" [ID: "<<i->id<<"] [Delay: "<<i->delay<<"] [Type: "<<i->type<<"] [Driven by: "<<i->driving_gate<<"]\n";
		cout<<"    Driving Gates ";
		for(list<u32>::const_iterator j = i->gates.begin(); j != i->gates.end(); j++)
			cout<<c.gate_list[*j].id<<" ";
		}
	cout<<"\nGate List\n";
	for(vector<struct gate>::const_iterator i = c.gate_list.begin(); i != c.gate_list.end(); i++){
		cout<<"\n\nGate:: [id: "<<i->id<<"] [Type: "<<c.get_gate_name(i->type)<<"] [fan_in: "<<i->fan_in<<"] [fan_out: "<<i->fan_out<<"]"
					<<" [a_time: "<<i->a_time<<"] [r_time: "<<i->r_time<<"] [slack: "<<i->slack<<"] [flag: "<<i->flag<<"] ";
		cout<<"\n Input Edges: ";
		for(list<u32>::const_iterator j = i->in_nets.begin(); j != i->in_nets.end(); j++)
			cout<<"  "<<c.get_edge_name(c.net_list[*j].id)<<" [ID: "<<c.net_list[*j].id<<"]";
		
		cout<<"\n Output Edges: ";
		for(list<u32>::const_iterator j = i->nets.begin(); j != i->nets.end(); j++)
			cout<<"  "<<c.get_edge_name(c.net_list[*j].id)<<" [ID: "<<c.net_list[*j].id<<"] ";
	}
	cout<<endl;
#endif
}
