use union_find::UnionFind;

// cargo test -p union_find


#[test]
fn new_union_find_has_correct_size() {
    let uf = UnionFind::new(10);

    assert_eq!(uf.size(), 10);
}

#[test]
fn new_union_find_single_element() {
    let mut uf = UnionFind::new(1);

    assert_eq!(uf.size(), 1);
    assert_eq!(uf.find(0), 0);
}

#[test]
fn new_union_find_elements_are_their_own_parents() {
    let mut uf = UnionFind::new(5);

    assert_eq!(uf.find(0), 0);
    assert_eq!(uf.find(1), 1);
    assert_eq!(uf.find(2), 2);
    assert_eq!(uf.find(3), 3);
    assert_eq!(uf.find(4), 4);
}

#[test]
fn union_two_elements() {
    let mut uf = UnionFind::new(5);

    uf.union(0, 1);

    assert_eq!(uf.find(0), uf.find(1));
}

#[test]
fn union_two_elements_is_symmetric() {
    let mut uf = UnionFind::new(5);

    uf.union(1, 0);

    assert_eq!(uf.find(0), uf.find(1));
}

#[test]
fn union_element_with_itself() {
    let mut uf = UnionFind::new(5);

    uf.union(2, 2);

    assert_eq!(uf.find(2), 2);
}

#[test]
fn union_same_elements_twice() {
    let mut uf = UnionFind::new(5);

    uf.union(0, 1);
    uf.union(0, 1);

    assert_eq!(uf.find(0), uf.find(1));
}

#[test]
fn union_same_elements_in_reverse_order() {
    let mut uf = UnionFind::new(5);

    uf.union(0, 1);
    uf.union(1, 0);

    assert_eq!(uf.find(0), uf.find(1));
}

#[test]
fn unrelated_elements_remain_separate() {
    let mut uf = UnionFind::new(5);

    uf.union(0, 1);

    assert_ne!(uf.find(0), uf.find(2));
    assert_ne!(uf.find(0), uf.find(3));
    assert_ne!(uf.find(1), uf.find(4));
}

#[test]
fn transitive_union() {
    let mut uf = UnionFind::new(5);

    uf.union(0, 1);
    uf.union(1, 2);

    assert_eq!(uf.find(0), uf.find(1));
    assert_eq!(uf.find(1), uf.find(2));
    assert_eq!(uf.find(0), uf.find(2));
}

#[test]
fn larger_transitive_union() {
    let mut uf = UnionFind::new(10);

    uf.union(0, 1);
    uf.union(1, 2);
    uf.union(2, 3);
    uf.union(3, 4);

    assert_eq!(uf.find(0), uf.find(4));
}

#[test]
fn multiple_disjoint_sets() {
    let mut uf = UnionFind::new(10);

    uf.union(0, 1);
    uf.union(1, 2);

    uf.union(3, 4);
    uf.union(4, 5);

    uf.union(6, 7);
    uf.union(7, 8);

    assert_eq!(uf.find(0), uf.find(2));
    assert_eq!(uf.find(3), uf.find(5));
    assert_eq!(uf.find(6), uf.find(8));

    assert_ne!(uf.find(0), uf.find(3));
    assert_ne!(uf.find(0), uf.find(6));
    assert_ne!(uf.find(3), uf.find(6));
}

#[test]
fn merge_two_existing_sets() {
    let mut uf = UnionFind::new(6);

    uf.union(0, 1);
    uf.union(1, 2);

    uf.union(3, 4);
    uf.union(4, 5);

    assert_ne!(uf.find(0), uf.find(3));

    uf.union(2, 3);

    assert_eq!(uf.find(0), uf.find(5));
}

#[test]
fn union_using_non_root_nodes() {
    let mut uf = UnionFind::new(6);

    uf.union(0, 1);
    uf.union(1, 2);
    uf.union(2, 3);

    // 0, 1, 2, and 3 are all in the same set.
    uf.union(0, 4);

    assert_eq!(uf.find(0), uf.find(4));
    assert_eq!(uf.find(1), uf.find(4));
    assert_eq!(uf.find(2), uf.find(4));
    assert_eq!(uf.find(3), uf.find(4));
}

#[test]
fn union_two_large_sets() {
    let mut uf = UnionFind::new(10);

    // First set: 0-4
    uf.union(0, 1);
    uf.union(1, 2);
    uf.union(2, 3);
    uf.union(3, 4);

    // Second set: 5-9
    uf.union(5, 6);
    uf.union(6, 7);
    uf.union(7, 8);
    uf.union(8, 9);

    assert_ne!(uf.find(0), uf.find(9));

    uf.union(4, 5);

    for i in 0..10 {
        assert_eq!(uf.find(0), uf.find(i));
    }
}

#[test]
fn path_compression_preserves_connectivity() {
    let mut uf = UnionFind::new(10);

    uf.union(0, 1);
    uf.union(1, 2);
    uf.union(2, 3);
    uf.union(3, 4);
    uf.union(4, 5);

    let root = uf.find(0);

    // Calling find repeatedly should always return the same root.
    assert_eq!(uf.find(0), root);
    assert_eq!(uf.find(1), root);
    assert_eq!(uf.find(2), root);
    assert_eq!(uf.find(3), root);
    assert_eq!(uf.find(4), root);
    assert_eq!(uf.find(5), root);
}

#[test]
fn repeated_find_does_not_change_result() {
    let mut uf = UnionFind::new(5);

    uf.union(0, 1);
    uf.union(1, 2);

    let root1 = uf.find(2);
    let root2 = uf.find(2);
    let root3 = uf.find(2);

    assert_eq!(root1, root2);
    assert_eq!(root2, root3);
}

#[test]
fn union_after_path_compression() {
    let mut uf = UnionFind::new(8);

    uf.union(0, 1);
    uf.union(1, 2);
    uf.union(2, 3);

    // Compress the path.
    uf.find(3);

    uf.union(3, 4);

    assert_eq!(uf.find(0), uf.find(4));
}

#[test]
fn all_elements_can_be_joined() {
    let n = 100;
    let mut uf = UnionFind::new(n);

    for i in 0..n - 1 {
        uf.union(i, i + 1);
    }

    let root = uf.find(0);

    for i in 0..n {
        assert_eq!(uf.find(i), root);
    }
}

#[test]
fn no_unions_means_everything_is_separate() {
    let n = 20;
    let mut uf = UnionFind::new(n);

    for i in 0..n {
        for j in 0..n {
            if i != j {
                assert_ne!(uf.find(i), uf.find(j));
            }
        }
    }
}

#[test]
fn arbitrary_union_pattern() {
    let mut uf = UnionFind::new(10);

    uf.union(0, 7);
    uf.union(3, 9);
    uf.union(2, 5);
    uf.union(7, 3);
    uf.union(1, 8);
    uf.union(5, 6);
    uf.union(8, 4);

    assert_eq!(uf.find(0), uf.find(7));
    assert_eq!(uf.find(0), uf.find(3));
    assert_eq!(uf.find(0), uf.find(9));

    assert_eq!(uf.find(2), uf.find(5));
    assert_eq!(uf.find(2), uf.find(6));

    assert_eq!(uf.find(1), uf.find(8));
    assert_eq!(uf.find(1), uf.find(4));

    assert_ne!(uf.find(0), uf.find(2));
    assert_ne!(uf.find(0), uf.find(1));
    assert_ne!(uf.find(2), uf.find(1));
}

#[test]
fn invalid_union_does_not_panic() {
    let mut uf = UnionFind::new(5);

    uf.union(100, 0);
    uf.union(0, 100);
    uf.union(100, 200);

    // Valid elements should still work.
    assert_eq!(uf.find(0), 0);
}

#[test]
fn invalid_find_returns_zero_with_current_api() {
    let mut uf = UnionFind::new(5);

    assert_eq!(uf.find(100), 0);
}

#[test]
fn zero_sized_union_find() {
    let mut uf = UnionFind::new(0);

    assert_eq!(uf.size(), 0);

    // Your current API handles this without panicking.
    uf.union(0, 0);
    assert_eq!(uf.find(0), 0);
}

#[test]
fn singleton_union_find() {
    let mut uf = UnionFind::new(1);

    uf.union(0, 0);

    assert_eq!(uf.find(0), 0);
}

#[test]
fn complex_set_merging() {
    let mut uf = UnionFind::new(12);

    // {0,1,2}
    uf.union(0, 1);
    uf.union(1, 2);

    // {3,4,5}
    uf.union(3, 4);
    uf.union(4, 5);

    // {6,7,8}
    uf.union(6, 7);
    uf.union(7, 8);

    // {9,10,11}
    uf.union(9, 10);
    uf.union(10, 11);

    // Merge first two.
    uf.union(0, 3);

    // Merge second two.
    uf.union(6, 9);

    assert_eq!(uf.find(0), uf.find(5));
    assert_eq!(uf.find(6), uf.find(11));

    assert_ne!(uf.find(0), uf.find(6));

    // Merge everything.
    uf.union(2, 8);

    let root = uf.find(0);

    for i in 0..12 {
        assert_eq!(uf.find(i), root);
    }
}