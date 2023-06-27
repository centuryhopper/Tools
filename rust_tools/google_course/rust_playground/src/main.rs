// https://google.github.io/comprehensive-rust/welcome.html
// can only convert smaller int types to larger ones but not vice versa or else overflow can happen
#![allow(unused_variables, dead_code)]


// #[derive(Copy, Clone, Debug)]
/// an entity with two integers and a string name
#[derive(Clone, Debug)]
struct Point(i32, i32, String);


fn multiply(x: i32, y: i32) -> i128
{
    (x * y).into()
}

fn transpose(matrix: [[i32; 3]; 3]) -> [[i32; 3]; 3]
{
    let mut new_matrix: [[i32; 3]; 3] = [[0,0,0];3];
    let (m,n) = (matrix.len(), matrix[0].len());
    for i in 0..m {
        for j in 0..n {
            new_matrix[i][j] = matrix[j][i];
        }
    }

    new_matrix
}

fn pretty_print(matrix: &[[i32; 3]; 3]) 
{
    for array in matrix
    {
        for i in array
        {
            print!("{i} ");
        }
        println!();
    }
}



struct Library {
    books: Vec<Book>,
}

#[derive(Debug)]
struct Book {
    title: String,
    year: u16,
}

impl Book {
    // This is a constructor, used below.
    fn new(title: &str, year: u16) -> Book {
        Book {
            title: String::from(title),
            year,
        }
    }
}

// Implement the methods below. Update the `self` parameter to
// indicate the method's required level of ownership over the object:
//
// - `&self` for shared read-only access,
// - `&mut self` for unique and mutable access,
// - `self` for unique access by value.
impl Library {
    fn new() -> Self {
        Self { books: Vec::new() }
    }

    fn len(&self) -> usize {
       self.books.len()
    }

    fn is_empty(&self) -> bool {
       self.books.is_empty()
    }

    fn add_book(&mut self, book: Book) {
       self.books.push(book)
    }

    fn print_books(&self) {
       for book in &self.books {
        println!("{:?}", book);
       }
    }

    fn oldest_book(&self) -> Option<&Book> {
        self.books
            .iter()
            .min_by_key(|book: &&Book| book.year)
    }
}

struct Foo {
    x: Option<(u32, u32)>,
    y: u32,
}

pub struct User {
    name: String,
    age: u32,
    weight: f32,
}

impl User {
    pub fn new(name: String, age: u32, weight: f32) -> Self {
        User {name:name, age:age, weight:weight}
    }

    pub fn name(&self) -> &str {
        &(self.name)
    }

    pub fn age(&self) -> u32 {
        self.age
    }

    pub fn weight(&self) -> f32 {
        self.weight
    }

    pub fn set_age(&mut self, new_age: u32) {
        self.age = new_age;
    }

    pub fn set_weight(&mut self, new_weight: f32) {
        self.weight = new_weight;
    }
}

#[test]
fn test_weight() {
    let bob = User::new(String::from("Bob"), 32, 155.2);
    assert_eq!(bob.weight(), 155.2);
}

#[test]
fn test_set_age() {
    let mut bob = User::new(String::from("Bob"), 32, 155.2);
    assert_eq!(bob.age(), 32);
    bob.set_age(33);
    assert_eq!(bob.age(), 33);
}


#[rustfmt::skip]
fn main()
{
    let bob = User::new(String::from("Bob"), 32, 155.2);
    println!("I'm {} and my age is {}", bob.name(), bob.age());
    

    // let foo = Foo { x:Some((1,2)), y: 1 };
    // match foo {
    //     Foo { y: 2, x: i }   => println!("y = 2, x = {i:?}"),
    //     Foo { x: Some(yo), y } => println!("{yo:?}, y = {y}"),
    //     Foo { y, .. }        => println!("y = {y}, other fields were ignored"),
    // }


    // let mut library = Library::new();

    // println!("The library is empty: {}", library.is_empty());
    
    // library.add_book(Book::new("Lord of the Rings", 1954));
    // library.add_book(Book::new("Alice's Adventures in Wonderland", 1865));
    
    // println!("The library is no longer empty: {}", library.is_empty());
        
    // match library.oldest_book() {
    //    Some(book) => println!("The oldest book is {}", book.title),
    //    None => println!("The library is empty!"),
    // }
    
    // println!("The library has {} books", library.len());
    // library.print_books();



    // let matrix = [
    //     [101, 102, 103], // <-- the comment makes rustfmt add a newline
    //     [201, 202, 203],
    //     [301, 302, 303],
    // ];

    // println!("matrix:");
    // pretty_print(&matrix);

    // let transposed = transpose(matrix);
    // println!("transposed:");
    // pretty_print(&transposed);


    // let matrix2 = [
    //     [1, 2, 3], // <-- the comment makes rustfmt add a newline
    //     [4, 5, 6],
    //     [7, 8, 9],
    // ];

    // println!("matrix:");
    // pretty_print(&matrix2);

    // let transposed2 = transpose(matrix2);
    // println!("transposed:");
    // pretty_print(&transposed2);


    // // let x: i16 = 15;
    // // let y: i32 = 1000;

    // // println!("{x} * {y} = {}", multiply(x.into(), y.into()));


    

    // let p1 = Point(3, 4, String::from("hi"));
    // let p2 = p1.clone(); // &p1 works too as this means borrowing the value of p1
    // println!("p1: {p1:?}");
    // println!("p2: {p2:?}");




    // let mut a: i32 = 10;
    // let b: &i32 = &a;
    // println!("b: {b}");

    // {
    //     let c: &mut i32 = &mut a;
    //     *c = 20;
    // }

    // println!("a: {a}");
    // // println!("b: {b}");

}





