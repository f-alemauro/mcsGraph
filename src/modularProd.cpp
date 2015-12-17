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

bool createVertexSet(Graph g1, Graph g2, Graph& out, std::map<std::string, std::pair<boost::graph_traits<Graph>::vertex_descriptor, boost::graph_traits<Graph>::vertex_descriptor> >& vertexMap){
	boost::graph_traits<Graph>::vertex_descriptor vOut;
	boost::property_map<Graph, boost::vertex_name_t>::type vNameOut = get(boost::vertex_name_t(), out);
	boost::property_map<Graph, boost::vertex_name_t>::type vName1 = get(boost::vertex_name_t(), g1);
	boost::property_map<Graph, boost::vertex_name_t>::type vName2 = get(boost::vertex_name_t(), g2);
	boost::graph_traits<Graph>::vertex_iterator v1, v1_end, v2, v2_end;
	std::pair<boost::graph_traits<Graph>::vertex_descriptor, boost::graph_traits<Graph>::vertex_descriptor> temp;

	for (boost::tie(v1, v1_end) = boost::vertices(g1); v1 != v1_end; ++v1){
		for (boost::tie(v2, v2_end) = boost::vertices(g2); v2 != v2_end; ++v2){
			vOut = boost::add_vertex(out);
			std::string name =vName1[*v1]+"."+vName2[*v2];
			vNameOut[vOut] = name;
			temp.first =*v1;
			temp.second = *v2;
			vertexMap[name]=temp;
		}
	}
	return true;
}

bool createEdgeSet(Graph g1, Graph g2, Graph& out, std::map<std::string, std::pair<boost::graph_traits<Graph>::vertex_descriptor, boost::graph_traits<Graph>::vertex_descriptor> >& vertexMap){
	boost::graph_traits<Graph>::vertex_iterator v1, v1_end, v2;
	boost::graph_traits<Graph>::vertex_descriptor u1,u2,u1p,u2p;
	boost::property_map<Graph, boost::vertex_name_t>::type vNameOut = get(boost::vertex_name_t(), out);
	std::pair<boost::graph_traits<Graph>::vertex_descriptor, boost::graph_traits<Graph>::vertex_descriptor> t1,t2;
	boost::graph_traits<Graph>::adjacency_iterator i,i_end;
	for (boost::tie(v1, v1_end) = boost::vertices(out); v1 != v1_end; ++v1){
		for (v2=v1+1; v2 != v1_end; ++v2){
			t1 =vertexMap[vNameOut[*v1]];
			t2 =vertexMap[vNameOut[*v2]];
			u1=t1.first;
			u2=t1.second;
			u1p=t2.first;
			u2p=t2.second;
			bool flagAdj1= false;
			bool flagAdj2= false;

			//check if u1 is adjacent to u1p
			for (boost::tie(i, i_end) = boost::adjacent_vertices(u1, g1); i != i_end && !flagAdj1; ++i){
				if (*i == u1p)
					flagAdj1 = true;
			}
			//check if u2 is not adjacent to u2p
			for (boost::tie(i, i_end) = boost::adjacent_vertices(u2, g2); i != i_end && !flagAdj2; ++i){
				if (*i == u2p)
					flagAdj2 = true;
			}

			if (!(flagAdj1^flagAdj2))
				boost::add_edge(*v1,*v2,out);
		}
	}
	return true;
}

bool modProductGraph(Graph g1, Graph g2, Graph& out){
	std::map<std::string, std::pair<boost::graph_traits<Graph>::vertex_descriptor, boost::graph_traits<Graph>::vertex_descriptor> > vertexMap;
	if (!createVertexSet(g1,g2,out,vertexMap)){
		std::cout <<"Error in adding vertices to the product graph!"<<std::endl;
		return false;
	}
	createEdgeSet(g1,g2,out,vertexMap);
	return true;
}

int main(int argc, char* argv[])
{
	Graph g1,g2,gProd;
	readGraphFromFile(argv[1],g1, 1);
	readGraphFromFile(argv[2],g2, 1);
	if(!modProductGraph(g1,g2,gProd)){
		std::cout <<"Error, stopping program!"<<std::endl;
		return -1;
	}
	saveGraphToFile("modularProd.dot",gProd,1);
	std::cout<<"Modular product computed and saved to disk!"<<std::endl;
	return 0;
}
