#include<iostream>
#include<functional>
#include<string>
#include<map>
#include<set>
#include<memory>
///////#include"rngraph.h"

//using namespace std;
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
	//std::string rnfilename = "./test_graph.txt";
	//std::ifstream rninfile(rnfilename);
	//rngraph<int> rng1(rninfile);
	//rninfile.close();
/*
	rngraph<int> rng0;
	rng0.PushEdge("1","2",1);
	rng0.PushEdge("1","4",3);
	rng0.PushEdge("2","3",2);
	rng0.PushEdge("2","5",1);
	rng0.PushEdge("3","4",4);
	rng0.PushEdge("4","5",5);

	auto rnv = [](std::string& rnid)->bool {std::cout<<rnid<<".";return false;};
	std::string rnorgin = "1";
	rng1.DFS(rnorgin, rnv);
	std::cout<<std::endl;
	rng1.BFS(rnorgin, rnv);
	std::cout<<std::endl;
*/
	auto rnv1 = [](const int& rni)->bool {return true;};
	auto rnv2 = [](int& rni)->bool {return true;};
	std::function<bool(const int&)> rnfun1 = rnv1;
	std::function<bool(int&)> rnfun2 = rnv2;    
	rnfun2 = rnfun1;

}
