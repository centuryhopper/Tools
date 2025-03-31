use std::{thread, time::Duration};
use std::f32::consts::PI;

const SCREEN_WIDTH: usize = 80;
const SCREEN_HEIGHT: usize = 24;
const R1: f32 = 1.0;   // Radius of the donut's cross-section
const R2: f32 = 2.0;   // Radius from the center of the donut to the middle of the tube
const K1: f32 = 15.0;  // Distance of the viewer from the donut
const K2: f32 = 5.0;   // Size scaling factor

pub fn donut() {
    let mut a = 0.0_f32;
    let mut b = 0.0_f32;

    loop {
        let mut output = [' '; SCREEN_WIDTH * SCREEN_HEIGHT];
        let mut z_buffer = [-1.0; SCREEN_WIDTH * SCREEN_HEIGHT];

        for theta in (0..628).step_by(7) {
            let theta = theta as f32 / 100.0;   // Angle around the tube (0 to 2π)
            
            for phi in (0..628).step_by(2) {
                let phi = phi as f32 / 100.0;   // Angle around the donut (0 to 2π)
                
                let sin_theta = theta.sin();
                let cos_theta = theta.cos();
                let sin_phi = phi.sin();
                let cos_phi = phi.cos();
                
                let sin_a = a.sin();
                let cos_a = a.cos();
                let sin_b = b.sin();
                let cos_b = b.cos();

                let circle_x = R2 + R1 * cos_theta;
                let circle_y = R1 * sin_theta;

                let x = circle_x * cos_b * cos_phi - circle_y * sin_b + K1;
                let y = circle_x * sin_b * cos_phi + circle_y * cos_b;
                let z = circle_x * sin_phi;

                let ooz = 1.0 / (z + K2);
                let xp = (SCREEN_WIDTH as f32 / 2.0 + K2 * ooz * x) as usize;
                let yp = (SCREEN_HEIGHT as f32 / 2.0 - K2 * ooz * y) as usize;

                let luminance = cos_theta * cos_phi * sin_b - sin_theta * sin_phi * cos_a - cos_theta * sin_phi * sin_b;
                let luminance_index = ((luminance + 1.0) * 4.0) as usize;
                let char_set = ['.', ',', '-', '~', '=', '*', '#', '$', '@'];

                if xp < SCREEN_WIDTH && yp < SCREEN_HEIGHT {
                    let idx = yp * SCREEN_WIDTH + xp;

                    if ooz > z_buffer[idx] {
                        z_buffer[idx] = ooz;
                        output[idx] = char_set[luminance_index.min(char_set.len() - 1)];
                    }
                }
            }
        }

        print!("\x1B[H");  // Move the cursor to the top left of the terminal
        for row in output.chunks(SCREEN_WIDTH) {
            println!("{}", row.iter().collect::<String>());
        }

        a += 0.04;
        b += 0.08;

        thread::sleep(Duration::from_millis(30));
    }
}
