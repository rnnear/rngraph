//test.cpp
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<set>
#include<memory>

#include"rngraph.h"
using namespace std;

int main()
{
	std::string rnfilename = "./test_graph.txt";
	std::ifstream rninfile(rnfilename);
	rngraph<int> rng0(rninfile);
	//rngraph<int> rng0;
	//rng0.PushEdge("1","2",1);
	//rng0.PushEdge("1","4",3);
	//rng0.PushEdge("2","3",2);
	//rng0.PushEdge("2","5",1);
	//rng0.PushEdge("3","4",4);
	//rng0.PushEdge("4","5",5);
	//rnedge<int, string> rne = ("0","1", 5);
	//rnedge<int, string> rne_copy = rne;
	//auto rnv = [](std::string& rnid)->bool {cout<<rnid<<".";return false;};
	std::string rnorgin = "a", rndest = "b";
	auto rnresult = rng0.rnDijkstra(rnorgin, rndest);
	rnresult.Display();
}
