use avl_tree::AvlTree;

// cargo test -p avl_tree


#[test]
fn new_tree_is_empty() {
    let tree = AvlTree::<i32>::new();

    assert_eq!(tree.height(), 0);
    assert!(!tree.contains(&10));
}

#[test]
fn insert_single_value() {
    let mut tree = AvlTree::new();

    tree.insert(10);

    assert_eq!(tree.height(), 1);
    assert!(tree.contains(&10));
    assert!(!tree.contains(&5));
}

#[test]
fn insert_multiple_values() {
    let mut tree = AvlTree::new();

    for value in [10, 5, 15, 3, 7, 12, 20] {
        tree.insert(value);
    }

    assert_eq!(tree.height(), 3);

    for value in [10, 5, 15, 3, 7, 12, 20] {
        assert!(tree.contains(&value));
    }
}

#[test]
fn contains_returns_false_for_missing_value() {
    let mut tree = AvlTree::new();

    for value in [10, 5, 15] {
        tree.insert(value);
    }

    assert!(!tree.contains(&1));
    assert!(!tree.contains(&7));
    assert!(!tree.contains(&20));
}

#[test]
fn duplicate_insert_does_not_add_node() {
    let mut tree = AvlTree::new();

    tree.insert(10);
    tree.insert(10);
    tree.insert(10);

    assert_eq!(tree.height(), 1);
    assert!(tree.contains(&10));
}

#[test]
fn ll_rotation() {
    let mut tree = AvlTree::new();

    tree.insert(30);
    tree.insert(20);
    tree.insert(10);

    // 30 -> 20 -> 10 should become:
    //
    //       20
    //      /  \
    //    10    30

    assert_eq!(tree.height(), 2);

    assert!(tree.contains(&10));
    assert!(tree.contains(&20));
    assert!(tree.contains(&30));
}

#[test]
fn rr_rotation() {
    let mut tree = AvlTree::new();

    tree.insert(10);
    tree.insert(20);
    tree.insert(30);

    // 10 -> 20 -> 30 should become:
    //
    //       20
    //      /  \
    //    10    30

    assert_eq!(tree.height(), 2);

    assert!(tree.contains(&10));
    assert!(tree.contains(&20));
    assert!(tree.contains(&30));
}

#[test]
fn lr_rotation() {
    let mut tree = AvlTree::new();

    tree.insert(30);
    tree.insert(10);
    tree.insert(20);

    // Should become:
    //
    //       20
    //      /  \
    //    10    30

    assert_eq!(tree.height(), 2);

    assert!(tree.contains(&10));
    assert!(tree.contains(&20));
    assert!(tree.contains(&30));
}

#[test]
fn rl_rotation() {
    let mut tree = AvlTree::new();

    tree.insert(10);
    tree.insert(30);
    tree.insert(20);

    // Should become:
    //
    //       20
    //      /  \
    //    10    30

    assert_eq!(tree.height(), 2);

    assert!(tree.contains(&10));
    assert!(tree.contains(&20));
    assert!(tree.contains(&30));
}

#[test]
fn remove_leaf() {
    let mut tree = AvlTree::new();

    for value in [10, 5, 15] {
        tree.insert(value);
    }

    tree.remove(&5);

    assert!(!tree.contains(&5));
    assert!(tree.contains(&10));
    assert!(tree.contains(&15));
    assert_eq!(tree.height(), 2);
}

#[test]
fn remove_node_with_one_child() {
    let mut tree = AvlTree::new();

    for value in [10, 5, 15, 3] {
        tree.insert(value);
    }

    tree.remove(&5);

    assert!(!tree.contains(&5));
    assert!(tree.contains(&3));
    assert!(tree.contains(&10));
    assert!(tree.contains(&15));
    assert_eq!(tree.height(), 2);
}

#[test]
fn remove_node_with_two_children() {
    let mut tree = AvlTree::new();

    for value in [20, 10, 30, 5, 15, 25, 35] {
        tree.insert(value);
    }

    tree.remove(&20);

    assert!(!tree.contains(&20));

    for value in [5, 10, 15, 25, 30, 35] {
        assert!(tree.contains(&value));
    }

    assert_eq!(tree.height(), 3);
}

#[test]
fn remove_root() {
    let mut tree = AvlTree::new();

    tree.insert(10);

    tree.remove(&10);

    assert_eq!(tree.height(), 0);
    assert!(!tree.contains(&10));
}

#[test]
fn remove_nonexistent_value() {
    let mut tree = AvlTree::new();

    for value in [10, 5, 15] {
        tree.insert(value);
    }

    tree.remove(&999);

    assert_eq!(tree.height(), 2);

    assert!(tree.contains(&10));
    assert!(tree.contains(&5));
    assert!(tree.contains(&15));
}

#[test]
fn remove_all_values() {
    let mut tree = AvlTree::new();

    for value in [10, 5, 15, 3, 7, 12, 20] {
        tree.insert(value);
    }

    for value in [3, 7, 5, 12, 20, 15, 10] {
        tree.remove(&value);
    }

    assert_eq!(tree.height(), 0);

    for value in [3, 7, 5, 12, 20, 15, 10] {
        assert!(!tree.contains(&value));
    }
}



#[test]
fn insert_into_empty_tree_after_removal() {
    let mut tree = AvlTree::new();

    tree.insert(10);
    tree.remove(&10);

    tree.insert(20);

    assert_eq!(tree.height(), 1);
    assert!(!tree.contains(&10));
    assert!(tree.contains(&20));
}


#[test]
fn remove_node_with_two_children_using_predecessor() {
    let mut tree = AvlTree::new();

    for value in [50, 30, 70, 20, 40, 60, 80] {
        tree.insert(value);
    }

    tree.remove(&50);

    assert!(!tree.contains(&50));

    for value in [20, 30, 40, 60, 70, 80] {
        assert!(tree.contains(&value));
    }
}


#[test]
fn remove_node_with_two_children_using_successor() {
    let mut tree = AvlTree::new();

    for value in [50, 30, 70, 20, 40, 60, 80, 55] {
        tree.insert(value);
    }

    tree.remove(&50);

    assert!(!tree.contains(&50));

    for value in [20, 30, 40, 55, 60, 70, 80] {
        assert!(tree.contains(&value));
    }
}


#[test]
fn remove_causes_ll_rebalance() {
    let mut tree = AvlTree::new();

    for value in [30, 20, 40, 10, 25, 50] {
        tree.insert(value);
    }

    tree.remove(&50);

    assert!(!tree.contains(&50));

    for value in [10, 20, 25, 30, 40] {
        assert!(tree.contains(&value));
    }

    assert_eq!(tree.height(), 3);
}


#[test]
fn remove_causes_rr_rebalance() {
    let mut tree = AvlTree::new();

    for value in [30, 20, 40, 10, 35, 50] {
        tree.insert(value);
    }

    tree.remove(&10);

    assert!(!tree.contains(&10));

    for value in [20, 30, 35, 40, 50] {
        assert!(tree.contains(&value));
    }

    assert_eq!(tree.height(), 3);
}


#[test]
fn insert_many_sorted_values() {
    let mut tree = AvlTree::new();

    for value in 1..=100 {
        tree.insert(value);
    }

    for value in 1..=100 {
        assert!(tree.contains(&value));
    }

    // An AVL tree should stay logarithmic rather than becoming
    // a 100-node linked list.
    assert!(tree.height() <= 7);
}


#[test]
fn insert_many_reverse_sorted_values() {
    let mut tree = AvlTree::new();

    for value in (1..=100).rev() {
        tree.insert(value);
    }

    for value in 1..=100 {
        assert!(tree.contains(&value));
    }

    assert!(tree.height() <= 7);
}


#[test]
fn insert_values_in_random_order() {
    let mut tree = AvlTree::new();

    for value in [42, 17, 93, 8, 31, 65, 100, 2, 12, 24, 37, 55, 75] {
        tree.insert(value);
    }

    for value in [42, 17, 93, 8, 31, 65, 100, 2, 12, 24, 37, 55, 75] {
        assert!(tree.contains(&value));
    }

    assert!(tree.height() <= 5);
}


#[test]
fn duplicate_insertion_does_not_change_height() {
    let mut tree = AvlTree::new();

    for value in [10, 5, 15, 3, 7, 12, 20] {
        tree.insert(value);
    }

    let height = tree.height();

    for value in [10, 5, 15, 3, 7, 12, 20] {
        tree.insert(value);
    }

    assert_eq!(tree.height(), height);
}


#[test]
fn remove_same_value_twice() {
    let mut tree = AvlTree::new();

    for value in [10, 5, 15] {
        tree.insert(value);
    }

    tree.remove(&5);
    tree.remove(&5);

    assert!(!tree.contains(&5));
    assert!(tree.contains(&10));
    assert!(tree.contains(&15));
}


#[test]
fn remove_causes_multiple_rebalances() {
    let mut tree = AvlTree::new();

    for value in 1..=15 {
        tree.insert(value);
    }

    for value in [1, 2, 3, 4, 5, 6, 7] {
        tree.remove(&value);
    }

    for value in 8..=15 {
        assert!(tree.contains(&value));
    }

    assert!(tree.height() <= 4);
}

