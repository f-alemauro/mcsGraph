#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphml.hpp>

int main(int,char*[])
{
	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_name_t,std::string>,boost::property<boost::edge_name_t,std::string> > Graph;

	enum { A, B, C, D, E, N };
    const int num_vertices = N;
    const char* name = "ABCDE";

    // writing out the edges in the graph
    typedef std::pair<int, int> Edge;
    Edge edge_array[] =
    { Edge(A,B), Edge(A,D), Edge(C,A), Edge(D,C),
      Edge(C,E), Edge(B,D), Edge(D,E) };
    //std::string e_names[] = { "a", "b", "c", "d", "d", "e", "f", "g", "h"};
    const int num_edges = sizeof(edge_array)/sizeof(edge_array[0]);

    Graph g(edge_array, edge_array + sizeof(edge_array) / sizeof(Edge), num_vertices);
    //Graph g(edge_array, edge_array + sizeof(edge_array) / sizeof(Edge), num_vertices);


    boost::property_map<Graph, boost::vertex_name_t>::type v_name = get(boost::vertex_name, g);
    boost::property_map<Graph, boost::edge_name_t>::type e_name = get(boost::edge_name, g);

    typedef boost::graph_traits<Graph>::vertex_descriptor VertexD;
    typedef boost::graph_traits<Graph>::edge_descriptor EdgeD;
    typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
    typedef boost::graph_traits<Graph>::edge_iterator Edge_iter;
    typedef boost::graph_traits<Graph>::out_edge_iterator Out_iter;
    typedef boost::graph_traits<Graph>::adjacency_iterator Adj_iter;

    typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;

    Out_iter out_begin, out_end;
    Adj_iter adj_begin, adj_end;

    EdgeD e;
    VertexD v,adj_v;
    IndexMap index = get(boost::vertex_index, g);
    int i=0;
    std::pair<vertex_iter, vertex_iter> vp;
    for (vp = vertices(g); vp.first != vp.second; ++vp.first,++i) {
          v = *vp.first;
          v_name[v] = "prova";
          for (boost::tie(out_begin, out_end) = out_edges(v, g); out_begin != out_end; ++out_begin) {
        	  e = *out_begin;
        	  VertexD src = source(e, g);
        	  VertexD targ = target(e, g);
        	  std::cout << "(" << index[src] << "," << index[targ] << ") ";
          }

          for (boost::tie(adj_begin, adj_end) = adjacent_vertices(v, g); adj_begin != adj_end; ++adj_begin) {
        	  adj_v = *adj_begin;
        	  std::cout<<index[adj_v];
        	  std::cout<<"("<<v_name[adj_v]<<")"<<std::endl;
          }

    }
    typedef boost::property_map<Graph, boost::edge_name_t>::type Map;
    Map d = boost::get(boost::edge_name,g);
    Edge_iter edge_begin, edge_end;
    boost:tie(edge_begin, edge_end) = boost::edges(g);
    for(; edge_begin != edge_end; ++edge_begin){
    	e = *edge_begin;
    	//std::cout<<boost::get(boost::edge_weight,e)<<std::endl;

    }
    boost::dynamic_properties dp;
    dp.property("Vname", get(boost::vertex_name_t(), g));
    dp.property("Ename", get(boost::edge_name_t(), g));
    boost::write_graphml(std::cout, g, dp, true);
    return 0;
  }
