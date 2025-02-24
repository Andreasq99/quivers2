
#include <Eigen/Dense>

#include <unordered_map>
#include <assert.h>
#include <iostream>

typedef std::unordered_map<int, std::pair<int,int>> index;
template <int q0>
class Quiver {
    int** adj; // Jagged array enumerating the edges of the quiver. adj[i][e] encodes the eth edge with source vertex i. If (i--e-->j) is an edge from i to j, then adj[i][e] = j - i, so that i + adj[i][e] = j.
    int* rows; // Array containing the lengths of each row of adj.
    std::unordered_map<int, std::pair<int,int>> indices; // Unordered map providing indices for the edges of the quiver.
    int E; // Number of edges in the quiver.

public:
    Quiver(){
        adj = new int*[q0];
        rows = new int[q0];
        // for (int i = 0; i < q0; i++){
        //     adj[i] = new int[1];
        //     adj[i][0] = (i == q0-1) ? 1-q0 : 1;
        //     rows[i]=1;
        // }
        E = q0;
        index indices;
        // for(int i = 0; i < E; i++){
        //     indices[i] = std::pair<int,int>(i,0);
        // }
    }

    Quiver(int** data, int* rowData){
        adj = new int*[q0];
        rows = new int[q0];
        index indices;
        std::cout << "constructing quiver\n";
        for(int i = 0; i < q0; i++){
            rows[i] = rowData[i];
            std::cout<< "row "<<i<<" has length "<<rows[i]<<"\n";
            adj[i] = new int[rows[i]];
            for(int j = 0; j < rowData[i]; j++){
                indices[i+j] = std::pair<int,int>(i,j);
                adj[i][j] = data[i][j];
                std::cout<<"adj position "<<i<<","<<j<<" value: "<<adj[i][j]<<"\n";
            }
        }
        E = indices.size();
        std::cout << "successfully constructed quiver from row and column data\n";
    }

    Quiver (int** data, int* rowData, index indData){
        adj = data;
        rows = rowData;
        indices = indData;
        E = indices.size();
    }

    int h(int i, int e){
        return i + adj[i][e];
    }

    void addEdge(int i, int diff){
        assert((i > 0 && i < q0) && "Source vertex index out of bounds!");
        assert((i + diff > 0 && i + diff < q0) && "Target vertex index out of bounds!");
        int indmax = 0;
        for(auto nind = indices.begin(); nind != indices.end(); nind++){
            if(nind->first > indmax){
                indmax = nind->first;
            }
        }
        indices[indmax+1] = std::pair<int,int>{i,rows[i]};
        adj[i][rows[i]] = diff;
        rows[i]++;
        E++;
    }

    Quiver deleteEdge(int ind){
        assert((indices.find(ind) != indices.end()) && "Edge index out of bounds!");
        std::pair<int,int> data = indices[ind];
        int i = data.first;
        int e = data.second;
        assert((i < q0 && e < rows[i]) && "Edge index out of bounds!");
        int** newAdj = new int*[q0];
        int* newRows = new int[q0];
        for(int j = 0; j < q0; j++){
            newRows[j]= (j == e) ? rows[j] - 1 : rows[j];
            newAdj[j] = new int[newRows[j]];
        }
        index newIndices = indices;
        int shift = 0;
        for(int k = 0; k < q0; k++){
            for (int j = 0; j < rows[k]; j++){
                if(k == i && j == e){
                    shift = 1;
                }
                if(k == i+1 && j == 0){
                    shift = 0;
                }
                newAdj[k][j] = adj[k][k+shift];
            }
        }
        newIndices.erase(indices.find(ind));
        return Quiver<q0>(newAdj,newRows,newIndices);
    }

    Quiver<q0-1> modEdge(int ind){
        assert((indices.find(ind) != indices.end()) && "Edge index out of bounds!");
        std::pair<int,int> data = indices[ind];
        int i = data.first;
        int e = data.second;
        assert((e < q0 && e < rows[e]) && "Edge index out of bounds!");
        assert((adj[i][e] != 0) && "Attempt to contract a loop!");
        int j = h(i,e);
        int** newAdj = new int*[q0-1];
        int* newRows = new int[q0-1];
        index newIndices;

        int past = 0; // Initialize arrays, set row lengths
        for(int k = 0; k < q0-1; k++){
            if(k == i){
                newRows[k] = rows[i] + rows[j] - 1;
            } else {
                if(k == j){
                    past = 1;
                }
                newRows[k] = rows[k + past];
            }
            adj[k] = new int[newRows[k]];
        }

        int m = std::min(i,j);
        int M = std::max(i,j);
        for(auto nind = indices.begin(); nind != indices.end(); nind++){
            if(nind != indices.find(ind)){ // Reassign all edges except the one being contracted
                int k = (nind -> second).first;
                int l = (nind -> second).second;
                if(h(k,l) == M){ // If the head of the edge is M
                    if(k == m){ // If the tail is m, the edge becomes a loop
                        newAdj[k][l] = 0;
                    } else if(k == M) { // If the edge was a loop, insert it as a loop at m
                        newAdj[m][rows[m] + l] = 0;
                    } else{ // If the edge did not originate from m or M, set its head to m
                        newAdj[k][l] = m - k;
                    }
                } else if(h(k,l) < M){ // If the head is less than M
                    if(k == M){ // If the tail is M, replace the tail with m.
                        newAdj[m][rows[m]+l] = adj[k][l];
                    } else if(k < M){ // If both are less than M, the difference does not change.
                        newAdj[k][l] = adj[k][l];
                    } else { // If the tail is greater than M, the difference decreases by 1 (the actual value is negative, so adding 1 decreases the distance). The tail index is shifted down 1.
                        newAdj[k][l] = adj[k+1][l] + 1;
                    }
                } else { // If the head is greater than M
                    if(k < M){ // If the tail is less than M, the difference decreases by 1
                        newAdj[k][l] = adj[k][l] - 1;
                    } else if(k == M){ // If the tail is M, replace the tail with m.
                        newAdj[m][rows[m] + l] = adj[k][l];
                    } else { // If both the head and tail are greater than M, the difference does not change, and both indices are shifted down 1.
                        newAdj[k][l] = adj[k+1][l+1];
                    }
                }
                if(k == M){ // Reinsert indices for edges, maintaining the order, and setting the tail of all edges from M to m.
                    newIndices.insert_or_assign(nind->first,std::pair<int,int>{k,rows[m]+l});
                } else {
                    newIndices.insert(*nind);
                }
            }
        }
        return Quiver<q0-1>(newAdj,newRows,newIndices);
    }

    Eigen::Matrix<int,q0,q0> adjMatrix(){
        Eigen::Matrix<int,q0,q0> adjmat;
        adjmat.setZero();
        for(int k = 0; k < q0; k++){
            for(int l = 0; l < rows[k]; l++){
                adjmat(k,h(k,l))++;
            }
        }
        return adjmat;
    }

    void printAdjMat(){
        std::cout << adjMatrix() << "\n\n";
    }

    void printIndex(){
        int count = 0;
        std::cout << "printing index:\n";
        for(auto nind = indices.begin(); nind != indices.end(); ++nind){
            if(count == 4){
                std::cout << "\n";
                count = 0;
            }
            std::cout << nind->first << " : " << "(" << (nind->second).first << "," << (nind->second).second << ");  ";
            count++;
        }
    }
};