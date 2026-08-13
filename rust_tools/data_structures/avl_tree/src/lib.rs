
struct Node<T> {
    value: T,
    height: i32,
    left: Option<Box<Node<T>>>,
    right: Option<Box<Node<T>>>,
}

pub struct AvlTree<T> {
    root: Option<Box<Node<T>>>,
}

fn rotate_right<T: Ord>(node: Box<Node<T>>) -> Box<Node<T>> {
    let mut p = node;
    let mut pl = p.left.take().unwrap();
    let plr = pl.right.take();
    p.left = plr;
    update_height(&mut p);
    pl.right = Some(p);
    update_height(&mut pl);
    return pl;
}

fn rotate_left<T: Ord>(node: Box<Node<T>>) -> Box<Node<T>> {
    let mut p = node;
    let mut pr = p.right.take().unwrap();
    let prl = pr.left.take();
    p.right = prl;
    update_height(&mut p);
    pr.left = Some(p);
    update_height(&mut pr);
    return pr;
}

/*
balance > 1
    ├── left child >= 0 → LL → rotate_right(node)
    └── left child <  0 → LR → rotate_left(left), rotate_right(node)

balance < -1
    ├── right child <= 0 → RR → rotate_left(node)
    └── right child >  0 → RL → rotate_right(right), rotate_left(node)
*/
fn rebalance<T: Ord + Clone>(mut node: Box<Node<T>>) -> Box<Node<T>> {
    update_height(&mut node);

    let balance = balance_factor(&node);

    // left heavy
    if balance > 1 {
        // left child won't be None here because tree
        // starting at 'node' is left heavy
        // as_ref works too because rust will dereference Box for you (i.e.deref coercion)
        // but we will use as_deref() for clarity
        if balance_factor(node.left.as_deref().unwrap()) >= 0 {
            // perform LL rotation
            /*          (p)
                     30
                    /  \
            (pl)  20   ...
                 /  \
                10   ... (plr)


                For LL, you have:

                      P
                    /   \
                   PL    PR
                  /  \
                 PLL  PLR

                After right rotation, you need:

                       PL
                     /    \
                    PLL    P
                         /   \
                        PLR   PR
            */
            node = rotate_right(node);
            return node;
        } else {
            // LR rotation

            /*
                For LR, you have:

                      P
                    /   \
                   PL    PR
                  /  \
                 PLL  PLR
                     /   \
                   PLRL  PLRR

                First, rotate PL left:

                            P
                          /   \
                        PLR    PR
                        / \
                       PL  PLRR
                      /  \
                    PLL  PLRL

                Then rotate P right:
                          PLR
                        /      \
                       PL        P
                      /  \      /   \
                    PLL  PLRL  PLRR  PR
            */

            // node is p
            let mut p = node;
            let pl = p.left.take().unwrap();
            

            // left rotation on pl
            p.left = Some(rotate_left(pl));

            // right rotation on p
            let final_plr = rotate_right(p);
            return final_plr;
        }
    }

    // right heavy
    if balance < -1 {
        if balance_factor(node.right.as_deref().unwrap()) <= 0 {
            // perform RR rotation
            /*
                For RR, you have:

                      P
                    /    \
                   PL     PR
                         /   \
                        PRL  PRR

                After left rotation, you need:

                    PR
                   /  \
                 P     PRR
                / \
               PL  PRL
            */
            node = rotate_left(node);
            return node;
        } else {
            // RL rotation
            /*
                For RL, you have:

                      P
                    /   \
                   PL    PR
                        /   \
                       PRL   PRR
                      /   \
                    PRLL   PRLR

                First right rotate on PR:

                      P
                    /   \
                   PL    PRL
                        /    \
                      PRLL    PR
                             /   \
                           PRLR  PRR

                Then rotate P left:

                      PRL
                    /      \
                   P         PR
                 /   \      /   \
                PL   PRLL PRLR  PRR

            */
            let mut p = node;
            let pr = p.right.take().unwrap();
            // let mut prl = pr.left.take().unwrap();
            // let prlr = prl.right.take();

            // right rotate on PR
            p.right = Some(rotate_right(pr));

            // left rotate on P
            let final_prl = rotate_left(p);
            return final_prl;
        }
    }

    node
}

/*
insert
  ↓
update height
  ↓
calculate balance factor
  ↓
┌───────────────┐
│ balanced?     │
└───────┬───────┘
        │
    no  ↓
   rotate
        ↓
   return node
*/
fn insert<T: Ord + Clone>(node: Option<Box<Node<T>>>, value: T) -> Option<Box<Node<T>>> {
    // check for None
    match node {
        Some(mut cur) => {
            if value > cur.value {
                /*
                    Take ownership of cur.right.
                    Give that subtree to the recursive call.
                    Get the modified subtree back.
                    Put it back into cur.right.
                */
                cur.right = insert(cur.right.take(), value);
            } else if value < cur.value {
                cur.left = insert(cur.left.take(), value);
            } else {
                return Some(rebalance(cur));
            }

            // if existing node is found then just return it
            Some(rebalance(cur))
        }

        None => {
            // created new node here for inserting
            return Some(Box::new(Node {
                value,
                height: 1,
                left: None,
                right: None,
            }));
        }
    }
}

fn inorder_predecessor<T: Ord>(node: &Option<Box<Node<T>>>) -> Option<&Node<T>> {
    /*

    You start with:
        &Option<Box<Node<T>>>
        and turn it into:
        &Node<T>
        using
        node.as_deref()?
        Then you can walk down the right side without taking ownership of anything
    */

    let mut cur = node.as_deref()?;
    while let Some(right) = cur.right.as_deref() {
        cur = right;
    }
    Some(cur)
}

fn inorder_successor<T: Ord>(node: &Option<Box<Node<T>>>) -> Option<&Node<T>> {
    let mut cur = node.as_deref()?;
    while let Some(left) = cur.left.as_deref() {
        cur = left;
    }
    Some(cur)
}

fn height<T>(node: &Option<Box<Node<T>>>) -> i32 {
    match node {
        Some(node) => node.height,
        None => 0,
    }
}

fn update_height<T>(node: &mut Box<Node<T>>) {
    node.height = 1 + height(&node.left).max(height(&node.right));
}

// balance > 0  → left-heavy
// balance < 0  → right-heavy
// balance == 0 → perfectly balanced
fn balance_factor<T>(node: &Node<T>) -> i32 {
    height(&node.left) - height(&node.right)
}

// recursively descend
// handle 0/1/2 children
// update height
// rebalance
fn remove<T: Ord + Clone>(root: Option<Box<Node<T>>>, value: &T) -> Option<Box<Node<T>>> {
    // let height = |node: &Option<Box<Node<T>>>| -> i32 {
    //     match node {
    //         Some(val) => val.height,
    //         None => 0
    //     }
    // };

    let mut node = match root {
        Some(node) => node,
        None => return None,
    };

    if *value < node.value {
        node.left = remove(node.left.take(), value);
        return Some(rebalance(node));
    } else if *value > node.value {
        node.right = remove(node.right.take(), value);
        return Some(rebalance(node));
    } else {
        // 0 or 1 child
        if node.left.is_none() {
            return node.right;
        }
        if node.right.is_none() {
            return node.left;
        }

        // two children case
        // pick the one with more height
        if height(&node.left) > height(&node.right) {
            let pred = inorder_predecessor(&node.left);
            node.value = pred.unwrap().value.clone();
            node.left = remove(node.left.take(), &node.value);
        } else {
            let succ = inorder_successor(&node.right);
            node.value = succ.unwrap().value.clone();
            node.right = remove(node.right.take(), &node.value);
        }
        return Some(rebalance(node));
    }
}

impl<T: Ord + Clone> AvlTree<T> {
    pub fn new() -> Self {
        Self { root: None }
    }

    pub fn insert(&mut self, value: T) -> () {
        self.root = insert(self.root.take(), value);
    }

    pub fn height(&self) -> i32 {
        // match &self.root {
        //     Some(node) => node.height,
        //     None => 0,
        // }

        self.root.as_ref().map_or(0, |node| node.height)
    }

    pub fn remove(&mut self, value: &T) {
        self.root = remove(self.root.take(), &value);
    }

    pub fn contains(&self, value: &T) -> bool {
        /*
            as_ref()
            Option<Box<Node>>
                ↓
            Option<&Box<Node>>

            "Give me a reference to the Box."

            while:

            as_deref()
            Option<Box<Node>>
                ↓
            Option<&Node>

            "Give me a reference to what's inside the Box."
        */
        let mut cur = self.root.as_deref();
        while let Some(node) = cur {
            if node.value == *value {
                return true;
            } else if node.value > *value {
                cur = node.left.as_deref();
            } else if node.value < *value {
                cur = node.right.as_deref();
            }
        }

        false
    }
}
