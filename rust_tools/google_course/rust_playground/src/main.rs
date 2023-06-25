// https://google.github.io/comprehensive-rust/welcome.html
// can only convert smaller int types to larger ones but not vice versa or else overflow can happen
#![allow(unused_variables, dead_code)]


fn multiply(x: i32, y: i32) -> i128
{
    (x * y).into()
}

fn transpose(matrix: [[i32; 3]; 3]) -> [[i32; 3]; 3]
{
    let mut newMatrix: [[i32; 3]; 3] = [[0,0,0];3];
    let (m,n) = (matrix.len(), matrix[0].len());
    for i in 0..m {
        for j in 0..n {
            newMatrix[i][j] = matrix[j][i];
        }
    }

    newMatrix
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

fn main() {
    let matrix = [
        [101, 102, 103], // <-- the comment makes rustfmt add a newline
        [201, 202, 203],
        [301, 302, 303],
    ];

    println!("matrix:");
    pretty_print(&matrix);

    let transposed = transpose(matrix);
    println!("transposed:");
    pretty_print(&transposed);


    let matrix2 = [
        [1, 2, 3], // <-- the comment makes rustfmt add a newline
        [4, 5, 6],
        [7, 8, 9],
    ];

    println!("matrix:");
    pretty_print(&matrix2);

    let transposed2 = transpose(matrix2);
    println!("transposed:");
    pretty_print(&transposed2);


    // let x: i16 = 15;
    // let y: i32 = 1000;

    // println!("{x} * {y} = {}", multiply(x.into(), y.into()));
}





