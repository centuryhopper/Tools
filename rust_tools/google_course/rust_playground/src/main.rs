// https://google.github.io/comprehensive-rust/welcome.html
// can only convert smaller int types to larger ones but not vice versa or else overflow can happen
#![allow(unused_variables, dead_code)]

use std::collections::HashSet;
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


#[derive(Debug, Default)]
struct Derived<'a> {
    x: u32,
    y: &'a str,
    z: Implemented<'a>,
}

#[derive(Debug)]
struct Implemented<'a>(&'a str);

impl Default for Implemented<'_> {
    fn default() -> Self {
        Self("John Smith".into())
    }
}


// We will have a number of widgets in our library:

//     Window: has a title and contains other widgets.
//     Button: has a label and a callback function which is invoked when the button is pressed.
//     Label: has a label.

// The widgets will implement a Widget trait, see below.

// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// ANCHOR: setup
pub trait Widget {
    /// Natural width of `self`.
    fn width(&self) -> usize;

    /// Draw the widget into a buffer.
    fn draw_into(&self, buffer: &mut dyn std::fmt::Write) -> Result<(), std::fmt::Error>;

    /// Draw the widget on standard output.
    fn draw(&self) {
        let mut buffer = String::new();
        self.draw_into(&mut buffer);
        println!("{buffer}");
    }
}

pub struct Label {
    label: String,
}

impl Label {
    fn new(label: &str) -> Label {
        Label {
            label: label.to_owned(),
        }
    }
}

pub struct Button {
    label: Label,
    callback: Box<dyn FnMut()>,
}

impl Button {
    fn new(label: &str, callback: Box<dyn FnMut()>) -> Button {
        Button {
            label: Label::new(label),
            callback,
        }
    }
}

pub struct Window {
    title: String,
    widgets: Vec<Box<dyn Widget>>,
}

impl Window {
    fn new(title: &str) -> Window {
        Window {
            title: title.to_owned(),
            widgets: Vec::new(),
        }
    }

    fn add_widget(&mut self, widget: Box<dyn Widget>) {
        self.widgets.push(widget);
    }

    fn inner_width(&self) -> usize {
        std::cmp::max(
            self.title.chars().count(),
            self.widgets.iter().map(|w| w.width()).max().unwrap_or(0),
        )
    }
}

// ANCHOR_END: setup

// ANCHOR: Window-width
impl Widget for Window {
    fn width(&self) -> usize {
        // ANCHOR_END: Window-width
        // Add 4 paddings for borders
        self.inner_width() + 4
    }

    // ANCHOR: Window-draw_into
    fn draw_into(&self, buffer: &mut dyn std::fmt::Write) -> Result<(), std::fmt::Error>{
        // ANCHOR_END: Window-draw_into
        let mut inner = String::new();
        for widget in &self.widgets {
            widget.draw_into(&mut inner);
        }

        let inner_width = self.inner_width();

        // TODO: after learning about error handling, you can change
        // draw_into to return Result<(), std::fmt::Error>. Then use
        // the ?-operator here instead of .unwrap().
        writeln!(buffer, "+-{:-<inner_width$}-+", "").unwrap();
        writeln!(buffer, "| {:^inner_width$} |", &self.title).unwrap();
        writeln!(buffer, "+={:=<inner_width$}=+", "").unwrap();
        for line in inner.lines() {
            writeln!(buffer, "| {:inner_width$} |", line).unwrap();
        }
        writeln!(buffer, "+-{:-<inner_width$}-+", "").unwrap();

        Ok(())
    }
}

// ANCHOR: Button-width
impl Widget for Button {
    fn width(&self) -> usize {
        // ANCHOR_END: Button-width
        self.label.width() + 8 // add a bit of padding
    }

    // ANCHOR: Button-draw_into
    fn draw_into(&self, buffer: &mut dyn std::fmt::Write) -> Result<(), std::fmt::Error>{
        // ANCHOR_END: Button-draw_into
        let width = self.width();
        let mut label = String::new();
        self.label.draw_into(&mut label);

        writeln!(buffer, "+{:-<width$}+", "").unwrap();
        for line in label.lines() {
            writeln!(buffer, "|{:^width$}|", &line).unwrap();
        }
        writeln!(buffer, "+{:-<width$}+", "").unwrap();

        Ok(())
    }
}

// ANCHOR: Label-width
impl Widget for Label {
    fn width(&self) -> usize {
        // ANCHOR_END: Label-width
        self.label
            .lines()
            .map(|line| line.chars().count())
            .max()
            .unwrap_or(0)
    }

    // ANCHOR: Label-draw_into
    fn draw_into(&self, buffer: &mut dyn std::fmt::Write) -> Result<(), std::fmt::Error> {
        // ANCHOR_END: Label-draw_into
        writeln!(buffer, "{}", &self.label)?;

        Ok(())
    }
}


// structured error handling
use std::{fs, io};
use std::io::Read;



fn read_username(path: &str) -> Result<String, io::Error> {
    let username_file_result = fs::File::open(path);
    let mut username_file = match username_file_result {
        Ok(file) => file,
        Err(err) => return Err(err),
    };

    let mut username = String::new();
    match username_file.read_to_string(&mut username) {
        Ok(_) => Ok(username),
        Err(err) => Err(err),
    }
}



mod ffi {
    use std::os::raw::{c_char, c_int};
    #[cfg(not(target_os = "macos"))]
    use std::os::raw::{c_long, c_ulong, c_ushort, c_uchar};

    // Opaque type. See https://doc.rust-lang.org/nomicon/ffi.html.
    #[repr(C)]
    pub struct DIR {
        _data: [u8; 0],
        _marker: core::marker::PhantomData<(*mut u8, core::marker::PhantomPinned)>,
    }

    // Layout according to the Linux man page for readdir(3), where ino_t and
    // off_t are resolved according to the definitions in
    // /usr/include/x86_64-linux-gnu/{sys/types.h, bits/typesizes.h}.
    #[cfg(not(target_os = "macos"))]
    #[repr(C)]
    pub struct dirent {
        pub d_ino: c_ulong,
        pub d_off: c_long,
        pub d_reclen: c_ushort,
        pub d_type: c_uchar,
        pub d_name: [c_char; 256],
    }

    // Layout according to the macOS man page for dir(5).
    #[cfg(all(target_os = "macos"))]
    #[repr(C)]
    pub struct dirent {
        pub d_fileno: u64,
        pub d_seekoff: u64,
        pub d_reclen: u16,
        pub d_namlen: u16,
        pub d_type: u8,
        pub d_name: [c_char; 1024],
    }

    extern "C" {
        pub fn opendir(s: *const c_char) -> *mut DIR;

        #[cfg(not(all(target_os = "macos", target_arch = "x86_64")))]
        pub fn readdir(s: *mut DIR) -> *const dirent;

        // See https://github.com/rust-lang/libc/issues/414 and the section on
        // _DARWIN_FEATURE_64_BIT_INODE in the macOS man page for stat(2).
        //
        // "Platforms that existed before these updates were available" refers
        // to macOS (as opposed to iOS / wearOS / etc.) on Intel and PowerPC.
        #[cfg(all(target_os = "macos", target_arch = "x86_64"))]
        #[link_name = "readdir$INODE64"]
        pub fn readdir(s: *mut DIR) -> *const dirent;

        pub fn closedir(s: *mut DIR) -> c_int;
    }
}

use std::ffi::{CStr, CString, OsStr, OsString};
use std::os::unix::ffi::OsStrExt;
use ffi::readdir;

#[derive(Debug)]
struct DirectoryIterator {
    path: CString,
    dir: *mut ffi::DIR,
}

impl DirectoryIterator {
    fn new(path: &str) -> Result<DirectoryIterator, String> {
        // Call opendir and return a Ok value if that worked,
        // otherwise return Err with a message.
        let path = CString::new(path).map_err(|err| format!("Invalid path: {err}"))?;

        let dir = unsafe { ffi::opendir(path.as_ptr()) };

        // null check
        if dir.is_null() {
            Err(format!("Could not open {:?}", path))
        } else {
            Ok(DirectoryIterator { path, dir })
        }
    }
}

impl Iterator for DirectoryIterator {
    type Item = OsString;
    fn next(&mut self) -> Option<OsString> {
        // Keep calling readdir until we get a NULL pointer back.
        let dir_entry = unsafe { ffi::readdir(self.dir) };
        if dir_entry.is_null()
        {
            return None;
        }

        let d_name = unsafe { CStr::from_ptr((*dir_entry).d_name.as_ptr()) };
        let os_str = OsStr::from_bytes(d_name.to_bytes());
        Some(os_str.to_owned())
    }
}

impl Drop for DirectoryIterator {
    fn drop(&mut self) {
        // Call closedir as needed.
        if !self.dir.is_null() {
            // SAFETY: self.dir is not NULL.
            if unsafe { ffi::closedir(self.dir) } != 0 {
                panic!("Could not close {:?}", self.path);
            }
        }
    }
}



use std::thread;
use std::time::Duration;
use std::sync::{mpsc, Arc, Mutex};



struct Fork;

#[derive(Clone)]
struct Philosopher {
    name: String,
    left_fork: Arc<Mutex<Fork>>,
    right_fork: Arc<Mutex<Fork>>,
    thoughts: mpsc::SyncSender<String>
}

impl Philosopher {
    fn think(&self) -> () {
        self.thoughts
            .send(format!("{} has a new idea!", &self.name))
            .unwrap();
    }

    fn eat(&self) -> () {
        println!("{} is trying to eat", &self.name);
        // Pick up forks...
        let left = self.left_fork.lock().unwrap();
        let right = self.right_fork.lock().unwrap();

        println!("{} is eating...", &self.name);
        thread::sleep(Duration::from_millis(10));
    }
}

static PHILOSOPHERS: &[&str] =
    &["Socrates", "Plato", "Aristotle", "Thales", "Pythagoras"];

/*
   . | . | . | . | . |


   . = philosopher
   | = fork

   philosopher MUST pick up both forks to eat

 */



 use reqwest::blocking::{get, Response};
 use reqwest::Url;
 use scraper::{Html, Selector};
 use thiserror::Error;
 
 #[derive(Error, Debug)]
 enum Error {
     #[error("request error: {0}")]
     ReqwestError(#[from] reqwest::Error),
 }
 
 fn extract_links(response: Response) -> Result<Vec<Url>, Error> {
     let base_url = response.url().to_owned();
     let document = response.text()?;
     let html = Html::parse_document(&document);
     let selector = Selector::parse("a").unwrap();
 
     let mut valid_urls = Vec::new();
     for element in html.select(&selector) {
         if let Some(href) = element.value().attr("href") {
             match base_url.join(href) {
                 Ok(url) => valid_urls.push(url),
                 Err(err) => {
                     println!("On {base_url}: could not parse {href:?}: {err} (ignored)",);
                 }
             }
         }
     }
 
     Ok(valid_urls)
 }
 


/*

Links: [
    "www.google.org/",
    "www.google.org/",
    "www.google.org/covid-19/",
    "www.google.org/our-work/",
    "www.google.org/our-approach/",
    "www.google.org/opportunities/",
    "www.google.org/latest/",
    "www.google.org/",
    "cybersecurityclinics.org/",
    "www.google.org/racial-justice/",
    "www.google.org/covid-19/",
    "www.google.org/our-approach/",
    "twitter.com/Googleorg",
    "www.youtube.com/user/Googleorg",
    "www.google.org/",
    "www.google.org/covid-19/",
    "www.google.org/our-work/",
    "www.google.org/our-approach/",
    "www.google.org/opportunities/",
    "www.google.com/nonprofits/",
    "edu.google.com/",
    "grow.google/",
    "sustainability.google/",
    "crisisresponse.google/",
    "ai.google/",
    "newsinitiative.withgoogle.com/",
    "www.google.com/",
    "policies.google.com/privacy",
    "policies.google.com/terms",
    "support.google.com/",
] 

 */

 use futures::executor::block_on;
 
 
 async fn count_to(count: i32) {
     for i in 1..=count {
         println!("Count is: {i}!");
         thread::sleep(Duration::from_millis(500));
     }
 }
 
 async fn async_main(count: i32) {
     count_to(count).await;
 }
 
//  fn main() {
//      println!("hi");
//      block_on(async_main(10));
//      println!("bye");
//  }




#[allow(dead_code)]
#[rustfmt::skip]
fn main() /*-> Result<(), String>*/
{
    let start_url = Url::parse("https://www.google.org").unwrap();
    let response = get(start_url).unwrap();
    let save_start = response.url().clone();
    println!("{}",save_start);
    match extract_links(response) {
        Ok(links) => {
            // remove dupes
            let new_links = links.iter().map(|url| {
                format!("https://{}{}",url.host().unwrap().to_owned() ,url.path())
            }).filter(|url| url != save_start.as_str()).collect::<HashSet<_>>();

            println!("Links: {new_links:#?} \n number of links: {:?}", new_links.len());

        }
        Err(err) => println!("Could not extract links: {err:#}"),
    }










    // let (tx, rx) = mpsc::sync_channel(10);

    // // Create forks (should be number of philosophers since dining table wraps around)
    // let forks = (0..PHILOSOPHERS.len()).map(|_| Arc::new(Mutex::new(Fork))).collect::<Vec<_>>();

    // // Create philosophers
    // let mut philosophers = vec![];
    // for (i, &p) in PHILOSOPHERS.into_iter().enumerate()
    // {   
    //     let mut left = forks[i].clone();
    //     let mut right = forks[(i+1) % forks.len()].clone();
    //     if i == forks.len() - 1 {
    //         std::mem::swap(&mut left, &mut right);
    //     }
    //     philosophers.push(
    //         Philosopher
    //         {
    //             name: p.to_owned(),
    //             left_fork: left,
    //             right_fork: right,
    //             thoughts: tx.clone(),
    //         }
    //     );
    // }
    // println!("{:?}", PHILOSOPHERS);

    // let handles = philosophers.into_iter().map(|phil| {
    //     thread::spawn(move || {
    //         for _ in 0..100 {
    //             phil.eat();
    //             phil.think();
    //             println!("++++++++++++++++++++++++++++");
    //         }
    //     })
    // }).collect::<Vec<_>>();

    // // Output their thoughts
    // drop(tx);
    // for msg in rx.iter() {
    //     println!("{msg}");
    // }
    
    
    // // clean up
    // for handle in handles {
    //     handle.join().unwrap();
    // }






















    // let iter = DirectoryIterator::new(".")?;
    // println!("files: {:#?}", iter.collect::<Vec<_>>());
    // Ok(())

    // thread::spawn(|| {
    //     for i in 1..10 {
    //         println!("Count in thread: {i}!");
    //         thread::sleep(Duration::from_millis(5));
    //     }
    // });

    // for i in 1..5 {
    //     println!("Main thread: {i}");
    //     thread::sleep(Duration::from_millis(5));
    // }

    // println!("{}", whoami::username());
    //fs::write("config.dat", "alice").unwrap();
    // let username = read_username("config.dat");
    // println!("username or error: {username:?}");

    // let file = fs::File::open("diary.txt");
    // match file {
    //     Ok(mut file) => {
    //         let mut contents = String::new();
    //         file.read_to_string(&mut contents);
    //         println!("Dear diary: {contents}");
    //     },
    //     Err(err) => {
    //         println!("The diary could not be opened: {err}");
    //     }
    // }
    // let mut window = Window::new("Rust GUI Demo 1.23");
    // window.add_widget(Box::new(Label::new("This is a small text GUI demo.")));
    // window.add_widget(Box::new(Button::new(
    //     "Click me!",
    //     Box::new(|| println!("You clicked the button!")),
    // )));
    // window.draw();

    // The output of the above program can be something simple like this:

    // ========
    // Rust GUI Demo 1.23
    // ========
    
    // This is a small text GUI demo.
    
    // | Click me! |




    // let default_struct: Derived = Default::default();
    // println!("{default_struct:#?}");

    // let almost_default_struct = Derived {
    //     y: "Y is set!".into(),
    //     ..Default::default()
    // };
    // println!("{almost_default_struct:#?}");

    // let nothing: Option<Derived> = None;
    // println!("{:#?}", nothing.unwrap_or_default());

    // let integer = GenericPoint { x: 5, y: 10 };
    // let float = GenericPoint { x: 1.0, y: 4.0 };
    
    // let p = GenericPoint { x: 5, y: 10.0 };

    // let test = &p;
    // let test2 = &test;
    // println!("{integer:?} and {float:?} and {p:?} and {:p} and {:p}", test, test2);
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





