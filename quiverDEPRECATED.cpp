
#include <Eigen/Dense>
// #include <matplot/matplot.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <bitset>


template <int Q0> 
class Quiver {
    Eigen::Matrix<int,Q0,Q0> adj;

    Eigen::Matrix<int,Q0,Q0> adjDelete(int i, int j){
        assert((i>-1 && j>-1 && i<Q0 && j<Q0) && "Indices out of bounds!");
        assert(adj(i,j)>0 && "No edge found!");
        Eigen::Matrix<int,Q0,Q0> m = adj;
        m(i,j) -= 1;
        return m;
    }

public:
    Quiver() {
        adj.setZero();
    }

    Quiver(Eigen::Matrix<int,Q0,Q0> data){
        adj = data;
    }

    void print(){
        std::cout << adj << "\n\n";
    }

    Quiver* deleteEdge(int i, int j){
        return new Quiver(adjDelete(i,j));
    }

    int getEdges(int i, int j){
        return adj(i,j);
    }

    int getV(){
        return Q0;
    }

    Quiver<Q0-1>* modEdge(int i, int j){
        assert((i>-1 && j>-1 && i<Q0 && j<Q0) && "Indices out of bounds!");
        assert(i!=j && "Attempt to contract a loop!");
        assert(adj(i,j)>0 && "No edge found!");
        Eigen::Matrix<int,Q0-1,Q0-1> m;
        int minimum = std::min(i,j);
        int maximum = std::max(i,j);
        std::bitset<Q0-1> b;
        b.set(minimum);
        for(int k = 0; k < maximum; k++){
            for(int l = 0; l < maximum; l++){
                m(k,l) = adj(k,l) + b[k]*adj(maximum,l) + b[l]*adj(k,maximum) + b[k]*b[l]*(adj(maximum,maximum)-1);
            }
        }
        std::bitset<Q0-1> c;
        for(int k = maximum; k < Q0-1; k++){
            c.set(k);
        }
        for(int k = 0; k < Q0-1; k++){
            int l = maximum-c[k]*maximum;
            while(l < Q0-1){
                m(k,l) = adj(k + c[k],l + c[l]) + b[k]*adj(k+1,l+1) + b[l]*adj(k+1,l+1);
                l++;
            }
        }
        return new Quiver<Q0-1>(m);
    }

    void displayQuiver(){
        std::vector<std::pair<size_t,size_t>> edges;
        for(int k = 0; k < Q0; k++){
            for(int l = 0; l < Q0; l++){
                for(int m = 0; m < getEdges(k,l); m++){
                    edges.push_back(std::pair<size_t,size_t>{k,l});
                }
            }
        }
        
    }

    
};