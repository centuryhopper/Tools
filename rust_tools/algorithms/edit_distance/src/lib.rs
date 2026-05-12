use pyo3::prelude::*;

#[pymodule]
fn edit_distance(m: &Bound<'_, PyModule>) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(min_distance, m)?)?;
    Ok(())
}


#[pyfunction]
fn min_distance(word1: String, word2: String) -> PyResult<i32> {
    Ok(min_distance_impl(word1, word2))
}


/*
      '' r o s
   ''  0 1 2 3
    h  1 1 2 3
    o  2 2 1 2
    r  3 2 2 2
    s  4 3 3 2
    e  5 4 4 3
*/

pub fn min_distance_impl(word1: String, word2: String) -> i32 {

    let word1_chars: Vec<char> = word1.chars().collect();
    let word2_chars: Vec<char> = word2.chars().collect();
    // unicode safe way to get string length
    let m = word1_chars.len();
    let n = word2_chars.len();
    let mut dp : Vec<Vec<i32>> = vec![vec![0; n+1]; m+1];

    for i in 0..=m
    {
        // print!("{} ", word1_chars[i]);
        dp[i][0] = i as i32;
    }
    // println!();

    for i in 0..=n
    {
        // print!("{} ", word2_chars[i]);
        dp[0][i] = i as i32;
    }
    // println!("{:?}", dp);

    for i in 1..=m
    {
        for j in 1..=n
        {
            if word1_chars[i-1] == word2_chars[j-1]
            {
                dp[i][j] = dp[i-1][j-1];
            }
            else
            {
                dp[i][j] = 1 + dp[i - 1][j - 1]
                    .min(dp[i - 1][j])
                    .min(dp[i][j - 1]);
            }
        }
    }
    dp[m][n]
}


