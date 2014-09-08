#ifndef __KERNDYNMEANS_HPP
#include<vector>
#include<stack>
#include<queue>
#include<iostream>
#include<algorithm>
#include<limits>
#include<boost/static_assert.hpp>
#include<boost/function.hpp>
#include<boost/bind.hpp>
#include<sys/time.h>
#include <ctime>
#include "gurobi_c++.h" //note: the use of this library requires gurobi!
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>

using namespace std;

typedef Eigen::SparseMatrix<double, Eigen::RowMajor> SMXd;
typedef Eigen::Triplet<double> TD;
typedef Eigen::MatrixXd MXd;
typedef Eigen::VectorXd VXd;

template <class G>
class KernDynMeans{
	public:
		KernDynMeans(double lambda, double Q, double tau, bool verbose = false);
		~KernDynMeans();
		//initialize a new step and cluster
		void cluster(const G& aff, const int nRestarts, const int nCoarsest, std::vector<int>& finalLabels, double& finalObj, double& tTaken);
		//reset DDP chain
		void reset();
	private:
		//clusters a refinement level with kernelized dyn means batch updates
		//tempalte so it works with C/D
		template <typename T> std::vector<int> clusterAtLevel(const T& aff, std::vector<int> initlbls);
		//compute the dynamic means objective given the current labels
		template<typename T> double objective(std::vector<T>& data, std::vector<int> lbls);
		//compute a minimum weight bipartite matching
		std::map<int, int> getMinWtMatching(vector< pair<int, int> > nodePairs, vector<double> edgeWeights ) const;
		//get the minimum weight old/new cluster correspondence
		template <typename T> std::vector<int> updateOldNewCorrespondence(std::vector<T>& data, std::vector<int> lbls);
		//get the updated data labels via dyn means iteration
		template <typename T> std::vector<int> updateLabels(std::vector<T>& data, std::vector<int> lbls);
		//update the state after all iterations are done
		void updateState(const vector<D>& data, const vector<int>& lbls);
		//do a base clustering using spectral methods + minimum weight matching
		template <typename T> std::vector<int> baseCluster(std::vector<T>& data);
		//utility function to orthonormalize a square matrix
		void orthonormalize(MXd& V) const; 

		GRBEnv* grbenv;
		double lambda, Q, tau;
		bool verbose;

		//during each step, constants which are information about the past steps
		//once each step is complete, these get updated
		std::vector<int> oldprmlbls;// require a mapping from old parameters to old labels
		int maxLblPrevUsed;         //because clusters that die are removed entirely to save computation
		std::vector<double> weights;
		std::vector<int> ages;
		std::vector<double> agecosts;
		std::vector<double> gammas;

		//TODO 
		//TODO
		//TODO REMOVE THE BELOW 4 FUNCTIOSN WHEN DONE TESTING WTIH DMEANS OBJ
		double computedynmeansobj(std::vector<D>& data, std::vector<int> lbls);
		double computedynmeansobj(std::vector<C>& data, std::vector<int> lbls);
		double computedynmeansobj2(std::vector<D>& data, std::vector<int> lbls);
		double computedynmeansobj2(std::vector<C>& data, std::vector<int> lbls);
};

template <class G>
class CoarseGraph{ //only stores upper triangular information (multiplies by 2 when necessary)
	public:
		//constructors
		CoarseGraph(const G& aff);
		CoarseGraph(const CoarseGraph& aff);
		//similarity functions
		double linSelfSimDD(int i);
		double quadSelfSimDD(int i);
		double selfSimPP(int i);
		double simDD(int i, int j);
		double simDP(int i, int j);
		//input labels for this coarsified graph, get the labels for the original refined graph
		std::vector<int> getRefinedLabels(const std::vector<int>& lbls);
		//get a mapping from col index in old parameters to actual cluster label
		std::vector<int> getOldPrmLbls();
	private:
		template <typename T> void coarsify(const T& aff);
		std::vector< std::pair<int, int> > refineMap;
		SMXd affdd, affdp;
		VXd linaffdd, affpp;
		std::vector<int> oldPrmLbls;
};

////try to split a cluster
//template <typename T> std::vector<int> clusterSplit(std::vector<T>& data, std::vector<int> lbls);
////try to merge two clusters
//template <typename T> std::vector<int> clusterMerge(std::vector<T>& data, std::vector<int> lbls);

#include "kerndynmeans_impl.hpp"
#define __KERNDYNMEANS_HPP
#endif /* __DYNMEANS_HPP */
