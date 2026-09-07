use std::cmp::Reverse;
use std::collections::{BinaryHeap, HashSet};
use std::fmt::Binary;

use petgraph::dot::{Config, Dot};
use petgraph::graph::UnGraph;
use union_find::UnionFind;

fn create_graph() {
    let mut graph = UnGraph::<&str, i32>::new_undirected();

    let a = graph.add_node("A");
    let b = graph.add_node("B");
    let c = graph.add_node("C");

    graph.add_edge(a, b, 5);
    graph.add_edge(b, c, 2);
    graph.add_edge(a, c, 8);

    // println!("{:?}", Dot::with_config(
    //     &graph,
    //     &[Config::EdgeNoLabel],
    // ));
    println!("{:?}", Dot::new(&graph));
}

/*
PRIM(Graph G, start):

    MST = empty set
    visited = empty set

    minHeap = empty priority queue

    add (0, start, NONE) to minHeap
    // (edge weight, current vertex, parent vertex)

    while minHeap is not empty:

        (weight, vertex, parent) = remove minimum from minHeap

        if vertex is already in visited:
            continue

        add vertex to visited

        if parent is not NONE:
            add edge (parent, vertex, weight) to MST

        for each edge (vertex, neighbor, edgeWeight):

            if neighbor is not in visited:
                add (edgeWeight, neighbor, vertex) to minHeap

    return MST


Start at any vertex
        ↓
Look at all edges leaving the current MST
        ↓
Choose the cheapest edge
        ↓
Add the new vertex
        ↓
Add its outgoing edges to the heap
        ↓
Repeat until all vertices are included
*/

// struct Vertex {
//     vertex: u32,
//     neighbor: u32,
//     edge_weight: u32,
// }

#[derive(Clone)]
struct Edge {
    neighbor: usize,
    weight: usize,
}
impl Edge {
    fn new(n: usize, w: usize) -> Self {
        Self {
            neighbor: n,
            weight: w,
        }
    }
}

fn run_kruskals(graph: &[Vec<Edge>], start: usize) -> (Vec<(usize, Edge)>, usize) {
    let n = graph.len();
    let mut uf = UnionFind::new(n);
    let mut mst: Vec<(usize, Edge)> = vec![];

    // flattened
    let mut edges: Vec<(usize, Edge)> = graph
        .iter()
        .enumerate()
        .flat_map(|(i, v)| v.iter().map(move |e| (i, e.clone())))
        .collect();
    // sort by edges in ascending order
    edges.sort_by_key(|(_, edge)| edge.weight);
    for (node, edge) in edges {
        if uf.find(node) != uf.find(edge.neighbor) {
            uf.union(node, edge.neighbor);
            mst.push((node, edge));
        }
    }

    let total_mst_edge_weights = mst.iter().map(|(_, e)| e.weight).sum();

    (mst, total_mst_edge_weights)
}

fn run_prims(graph: &[Vec<Edge>], start: usize) -> (Vec<(usize, Edge)>, usize) {
    let mut mst: Vec<(usize, Edge)> = vec![];
    let mut seen = HashSet::<usize>::new();
    // vertex id, edge weight, parent vertex
    let mut mh: BinaryHeap<Reverse<(usize, usize, Option<usize>)>> = BinaryHeap::new();

    mh.push(Reverse((start, 0, None)));
    while !mh.is_empty() {
        let Reverse((vertex, edge_weight, parent)) = mh.pop().unwrap();
        if seen.contains(&vertex) {
            continue;
        }
        seen.insert(vertex);
        if parent.is_some() {
            mst.push((vertex, Edge::new(parent.unwrap(), edge_weight)));
        }

        for edge in &graph[vertex] {
            if !seen.contains(&edge.neighbor) {
                mh.push(Reverse((edge.neighbor, edge.weight, Some(vertex))));
            }
        }
    }

    let total_mst_edge_weights = mst.iter().map(|(_, e)| e.weight).sum();

    (mst, total_mst_edge_weights)
}

fn f(c: char) -> usize {
    if !c.is_alphabetic() {
        panic!("please enter an alphabetical character")
    }
    // println!("c: {}", c);
    (u32::from(c) - (if c.is_uppercase() { 65 } else { 97 })) as usize
}

fn test_prims_and_kruskals() {
    let mut graph: [Vec<Edge>; 7] = std::array::from_fn(|_| Vec::new());

    // assign neighbors of vertex 0 and their edge weights
    graph[f('A')] = vec![
        Edge::new(f('B'), 2),
        Edge::new(f('C'), 3),
        Edge::new(f('D'), 3),
    ];

    graph[f('B')] = vec![
        Edge::new(f('A'), 2),
        Edge::new(f('C'), 4),
        Edge::new(f('E'), 3),
    ];

    graph[f('C')] = vec![
        Edge::new(f('A'), 3),
        Edge::new(f('D'), 5),
        Edge::new(f('F'), 6),
        Edge::new(f('E'), 1),
        Edge::new(f('B'), 4),
    ];

    graph[f('D')] = vec![
        Edge::new(f('A'), 3),
        Edge::new(f('C'), 5),
        Edge::new(f('F'), 7),
    ];

    graph[f('E')] = vec![
        Edge::new(f('F'), 8),
        Edge::new(f('C'), 1),
        Edge::new(f('B'), 3),
    ];

    graph[f('F')] = vec![
        Edge::new(f('D'), 7),
        Edge::new(f('G'), 9),
        Edge::new(f('E'), 8),
        Edge::new(f('C'), 6),
    ];

    graph[f('G')] = vec![Edge::new(f('F'), 9)];

    // mst is minimum spanning tree
    let (mst, total_weight) = run_kruskals(&graph, f('G'));
    // let (mst, total_weight) = run_prims(&graph, f('G'));
    println!(
        "mst: {:?}",
        mst.iter()
            .map(|(vertex, edge)| {
                (
                    char::from_u32(*vertex as u32 + 65).unwrap(),
                    char::from_u32(edge.neighbor as u32 + 65).unwrap(),
                    edge.weight,
                )
            })
            .collect::<Vec<_>>()
    );
    println!("total_weight: {:?}", total_weight);
}

fn djikstras_algo(graph: &[Vec<Edge>], source: usize) -> Vec<usize> {
    let n = graph.len();
    let mut distances = vec![usize::MAX; n];
    // initialize distance from the source to itself to 0
    distances[source] = 0;
    let mut seen: HashSet<usize> = HashSet::new();

    /*
        In Rust, a BinaryHeap<(A, B)> orders tuples lexicographically using the tuple's Ord implementation.

        That means it compares:
            The first element
            If those are equal, the second element
            Then the third, etc.
    */
    // (total distance from source, vertex)
    let mut mh: BinaryHeap<Reverse<(usize, usize)>> = BinaryHeap::new();
    mh.push(Reverse((0, source)));

    while !mh.is_empty() {
        let Reverse((_, cur)) = mh.pop().unwrap();

        if seen.contains(&cur) {
            continue;
        }
        seen.insert(cur);

        // explore neighbors and update distance to that neighbor
        for edge in &graph[cur] {
            let new_dist = edge.weight + distances[cur];

            if distances[edge.neighbor] > new_dist {
                distances[edge.neighbor] = new_dist;
                mh.push(Reverse((new_dist, edge.neighbor)));
            }
        }
    }

    distances
}

// cargo run --quiet > graph.dot
// dot -Tpng graph.dot -o graph.png
fn main() {
    let mut graph = [
        vec![],
        vec![Edge::new(2, 2), Edge::new(3, 4)],
        vec![Edge::new(4, 7), Edge::new(3, 1)],
        vec![Edge::new(5, 3)],
        vec![Edge::new(6, 1)],
        vec![Edge::new(4, 2), Edge::new(6, 5)],
        vec![],
    ];

    let mut graph2 = [
        vec![Edge::new(f('B'), 4), Edge::new(f('C'), 2)],
        vec![
            Edge::new(f('D'), 2),
            Edge::new(f('E'), 3),
            Edge::new(f('C'), 3),
        ],
        vec![
            Edge::new(f('B'), 1),
            Edge::new(f('D'), 4),
            Edge::new(f('E'), 5),
        ],
        vec![],
        vec![Edge::new(f('D'), 1)],
    ];

    let result = djikstras_algo(&graph2, f('A'));
    // println!("distances: {:?}", &result[1..]);
    println!("distances: {:?}", result);

    // create_graph();
}
