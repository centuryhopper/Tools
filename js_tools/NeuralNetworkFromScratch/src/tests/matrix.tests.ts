import { Matrix } from '../utils/matrix.js';

const array3d: number[][][] = [
  [
    [-3, 2, -1],
    [6, -6, 7],
    [3, -4, 4],
  ],
  [
    [1, 2, 3],
    [0, 1, 4],
    [5, 6, 0],
  ],
  [
    [2, -1, 0],
    [-1, 2, -1],
    [0, -1, 2],
  ],
  [
    [3, 2, -1],
    [2, -2, 4],
    [-1, 0.5, -1],
  ],
];

const fourByFours: number[][][] = [
  [
    [1, 0, 2, -1],
    [3, 0, 0, 5],
    [2, 1, 4, -3],
    [1, 0, 5, 0],
  ],
  [
    [1, 0, 4, -6],
    [2, 5, 0, 3],
    [-1, 2, 3, 5],
    [2, 1, -2, 3],
  ],
  [
    [1, 2, 3, 4],
    [1, 0, 2, 0],
    [0, 1, 2, 3],
    [2, 3, 0, 0],
  ],
];

function extract2DSlices(array3D: number[][][]): number[][][] {
  return array3D.map(depthSlice => {
    return depthSlice.map(row => [...row]); // Clone each row for safety
  });
}

function runMatrixTests() {
  const slices = extract2DSlices(fourByFours);
  for (const slice of slices) {
    const matrix = Matrix.from2dArray(slice);
    matrix.print();
    const det = Matrix.determinantNaive(matrix);
    console.log(`determinant is ${det}`);
  }
}

runMatrixTests();