// directed acyclic graph
/* This is a class template of graph(DAG)
 * Condition: Weight type must reload operation< and operation+;(required by rngraph::rnDijkstra(...))
 *			  Vertex type must reload operation<<;(required by rngraph::Display(...), rngraph::DisplayU(...) and rnpath::Display(...))
 * Tips: Vertex type is set to string as default;
 *		 You'd better reload operation< of vertex type;(promote efficient)
 */
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<forward_list>
#include<map>
#include<unordered_set>
#include<queue>
#include<utility>
#include<tuple>
#include<memory>
#include<algorithm>
#include<stdexcept>
#include"HasOperatorLess.h"

#define DEBUG
#define DEBUG1

#ifndef rngraph_h
#define rngraph_h
#define rnposition_type std::size_t
#define NULL_PTR nullptr
#define NULL_POS static_cast<std::size_t>(-1)
template<typename T1, typename T2> class rnpath;
template<typename T1, typename T2> class rnedge;
template<typename T1, typename T2> class rngraph;

//auxiliary class template and function template, including 'rnedge', 'rnpath', 'rncompare', 'rncompare2', 'rncompare3'
template<typename Value_T, typename ID_T>
class rnedge
{
public:
	friend class rngraph<Value_T, ID_T>;
	rnedge() = default;
	rnedge(const ID_T& rnid_from, const ID_T& rnid_to, const Value_T& rnweight):from(rnid_from), 
		to(rnid_to), weight(rnweight){}
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
template<typename Value_T, typename ID_T>
class rnpath
{
public:
	rnpath() = default;
	rnpath(std::shared_ptr<std::forward_list<ID_T>>& rnflist_ptr, const Value_T& rnsum_w):
		path_flist_ptr(rnflist_ptr), sum_weight(rnsum_w){}
	bool IsValid() const
	{
		if(!(path_flist_ptr))
			return false;
		return true;
	}
	Value_T GetWeight() const
	{
		return sum_weight;
	}
	void Display(std::ostream &rnos = std::cout, const std::string &rnstr = "->") const
	{
		if(!IsValid())
			return;
		auto rnb = path_flist_ptr->cbegin(), rne = path_flist_ptr->cend();
		auto rntemp_iter = rnb;
		while(rnb != rne)
		{
			if(++rntemp_iter == rne)
				break;
			rnos<<*rnb<<""<<rnstr<<"";
			++rnb;
		}
		rnos<<*rnb<<std::endl;
	}
private:
	std::shared_ptr<std::forward_list<ID_T>> path_flist_ptr;
	Value_T sum_weight;
};
template<typename Value_T, typename ID_T>
struct rncompare
{
	bool operator()(const std::pair<rnposition_type, Value_T>& rnleft, const std::pair<rnposition_type, Value_T>& rnright)
	{
		return !(rngraph<Value_T, ID_T>::get_value(rnleft.second) < rngraph<Value_T, ID_T>::get_value(rnright.second));
	}
};
template<typename Value_T, typename ID_T>
struct rncompare2
{
	bool operator()(const std::tuple<rnposition_type, rnposition_type, Value_T, Value_T> rnleft, 
			const std::tuple<rnposition_type, rnposition_type, Value_T, Value_T> rnright)
	{
		return !(rngraph<Value_T, ID_T>::get_value(std::get<2>(rnleft)) < rngraph<Value_T, ID_T>::get_value(std::get<2>(rnright)));
	}
};
//function template
template<typename Value_T, typename ID_T>
bool rncompare3(const std::tuple<rnposition_type, rnposition_type, Value_T>& rnleft, 
				const std::tuple<rnposition_type, rnposition_type, Value_T>& rnright)
{
	return !(rngraph<Value_T, ID_T>::get_value(std::get<2>(rnleft)) < rngraph<Value_T, ID_T>::get_value(std::get<2>(rnright)));
}
//class template rngraph
/* info about DS
 * graph: vector(pair(ID_T, ptr->vector(pair(pos, value_T))))
 *                    from                   to
 * graph_temp: vector(pair(bool, ptr->priority_queue(pair(pos, value_T))))  in rnDijkstra
 */
template<typename Value_T, typename ID_T = std::string>
class rngraph
{
	friend class rncompare<Value_T, ID_T>;
	friend class rncompare2<Value_T, ID_T>;
	friend bool rncompare3<Value_T, ID_T>(const std::tuple<rnposition_type, rnposition_type, Value_T>&, 
				const std::tuple<rnposition_type, rnposition_type, Value_T>&);
	static bool support_less_operator;
	static void initialize()
	{
		support_less_operator = HasOperatorLess<ID_T>::value;
	}
	static Value_T get_value(const Value_T& rnweight)  //get weight from object of Value_T, 
	{												   //getting value of Value_T object directly is accepted as default
		return rnweight;                               //You can process the true value of weight, and this function will be called in the rncompare*(...) 
	}												   //and GetWeight(...)
public: 
	typedef rnposition_type size_type;
	rngraph() 
	{
		initialize();   // judge whether ID_T overloaded operator< or not
	}
	rngraph(std::istream& rninput_file): rngraph() // construct graph by a formatted txt file
	{
		while(rninput_file)
		{
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
	rngraph(std::istream& rninput_file, int undirected): rngraph() // construct graph by a formatted txt file
	{
		while(rninput_file)
		{
			ID_T rnfrom, rnto;
			Value_T rnvalue;
			rninput_file>>rnfrom>>rnto>>rnvalue;
			if(rnfrom == "")
				continue;
#ifndef DEBUG
			std::cout<<"from:"<<rnfrom<<" to:"<<rnto<<std::endl;
#endif
			PushEdgeU(rnfrom, rnto, rnvalue);
		}
		rninput_file.clear();
	}

	~rngraph() = default;
//add net edge
	bool PushEdge(const ID_T& rnfrom_id, const ID_T& rnto_id, const Value_T& rnval) //
	{
		if(rnfrom_id == rnto_id)
		{
			std::cout<<"ring: "<<rnfrom_id<<std::endl;
			throw std::runtime_error("add ring to graph is not permitted!");
		}
		try
		{
			if(IsEdge(rnfrom_id, rnto_id))
				throw std::invalid_argument("try to add repeated edge.");
		}catch(std::invalid_argument err)
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
			add_pos(rnto_id, rnto_pos);         //add to ID-postion list
		}

		auto rnfrom_pos = get_pos(rnfrom_id);
		if(rnfrom_pos == NULL_POS)
		{
			auto rnptr = std::make_shared<std::vector<std::pair<size_type, Value_T>>>();
			graph.push_back({rnfrom_id, rnptr});
			auto rnpos = graph.size() - 1;
			add_pos(rnfrom_id, rnpos);             //add to ID-postion list
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
		return PushEdge(rnfrom_id, rnto_id, rnval)&&PushEdge(args...);
	}
	bool PushEdgeU(const ID_T& rnid1, const ID_T& rnid2, const Value_T& rnval)
	{
		return (PushEdge(rnid1, rnid2, rnval))&&(PushEdge(rnid2, rnid1, rnval));
	}
	template<typename...Args>
	bool PushEdgeU(const ID_T& rnid1, const ID_T& rnid2, const Value_T& rnval, const Args&...args)
	{
		if((sizeof...(args) % 3))
			throw std::invalid_argument("the number of arguments in PushEdge() is wrong!");
		return PushEdgeU(rnid1, rnid2, rnval)&&PushEdgeU(args...);
	}
	bool PushEdge(const rnedge<Value_T, ID_T>& rne)
	{
		return PushEdge(rne.from, rne.to, rne.weight);
	}
	template<typename...Args>
	bool PushEdge(const rnedge<Value_T, ID_T>& rne, const Args&...args)
	{
		return PushEdge(rne)&&PushEdge(args...);
	}
	bool PushEdgeU(const rnedge<Value_T, ID_T>& rne)
	{
		return PushEdgeU(rne.from, rne.to, rne.weight);
	}
	template<typename...Args>
	bool PushEdgeU(const rnedge<Value_T, ID_T>& rne, const Args...args)
	{
		return PushEdgeU(rne)&&PushEdgeU(args...);	
	}
//net information
	bool IsEdge(const ID_T& rnfrom_id, const ID_T& rnto_id) const
	{
		auto rnptr = get_neighbor(rnfrom_id);
		auto rnto_pos = get_pos(rnto_id);
		if(rnptr == NULL_PTR || rnto_pos == NULL_POS)
			return false;
		auto rncompare_temp = [&rnto_pos](const std::pair<size_type, Value_T>& rnpair){return rnpair.first == rnto_pos;};
		auto rniter = std::find_if(rnptr->cbegin(), rnptr->cend(), rncompare_temp);
		if(rniter != rnptr->cend())
			return true;
		return false;
	}
	bool IsEdge(const rnedge<Value_T, ID_T>& rne) const
	{
		return IsEdge(rne.from, rne.to);
	}
	bool IsVertex(const ID_T& rnid) const
	{
		return get_pos(rnid) != NULL_POS;
	}
	bool Empty() const
	{
		return size() == 0;
	}
	size_type size() const
	{
		return graph.size();
	}
	size_type Edges() const 
	{
		return edges;
	}
	Value_T GetWeight(const ID_T& rnfrom_id, const ID_T& rnto_id) const
	{
		
		auto rnptr = get_neighbor(rnfrom_id);
		auto rnto_pos = get_pos(rnto_id);
		if(rnptr == NULL_PTR || rnto_pos == NULL_POS)
			throw std::invalid_argument("Invalid edge!!\n");
		auto rncompare_temp = [&rnto_pos](const std::pair<size_type, Value_T>& rnpair){return rnpair.first == rnto_pos;};
		auto rniter = std::find_if(rnptr->cbegin(), rnptr->cend(), rncompare_temp);
		if(rniter == rnptr->cend())
			throw std::invalid_argument("Invalid edge!!\n");
		return get_value(rniter->second);
		//return rniter->second;
	}
	Value_T GetWeight(const rnedge<Value_T, ID_T>& rne) const
	{
		return GetWeight(rne.from, rne.to);
	}
	void Display(std::ostream& rnos = std::cout) const  //display edges of directed graph
	{										//print rnwide edges every line and print same orgin vertex every line 
		std::string rnlink = "->";
		size_type rnwide = 10;
		size_type rnsize = size();
		for(size_type rnpos = 0; rnpos < rnsize; ++rnpos)
		{
			auto rnpair = get_neighbor_iter_pair(rnpos);
			auto rnbegin = rnpair.first;
			auto rnend = rnpair.second;
			size_type rncnt = rnwide - 1;
			auto rnpos_ID = get_ID(rnpos);
			if(rnbegin != rnend)
				rnos<<rnpos_ID<<rnlink<<get_ID(rnbegin->first);
			++rnbegin;
			while(rnbegin != rnend)
			{
				if(!rncnt)
				{
					rncnt = rnwide;
					rnos<<std::endl;
				}
				else
					rnos<<" ";
				rnos<<rnpos_ID<<rnlink<<get_ID(rnbegin->first);
				--rncnt;
				++rnbegin;
			}
			rnos<<std::endl;
		}
	}
	void DisplayU(std::ostream& rnos = std::cout) const  //display edges of undirected graph
	{										 //print rnwide edges every line and print same orgin vertex every line 
		std::string rnlink = "--";
		size_type rnwide = 10;
		size_type rnsize = size();
		std::unordered_set<std::string> rnmark_uset;
		for(size_type rnpos = 0; rnpos < rnsize; ++rnpos)
		{
			auto rnpair = get_neighbor_iter_pair(rnpos);
			auto rnbegin = rnpair.first;
			auto rnend = rnpair.second;
			size_type rncnt = rnwide;
			auto rnpos_ID = get_ID(rnpos);
			while(rnbegin != rnend)
			{
				std::string rnkey = std::to_string(rnpos) + "to" + std::to_string(rnbegin->first);
				if(rnmark_uset.find(rnkey) == rnmark_uset.end())
				{
					rnos<<rnpos_ID<<rnlink<<get_ID(rnbegin->first);
					--rncnt;
					rnmark_uset.insert(rnkey);
					std::string rnkey2 = std::to_string(rnbegin->first) + "to" + std::to_string(rnpos); 
					rnmark_uset.insert(rnkey2);
					++rnbegin;
					break;
				}
				++rnbegin;
			}
			while(rnbegin != rnend)
			{
			
				std::string rnkey = std::to_string(rnpos) + "to" + std::to_string(rnbegin->first);
				if(rnmark_uset.find(rnkey) == rnmark_uset.end())
				{
					if(!rncnt)
					{
						rncnt = rnwide;
						rnos<<std::endl;
					}
					else 
					{
						rnos<<" ";
					}
					rnos<<rnpos_ID<<rnlink<<get_ID(rnbegin->first);
					--rncnt;
					rnmark_uset.insert(rnkey);
					std::string rnkey2 = std::to_string(rnbegin->first) + "to" + std::to_string(rnpos); 
					rnmark_uset.insert(rnkey2);
				}
				++rnbegin;
			}
			rnos<<std::endl;
		}
	}


//important methods
	rnpath<Value_T, ID_T> rnDijkstra(const ID_T&, const ID_T&) const; // Shortest path algorithem 
	std::pair<rngraph<Value_T, ID_T>, Value_T> rnPrim(const ID_T&) const; // minimal spanning tree
	std::pair<rngraph<Value_T, ID_T>, Value_T> rnPrim() const;
	std::pair<bool, ID_T*> DFS(ID_T& rnorgin, std::function<bool(ID_T&)> rnvisit) //depth-first search  one-place predicate bool(ID_t&)
	{
		if(Empty())
			return {false, &(rnorgin)};
		std::vector<bool> rnrecord_vec(graph.size(), false); //visited table
		auto rnorgin_pos = get_pos(rnorgin);
		auto rnret = dfs_recursion(rnorgin_pos, rnvisit, rnrecord_vec);
		return rnret;
	}
	bool DFS(const ID_T& rnorgin, std::function<bool(const ID_T&)> rnvisit) const                           
	{
		ID_T rnorgin_temp = rnorgin;
		std::function<bool(ID_T&)> rnvisit_temp = rnvisit; //for function template converting type1(const type2) to type1(type2) is valid
		auto rnret = DFS(rnorgin_temp, rnvisit_temp);
		return rnret.first;
	}
	std::pair<bool, ID_T*> BFS(ID_T& rnorgin, std::function<bool(ID_T&)> rnvisit);  //breadth-first traverse
	bool BFS(const ID_T& rnorgin, std::function<bool(const ID_T&)> rnvisit) const
	{
		ID_T rnorgin_temp = rnorgin;
		std::function<bool(ID_T&)> rnvisit_temp = rnvisit;
		auto rnret = BFS(rnorgin_temp, rnvisit_temp);
		return rnret.first;
	}
	//ID_T& BFS(const ID_T& rnorgin, const ID_T& rndest);
	//void push_vertex(size_type, size_type, const Value_T&); //iterator exit 
	//template<typename...Args>
	//void push_vertex(size_type, size_type, const Value_T&, const Args&...args);  //mutable parameter function template
private:
	std::shared_ptr<std::vector<std::vector<std::pair<Value_T, size_type>>>> adj_lst2adj_mat() const //convert data structure 
	{																								 //from adjacent list to adjacent matrix
		std::vector<std::pair<Value_T, size_type>> rnrow(size(), {Value_T(), 0});
		auto rnret_ptr = std::make_shared<std::vector<std::vector<std::pair<Value_T, size_type>>>>(size(), rnrow);
		for(size_type rnrow_n = 0; rnrow_n < size(); ++rnrow_n)
		{
			auto rnpair = get_neighbor_iter_pair(rnrow_n);
			auto rnbegin = rnpair.first;
			auto rnend = rnpair.second;
			while(rnbegin != rnend)
			{
				((*rnret_ptr)[rnrow_n][rnbegin->first]).first = rnbegin->second;
				((*rnret_ptr)[rnrow_n][rnbegin->first]).second += 1;
				++rnbegin;
			}
		}
		return rnret_ptr;
	}
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
	ID_T get_ID(const size_type& rnpos) const
	{
		return graph[rnpos].first;
	}
	void add_pos(const ID_T& rnid, const size_type& rnpos)
	{
		if(support_less_operator)
			pos_map.insert({rnid, rnpos});
		else
			pos_vec.push_back({rnid, rnpos});
	}
	std::pair<typename std::vector<std::pair<size_type, Value_T>>::iterator, 
		typename std::vector<std::pair<size_type, Value_T>>::iterator> get_neighbor_iter_pair(const size_type& rnpos)
	{
		return {(graph[rnpos].second)->begin(), (graph[rnpos].second)->end()};
	}

	std::pair<typename std::vector<std::pair<size_type, Value_T>>::const_iterator,
		typename std::vector<std::pair<size_type, Value_T>>::const_iterator> get_neighbor_iter_pair(const size_type& rnpos) const
	{
		return {(graph[rnpos].second)->cbegin(), (graph[rnpos].second)->cend()};
	}

	std::pair<bool, ID_T*> dfs_recursion(const size_type& rnorgin, std::function<bool(ID_T&)> rnvisit, std::vector<bool>& rnrecord_vec);  //depth-first
//data member
	std::vector<std::pair<ID_T, std::shared_ptr<std::vector<std::pair<size_type, Value_T>>>>> graph; 
	std::vector<std::pair<ID_T, size_type>> pos_vec;
	std::map<ID_T, size_type> pos_map;
	size_type edges = 0;
};

/// static member initialization
template<typename Value_T, typename ID_T>
bool rngraph<Value_T, ID_T>::support_less_operator = false;  //initialize the signal

///define member function
template<typename Value_T, typename ID_T>
std::pair<bool, ID_T*> rngraph<Value_T, ID_T>::dfs_recursion
		(const size_type& rnorgin, std::function<bool(ID_T&)> rnvisit, std::vector<bool>& rnrecord_vec) //depth-first traverse
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
std::pair<bool, ID_T*> rngraph<Value_T, ID_T>::BFS
			(ID_T& rnorgin, std::function<bool(ID_T&)> rnvisit)
{
	if(Empty())
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
template<typename Value_T, typename ID_T>
rnpath<Value_T, ID_T> rngraph<Value_T, ID_T>::rnDijkstra(const ID_T& rnorgin, const ID_T& rndest) const
{
	auto rnorgin_pos = get_pos(rnorgin);
	auto rndest_pos = get_pos(rndest);
	if(rnorgin_pos == NULL_POS||rndest_pos == NULL_POS)
		throw std::runtime_error("Input wrong arguments!!");
	if(rnorgin == rndest)
	{
		if(IsEdge(rnorgin, rndest))
		{
			auto rnptr = std::make_shared<std::forward_list<ID_T>>();
			rnptr->insert_after(rnptr->before_begin(), rndest);
			rnptr->insert_after(rnptr->before_begin(), rnorgin);
			return rnpath<Value_T, ID_T>(rnptr, GetWeight(rnorgin, rndest));
		}
		else
			return rnpath<Value_T, ID_T>();
	}

	typedef std::pair<size_type, Value_T> P_ELEMENT_T;
	typedef std::tuple<size_type, size_type, Value_T, Value_T> P_ELEMENT_T2;
	typedef std::vector<P_ELEMENT_T> P_ELEMENT_CONTAINER_T;
	typedef std::vector<P_ELEMENT_T2> P_ELEMENT_CONTAINER_T2;
	typedef std::priority_queue<P_ELEMENT_T, P_ELEMENT_CONTAINER_T, rncompare<Value_T, ID_T>> P_T;
	typedef std::priority_queue<P_ELEMENT_T2, P_ELEMENT_CONTAINER_T2, rncompare2<Value_T, ID_T>> P_T2;
//rebuild
	std::vector<std::pair<bool, std::shared_ptr<P_T>>> rngraph_temp;   //bool means the node has been marked or not
	rngraph_temp.reserve(graph.size());
	for(const auto &rnindex: graph)                                    //use priority_queue to rebuild graph data container
	{
		std::shared_ptr<P_T> rnptr = std::make_shared<P_T>();
		rngraph_temp.push_back({false, rnptr});                   //initialize unmarked
		auto rnbegin = rnindex.second->begin();
		auto rnend = rnindex.second->end();
		for(; rnbegin != rnend; ++rnbegin)
			rnptr->push(*rnbegin);
	}
//
	
	std::map<size_type, std::pair<size_type, Value_T>> rnmark_map; // map(this ID, pair<last ID, to_this_weight>)-> marked node container
	P_T2 rnpq;					    // priority_queue(tuple<this node position, last node position, to_this_weight, to_last_weight>)->candidate queue
	rnpq.push(std::make_tuple(rnorgin_pos, rnorgin_pos, Value_T(), Value_T()));     //push orgin node into candidate queue
	//Value_T rntotal_weight = Value_T();
	bool rncomplete = false;
	while(rnpq.size() != 0)
	{
		P_ELEMENT_T2 rnmarking_node_tuple = rnpq.top();
		auto rnthis_pos = std::get<0>(rnmarking_node_tuple);
	    auto rnlast_pos = std::get<1>(rnmarking_node_tuple);
		auto rnto_this_weight = std::get<2>(rnmarking_node_tuple);
		auto rnto_last_weight = std::get<3>(rnmarking_node_tuple);
		rnpq.pop();
		
		if(rngraph_temp[rnthis_pos].first == false)
		{
			rnmark_map.insert({rnthis_pos, {rnlast_pos, rnto_this_weight}});  //mark this node
			rngraph_temp[rnthis_pos].first = true;        //record sign
		}

		if(rnthis_pos == rndest_pos)
		{
			//rntotal_weight = rnto_this_weight;
			rncomplete = true;
			break;
		}
//last candidate
		while(rngraph_temp[rnlast_pos].second->size() != 0)
		{
			auto rnlast_candidate_pair = rngraph_temp[rnlast_pos].second->top();
			rngraph_temp[rnlast_pos].second->pop();
			if(rngraph_temp[rnlast_candidate_pair.first].first == false)
			{
				rnpq.push(std::make_tuple(rnlast_candidate_pair.first, rnlast_pos, rnlast_candidate_pair.second + rnto_last_weight, rnto_last_weight));
				break;                                                                         // Value_T must overload operator+
			}
		}
//
//this candidate
		while(rngraph_temp[rnthis_pos].second->size() != 0)
		{
			auto rnthis_candidate_pair = rngraph_temp[rnthis_pos].second->top();
			rngraph_temp[rnthis_pos].second->pop();
			if(rngraph_temp[rnthis_candidate_pair.first].first == false)
			{
				rnpq.push(std::make_tuple(rnthis_candidate_pair.first, rnthis_pos, rnthis_candidate_pair.second + rnto_this_weight, rnto_this_weight));
				break;
			}
		}
//
//to mark more node before finding the dest
		if(rnpq.size() == 0)
		{
			for(const auto &rnindex: rnmark_map)
			{
				while(rngraph_temp[rnindex.first].second->size() != 0)
				{
					auto rncandidate = rngraph_temp[rnindex.first].second->top();
					rngraph_temp[rnindex.first].second->pop();
					if(rngraph_temp[rncandidate.first].first == false)
					{
						rnpq.push(std::make_tuple(rncandidate.first, rnindex.first, rncandidate.second + rnindex.second.second, rnindex.second.second));
						break;
					}
				}
			}
		}
//
	}
	if(!rncomplete)
		return rnpath<Value_T, ID_T>();
	//find path
	auto rnpath_fl_p = std::make_shared<std::forward_list<ID_T>>();
	auto rntotal_weight = rnmark_map[rndest_pos].second;
	auto rnnow_pos = rndest_pos;
	rnpath_fl_p->insert_after(rnpath_fl_p->before_begin(), graph[rnnow_pos].first);
	while(rnnow_pos != rnorgin_pos)
	{
		rnnow_pos = rnmark_map[rnnow_pos].first;
		rnpath_fl_p->insert_after(rnpath_fl_p->before_begin(), graph[rnnow_pos].first);
	}
	rnpath<Value_T, ID_T> rnpath_ret(rnpath_fl_p, rntotal_weight);
	return rnpath_ret;
}
template<typename Value_T, typename ID_T>
std::pair<rngraph<Value_T, ID_T>, Value_T> rngraph<Value_T, ID_T>::rnPrim(const ID_T& rnorgin) const  // minimal spanning tree
{
	auto rnorgin_pos = get_pos(rnorgin);
	if(Empty()||rnorgin_pos == NULL_POS)
		throw std::runtime_error("Empty graph or Wrong vertex ID!");
	std::priority_queue<std::tuple<rnposition_type, rnposition_type, Value_T>, std::vector<std::tuple<rnposition_type, rnposition_type, Value_T>>,
								decltype(rncompare3<Value_T, ID_T>)*> rnpq(rncompare3<Value_T, ID_T>);
	std::vector<bool> rnmark_vec(size(), false);    //mark set
	auto rnpair = get_neighbor_iter_pair(rnorgin_pos);
	auto rnbegin_orgin = rnpair.first;
	auto rnend_orgin = rnpair.second;
	rnmark_vec[rnorgin_pos] = true;
	while(rnbegin_orgin != rnend_orgin)
	{
		rnpq.push(std::make_tuple(rnorgin_pos, rnbegin_orgin->first, rnbegin_orgin->second));
		++rnbegin_orgin;
	}
	size_type rncnt = 0;  // the number of edges in the mst
	auto rnsize = size(); // the number of vertex in the graph
	std::pair<rngraph<Value_T, ID_T>, Value_T> rnret_pair;    // <mst, weight_sum>
	while((rncnt != rnsize - 1)&&rnpq.size())
	{
		auto rnnext_tuple = rnpq.top();
		rnpq.pop();
		if(rnmark_vec[std::get<1>(rnnext_tuple)])
			continue;
		rnret_pair.first.PushEdgeU(get_ID(std::get<0>(rnnext_tuple)), get_ID(std::get<1>(rnnext_tuple)),
				std::get<2>(rnnext_tuple)); //add edge into mst
		rnret_pair.second += std::get<2>(rnnext_tuple);    // record the weight;
		rnmark_vec[std::get<1>(rnnext_tuple)] = true;      // mark the vertex
		++rncnt;

		auto rnpair = get_neighbor_iter_pair(std::get<1>(rnnext_tuple));
		auto rnbegin = rnpair.first;
		auto rnend = rnpair.second;
		while(rnbegin != rnend)                           // add neighbor edges into priority queue
		{
			if(rnmark_vec[rnbegin->first])
			{
				++rnbegin;
				continue;
			}
			rnpq.push(std::make_tuple(std::get<1>(rnnext_tuple), rnbegin->first, rnbegin->second));
			++rnbegin;
		}
	}
	return std::move(rnret_pair);
}
template<typename Value_T, typename ID_T>
std::pair<rngraph<Value_T, ID_T>, Value_T> rngraph<Value_T, ID_T>::rnPrim() const
{
	if(Empty())
		return std::pair<rngraph<Value_T, ID_T>, Value_T>();
	return rnPrim(get_ID(0));
}

///
#endif

