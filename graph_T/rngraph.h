// directed acyclic graph
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>
#include<queue>
#include<utility>
#include<memory>
#include<algorithm>
#include<stdexcept>
#include"HasOperatorLess.h"


#define DEBUG1
#ifndef rngraph_h
#define rngraph_h
#define NULL_PTR nullptr
#define NULL_POS static_cast<unsigned>(-1)
template<typename T1, typename T2> class rnpath;
template<typename T1, typename T2> class rnedge;
template<typename T1, typename T2> class rngraph;
template<typename Value_T, typename ID_T>
class rnedge
{
public:
	rnedge() = default;
	rnedge(const ID_T& rnid_from, const ID_T& rnid_to, const Value_T& rnweight):from(rnid_from), 
		to(rnid_to), weight(rnweight)
	{}
	rnedge(const rnedge& rncopy):rnedge(rncopy.from, rncopy.to, rncopy.weight){}
	rnedge& operator=(rnedge& rncopy)
	{
		from = rncopy.from;
		to = rncopy.to;
		weight = rncopy.weight;
		return *this;
	}
private:
	ID_T from;
	ID_T to;
	Value_T weight;
};
template<typename Value_T, typename ID_T = std::string>
class rngraph
{
	friend class rnedge<Value_T, ID_T>;
	static bool support_less_operator;
	static void initialize()
	{
		support_less_operator = HasOperatorLess<ID_T>::value;
	}
public: 
	typedef std::size_t size_type;
	rngraph() 
	{
		initialize();   // judge whether ID_T overloaded operator< or not
	}
	rngraph(std::istream& rninput_file): rngraph() // construct graph by a formatted txt file
	{
#ifndef DEBUG
int i = 0;
#endif
		while(rninput_file)
		{
#ifndef DEBUG
//std::cout<<++i<<std::endl;
#endif
			ID_T rnfrom, rnto;
			Value_T rnvalue;
			rninput_file>>rnfrom>>rnto>>rnvalue;
			if(rnfrom == "")
				continue;
#ifndef DEBUG
std::cout<<"from:"<<rnfrom<<" to:"<<rnto<<std::endl;
#endif
			PushEdge(rnfrom, rnto, rnvalue);
		}
		rninput_file.clear();
	}
	~rngraph(){}

	//add net edge
	bool PushEdge(const ID_T& rnfrom_id, const ID_T& rnto_id, const Value_T& rnval) //iterator exit
	{
		if(rnfrom_id == rnto_id)
		{
			std::cout<<"ring: "<<rnfrom_id<<std::endl;
			throw std::invalid_argument("add ring to graph is not permitted!");
		}
		try
		{
			if(IsEdge(rnfrom_id, rnto_id))
				throw std::invalid_argument("try to add repeated edge.");
		}catch(std::exception err)
		{
			std::cerr<<err.what()
				<<"\nTry to add repeated edge? Enter y or any else"<<std::endl;
			char rnc = '\0';
			std::cin>>rnc;
			if(rnc != 'y')
				return false;
		}
#ifndef DEBUG1
		std::cout<<"1"<<std::endl;
#endif
		auto rnto_pos = get_pos(rnto_id);
		if(rnto_pos == NULL_POS)
		{
			auto rnptr = std::make_shared<std::vector<std::pair<size_type, Value_T>>>();
			graph.push_back({rnto_id, rnptr});
#ifndef DEBUG1
		std::cout<<"2"<<std::endl;
#endif

			rnto_pos = graph.size() - 1;
			add_pos(rnto_id, rnto_pos);         //add to ID-to-pos list
		}

		auto rnfrom_pos = get_pos(rnfrom_id);
		if(rnfrom_pos == NULL_POS)
		{
			auto rnptr = std::make_shared<std::vector<std::pair<size_type, Value_T>>>();
			graph.push_back({rnfrom_id, rnptr});
			auto rnpos = graph.size() - 1;
			add_pos(rnfrom_id, rnpos);             //add to ID-to-pos list
			rnptr->push_back({rnto_pos, rnval});
#ifndef DEBUG1
			std::cout<<"3"<<std::endl;
#endif

		}
		else
		{
			auto rnptr = graph[rnfrom_pos].second;
			rnptr->push_back({rnto_pos, rnval});
		}
		++edges;
		return true;
	}
	template<typename...Args>
	bool PushEdge(const ID_T& rnfrom_id, const ID_T& rnto_id, const Value_T& rnval, const Args&...args) //mutable parameter function template
	{
		if((sizeof...(args) % 3))
			throw std::invalid_argument("the number of arguments in PushEdge() is wrong!");
		PushEdge(rnfrom_id, rnto_id, rnval);
		return PushEdge(args...);
	}
	bool PushEdge(const rnedge<Value_T, ID_T>& rne)
	{
		return PushEdge(rne.from, rne.to, rne.weight);
	}
	template<typename...Args>
	bool PushEdge(const rnedge<Value_T, ID_T>& rne, const Args&...args)
	{
		PushEdge(rne);
		return PushEdge(args...);
	}
	//net information
	bool IsEdge(const ID_T& rnfrom_id, const ID_T& rnto_id) const
	{
		return (static_cast<unsigned>(GetWeight(rnfrom_id, rnto_id)) != NULL_POS);
	}
	bool IsEdge(const rnedge<Value_T, ID_T>& rne) const
	{
		return IsEdge(rne.from, rne.to);
	}
	bool IsVertex(const ID_T& rnid) const
	{
		return get_pos(rnid) != NULL_POS;
	}
	size_type size() const
	{
		return graph.size();
	}
//private:
	//Value_T get_value(const Value_T& rnweight) const  //get weight from object of Value_T, 
	//{								      //getting Value_T value directly is accepted as default
	//	return rnweight;
	//}
//public:
	Value_T GetWeight(const ID_T& rnfrom_id, const ID_T& rnto_id) const //-> decltype(get_value(Value_T())) const
	{
		
		auto rnptr = get_neighbor(rnfrom_id);
		auto rnto_pos = get_pos(rnto_id);
		if(rnptr == NULL_PTR || rnto_pos == NULL_POS)
			return NULL_POS;
		auto rncompare_temp = [&rnto_pos](const std::pair<size_type, Value_T>& rnpair){return rnpair.first == rnto_pos;};
		auto rniter = std::find_if(rnptr->cbegin(), rnptr->cend(), rncompare_temp);
		if(rniter == rnptr->cend())
			return NULL_POS;
		//return get_value(rniter->second);
		return rniter->second;
	}
	rnpath<ID_T, Value_T> rnDijkstra(const ID_T&, const ID_T&) const;
	std::pair<bool, ID_T*> DFS(ID_T& rnorgin, std::function<bool(ID_T&)> rnvisit) //depth-first search  one-place predicate bool(ID_t&)
	{
		if(size() == 0)
			return {false, &(rnorgin)};
		std::vector<bool> rnrecord_vec(graph.size(), false); //visited table
		auto rnorgin_pos = get_pos(rnorgin);
		auto rnret = dfs_recursion(rnorgin_pos, rnvisit, rnrecord_vec);
		return rnret;
	}
	std::pair<bool, ID_T*> BFS(ID_T& rnorgin, std::function<bool(ID_T&)> rnvisit);  //breadth-first traverse
	//ID_T& DFS(const ID_T& rnorgin, const ID_T& rndest);                            
	//ID_T& BFS(const ID_T& rnorgin, const ID_T& rndest);
	//void push_vertex(size_type, size_type, const Value_T&); //iterator exit 
	//template<typename...Args>
	//void push_vertex(size_type, size_type, const Value_T&, const Args&...args);  //mutable parameter function template
private:
	
	std::shared_ptr<std::vector<std::pair<size_type, Value_T>>> get_neighbor(const ID_T& rnid) const
	{
		auto rnpos = get_pos(rnid);
		if(NULL_POS == rnpos)
			return NULL_PTR;
		return graph[rnpos].second;
	}
	size_type get_pos(const ID_T& rnid) const
	{
		if(support_less_operator)
		{
			auto rniter =  pos_map.find(rnid);
			if(rniter == pos_map.end())
				return NULL_POS;
			return rniter->second;
		}
		auto rncompare_temp = [&](const std::pair<ID_T, size_type>& rnele)
		{return rnele.first == rnid;};
		auto rniter = std::find_if(pos_vec.cbegin(), pos_vec.cend(), rncompare_temp);
		if(rniter == pos_vec.end())
			return NULL_POS;
		return rniter->second;
	}
	void add_pos(const ID_T& rnid, const size_type& rnpos)
	{
		if(support_less_operator)
			pos_map.insert({rnid, rnpos});
		else
			pos_vec.push_back({rnid, rnpos});
	}
	std::pair<typename std::vector<std::pair<size_type, Value_T>>::iterator, typename std::vector<std::pair<size_type, Value_T>>::iterator> get_neighbor_iter_pair(const size_type& rnpos)
	{
		return {(graph[rnpos].second)->begin(), (graph[rnpos].second)->end()};
	}
	std::pair<bool, ID_T*> dfs_recursion(const size_type& rnorgin, std::function<bool(ID_T&)> rnvisit, std::vector<bool>& rnrecord_vec);  //depth-first
	std::vector<std::pair<ID_T, std::shared_ptr<std::vector<std::pair<size_type, Value_T>>>>> graph; 
	std::vector<std::pair<ID_T, size_type>> pos_vec;
	std::map<ID_T, size_type> pos_map;
	size_type edges = 0;
	//std::shared_ptr<std::vector<std::pair<ID_T, size_type>>> id_pos_vec_ptr = std::make_shared<std::vector<std::pair<ID_T, size_type>>>();

};
template<typename Value_T, typename ID_T>
bool rngraph<Value_T, ID_T>::support_less_operator = false;
template<typename Value_T, typename ID_T>
std::pair<bool, ID_T*> rngraph<Value_T, ID_T>::dfs_recursion(const size_type& rnorgin, std::function<bool(ID_T&)> rnvisit, std::vector<bool>& rnrecord_vec) //depth-first traverse
{
	auto rnpair = get_neighbor_iter_pair(rnorgin);
	auto rnbegin = rnpair.first, rnend = rnpair.second;
	if(!(rnrecord_vec[rnorgin]))
	{
		auto rngoal = rnvisit(graph[rnorgin].first);
		rnrecord_vec[rnorgin] = true;  //visit sign
		if(rngoal)
			return {true, &(graph[rnorgin].first)};
	}
	else
		return {false, &(graph[rnorgin].first)};
		
	while(rnbegin != rnend)
	{
		auto rnret_pair = dfs_recursion((*rnbegin).first, rnvisit, rnrecord_vec);
		if(rnret_pair.first)
			return {true, (rnret_pair.second)};
		++rnbegin;
	}
	return {false, &(graph[rnorgin].first)};
}
template<typename Value_T, typename ID_T>
std::pair<bool, ID_T*> rngraph<Value_T, ID_T>::BFS(ID_T& rnorgin, std::function<bool(ID_T&)> rnvisit)
{
	if(size() == 0)
		return {false, &(rnorgin)};
	std::vector<bool> rnrecord_vec(graph.size(), false); //visited table
	std::queue<size_type> rnq;
	size_type rnorgin_pos = get_pos(rnorgin);
	rnq.push(rnorgin_pos);
	while(rnq.size() != 0)
	{
		size_type rntemp_pos = rnq.front();
		rnq.pop();
		bool rnsign = false;
		if(!(rnrecord_vec[rntemp_pos]))                       //there are repeated elements in the queue
			rnsign = rnvisit(graph[rntemp_pos].first);
		rnrecord_vec[rntemp_pos] = true;
		if(rnsign)
			return {true, &(graph[rntemp_pos].first)};
		auto rniter_pair = get_neighbor_iter_pair(rntemp_pos);
		for(auto rnbegin = rniter_pair.first; rnbegin != rniter_pair.second; ++rnbegin)
		{
			if(!(rnrecord_vec[rnbegin->first]))
				rnq.push(rnbegin->first);
		}
	}
	return {false, &(graph[rnorgin_pos].first)};
}
#endif

