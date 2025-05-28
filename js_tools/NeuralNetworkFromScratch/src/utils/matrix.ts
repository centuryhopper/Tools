export class Matrix
{
  private readonly _rows: number
  private readonly _cols: number
  private _data: number[][]

  get data() {
    return this._data
  }
  get rows() {
    return this._rows
  }

  get cols() {
    return this._cols
  }

  constructor(rows: number, cols: number)
  {
    this._rows = rows;
    this._cols = cols;
    // 2-d array with all 0s
    this._data = Array.from({ length: rows }, () => Array(cols).fill(0))
  }

  // Transpose a matrix (rows become columns and vice versa)
  static transpose(m: Matrix): Matrix
  {
    let result = new Matrix(m._cols, m._rows);
    for (let i = 0; i < result._rows; i++)
    {
      for (let j = 0; j < result._cols; j++)
      {
        result._data[i][j] = m._data[j][i];
      }
    }
    return result;
  }

  /**
   * get the shape (rows by columns) of a matrix
   */
  getShape(): [number, number]
  {
    return [this._data.length, this._data[0].length]
  }

  scale(scalar: number): void
  {
    for (let i = 0; i < this._data.length; i++)
    {
      for (let j = 0; j < this._data[0].length; j++)
      {
        this._data[i][j] *= scalar;
      }
    }
  }

  sumBias(biases: number[]): void
  {
    if (this.cols != biases.length)
    {
      return
    }

    for (let i = 0; i < this.rows; i++)
    {
      for (let j = 0; j < this.cols; j++)
      {
        this.data[i][j] += biases[j];
      }
    }

  }

  sum(scalar: number): void
  {
    for (let i = 0; i < this._data.length; i++)
    {
      for (let j = 0; j < this._data[0].length; j++)
      {
        this._data[i][j] += scalar;
      }
    }
  }

  sub(scalar: number): void
  {
    for (let i = 0; i < this._data.length; i++)
    {
      for (let j = 0; j < this._data[0].length; j++)
      {
        this._data[i][j] -= scalar;
      }
    }
  }

  randomize(): Matrix
  {
    for (let i = 0; i < this._rows; i++)
    {
      for (let j = 0; j < this._cols; j++)
      {
        // random values between -1 and 1
        // Math.random() * (max - min) + min
        this._data[i][j] = Math.random() * (1 - (-1)) + (-1);
      }
    }

    return this
  }

  map(func: (val: number) => number): void
  {

    for (let i = 0; i < this._rows; i++)
    {
      for (let j = 0; j < this._cols; j++)
      {
        let val = this._data[i][j];
        this._data[i][j] = func(val);
      }
    }
  }

  fill(data: number[][]): void
  {
    this._data = data;
  }

  print(): void
  {
    console.table(this._data);
  }

  reset(): void
  {
    for (let i = 0; i < this._rows; i++)
    {
      for (let j = 0; j < this._cols; j++)
      {
        this._data[i][j] = 0;
      }
    }
  }

  // Generate a matrix with random values normalized between -1 and 1
  generateRandomMatrix(shape: [number, number]): Matrix
  {
    const [rows, cols] = shape;
    // const matrix: number[][] = Array.from({ length: rows }, () => []);

    for (let i = 0; i < rows; i++)
    {
      for (let j = 0; j < cols; j++)
      {
        const randomValue = Math.random() * 2 - 1; // value between -1 and 1
        this._data[i][j] = randomValue;
      }
    }

    return this;
  }

  static add(m1: Matrix, m2: Matrix): Matrix | undefined
  {
    if (m1._cols !== m2._cols || m1._rows !== m2._rows)
    {
      console.error("Cannot add matrices with different dimensions.");
      return undefined;
    }

    let result = new Matrix(m1._rows, m1._cols);

    for (let i = 0; i < result._rows; i++)
    {
      for (let j = 0; j < result._cols; j++)
      {
        result._data[i][j] = m1._data[i][j] + m2._data[i][j];
      }
    }
    return result;

  }

  static subtract(m1: Matrix, m2: Matrix)
  {
    if (m1._cols !== m2._cols || m1._rows !== m2._rows)
    {
      console.error("Cannot subtract two matrices with different dimensions.");
      return undefined;
    }

    let result = new Matrix(m1._rows, m1._cols);

    for (let i = 0; i < result._rows; i++)
    {
      for (let j = 0; j < result._cols; j++)
      {
        result._data[i][j] = m1._data[i][j] - m2._data[i][j];
      }
    }
    return result;
  }

  // Multiply two matrices
  static multiply(m1: Matrix, m2: Matrix): Matrix | undefined
  {
    if (m1._cols !== m2._rows)
    {
      console.error("Incompatible matrix dimensions for multiplication.");
      return undefined;
    }

    const result: Matrix = new Matrix(m1._rows, m2._cols);

    for (let i = 0; i < result._rows; i++)
    {
      result._data[i] = [];
      for (let j = 0; j < result._cols; j++)
      {
        let sum = 0;
        for (let k = 0; k < m2._rows; k++)
        {
          sum += m1._data[i][k] * m2._data[k][j];
        }
        result._data[i][j] = sum;
      }
    }
    return result;
  }
}
