//test.cpp
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<set>
#include<memory>

#include"rngraph.h"
using namespace std;
//class rnt
//{
//public:
//	rnt(int i):a(i)
//	{
//		cout<<"rnt(int)"<<endl;
//	}
//	rnt(const rnt& rncopy):rnt(rncopy.a)
//	{
//		cout<<"rnt(const rnt&)"<<endl;
//	}
//	rnt(const rnt&& rncopy):rnt(rncopy.a)
//	{
//		cout<<"rnt(const rnt&&)"<<endl;
//	}
//	rnt &operator=(rnt &rncopy)
//	{
//		a=rncopy.a;
//		cout<<"operator="<<endl;
//		return *this;
//	}
//private:
//	int a;
//	std::map<int, std::string> rnmap;
//       	//std::shared_ptr<std::map<size_t,size_t>> rnid_pos_map_ptr = std::make_shared<std::map<size_t,size_t>>(); //new std::map<int, size_t>);
//};
int main()
{
//	using size_type = std::size_t;
//	using ID_T = string;
	std::string rnfilename = "./test_graph.txt";
	std::ifstream rninfile(rnfilename);
	rngraph<int> rng0(rninfile);
	//rng0.PushEdge("1","2",1);
	//rng0.PushEdge("1","4",3);
	//rng0.PushEdge("2","3",2);
	//rng0.PushEdge("2","5",1);
	//rng0.PushEdge("3","4",4);
	//rng0.PushEdge("4","5",5);
	//rnedge<int, string> rne = ("0","1", 5);
	//rnedge<int, string> rne_copy = rne;
	auto rnv = [](std::string& rnid)->bool {cout<<rnid<<".";return false;};
	std::string rnorgin = "1";
	rng0.DFS(rnorgin, rnv);
}
