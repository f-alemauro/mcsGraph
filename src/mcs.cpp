#include <iostream>
#include <string>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/graphviz.hpp>


namespace fs = boost::filesystem;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_name_t,std::string>,boost::property<boost::edge_name_t,std::string> > Graph;
int fileId;

bool saveGraphToFile(std::string file, Graph g, int selector)
{
	std::ofstream outfile (file.c_str(),std::ofstream::binary);
	boost::dynamic_properties dp;
	dp.property("Vname", get(boost::vertex_name_t(), g));
	dp.property("Ename", get(boost::edge_name_t(), g));
	if(selector==0)
		boost::write_graphml(outfile, g, dp, true);
	else
		boost::write_graphviz(outfile, g);
	outfile.close();
	return true;

}
bool readGraphFromFile(std::string file, Graph& g, int selector)
{
	if (fs::exists(file.c_str())){
		std::ifstream inFile (file.c_str(), std::ifstream::binary);
		boost::dynamic_properties dp;
		dp.property("Vname", get(boost::vertex_name_t(), g));
		dp.property("Ename", get(boost::edge_name_t(), g));
		if(selector==0)
			boost::read_graphml(inFile, g, dp);
		else
			boost::read_graphviz(inFile, g, dp, "Vname");
		return true;
	}
	else
		return false;

}

template <typename OutputStream>
struct clique_printer
{

	boost::property_map<Graph, boost::vertex_name_t>::type v_names;
	boost::property_map<Graph, boost::edge_name_t>::type e_names;
	boost::property_map<Graph, boost::vertex_name_t>::type v_namesNew;
	boost::property_map<Graph, boost::edge_name_t>::type e_namesNew;

	clique_printer(OutputStream& stream, boost::property_map<Graph, boost::vertex_name_t>::type v_namesP, boost::property_map<Graph, boost::edge_name_t>::type e_namesP)
        : os(stream)
    {

		fileId = 0;
		v_names = v_namesP;
    	e_names = e_namesP;
    }

    template <typename Clique, typename TGraph>
    void clique(const Clique& c, const TGraph& g)
    {
    	os<<"Found new clique!"<<std::endl;
    	Graph gNew(0);
    	v_namesNew = get(boost::vertex_name, gNew);
    	e_namesNew = get(boost::edge_name, gNew);
    	typename Clique::const_iterator i, end = c.end();
    	os<<c.end()-c.begin();
    	for(i = c.begin(); i != end; ++i) {
    		Graph::vertex_descriptor v0 = boost::add_vertex(gNew);
    		v_namesNew[v0] = v_names[*i];
    	}

    	//saveGraphToFile("out"+ boost::lexical_cast<std::string>(fileId) +".xml",gNew, 0);
    	saveGraphToFile("out"+ boost::lexical_cast<std::string>(fileId) +".dot",gNew, 1);

    	os<<++fileId;
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
		if(stat){
			saveGraphToFile("testOut.dot",g, 1);
			boost::bron_kerbosch_all_cliques(g, vis);
		}
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
