#include <iostream>
//#include <string>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/copy.hpp>
#include <boost/algorithm/string.hpp>

namespace fs = boost::filesystem;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_name_t,std::string>,boost::property<boost::edge_name_t,std::string> > Graph;

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

bool lineGraph(Graph g1, Graph& out){
	boost::graph_traits<Graph>::edge_iterator e1,e1_end;
	boost::property_map<Graph, boost::edge_name_t>::type eName = boost::get(boost::edge_name_t(), g1);
	boost::property_map<Graph, boost::edge_name_t>::type eNameOut = boost::get(boost::edge_name_t(), out);
	boost::property_map<Graph, boost::vertex_name_t>::type vName = get(boost::vertex_name_t(), g1);
	boost::property_map<Graph, boost::vertex_name_t>::type vNameOut = get(boost::vertex_name_t(), out);
	boost::graph_traits<Graph>::vertex_iterator v1,v2,v1_end,v2_end;
    boost::graph_traits<Graph>::vertex_descriptor s1,t1,s2,t2,v;
    std::map<std::string, boost::graph_traits<Graph>::edge_descriptor> vertexMap;
    for (boost::tie(e1, e1_end) = boost::edges(g1); e1 != e1_end; ++e1)
	{
		v =boost::add_vertex(out);
		std::string name = eName[*e1];
		vNameOut[v] = name;
		vertexMap[name]= *e1;
	}

	for (boost::tie(v1, v1_end) = boost::vertices(out); v1 != v1_end; ++v1){
		for (v2= v1+1; v2 != v1_end; ++v2){
			s1 = boost::source(vertexMap[vNameOut[*v2]],out);
			t1 = boost::target(vertexMap[vNameOut[*v2]],out);
			s2 = boost::source(vertexMap[vNameOut[*v1]],out);
			t2 = boost::target(vertexMap[vNameOut[*v1]],out);
			if (s1 == s2)
				eNameOut[boost::add_edge(*v1,*v2,out).first] = vName(s1);
			else if( s1 == t2)
				eNameOut[boost::add_edge(*v1,*v2,out).first] = vName(s1);
			else if( t1==s2 )
				eNameOut[boost::add_edge(*v1,*v2,out).first] = vName(t1);
			else if (t1==t2)
				eNameOut[boost::add_edge(*v1,*v2,out).first] = vName(t1);

		}
	}
		return true;
}

int main(int argc, char* argv[])
{
	Graph g1,lg1;
	readGraphFromFile(argv[1],g1, 1);
	if(!lineGraph(g1,lg1)){
		std::cout <<"Error, stopping program!"<<std::endl;
		return -1;
	}
	saveGraphToFile(std::string("lg_")+argv[1],lg1,1);
	std::cout <<"Line graph created and saved to disk!"<<std::endl;
	return 0;
}
