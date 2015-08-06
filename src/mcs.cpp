#include <iostream>
#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/copy.hpp>

namespace fs = boost::filesystem;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_name_t,std::string>,boost::property<boost::edge_name_t,std::string> > Graph;
int fileId;

bool saveGraphToFile(std::string file, Graph g, int selector)
{
	std::ofstream outfile (file.c_str(),std::ofstream::binary);

	boost::dynamic_properties dp;
	dp.property("label", get(boost::vertex_name_t(), g));
	dp.property("node_id", get(boost::vertex_index_t(), g));
	dp.property("label", get(boost::edge_name_t(), g));

	if(selector==0)
		boost::write_graphml(outfile, g, dp, true);
	else
		boost::write_graphviz_dp(outfile, g, dp, "node_id");
	outfile.close();
	return true;

}
bool readGraphFromFile(std::string file, Graph& g, int selector)
{
	if (fs::exists(file.c_str())){
		std::ifstream inFile (file.c_str(), std::ifstream::binary);

		boost::dynamic_properties dp;
		dp.property("label", get(boost::vertex_name_t(), g));
		dp.property("node_id", get(boost::vertex_index_t(), g));
		dp.property("label", get(boost::edge_name_t(), g));

		if(selector==0)
			boost::read_graphml(inFile, g, dp);
		else
			boost::read_graphviz(inFile, g, dp, "label");
		return true;
	}
	else
		return false;

}

template <typename Clique>
struct vertexFilter
{
	vertexFilter() { }
	vertexFilter(Clique c) { thisC = c;}
	bool operator()(const typename boost::graph_traits<Graph>::vertex_descriptor& v) const
	{
		bool inClique = false;
		typename Clique::const_iterator i, end = thisC.end();
		for(i = thisC.begin(); i != end; ++i) {
			if (v==*i)
				inClique = true;
		}
		return inClique;

	}
	Clique thisC;
};

template <typename Clique>
struct edgeFilter
{
	edgeFilter() { }
	edgeFilter(Clique c, Graph g) { thisC = c; thisG = g;}
	bool operator()(const typename boost::graph_traits<Graph>::edge_descriptor& e) const
	{

		int inClique = 0;
		VertexDescriptor u,v;
		u = boost::source(e,thisG);
		v = boost::target(e,thisG);

		typename Clique::const_iterator i, end = thisC.end();
		for(i = thisC.begin(); i != end; ++i) {
			if (u==*i)
				inClique++;
			else if (v==*i)
				inClique++;
		}

		bool filter = inClique ==2;
		return filter;

	}
	typedef boost::graph_traits<Graph>::vertex_descriptor VertexDescriptor;
	Clique thisC;
	Graph thisG;
};

template <typename OutputStream>
struct clique_printer
{
	boost::property_map<Graph, boost::vertex_name_t>::type v_names;
	boost::property_map<Graph, boost::edge_name_t>::type e_names;
	boost::property_map<Graph, boost::vertex_name_t>::type v_namesNew;
	boost::property_map<Graph, boost::edge_name_t>::type e_namesNew;
	int maxDim;
	Graph maxClique;

	clique_printer(OutputStream& stream, boost::property_map<Graph, boost::vertex_name_t>::type v_namesP, boost::property_map<Graph, boost::edge_name_t>::type e_namesP)
        : os(stream)
    {
		maxDim = 0;
		fileId = 0;
		v_names = v_namesP;
    	e_names = e_namesP;
    }

    template <typename Clique, typename Graph>
    void clique(const Clique& c, const Graph& g)
    {
    	vertexFilter<Clique> filterV(c);
    	edgeFilter<Clique> filterE(c, g);

    	typedef boost::filtered_graph<Graph, edgeFilter<Clique>, vertexFilter<Clique> > FilteredGraphType;
        FilteredGraphType filteredGraph(g, filterE, filterV); // (graph, EdgePredicate, VertexPredicate)
        Graph temp;
        boost::copy_graph(filteredGraph, temp);
        int dim = boost::num_vertices(temp);
        if(dim>maxDim){
        	maxClique = temp;
        }
        saveGraphToFile("maxClique.dot", maxClique,1);
    }
    OutputStream& os;
};

int main(int argc, char* argv[])
{
	size_t cliques;
	Graph g;
	bool stat = false;
	boost::property_map<Graph, boost::vertex_name_t>::type v_name = get(boost::vertex_name, g);
	boost::property_map<Graph, boost::edge_name_t>::type e_name = get(boost::edge_name, g);
	clique_printer<std::ostream> vis(std::cout, v_name, e_name);
	if(argc==2){
		if(fs::extension(argv[1])==".dot")
			stat = readGraphFromFile(argv[1],g, 1);
		else if(fs::extension(argv[1])==".xml")
			stat = readGraphFromFile(argv[1],g, 0);
		else{
			std::cout<<"File type not supported!"<<std::endl;
			return -1;
		}
		if(stat)
			boost::bron_kerbosch_all_cliques(g, vis);
		else{
			std::cout<<"File does not exists!"<<std::endl;
			return -1;
		}
	}
	else
	{
		std::cout<<"Missing path to graph file!"<<std::endl;
		return -1;
	}

//	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_name_t,std::string>,boost::property<boost::edge_name_t,std::string> > Graph;
//
//	enum { A, B, C, D, E, N };
//    const int num_vertices = N;
//    const char* name = "ABCDE";
//
//    // writing out the edges in the graph
//    typedef std::pair<int, int> Edge;
//    Edge edge_array[] =
//    { Edge(A,B), Edge(A,D), Edge(C,A), Edge(D,C),
//      Edge(C,E), Edge(B,D), Edge(D,E) };
//    //std::string e_names[] = { "a", "b", "c", "d", "d", "e", "f", "g", "h"};
//    const int num_edges = sizeof(edge_array)/sizeof(edge_array[0]);
//
//    Graph g(edge_array, edge_array + sizeof(edge_array) / sizeof(Edge), num_vertices);
//    //Graph g(edge_array, edge_array + sizeof(edge_array) / sizeof(Edge), num_vertices);
//
//
//    boost::property_map<Graph, boost::vertex_name_t>::type v_name = get(boost::vertex_name, g);
//    boost::property_map<Graph, boost::edge_name_t>::type e_name = get(boost::edge_name, g);
//
//    typedef boost::graph_traits<Graph>::vertex_descriptor VertexD;
//    typedef boost::graph_traits<Graph>::edge_descriptor EdgeD;
//    typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
//    typedef boost::graph_traits<Graph>::edge_iterator Edge_iter;
//    typedef boost::graph_traits<Graph>::out_edge_iterator Out_iter;
//    typedef boost::graph_traits<Graph>::adjacency_iterator Adj_iter;
//
//    typedef boost::property_map<Graph, boost::vertex_index_t>::type IndexMap;
//
//    Out_iter out_begin, out_end;
//    Adj_iter adj_begin, adj_end;
//
//    EdgeD e;
//    VertexD v,adj_v;
//    IndexMap index = get(boost::vertex_index, g);
//    int i=0;
//    std::pair<vertex_iter, vertex_iter> vp;
//    for (vp = vertices(g); vp.first != vp.second; ++vp.first,++i) {
//          v = *vp.first;
//          v_name[v] = "prova";
//          for (boost::tie(out_begin, out_end) = out_edges(v, g); out_begin != out_end; ++out_begin) {
//        	  e = *out_begin;
//        	  VertexD src = source(e, g);
//        	  VertexD targ = target(e, g);
//        	  std::cout << "(" << index[src] << "," << index[targ] << ") ";
//          }
//
//          for (boost::tie(adj_begin, adj_end) = adjacent_vertices(v, g); adj_begin != adj_end; ++adj_begin) {
//        	  adj_v = *adj_begin;
//        	  std::cout<<index[adj_v];
//        	  std::cout<<"("<<v_name[adj_v]<<")"<<std::endl;
//          }
//
//    }
//    typedef boost::property_map<Graph, boost::edge_name_t>::type Map;
//    Map d = boost::get(boost::edge_name,g);
//    Edge_iter edge_begin, edge_end;
//    boost:tie(edge_begin, edge_end) = boost::edges(g);
//    for(; edge_begin != edge_end; ++edge_begin){
//    	e = *edge_begin;
//    	//std::cout<<boost::get(boost::edge_weight,e)<<std::endl;
//
//    }
//
//    std::ofstream outfile ("test.gml",std::ofstream::binary);
//
//    boost::dynamic_properties dp;
//    dp.property("Vname", get(boost::vertex_name_t(), g));
//    dp.property("Ename", get(boost::edge_name_t(), g));
//    boost::write_graphml(outfile, g, dp, true);
//    outfile.close();
//    return 0;
  }
