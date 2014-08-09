/*
 * SimmelianBackboneGTest.cpp
 *
 *  Created on: 31.05.2014
 *      Author: Gerd Lindner
 */

#ifndef NOGTEST

#include "SimmelianBackboneGTest.h"

#include "../../backbones/Backbones.h"
#include "../../backbones/SimmelianJaccardAttributizer.h"
#include "../../backbones/ChibaNishizekiTriangleCounter.h"
#include "../../graph/GraphGenerator.h"

namespace NetworKit {

TEST_F(SimmelianBackboneGTest, testOverlapCounting) {
	//Build up a ranked neighborhood graph. Notation: Ego/Alter/Simmeliannes/Rank
	std::vector<RankedNeighbors> neighbors(2);
	neighbors[0].push_back(RankedEdge(0,1,3,1));
	neighbors[0].push_back(RankedEdge(0,2,2,2));
	neighbors[0].push_back(RankedEdge(0,5,2,2));
	neighbors[0].push_back(RankedEdge(0,3,1,4));
	neighbors[0].push_back(RankedEdge(0,6,1,4));

	neighbors[1].push_back(RankedEdge(1,0,3,1));
	neighbors[1].push_back(RankedEdge(1,2,2,2));
	neighbors[1].push_back(RankedEdge(1,4,2,2));
	neighbors[1].push_back(RankedEdge(1,3,1,4));

	SimmelianJaccardAttributizer simmel;
	Redundancy r (0, 0.0);

	r = simmel.getOverlap(0, 1, neighbors, 1);
	EXPECT_EQ(1, r.overlap) << "wrong overlap";
	EXPECT_DOUBLE_EQ((1.0/1.0), r.jaccard) << "wrong jaccard index";

	r = simmel.getOverlap(1, 0, neighbors, 1);
	EXPECT_EQ(1, r.overlap) << "wrong overlap";
	EXPECT_DOUBLE_EQ((1.0/1.0), r.jaccard) << "wrong jaccard index";

	r = simmel.getOverlap(0, 1, neighbors, 2);
	EXPECT_EQ(2, r.overlap) << "wrong overlap";
	EXPECT_DOUBLE_EQ((1.0/1.0), r.jaccard) << "wrong jaccard index";

	r = simmel.getOverlap(0, 1, neighbors, 3);
	EXPECT_EQ(2, r.overlap) << "wrong overlap";
	EXPECT_DOUBLE_EQ((1.0/1.0), r.jaccard) << "wrong jaccard index";

	r = simmel.getOverlap(0, 1, neighbors, 4);
	EXPECT_EQ(3, r.overlap) << "wrong overlap";
	EXPECT_DOUBLE_EQ((1.0/1.0), r.jaccard) << "wrong jaccard index";
}

TEST_F(SimmelianBackboneGTest, testRankedNeighborhood) {
	Graph g(10);

	g.addEdge(4,5);
	g.addEdge(4,6);
	g.addEdge(5,6);

	g.addEdge(4,8);
	g.addEdge(4,7);
	g.addEdge(7,8);

	g.addEdge(4,9);
	g.addEdge(8,9);
	g.indexEdges();

	//Apply triangle counting algorithm
	ChibaNishizekiTriangleCounter counter;
	EdgeAttribute triangles = counter.getAttribute(g, EdgeAttribute(g.upperEdgeIdBound()));

	//Actual test: ranked neighborhood
	SimmelianJaccardAttributizer simmel;
	std::vector<RankedNeighbors> neighborhood = simmel.getRankedNeighborhood(g, triangles);

	//Neighborhood of 4
	EXPECT_EQ(5, neighborhood[4].size());
	EXPECT_EQ(RankedEdge(4, 8, 2, 1), neighborhood[4][0]);
	EXPECT_EQ(RankedEdge(4, 5, 1, 2), neighborhood[4][1]);
	EXPECT_EQ(RankedEdge(4, 6, 1, 2), neighborhood[4][2]);
	EXPECT_EQ(RankedEdge(4, 7, 1, 2), neighborhood[4][3]);
	EXPECT_EQ(RankedEdge(4, 9, 1, 2), neighborhood[4][4]);

	//Neighborhood of 8
	EXPECT_EQ(3, neighborhood[8].size());
	EXPECT_EQ(RankedEdge(8, 4, 2, 1), neighborhood[8][0]);
	EXPECT_EQ(RankedEdge(8, 7, 1, 2), neighborhood[8][1]);
	EXPECT_EQ(RankedEdge(8, 9, 1, 2), neighborhood[8][2]);
}

TEST_F(SimmelianBackboneGTest, testRankedNeighborhoodSkippedRanks) {
	Graph g(7);

	g.addEdge(0,1);
	g.addEdge(0,2);
	g.addEdge(0,3);
	g.addEdge(0,4);
	g.addEdge(0,5);
	g.addEdge(0,6);

	g.addEdge(1,2);
	g.addEdge(2,3);
	g.addEdge(3,4);
	g.addEdge(4,5);
	g.addEdge(4,6);
	g.indexEdges();

	//Apply triangle counting algorithm
	ChibaNishizekiTriangleCounter counter;
	EdgeAttribute triangles = counter.getAttribute(g, EdgeAttribute(g.upperEdgeIdBound()));

	//Actual test: ranked neighborhood
	SimmelianJaccardAttributizer simmel;
	std::vector<RankedNeighbors> neighborhood = simmel.getRankedNeighborhood(g, triangles);

	//Neighborhood of 0
	EXPECT_EQ(6, neighborhood[0].size());
	EXPECT_EQ(RankedEdge(0, 4, 3, 1), neighborhood[0][0]);
	EXPECT_EQ(RankedEdge(0, 2, 2, 2), neighborhood[0][1]);
	EXPECT_EQ(RankedEdge(0, 3, 2, 2), neighborhood[0][2]);
	EXPECT_EQ(RankedEdge(0, 1, 1, 4), neighborhood[0][3]);
	EXPECT_EQ(RankedEdge(0, 5, 1, 4), neighborhood[0][4]);
	EXPECT_EQ(RankedEdge(0, 6, 1, 4), neighborhood[0][5]);

	//Neighborhood of 4
	EXPECT_EQ(4, neighborhood[4].size());
	EXPECT_EQ(RankedEdge(4, 0, 3, 1), neighborhood[4][0]);
	EXPECT_EQ(RankedEdge(4, 3, 1, 2), neighborhood[4][1]);
	EXPECT_EQ(RankedEdge(4, 5, 1, 2), neighborhood[4][2]);
	EXPECT_EQ(RankedEdge(4, 6, 1, 2), neighborhood[4][3]);
}

TEST_F(SimmelianBackboneGTest, testOverlapFiltering) {
	Graph g(10);

	g.addEdge(0,1);
	g.addEdge(1,2);
	g.addEdge(2,3);
	g.addEdge(3,4);
	g.addEdge(4,0);

	g.addEdge(0,2);
	g.addEdge(1,3);
	g.addEdge(2,4);
	g.addEdge(3,0);
	g.addEdge(4,1);

	g.addEdge(5,6);
	g.addEdge(6,7);
	g.addEdge(7,8);
	g.addEdge(8,9);
	g.addEdge(9,5);

	g.addEdge(5,7);
	g.addEdge(6,8);
	g.addEdge(7,9);
	g.addEdge(8,5);
	g.addEdge(9,6);

	g.addEdge(0,6);
	g.addEdge(0,5);
	g.addEdge(5,1);
	g.indexEdges();

	ChibaNishizekiTriangleCounter counter;
	EdgeAttribute triangleCounts = counter.getAttribute(g, EdgeAttribute(g.upperEdgeIdBound()));

	SimmelianBackboneParametric simmel(2, 1);
	Graph b = simmel.calculate(g, triangleCounts);

	EXPECT_EQ(20, b.numberOfEdges());

	EXPECT_FALSE(b.hasEdge(0,6));
	EXPECT_FALSE(b.hasEdge(0,5));
	EXPECT_FALSE(b.hasEdge(5,1));

	EXPECT_TRUE(b.hasEdge(0,1));
	EXPECT_TRUE(b.hasEdge(1,2));
	EXPECT_TRUE(b.hasEdge(2,3));
	EXPECT_TRUE(b.hasEdge(3,4));
	EXPECT_TRUE(b.hasEdge(4,0));
}

TEST_F(SimmelianBackboneGTest, testBackboneTrivial) {
	Graph g(5);

	g.addEdge(0,1);
	g.addEdge(0,2);
	g.addEdge(1,2);
	g.indexEdges();

	ChibaNishizekiTriangleCounter counter;
	EdgeAttribute triangleCounts = counter.getAttribute(g, EdgeAttribute(g.upperEdgeIdBound()));

	//Parametric
	SimmelianBackboneParametric simmel(1, 0);
	Graph b = simmel.calculate(g, triangleCounts);
	EXPECT_EQ(3, b.numberOfEdges()) << "wrong edge count in backbone";
	EXPECT_EQ(5, b.numberOfNodes()) << "wrong node count in backbone";
}

TEST_F(SimmelianBackboneGTest, testBackboneConnectedGraph) {
	GraphGenerator graphGen;
	Graph g = graphGen.makeCompleteGraph(25);
	g.indexEdges();

	ChibaNishizekiTriangleCounter counter;
	EdgeAttribute triangleCounts = counter.getAttribute(g, EdgeAttribute(g.upperEdgeIdBound()));

	//Parametric
	SimmelianBackboneParametric simmel(25, 5);
	Graph b = simmel.calculate(g, triangleCounts);
	EXPECT_EQ(300, b.numberOfEdges()) << "wrong edge count in backbone";
}

}
/* namespace NetworKit */

#endif /*NOGTEST */
