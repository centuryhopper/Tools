// ActivationFunctions.ts

import type { Matrix } from "./matrix.js";

export class ActivationFunctions {
  // Sigmoid function
  static sigmoid(x: number): number {
    return 1 / (1 + Math.exp(-x));
  }

  // Derivative of sigmoid (assumes input is sigmoid(x))
  static dSigmoid(y: number): number {
    return y * (1 - y);
  }

  // Tanh function
  static tanh(x: number): number {
    const ePos = Math.exp(x);
    const eNeg = Math.exp(-x);
    return (ePos - eNeg) / (ePos + eNeg);
  }

  // Derivative of tanh (assumes input is tanh(x))
  static dTanh(y: number): number {
    return 1 - y * y;
  }

  // ReLU function
  static relu(x: number): number {
    return Math.max(0, x);
  }

  // Derivative of ReLU
  static dReLU(x: number): number {
    return x > 0 ? 1 : 0;
  }

  // Softmax applied to a 2D matrix
  static softmax(matrix: Matrix): Matrix {
    const result = new Matrix(matrix.rows, matrix.cols);

    for (let i = 0; i < matrix.rows; i++) {
      let max = -Infinity;
      for (let j = 0; j < matrix.cols; j++) {
        max = Math.max(max, matrix.data[i][j]);
      }

      let sum = 0;
      for (let j = 0; j < matrix.cols; j++) {
        sum += Math.exp(matrix.data[i][j] - max);
      }

      for (let j = 0; j < matrix.cols; j++) {
        result.data[i][j] = Math.exp(matrix.data[i][j] - max) / sum;
      }
    }

    return result;
  }
}
