// https://google.github.io/comprehensive-rust/welcome.html
// can only convert smaller int types to larger ones but not vice versa or else overflow can happen
#![allow(unused_variables, dead_code)]

use std::num;


// #[derive(Copy, Clone, Debug)]
/// an entity with two integers and a string name
#[derive(Clone, Debug)]
struct NamedPoint(i32, i32, String);


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

struct Foo
{
    x: Option<(u32, u32)>,
    y: u32,
}

pub struct User
{
    name: String,
    age: u32,
    weight: f32,
}

impl User
{
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

// #[test]
// fn test_weight()
// {
//     let bob = User::new(String::from("Bob"), 32, 155.2);
//     assert_eq!(bob.weight(), 155.2);
// }

// #[test]
// fn test_set_age()
// {
//     let mut bob = User::new(String::from("Bob"), 32, 155.2);
//     assert_eq!(bob.age(), 32);
//     bob.set_age(33);
//     assert_eq!(bob.age(), 33);
// }

#[derive(Copy, Clone, Debug)]
pub struct Point
{
    x : i32,
    y : i32,
}

impl Point
{
    pub fn new(x:i32, y:i32) -> Self
    {
        Self { x: x, y: y }
    }

    pub fn dist(&self, other_pt: Point) -> f64
    {
        (((other_pt.x-self.x) as f64).powi(2) + ((other_pt.y-self.y) as f64).powi(2)).sqrt()
    }

    pub fn magnitude(&self) -> f64
    {
        ((self.x as f64).powi(2) + (self.y as f64).powi(2)).sqrt()
    }
}

use std::ops::Add;

impl Add<Point> for Point {
    type Output = Point;

    fn add(self, other: Point) -> Point {
        let x = self.x + other.x;
        let y = self.y + other.y;
        Point::new(x, y)
    }
}

impl std::ops::Sub for Point {
    type Output = Point;

    fn sub(self, other: Self) -> Self::Output {
        Self {
            x: self.x - other.x,
            y: self.y - other.y,
        }
    }
}


use std::cmp::PartialEq;

impl PartialEq for Point {
    fn eq(&self, other: &Point) -> bool {
        self.x == other.x && self.y == other.y
    }
}

pub struct Polygon
{
    points: Vec<Point>,
    index: usize,
}

impl Polygon
{
    pub fn new() -> Self
    {
        Self { points: vec![], index: 0 }
    }

    pub fn add_point(&mut self, p: Point)
    {
        self.points.push(p);
    }

    pub fn left_most_point(&self) -> Option<Point>
    {
        self.points.iter().min_by_key(|p| p.x).copied()
    }

    pub fn iter(&self) -> std::slice::Iter<Point> {
        self.points.iter()
    }

    pub fn length(&self) -> f64
    {
        if self.points.is_empty()
        {
            return 0.0
        }

        let mut ans = 0.0;
        let mut curr = self.points[0];
        
        for point in &self.points[1..]
        {
            ans += point.dist(curr);
            curr = *point;
        }

        // connect the first and last points as well
        ans += curr.dist(self.points[0]);
        ans
    }
}

impl Iterator for Polygon
{
    type Item = Point;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index < self.points.len() {
            let item = self.points[self.index];
            self.index += 1;
            Some(item)
        } else {
            None
        }
    }
}


pub struct Circle
{
    center : Point,
    radius: i32,
}

impl Circle
{
    pub fn new(p: Point, r: i32) -> Self
    {
        Self {center:p, radius: r}
    }
    /// 2 * pi * r
    pub fn circumference(&self) -> f64
    {
        2.0 * std::f64::consts::PI * self.radius as f64
    }
}

pub enum Shape {
    Polygon(Polygon),
    Circle(Circle),
}

impl From<Polygon> for Shape {
    fn from(poly: Polygon) -> Self {
        Shape::Polygon(poly)
    }
}

impl From<Circle> for Shape {
    fn from(circle: Circle) -> Self {
        Shape::Circle(circle)
    }
}

impl Shape {
    pub fn perimeter(&self) -> f64
    {
        match self {
            Self::Circle(c) => c.circumference(),
            Self::Polygon(p) => p.length()
        }
    }
}

// #[cfg(test)]
// mod tests {
//     use super::*;

//     fn round_two_digits(x: f64) -> f64 {
//         (x * 100.0).round() / 100.0
//     }

//     #[test]
//     fn test_point_magnitude() {
//         let p1 = Point::new(12 , 13 );
//         assert_eq!(round_two_digits(p1.magnitude()), 17.69);
//     }

//     #[test]
//     fn test_point_dist() {
//         let p1 = Point::new(10, 10);
//         let p2 = Point::new(14, 13);
//         assert_eq!(round_two_digits(p1.dist(p2)), 5.00);
//     }

//     #[test]
//     fn test_point_add() {
//         let p1 = Point::new(16, 16);
//         let p2 = p1 + Point::new(-4, 3);
//         assert_eq!(p2, Point::new(12, 19));
//     }

//     #[test]
//     fn test_polygon_left_most_point() {
//         let p1 = Point::new(12, 13);
//         let p2 = Point::new(16, 16);

//         let mut poly = Polygon::new();
//         poly.add_point(p1);
//         poly.add_point(p2);
//         assert_eq!(poly.left_most_point(), Some(p1));
//     }

//     #[test]
//     fn test_polygon_iter() {
//         let p1 = Point::new(12, 13);
//         let p2 = Point::new(16, 16);

//         let mut poly: Polygon = Polygon::new();
//         poly.add_point(p1);
//         poly.add_point(p2);

//         let points = poly.iter().cloned().collect::<Vec<_>>();
//         assert_eq!(points, vec![Point::new(12, 13), Point::new(16, 16)]);
//     }

//     #[test]
//     fn test_shape_perimeters()
//     {
//         let mut poly = Polygon::new();
//         poly.add_point(Point::new(12, 13));
//         poly.add_point(Point::new(17, 11));
//         poly.add_point(Point::new(16, 16));

//         let shapes = vec![
//             Shape::from(poly),
//             Shape::from(Circle::new(Point::new(10, 20), 5)),
//         ];

//         let perimeters = shapes
//             .iter()
//             .map(Shape::perimeter)
//             .map(round_two_digits)
//             .collect::<Vec<f64>>();
//         assert_eq!(perimeters, vec![15.48, 31.42]);
//     }
// }


pub fn sum_digits<'a>(num: &'a u32) -> u32
{
    let mut ans = 0;
    let mut tmp = *num;

    while tmp > 0 {
        ans += tmp % 10;
        tmp /= 10;
    }

    ans
}


/// The Luhn algorithm is used to validate credit card numbers. The algorithm takes a string as input and does the following to validate the credit card number:
/// Ignore all spaces. Reject number with less than two digits.
/// 
/// Moving from right to left, double every second digit: for the number 1234, we double 3 and 1.
/// 
/// After doubling a digit, sum the digits. So doubling 7 becomes 14 which becomes 5.
/// 
/// Sum all the undoubled and doubled digits.
/// 
/// The credit card number is valid if the sum ends with 0.
pub fn luhn(cc_number: &str) -> bool {
    let mut ret_val : Vec<u32> = vec![];
    let mut digit_counter = 0;
    
    // double every second digit from right to left
    for ch in cc_number.chars().rev()
    {
        // skip spaces
        if !ch.is_digit(10) {
            continue;
        }

        digit_counter +=1;

        if digit_counter % 2 == 0 {
            let mut num = ch as u32 - '0' as u32;
            num *= 2;
            num = sum_digits(&num);
            ret_val.push(num);
        }
        else
        {
            ret_val.push(ch as u32 - '0' as u32);
        }

    }

    // println!("{:?}", ret_val);

    // reject all answers with less than 2 digits
    if ret_val.len() < 2 {
        return false;
    }

    // finally, sum all digits
    let final_val : u32 = ret_val.iter().sum();

    // println!("{final_val}");

    final_val % 10 == 0
}


// #[test]
// fn test_non_digit_cc_number() {
//     assert!(!luhn("foo"));
// }

// #[test]
// fn test_empty_cc_number() {
//     assert!(!luhn(""));
//     assert!(!luhn(" "));
//     assert!(!luhn("  "));
//     assert!(!luhn("    "));
// }

// #[test]
// fn test_single_digit_cc_number() {
//     assert!(!luhn("0"));
// }

// #[test]
// fn test_two_digit_cc_number() {
//     assert!(luhn(" 0 0 "));
// }

// #[test]
// fn test_valid_cc_number() {
//     assert!(luhn("4263 9826 4026 9299"));
//     assert!(luhn("4539 3195 0343 6467"));
//     assert!(luhn("7992 7398 713"));
// }

// #[test]
// fn test_invalid_cc_number() {
//     assert!(!luhn("4223 9826 4026 9299"));
//     assert!(!luhn("4539 3195 0343 6476"));
//     assert!(!luhn("8273 1232 7352 0569"));
// }



/// two pointer approach (O(n) time, where n is max(len(prefix), len(request_path), O(1) space)
/// could also use a vector and split on '/' but that would incur an O(n) space complexity and wouldn't be as space efficient
pub fn prefix_matches(prefix: &str, request_path: &str) -> bool
{
    // request_path must be at least as long as the prefix
    if request_path.chars().count() < prefix.chars().count() {
        return false;
    }

    // compare character by character, the prefix should finish exhausting first
    let n = prefix.chars().count();
    let m = request_path.chars().count();

    let (mut i, mut j) = (0,0);

    while i < n || j < m {

        // already looped thru prefix by this point
        if i == n {
            if m == n
            {
                break;
            }
            // println!("{i}");
            
            let request_path_c = request_path.chars().nth(j).unwrap();
            // println!("{request_path_c}");
            if request_path_c != '/'
            {
                return false;
            }

            return true;
        }

        let request_path_c = request_path.chars().nth(j).unwrap();
        let prefix_c = prefix.chars().nth(i).unwrap();

        // wildcard cases
        if request_path_c == '*' {
            // advance pointers accordingly
            j+=1;
            while i < n && prefix.chars().nth(i).unwrap() != '/' {
                i+=1;
            }
            continue;
        }

        if prefix_c == '*' {
            // advance pointers accordingly
            i+=1;
            while j < m && request_path.chars().nth(j).unwrap() != '/' {
                j+=1;
            }
            continue;
        }

        // basic character comparison case
        if prefix_c != request_path_c {
            return false;
        }

        i+=1;
        j+=1;
    }

    true
}

// #[test]
// fn test_matches_without_wildcard() {
//     assert!(prefix_matches("/v1/publishers", "/v1/publishers"));
//     assert!(prefix_matches("/v1/publishers", "/v1/publishers/abc-123"));
//     assert!(prefix_matches("/v1/publishers", "/v1/publishers/abc/books"));

//     assert!(!prefix_matches("/v1/publishers", "/v1"));
//     assert!(!prefix_matches("/v1/publishers", "/v1/publishersBooks"));
//     assert!(!prefix_matches("/v1/publishers", "/v1/parent/publishers"));
// }

// #[test]
// fn test_matches_with_wildcard() {
//     assert!(prefix_matches(
//         "/v1/publishers/*/books",
//         "/v1/publishers/foo/books"
//     ));
//     assert!(prefix_matches(
//         "/v1/publishers/*/books",
//         "/v1/publishers/bar/books"
//     ));
//     assert!(prefix_matches(
//         "/v1/publishers/*/books",
//         "/v1/publishers/foo/books/book1"
//     ));

//     assert!(!prefix_matches("/v1/publishers/*/books", "/v1/publishers"));
//     assert!(!prefix_matches(
//         "/v1/publishers/*/books",
//         "/v1/publishers/foo/booksByAuthor"
//     ));
// }

#[derive(Debug)]
struct GenericPoint<T,U> {
    x: T,
    y: U,
}

#[allow(dead_code)]
#[rustfmt::skip]
fn main()
{

    let integer = GenericPoint { x: 5, y: 10 };
    let float = GenericPoint { x: 1.0, y: 4.0 };
    
    let p = GenericPoint { x: 5, y: 10.0 };
    println!("{integer:?} and {float:?} and {p:?}");
    // println!("{:?}", prefix_matches("/v1/publishers", "/v1/publishers/abc-123"));

    // let bob = User::new(String::from("Bob"), 32, 155.2);
    // println!("I'm {} and my age is {}", bob.name(), bob.age());
    
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





