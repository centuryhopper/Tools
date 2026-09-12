#include <numeric>
#include <vector>

namespace custom_data_structures {
    class UnionFind
    {
        private:
            std::vector<int> rank;
            std::vector<int> parent;
            
        public:
            
            UnionFind(int n) : rank(n, 0), parent(n)
            {
                std::iota(parent.begin(), parent.end(), 0);
            }
    
            int find(int x)
            {
                if (parent[x] == x) return x;
                parent[x] = find(parent[x]);
                return parent[x];
            }
    
            void unite(int x, int y)
            {
                int rootX = find(x), rootY = find(y);
                if (rootX == rootY) return;
                
                if (rank[rootX] > rank[rootY])
                {
                    parent[rootY] = rootX;
                }
                else if (rank[rootY] > rank[rootX])
                {
                    parent[rootX] = rootY;
                }
                else
                {
                    parent[rootX] = rootY;
                    rank[rootY]+=1;
                }
            }
    };
}
