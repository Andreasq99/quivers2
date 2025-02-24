
#include "quiver.cpp"

#include <vector>
#include <iostream>


template<int n>
int* rowsProcessor(Eigen::Matrix<int,n,n,0,n,n> mat){
    int* rows = new int[n];
    for(int k = 0; k < n; k++){
        rows[k]=0;
        for(int l = 0; l < n; l++){
            rows[k] += mat(k,l);
        }
    }
    return rows;
}

template<int n>
int** adjProcessor(Eigen::Matrix<int,n,n,0,n,n> mat, int* rows){
    int** adj = new int*[n];
    for(int k = 0; k < n; k++){
        if(rows[k] > 0) {
            adj[k] = new int[rows[k]];
        } else {
            adj[k] = nullptr;
        }
        int count = 0;
        for(int l = 0; l < n; l++){
            for(int p = 0; p < mat(k,l); p++){
                adj[k][count] = l-k;
                count++;
            }
        }
    }
    return adj;
}

int main(){
    Eigen::Matrix<int,5,5> m;
    m << 
        1, 0, 0, 2, 1,
        0, 0, 0, 1, 1,
        1, 1, 0, 1, 0,
        0, 2, 3, 0, 1,
        0, 1, 1, 0, 2;
    int* rows = rowsProcessor(m);
    for(int k=0; k<5; k++){
        std::cout<<rows[k]<<"\n";
    }
    int** adj = adjProcessor(m,rows);
    for(int k = 0; k < 5; k++){
        if(adj[k]!=nullptr){
            for(int l = 0; l < rows[k]; l++){
                std::cout << adj[k][l] << ",";
            }
        }
        std::cout << "\n";
    }
    Quiver<5> q(adj,rows);
    q.printAdjMat();
    q.printIndex();
    Quiver<5> p = q.deleteEdge(4);
    p.printIndex();
    p.printAdjMat();
    // Quiver<4> r = p.modEdge(6);
    // r.printAdjMat();
    // r->displayQuiver();
    return 0;
}
