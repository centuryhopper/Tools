pub struct UnionFind {
    rank: Vec<usize>,
    parent: Vec<usize>,
    size: usize,
}

impl UnionFind {
    pub fn new(n: usize) -> Self {
        Self {
            rank: vec![0; n],
            parent: (0..n).collect(),
            size: n,
        }
    }

    pub fn size(&self) -> usize {
        self.size
    }

    pub fn union(&mut self, i: usize, j: usize) {
        if i >= self.size || j >= self.size {
            println!("At least one of the values are out of bounds");
            return;
        }

        let root_i = self.find(i);
        let root_j = self.find(j);

        if root_i == root_j {
            return;
        }

        if self.rank[root_i] > self.rank[root_j] {
            self.parent[root_j] = root_i;
        }
        else if self.rank[root_j] > self.rank[root_i] {
            self.parent[root_i] = root_j;
        }
        else {
            self.parent[root_i] = root_j;
            self.rank[root_j]+=1;
        }

    }

    pub fn find(&mut self, i: usize) -> usize {
        if i >= self.size {
            println!("Value is out of bounds");
            return 0;
        }

        if self.parent[i] == i {
            return i;
        }

        self.parent[i] = self.find(self.parent[i]);
        return self.parent[i];
    }
}