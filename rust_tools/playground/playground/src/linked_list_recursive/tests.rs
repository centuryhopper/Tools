use super::*; // everything from linked_list_recursive, private items included

use std::rc::Rc;

// ---------- invariant checker ----------

/// Checks everything that must always be true about the list.
fn check_invariants(list: &LinkedList) {
    let v = list.to_vec();
    assert_eq!(list.size, v.len(), "size doesn't match number of nodes");
    assert_eq!(list.front(), v.first().copied(), "front() wrong");
    assert_eq!(list.back(), v.last().copied(), "back() wrong");

    match (&list.head, &list.tail) {
        (None, None) => assert_eq!(list.size, 0),
        (Some(_), Some(tail)) => {
            assert!(tail.borrow().next.is_none(), "tail.next must be None");

            // walk from head and make sure the last node IS the tail node
            let mut cur = list.head.clone();
            let mut last = None;
            while let Some(node) = cur {
                cur = node.borrow().next.clone();
                last = Some(node);
            }
            assert!(
                Rc::ptr_eq(last.as_ref().unwrap(), tail),
                "tail isn't the last node"
            );
        }
        _ => panic!("head and tail must both be Some or both be None"),
    }
}

// ---------- every position ----------

#[test]
fn push_at_every_position() {
    let base = [10, 20, 30, 40, 50];
    for idx in 0..=base.len() {
        let mut list = list_from(&base);
        list.push_at(idx, -1);

        let mut expected = base.to_vec();
        expected.insert(idx, -1);

        assert_eq!(list.to_vec(), expected, "push_at({idx}) wrong");
        check_invariants(&list);
    }
}

#[test]
fn erase_at_every_position() {
    let base = [10, 20, 30, 40, 50];
    for idx in 0..base.len() {
        let mut list = list_from(&base);
        list.erase_at(idx);

        let mut expected = base.to_vec();
        expected.remove(idx);

        assert_eq!(list.to_vec(), expected, "erase_at({idx}) wrong");
        check_invariants(&list);
    }
}

#[test]
fn erase_front_until_empty_checking_each_step() {
    let mut list = list_from(&[1, 2, 3, 4, 5]);
    let mut expected = vec![1, 2, 3, 4, 5];
    while !expected.is_empty() {
        list.erase_front();
        expected.remove(0);
        assert_eq!(list.to_vec(), expected);
        check_invariants(&list);
    }
}

#[test]
fn erase_back_until_empty_checking_each_step() {
    let mut list = list_from(&[1, 2, 3, 4, 5]);
    let mut expected = vec![1, 2, 3, 4, 5];
    while !expected.is_empty() {
        list.erase_back();
        expected.pop();
        assert_eq!(list.to_vec(), expected);
        check_invariants(&list);
    }
}

#[test]
fn erase_at_zero_repeatedly_empties_list() {
    let mut list = list_from(&[1, 2, 3]);
    for _ in 0..3 {
        list.erase_at(0);
        check_invariants(&list);
    }
    assert_eq!(list.size, 0);
}

#[test]
fn build_list_only_with_push_at() {
    let mut list = LinkedList::new();
    list.push_at(0, 2); // [2]
    list.push_at(0, 0); // [0, 2]
    list.push_at(1, 1); // [0, 1, 2]
    list.push_at(3, 3); // [0, 1, 2, 3]
    assert_eq!(list.to_vec(), vec![0, 1, 2, 3]);
    check_invariants(&list);
}

// ---------- values ----------

#[test]
fn extreme_values() {
    let list = list_from(&[i32::MIN, 0, i32::MAX, -1]);
    assert_eq!(list.to_vec(), vec![i32::MIN, 0, i32::MAX, -1]);
    assert_eq!(list.find(i32::MIN), 0);
    assert_eq!(list.find(i32::MAX), 2);
    assert_eq!(list.find(-1), 3);
}

#[test]
fn duplicates_are_kept_in_order() {
    let mut list = list_from(&[7, 7, 7]);
    list.push_at(1, 7);
    assert_eq!(list.to_vec(), vec![7, 7, 7, 7]);
    list.erase_at(2);
    assert_eq!(list.to_vec(), vec![7, 7, 7]);
    check_invariants(&list);
}

#[test]
fn find_last_element() {
    let list = list_from(&[3, 1, 4, 1, 5]);
    assert_eq!(list.find(5), 4);
}

// ---------- memory: no leaks, no cycles ----------

#[test]
fn clear_frees_nodes() {
    let mut list = list_from(&[1, 2, 3]);
    let first = list.head.clone().unwrap(); // our own extra pointer to node 1
    let last = list.tail.clone().unwrap(); // and to node 3

    list.clear();

    // if the list let go of everything, only our clones remain
    assert_eq!(Rc::strong_count(&first), 1);
    assert_eq!(Rc::strong_count(&last), 1);
}

#[test]
fn erase_at_frees_removed_node() {
    let mut list = list_from(&[1, 2, 3]);
    let middle = list.head.as_ref().unwrap().borrow().next.clone().unwrap();

    list.erase_at(1);

    assert_eq!(
        Rc::strong_count(&middle),
        1,
        "list still holds the erased node"
    );
    assert!(
        middle.borrow().next.is_none(),
        "erased node still points into the list"
    );
}

#[test]
fn erase_back_frees_old_tail() {
    let mut list = list_from(&[1, 2, 3]);
    let old_tail = list.tail.clone().unwrap();

    list.erase_back();

    assert_eq!(Rc::strong_count(&old_tail), 1);
    check_invariants(&list);
}

// ---------- randomized model test ----------

/// Tiny deterministic random number generator, so no `rand` crate is needed.
struct Lcg(u64);
impl Lcg {
    fn next(&mut self, max: usize) -> usize {
        self.0 = self
            .0
            .wrapping_mul(6364136223846793005)
            .wrapping_add(1442695040888963407);
        ((self.0 >> 33) as usize) % max
    }
}

#[test]
fn random_operations_match_vec() {
    let mut rng = Lcg(12345);
    let mut list = LinkedList::new();
    let mut model: Vec<i32> = Vec::new();

    for step in 0..5000 {
        let value = rng.next(1000) as i32 - 500;
        let op = if model.is_empty() {
            rng.next(3)
        } else {
            rng.next(6)
        };

        match op {
            0 => {
                list.push_back(value);
                model.push(value);
            }
            1 => {
                list.push_front(value);
                model.insert(0, value);
            }
            2 => {
                let idx = rng.next(model.len() + 1);
                list.push_at(idx, value);
                model.insert(idx, value);
            }
            3 => {
                list.erase_front();
                model.remove(0);
            }
            4 => {
                list.erase_back();
                model.pop();
            }
            _ => {
                let idx = rng.next(model.len());
                list.erase_at(idx);
                model.remove(idx);
            }
        }

        assert_eq!(list.to_vec(), model, "mismatch at step {step} (op {op})");
        check_invariants(&list);
    }
}

// ---------- big list ----------

#[test]
fn clear_large_list_does_not_overflow_stack() {
    std::thread::Builder::new()
        .stack_size(16 * 1024 * 1024 * 1024) // 64 MB instead of the default 2 MB
        .spawn(|| {
            let mut list = LinkedList::new();
            for i in 0..50_000_000 {
                list.push_front(i);
            }
            list.clear();
            assert_eq!(list.size, 0);
        })
        .unwrap() // the thread was created
        .join() // wait for it to finish
        .unwrap(); // re-raise any panic, so a failed assert still fails the test
}

#[test]
fn new_list_is_empty() {
    let list = LinkedList::new();
    assert_eq!(list.size, 0);
}

#[test]
fn list_clear_test() {
    let mut list = LinkedList::new();
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    list.clear();

    assert_eq!(list.size, 0);
    assert_eq!(list.to_vec(), vec![]);
}

#[test]
fn list_push_back_test() {
    let mut list = LinkedList::new();
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    assert_eq!(list.to_vec(), (1..=5).collect::<Vec<_>>());
}

#[test]
fn list_push_front_test() {
    let mut list = LinkedList::new();
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    list.push_front(4);
    list.push_front(5);

    assert_eq!(list.to_vec(), (1..=5).rev().collect::<Vec<_>>());
}

#[test]
fn list_find() {
    let mut list = LinkedList::new();
    for i in 0..10 {
        list.push_back(i);
    }
    assert_eq!(list.find(0), 0);
    assert_eq!(list.find(2), 2);
    assert_eq!(list.find(5), 5);
    assert_eq!(list.find(9), 9);
    //println!("list size: {}", list.size);
    assert_eq!(list.find(-1), list.size);
}

#[test]
fn list_push_at() {
    let mut list = LinkedList::new();
    for i in (0..=20).step_by(2) {
        list.push_back(i);
    }

    for (idx, value) in vec![(4, -1), (5, -3), (3, -2)] {
        list.push_at(idx, value);
    }

    assert_eq!(
        list.to_vec(),
        vec![0, 2, 4, -2, 6, -1, -3, 8, 10, 12, 14, 16, 18, 20]
    );
}

#[test]
fn list_erase_front() {
    let mut list = LinkedList::new();
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    list.push_front(4);
    list.push_front(5);

    list.erase_front();
    assert_eq!(list.front(), Some(4));
    list.erase_front();
    assert_eq!(list.front(), Some(3));
    list.erase_front();
    assert_eq!(list.front(), Some(2));

    list.clear();

    assert_eq!(list.front(), None);
}

#[test]
fn list_erase_back() {
    let mut list = LinkedList::new();
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    list.erase_back();
    assert_eq!(list.back(), Some(4));
    list.erase_back();
    assert_eq!(list.back(), Some(3));
    list.erase_back();
    assert_eq!(list.back(), Some(2));

    list.clear();

    assert_eq!(list.back(), None);
}

#[test]
fn list_erase_at() {
    let mut list = LinkedList::new();
    for i in (0..=20).step_by(2) {
        list.push_back(i);
    }

    for idx in [4, 5, 3] {
        list.erase_at(idx);
    }

    assert_eq!(list.to_vec(), vec![0, 2, 4, 10, 14, 16, 18, 20]);
}

// ---------- helpers ----------

fn list_from(values: &[i32]) -> LinkedList {
    let mut list = LinkedList::new();
    for &v in values {
        list.push_back(v);
    }
    list
}

// ---------- new / empty ----------

#[test]
fn new_list_front_back_are_none() {
    let list = LinkedList::new();
    assert_eq!(list.front(), None);
    assert_eq!(list.back(), None);
    assert_eq!(list.to_vec(), Vec::<i32>::new());
}

#[test]
fn clear_on_empty_list() {
    let mut list = LinkedList::new();
    list.clear();
    assert_eq!(list.size, 0);
    assert_eq!(list.to_vec(), Vec::<i32>::new());
}

#[test]
fn list_reusable_after_clear() {
    let mut list = list_from(&[1, 2, 3]);
    list.clear();
    list.push_back(7);
    list.push_front(6);
    assert_eq!(list.to_vec(), vec![6, 7]);
    assert_eq!(list.front(), Some(6));
    assert_eq!(list.back(), Some(7));
    assert_eq!(list.size, 2);
}

// ---------- push_back / push_front ----------

#[test]
fn single_push_back_sets_front_and_back() {
    let mut list = LinkedList::new();
    list.push_back(42);
    assert_eq!(list.front(), Some(42));
    assert_eq!(list.back(), Some(42));
    assert_eq!(list.size, 1);
}

#[test]
fn single_push_front_sets_front_and_back() {
    let mut list = LinkedList::new();
    list.push_front(42);
    assert_eq!(list.front(), Some(42));
    assert_eq!(list.back(), Some(42));
    assert_eq!(list.size, 1);
}

#[test]
fn mixed_push_front_and_back() {
    let mut list = LinkedList::new();
    list.push_back(2);
    list.push_front(1);
    list.push_back(3);
    list.push_front(0);
    assert_eq!(list.to_vec(), vec![0, 1, 2, 3]);
    assert_eq!(list.front(), Some(0));
    assert_eq!(list.back(), Some(3));
    assert_eq!(list.size, 4);
}

#[test]
fn push_back_after_push_front_updates_tail() {
    let mut list = LinkedList::new();
    list.push_front(1);
    list.push_back(2);
    assert_eq!(list.back(), Some(2));
    assert_eq!(list.to_vec(), vec![1, 2]);
}

// ---------- push_at ----------

#[test]
fn push_at_zero_on_empty_list() {
    let mut list = LinkedList::new();
    list.push_at(0, 9);
    //println!("{:?}", list.to_vec());
    assert_eq!(list.to_vec(), vec![9]);
    assert_eq!(list.front(), Some(9));
    assert_eq!(list.back(), Some(9));
    assert_eq!(list.size, 1);
}

#[test]
fn push_at_zero_inserts_at_front() {
    let mut list = list_from(&[10, 20, 30]);
    list.push_at(0, 5);
    assert_eq!(list.to_vec(), vec![5, 10, 20, 30]);
    assert_eq!(list.front(), Some(5));
    assert_eq!(list.size, 4);
}

#[test]
fn push_at_size_appends_and_updates_tail() {
    let mut list = list_from(&[10, 20, 30]);
    list.push_at(3, 40);
    assert_eq!(list.to_vec(), vec![10, 20, 30, 40]);
    assert_eq!(list.back(), Some(40));
    assert_eq!(list.size, 4);
}

#[test]
fn push_at_last_index_goes_before_last_element() {
    let mut list = list_from(&[10, 20, 30]);
    list.push_at(2, 25);
    assert_eq!(list.to_vec(), vec![10, 20, 25, 30]);
    assert_eq!(list.back(), Some(30));
}

#[test]
fn push_at_then_push_back_keeps_order() {
    let mut list = list_from(&[1, 3]);
    list.push_at(1, 2);
    list.push_back(4);
    assert_eq!(list.to_vec(), vec![1, 2, 3, 4]);
    assert_eq!(list.size, 4);
}

#[test]
#[should_panic]
fn push_at_past_size_panics() {
    let mut list = list_from(&[1, 2, 3]);
    list.push_at(4, 99);
}

// ---------- erase_front / erase_back ----------

#[test]
fn erase_front_single_element_empties_list() {
    let mut list = list_from(&[1]);
    list.erase_front();
    assert_eq!(list.size, 0);
    assert_eq!(list.front(), None);
    assert_eq!(list.back(), None);
    assert_eq!(list.to_vec(), Vec::<i32>::new());
}

#[test]
fn erase_back_single_element_empties_list() {
    let mut list = list_from(&[1]);
    list.erase_back();
    assert_eq!(list.size, 0);
    assert_eq!(list.front(), None);
    assert_eq!(list.back(), None);
}

#[test]
fn erase_back_then_push_back() {
    let mut list = list_from(&[1, 2, 3]);
    list.erase_back();
    list.push_back(9);
    assert_eq!(list.to_vec(), vec![1, 2, 9]);
    assert_eq!(list.back(), Some(9));
}

#[test]
fn erase_front_then_push_front() {
    let mut list = list_from(&[1, 2, 3]);
    list.erase_front();
    list.push_front(9);
    assert_eq!(list.to_vec(), vec![9, 2, 3]);
    assert_eq!(list.front(), Some(9));
}

#[test]
fn erase_everything_from_back_then_reuse() {
    let mut list = list_from(&[1, 2, 3]);
    list.erase_back();
    list.erase_back();
    list.erase_back();
    assert_eq!(list.size, 0);
    list.push_back(5);
    assert_eq!(list.to_vec(), vec![5]);
    assert_eq!(list.front(), Some(5));
    assert_eq!(list.back(), Some(5));
}

#[test]
fn erase_front_and_back_alternating() {
    let mut list = list_from(&[1, 2, 3, 4, 5]);
    list.erase_front();
    list.erase_back();
    assert_eq!(list.to_vec(), vec![2, 3, 4]);
    list.erase_front();
    list.erase_back();
    assert_eq!(list.to_vec(), vec![3]);
    assert_eq!(list.front(), Some(3));
    assert_eq!(list.back(), Some(3));
    assert_eq!(list.size, 1);
}

// ---------- erase_at ----------

#[test]
fn erase_at_zero_removes_front() {
    let mut list = list_from(&[1, 2, 3]);
    list.erase_at(0);
    assert_eq!(list.to_vec(), vec![2, 3]);
    assert_eq!(list.front(), Some(2));
    assert_eq!(list.size, 2);
}

#[test]
fn erase_at_last_index_updates_tail() {
    let mut list = list_from(&[1, 2, 3]);
    list.erase_at(2);
    assert_eq!(list.to_vec(), vec![1, 2]);
    assert_eq!(list.back(), Some(2));
    assert_eq!(list.size, 2);
}

#[test]
fn erase_at_last_then_push_back() {
    let mut list = list_from(&[1, 2, 3]);
    list.erase_at(2);
    list.push_back(4);
    assert_eq!(list.to_vec(), vec![1, 2, 4]);
}

#[test]
fn erase_at_only_element() {
    let mut list = list_from(&[7]);
    list.erase_at(0);
    assert_eq!(list.size, 0);
    assert_eq!(list.front(), None);
    assert_eq!(list.back(), None);
}

#[test]
fn erase_at_middle_updates_size() {
    let mut list = list_from(&[1, 2, 3, 4]);
    list.erase_at(1);
    print_list(&list);
    assert_eq!(list.to_vec(), vec![1, 3, 4]);
    assert_eq!(list.size, 3);
}

#[test]
fn push_at_then_erase_at_restores_list() {
    let mut list = list_from(&[1, 2, 3]);
    list.push_at(1, 99);
    list.erase_at(1);
    assert_eq!(list.to_vec(), vec![1, 2, 3]);
    assert_eq!(list.size, 3);
}

// ---------- find ----------

#[test]
fn find_on_empty_list_returns_size() {
    let list = LinkedList::new();
    assert_eq!(list.find(1), 0);
}

#[test]
fn find_returns_first_occurrence() {
    let list = list_from(&[5, 7, 5, 7]);
    assert_eq!(list.find(5), 0);
    assert_eq!(list.find(7), 1);
}

#[test]
fn find_after_push_front() {
    let mut list = list_from(&[1, 2]);
    list.push_front(0);
    assert_eq!(list.find(0), 0);
    assert_eq!(list.find(2), 2);
}

fn print_list(list: &LinkedList) {
    println!("list: {:?}", list.to_vec());
}

#[test]
fn find_after_erase() {
    let mut list = list_from(&[1, 2, 3]);
    list.erase_at(1);
    //print_list(&list);
    //println!("find(2): {}", list.find(2));
    assert_eq!(list.find(2), list.size);
    assert_eq!(list.find(3), 1);
}

// ---------- larger lists ----------

#[test]
fn many_push_backs_keep_order_and_size() {
    let list = list_from(&(0..1000).collect::<Vec<_>>());
    assert_eq!(list.size, 1000);
    assert_eq!(list.front(), Some(0));
    assert_eq!(list.back(), Some(999));
    assert_eq!(list.find(500), 500);
}
