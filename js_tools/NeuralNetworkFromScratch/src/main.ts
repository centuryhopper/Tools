// // import { NeuralNetworkLayer } from "./classes/neural_network_layer.js";
// // import { MatrixMath } from "./utils/matrix_math.js";

// // const button = document.getElementById('myButton') as HTMLButtonElement;
// // const output = document.getElementById('output') as HTMLParagraphElement;

// // button.addEventListener('click', () => {
// //   output.textContent = 'Button clicked! TypeScript works!';
// // });

// // const main = () => {
// //   /*
// //   // const A = MatrixMath.generateRandomMatrix([2, 3]);
// //   // const B = MatrixMath.generateRandomMatrix([3, 2]);

// //   // console.log('A:');
// //   // A.forEach((row) => console.table(row))
// //   // console.log('\n\n\n\n\n');
// //   // console.log('B:');
// //   // B.forEach((row) => console.table(row))

// //   // // console.log(`A:`, A);
// //   // // console.log(`B:`, B);

// //   // const result = MatrixMath.multiplyMatrices(A, B);
// //   // console.log('\n\n\n\n\n');
// //   // console.log(`A x B:`);
// //   // result.forEach((row) => console.table(row))

// //   */

// //   // let inputs: number[] = [1, 2, 3, 2.5]
// //   let lstOfWeights: number[][] = [
// //       [0.2, 0.8, -0.5, 1.0],
// //       [0.5, -0.91, 0.26, -0.5],
// //       [-0.26, -0.27, 0.17, 0.87],
// //   ]

// //   const layer1 = new NeuralNetworkLayer(4, 5)
// //   const layer1Result = layer1.forward([
// //     [1,2,3,2.5],
// //     [2.0,5.0,-1.0,2.0],
// //     [-1.5,2.7,3.3,-0.8],
// //   ])
// //   layer1Result.forEach((row) => console.table(row))

// //   console.log('\n\n\n\n\n');


// //   const layer2 = new NeuralNetworkLayer(5,2)
// //   const layer2Result = layer2.forward(layer1Result)
// //   layer2Result.forEach((row) => console.table(row))


// // }

// // main()


import { NeuralNetworkLayer } from "./classes/neural_network_layer.js";
// import p5 from "p5"
// Declare p5 global to make TS happy
declare const p5: any;

let layer1: NeuralNetworkLayer;
let layer2: NeuralNetworkLayer;
let layer1Result: number[][] = [];
let layer2Result: number[][] = [];

const sketch = (p) => {
  p.setup = () => {
    p.createCanvas(800, 600);
    p.background(30);
    p.noLoop();

   

    // Sample input
    // const inputs = [
    //   [1, 2, 3, 2.5],
    //   [2.0, 5.0, -1.0, 2.0],
    //   [-1.5, 2.7, 3.3, -0.8],
    // ];

    const inputs = [
      [1.0, 2.1, 3.3, 4.0, 5.5],
      [0.0, 0.2, 0.3, 0.4, 0.5],
      [5.5, 4.4, 3.3, 2.2, 1.1],
      [9.9, 8.8, 7.7, 6.6, 5.5],
      [1.2, 2.3, 3.4, 4.5, 5.6],
      [6.1, 5.2, 4.3, 3.4, 2.5],
      [0.9, 1.8, 2.7, 3.6, 4.5],
      [3.3, 2.2, 1.1, 0.0, -1.1],
      [7.0, 6.0, 5.0, 4.0, 3.0],
      [0.5, 1.0, 1.5, 2.0, 2.5]
    ];

     // Initialize layers
    layer1 = new NeuralNetworkLayer(inputs[0].length, 5);
    layer2 = new NeuralNetworkLayer(5, 4);


    layer1Result = layer1.forward(inputs);
    layer2Result = layer2.forward(layer1Result);
  };

  p.draw = () => {
    p.background(30);
    p.fill(255);
    p.textSize(16);
    p.text("Neural Network Output", 20, 30);

    let y = 60;
    for (const row of layer2Result) {
      p.text(row.map((n) => n.toFixed(2)).join(", "), 20, y);
      y += 30;
    }
  };
};

new p5(sketch);




// const sketch = (p: any) => {
//   p.setup = () => {
//     p.createCanvas(400, 400);
//     p.background(220);
//   };

//   p.draw = () => {
//     p.fill(255, 0, 0);
//     p.ellipse(p.width / 2, p.height / 2, 100, 100);
//   };
// };

// new p5(sketch);


