// @ts-check

export class Matrix {
  /** @type {number} */
  #rows;
  /** @type {number} */
  #cols;
  /** @type {number[][]} */
  #data;

  /**
   * @param {number} rows
   * @param {number} cols
   */
  constructor(rows, cols) {
    this.#rows = rows;
    this.#cols = cols;
    this.#data = Array.from({ length: rows }, () => Array(cols).fill(0));
  }

  /**
   * @param {number[][]} inputArray
   * @returns {Matrix}
   */
  static from2dArray(inputArray) {
    const m = new Matrix(inputArray.length, inputArray[0].length);
    m.#data = inputArray.map((row) => [...row]);
    return m;
  }

  /**
   * [inputArray.length x 1]
   * @param {number[]} inputArray
   * @returns {Matrix}
   */
  static fromArray(inputArray) {
    const m = new Matrix(inputArray.length, 1);
    for (let i = 0; i < inputArray.length; i++) {
      m.#data[i][0] = inputArray[i];
    }
    return m;
  }

  /**
   * @param {Matrix} m
   * @returns {number[]}
   */
  static toArray(m) {
    return m.#data.map((row) => row[0]);
  }

  /**
   * @param {Matrix} m
   * @returns {Matrix}
   */
  static transpose(m) {
    const result = new Matrix(m.#cols, m.#rows);
    for (let i = 0; i < m.#cols; i++) {
      for (let j = 0; j < m.#rows; j++) {
        result.#data[i][j] = m.#data[j][i];
      }
    }
    return result;
  }

  /**
   * get matrix dimensions
   * @returns {[number, number]}
   */
  shape() {
    return [this.#rows, this.#cols];
  }

  /** @param {number} scalar */
  scale(scalar) {
    this.#data.forEach((row, i) => {
      row.forEach((_, j) => {
        this.#data[i][j] *= scalar;
      });
    });
  }

  /** @returns {number} */
  sumAll() {
    let total = 0;
    for (const row of this.#data) for (const v of row) total += v;
    return total;
  }

  /** @param {number[]} biases */
  sumBias(biases) {
    if (this.#cols !== biases.length) {
      throw new Error("number of columns should match the number of biases");
    }
    for (let i = 0; i < this.#rows; i++) {
      for (let j = 0; j < this.#cols; j++) {
        this.#data[i][j] += biases[j];
      }
    }
  }

  /** @param {number} scalar */
  addScalar(scalar) {
    this.#data.forEach((row, i) => {
      row.forEach((_, j) => {
        this.#data[i][j] += scalar;
      });
    });
  }

  /** @param {number} scalar */
  subtractScalar(scalar) {
    this.#data.forEach((row, i) => {
      row.forEach((_, j) => {
        this.#data[i][j] -= scalar;
      });
    });
  }

  /** @returns {Matrix} */
  randomize() {
    this.#data = this.#data.map((row) => row.map(() => Math.random() * 2 - 1));
    return this;
  }

  /** @returns {Matrix} */
  clone() {
    return Matrix.from2dArray(this.#data);
  }

  /** @param {(val: number) => number} func */
  map(func) {
    this.#data = this.#data.map((row) => row.map((v) => func(v)));
  }

  /** @param {number[][]} data */
  fill(data) {
    if (data.length !== this.#rows || data[0].length !== this.#cols) {
      console.error("Shape mismatch in fill().");
      return;
    }
    this.#data = data.map((row) => [...row]);
  }

  print() {
    // console.log(this.#data.map(row => row.map(v => v.toFixed(2)).join("\t")).join("\n"));
    console.table(this.#data);
  }

  reset() {
    this.#data = Array.from({ length: this.#rows }, () =>
      Array(this.#cols).fill(0),
    );
  }

  /**
   * @param {Matrix} matrix
   * @returns {number}
   */
  static determinantNaive(matrix) {
    if (matrix.#rows !== matrix.#cols || matrix.#rows < 1) {
      console.error("Matrix must be square for determinant.");
      return 0;
    }

    /**
     * @param {Matrix} m
     * @param {number} colToSkip
     * @returns {Matrix}
     */
    const getSubMatrix = (m, colToSkip) => {
      /** @type {number[][]} */
      const data = [];
      for (let i = 1; i < m.#rows; i++) {
        /** @type {number[]} */
        const row = [];
        for (let j = 0; j < m.#cols; j++) {
          if (j !== colToSkip) row.push(m.#data[i][j]);
        }
        data.push(row);
      }
      return Matrix.from2dArray(data);
    };

    /**
     * @param {Matrix} mat
     * @param {number} m
     * @param {number} n
     * @returns {number}
     */
    const runner = (mat, m, n) => {
      if (m === 1 && n === 1) return mat.#data[0][0];
      if (m === 2 && n === 2)
        return (
          mat.#data[0][0] * mat.#data[1][1] - mat.#data[0][1] * mat.#data[1][0]
        );

      let sum = 0;
      for (let j = 0; j < n; j++) {
        const subMat = getSubMatrix(mat, j);
        sum +=
          mat.#data[0][j] *
          runner(subMat, m - 1, n - 1) *
          (j % 2 === 0 ? 1 : -1);
      }
      return sum;
    };

    return runner(matrix, matrix.#rows, matrix.#cols);
  }

  /** @returns {number[][]} */
  get data() {
    return this.#data;
  }

  /** @returns {number} */
  get rows() {
    return this.#rows;
  }

  /** @returns {number} */
  get cols() {
    return this.#cols;
  }

  /**
   * @param {number} rowIdx
   * @returns {number[]}
   */
  getRow(rowIdx) {
    return this.#data[rowIdx];
  }

  /**
   * @param {number} row
   * @param {number} col
   * @returns {number}
   */
  getElement(row, col) {
    return this.#data[row][col];
  }

  /**
   * @param {number} row
   * @param {number} col
   * @param {number} value
   */
  setElement(row, col, value) {
    this.#data[row][col] = value;
  }

  /** @param {(val: number) => number} func */
  apply(func) {
    this.map(func);
  }

  /**
   * Performs an element-wise matrix multiplication (for the non-matching dimensions, consider using dot())
   * @param {Matrix} m1
   * @param {Matrix} m2
   * @returns {Matrix}
   */
  static multiply(m1, m2) {
    if (m1.#rows !== m2.#rows || m1.#cols !== m2.#cols) {
      throw new Error("Shape mismatch for element-wise multiplication.");
    }
    const result = new Matrix(m1.#rows, m1.#cols);
    for (let i = 0; i < m1.#rows; i++) {
      for (let j = 0; j < m1.#cols; j++) {
        result.#data[i][j] = m1.#data[i][j] * m2.#data[i][j];
      }
    }
    return result;
  }

  /** @returns {Matrix} */
  softmax() {
    for (let i = 0; i < this.#rows; i++) {
      const max = Math.max(...this.#data[i]);
      const exps = this.#data[i].map((v) => Math.exp(v - max));
      const sum = exps.reduce((acc, val) => acc + val, 0);
      this.#data[i] = exps.map((v) => v / sum);
    }
    return this;
  }

  /**
   * @param {Matrix} a
   * @param {Matrix} b
   * @returns {Matrix}
   */
  static add(a, b) {
    if (a.#rows !== b.#rows || a.#cols !== b.#cols)
      throw new Error("Matrix dimensions must match for addition");
    const result = new Matrix(a.#rows, a.#cols);
    for (let i = 0; i < a.#rows; i++) {
      for (let j = 0; j < a.#cols; j++) {
        result.#data[i][j] = a.#data[i][j] + b.#data[i][j];
      }
    }
    return result;
  }

  /**
   * @param {Matrix} a
   * @param {Matrix} b
   * @returns {Matrix}
   */
  static subtract(a, b) {
    if (a.#rows !== b.#rows || a.#cols !== b.#cols)
      throw new Error("Matrix dimensions must match for subtraction");
    const result = new Matrix(a.#rows, a.#cols);
    for (let i = 0; i < a.#rows; i++) {
      for (let j = 0; j < a.#cols; j++) {
        result.#data[i][j] = a.#data[i][j] - b.#data[i][j];
      }
    }
    return result;
  }

  /**
   * @param {Matrix} m
   * @param {number} scalar
   * @returns {Matrix}
   */
  static scale(m, scalar) {
    const result = new Matrix(m.#rows, m.#cols);
    for (let i = 0; i < m.#rows; i++) {
      for (let j = 0; j < m.#cols; j++) {
        result.#data[i][j] = m.#data[i][j] * scalar;
      }
    }
    return result;
  }

  /**
   * @param {Matrix} a
   * @param {Matrix} b
   * @returns {Matrix}
   */
  static dot(a, b) {
    if (a.#cols !== b.#rows)
      throw new Error("Incompatible matrix dimensions for multiplication");
    const result = new Matrix(a.#rows, b.#cols);
    for (let i = 0; i < result.#rows; i++) {
      for (let j = 0; j < result.#cols; j++) {
        let sum = 0;
        for (let k = 0; k < a.#cols; k++) {
          sum += a.#data[i][k] * b.#data[k][j];
        }
        result.#data[i][j] = sum;
      }
    }
    return result;
  }
}
