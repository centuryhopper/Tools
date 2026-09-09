#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>
#include <ranges>
#include <queue>


#include <fmt/core.h>
#include <fmt/ranges.h>

#include "../include/bst/BST.hpp"

#define pii std::pair<int, int>
#define vvpii std::vector<std::vector<pii>>
#define vvi std::vector<std::vector<int>>
#define vvtiii std::vector<std::vector<std::tuple<int, int, int>>>

// cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

std::optional<std::vector<int>> bellmanFord(const vvtiii &graph, int source)
{
  int n = graph.size();
  if (source < 0 || source >= n)
  {
    return std::nullopt;
  }
  // set all distances to int_max to begin with
  std::vector<int> distances(n, INT32_MAX);
  // set distance at source to 0
  distances[source] = 0;

  // loop v-1 times
  // examine each vertex and see if we can relax neighbors
  for (int i = 0; i < n - 1; i++)
  {
    bool changed = false;
    for (const auto &edges : graph)
    {
      for (const auto &[cur, neighbor, wt] : edges)
      {
        // fmt::print("cur, neighbor, wt: {},{},{}\n", (char) (cur + 97),(char)(neighbor+97),wt);
        if (distances[cur] != INT32_MAX && distances[neighbor] > distances[cur] + wt)
        {
          distances[neighbor] = distances[cur] + wt;
          changed = true;
        }
      }
    }
    if (!changed)
    {
      break;
    }
  }

  // fmt::print("distances: {}\n", distances);

  // loop thru edges one more time to see if there's a negative cycle
  for (const auto &edges : graph)
  {
    for (const auto &[cur, neighbor, wt] : edges)
    {
      if (distances[cur] != INT32_MAX && distances[neighbor] > distances[cur] + wt)
      {
        return std::nullopt;
      }
    }
  }

  return distances;
}

int f(char c)
{
  if (!isalpha(c))
    return -1;
  return isupper(c) ? (int)c - 'A' : (int)c - 'a';
}

void runBellmanFord()
{
  vvtiii graph = {
      {
          {f('a'), f('b'), 6},
          {f('a'), f('c'), 4},
          {f('a'), f('d'), 5},
      },

      {
          {f('b'), f('e'), -1},
      },

      {
          {
              f('c'),
              f('b'),
              -2,
          },
          {f('c'), f('e'), 3},
      },

      {
          {f('d'), f('c'), -2},
          {f('d'), f('f'), -1},
      },

      {
          {f('e'), f('f'), 3},
      },

      {

      },
  };

  fmt::print("a: {}\n", f('a'));

  auto result = bellmanFord(graph, f('a'));

  if (result)
  {
    fmt::print("result: {}\n", *result);
  }
  else
  {
    fmt::print("No result / negative cycle\n");
  }
}

template <size_t m, size_t n>
vvi floydWarshall(const int (&adjMat)[m][n])
{
  int dp[m + 1][m][n];
  memset(dp, 0, sizeof(dp));

  // initialize starting matrix
  for (int i = 0; i < m; i++)
  {
    for (int j = 0; j < n; j++)
    {
      dp[0][i][j] = adjMat[i][j];
    }
  }

  // build rest of matrix using starting matrix
  for (int k = 1; k <= m; k++)
  {
    for (int i = 0; i < m; i++)
    {
      for (int j = 0; j < n; j++)
      {
        if (dp[k - 1][i][k - 1] == INT32_MAX || dp[k - 1][k - 1][j] == INT32_MAX)
        {
          dp[k][i][j] = dp[k - 1][i][j];
        }
        else
        {
          dp[k][i][j] = std::min(dp[k - 1][i][j], dp[k - 1][i][k - 1] + dp[k - 1][k - 1][j]);
        }
      }
    }
  }

  for (int k = 0; k < m + 1; k++)
    fmt::print("dp[{}]: {}\n", k, dp[k]);

  vvi result(m, std::vector<int>(n));

  for (size_t i = 0; i < m; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      result[i][j] = dp[m][i][j];
    }
  }

  return result;
}

std::vector<int> topologicalSort(const vvi& adj)
{
  std::unordered_set<int> seen;
  std::vector<int> result;

  auto run = [&](auto&& self, int curVertex) -> void
  {
    seen.insert(curVertex);

    for (const auto& neighbor : adj[curVertex])
    {
      if (!seen.contains(neighbor))
      {
        self(self, neighbor);
      }
    }

    result.push_back(curVertex);
  };
  
  for (int v=0;v<adj.size();v++)
  {
    if (seen.contains(v)) continue;
    run(run, v);
  }

  std::reverse(result.begin(), result.end());

  return result;
}

/// Implemented using Khan's algorithm
std::vector<int> topologicalSort_BFS(const vvi& adj)
{
  int n=adj.size();
  std::vector<int> incomingEdges(n,0);

  // populate incoming edges
  for (const auto& edges : adj)
  {
    for (int i=0;i<edges.size();i++)
    {
      incomingEdges[edges[i]]+=1;
    }
  }

  std::vector<int> result;
  std::priority_queue<int, std::vector<int>, std::greater<int>> mh;

  // use queue if you dont care about ordering of vertices
  // std::queue<int> q;
  for (int i=0;i<n;i++)
  {
    if (incomingEdges[i] == 0)
    {
      mh.push(i);
    }
  }
  while (!mh.empty())
  {
    auto v = mh.top();
    mh.pop();

    for (const auto& neighbor : adj[v])
    {
      incomingEdges[neighbor]-=1;
      if (incomingEdges[neighbor]==0)
        mh.push(neighbor);
    }

    result.push_back(v);
  }

  if (result.size() != n)
  {
      return {};
  }

  return result;

}

int main()
{
  vvi topoGraph = {
    {
      f('c'),
    },
    {
      f('c'),f('d'),
    },
    {
      f('e'),
    },
    {
      f('f'),
    },
    {
      f('f'),f('h'),
    },
    {
      f('g'),
    },
    {
    },
    {
    },
  };

  // auto result = topologicalSort(topoGraph);
  auto result = topologicalSort_BFS(topoGraph);
  auto chars =
    result
    | std::views::transform([](int x) { return static_cast<char>('a'+x); })
    | std::ranges::to<std::vector>();
  fmt::print("result: {}\n", chars);

  
  // constexpr std::size_t m = 4, n = 4;
  // int adjMat[m][n] = {
  //     {0, 3, INT32_MAX, 7},
  //     {8, 0, 2, INT32_MAX},
  //     {5, INT32_MAX, 0, 1},
  //     {2, INT32_MAX, INT32_MAX, 0},
  // };

  // auto result = floydWarshall(adjMat);

  // fmt::print("result: {}\n", result);

  return 0;
}
