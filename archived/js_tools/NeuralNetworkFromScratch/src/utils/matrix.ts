
export class Matrix
{
    private _rows: number;
    private _cols: number;
    private _data: number[][];

    constructor(rows: number, cols: number)
    {
        this._rows = rows;
        this._cols = cols;
        this._data = Array.from({ length: rows }, () => Array(cols).fill(0));
    }

    static from2dArray(inputArray: number[][]): Matrix
    {
        const m = new Matrix(inputArray.length, inputArray[0].length);
        m._data = inputArray.map(row => [...row]);
        return m;
    }

    /**
     * [inputarray.length x 1]
     */
    static fromArray(inputArray: number[]): Matrix
    {
        const m = new Matrix(inputArray.length, 1);
        for (let i = 0; i < inputArray.length; i++)
        {
            m._data[i][0] = inputArray[i];
        }
        return m;
    }

    static toArray(m: Matrix): number[]
    {
        return m._data.map(row => row[0]);
    }

    static transpose(m: Matrix): Matrix
    {
        const result = new Matrix(m._cols, m._rows);
        for (let i = 0; i < m._cols; i++)
        {
            for (let j = 0; j < m._rows; j++)
            {
                result._data[i][j] = m._data[j][i];
            }
        }
        return result;
    }

    /**
     * get matrix dimensions
     */
    shape(): [number, number]
    {
        return [this._rows, this._cols];
    }

    scale(scalar: number): void
    {
        this._data.forEach((row, i) =>
        {
            row.forEach((_, j) =>
            {
                this._data[i][j] *= scalar;
            });
        });
    }

    sumBias(biases: number[]): void
    {
        if (this._cols !== biases.length) return;
        for (let i = 0; i < this._rows; i++)
        {
            for (let j = 0; j < this._cols; j++)
            {
                this._data[i][j] += biases[j];
            }
        }
    }

    sum(scalar: number): void
    {
        this._data.forEach((row, i) =>
        {
            row.forEach((_, j) =>
            {
                this._data[i][j] += scalar;
            });
        });
    }

    sub(scalar: number): void
    {
        this._data.forEach((row, i) =>
        {
            row.forEach((_, j) =>
            {
                this._data[i][j] -= scalar;
            });
        });
    }

    randomize(): Matrix
    {
        this._data = this._data.map(row => row.map(() => Math.random() * 2 - 1));
        return this;
    }

    clone(): Matrix
    {
        return Matrix.from2dArray(this._data);
    }

    map(func: (val: number) => number): void
    {
        this._data = this._data.map(row => row.map(func));
    }

    fill(data: number[][]): void
    {
        if (data.length !== this._rows || data[0].length !== this._cols)
        {
            console.error("Shape mismatch in fill().");
            return;
        }
        this._data = data.map(row => [...row]);
    }

    print(): void
    {
        // console.log(this._data.map(row => row.map(v => v.toFixed(2)).join("\t")).join("\n"));
        console.table(this._data);
    }

    reset(): void
    {
        this._data = Array.from({ length: this._rows }, () => Array(this._cols).fill(0));
    }

    static determinantNaive(matrix: Matrix): number
    {
        if (matrix._rows !== matrix._cols || matrix._rows < 1)
        {
            console.error("Matrix must be square for determinant.");
            return 0;
        }

        const getSubMatrix = (m: Matrix, colToSkip: number): Matrix =>
        {
            const data: number[][] = [];
            for (let i = 1; i < m._rows; i++)
            {
                const row: number[] = [];
                for (let j = 0; j < m._cols; j++)
                {
                    if (j !== colToSkip) row.push(m._data[i][j]);
                }
                data.push(row);
            }
            return Matrix.from2dArray(data);
        }

        const runner = (mat: Matrix, m: number, n: number): number =>
        {
            if (m === 1 && n === 1) return mat._data[0][0];
            if (m === 2 && n === 2)
                return mat._data[0][0] * mat._data[1][1] - mat._data[0][1] * mat._data[1][0];

            let sum = 0;
            for (let j = 0; j < n; j++)
            {
                const subMat = getSubMatrix(mat, j);
                sum += mat._data[0][j] * runner(subMat, m - 1, n - 1) * (j % 2 === 0 ? 1 : -1);
            }
            return sum;
        }

        return runner(matrix, matrix._rows, matrix._cols);
    }

    get data(): number[][]
    {
        return this._data;
    }

    get rows(): number
    {
        return this._rows;
    }

    get cols(): number
    {
        return this._cols;
    }

    getRow(rowIdx: number): number[]
    {
        return this._data[rowIdx];
    }

    getElement(row: number, col: number): number
    {
        return this._data[row][col];
    }

    setElement(row: number, col: number, value: number): void
    {
        this._data[row][col] = value;
    }

    apply(func: (val: number) => number): void
    {
        this.map(func);
    }

    /**
     * Performs an element-wise matrix multiplication (for the non-matching dimensions, consider using dot())
     */
    static multiply(m1: Matrix, m2: Matrix): Matrix
    {
        if (m1._rows !== m2._rows || m1._cols !== m2._cols)
        {
            // console.error("Shape mismatch for element-wise multiplication.");
            throw new Error("Shape mismatch for element-wise multiplication.")
        }
        const result = new Matrix(m1._rows, m1._cols);
        for (let i = 0; i < m1._rows; i++)
        {
            for (let j = 0; j < m1._cols; j++)
            {
                result._data[i][j] = m1._data[i][j] * m2._data[i][j];
            }
        }
        return result;
    }

    softmax(): Matrix
    {
        for (let i = 0; i < this._rows; i++)
        {
            const max = Math.max(...this._data[i]);
            const exps = this._data[i].map(v => Math.exp(v - max));
            const sum = exps.reduce((acc, val) => acc + val, 0);
            this._data[i] = exps.map(v => v / sum);
        }
        return this;
    }

    static add(a: Matrix, b: Matrix): Matrix
    {
        if (a._rows !== b._rows || a._cols !== b._cols) throw new Error("Matrix dimensions must match for addition");
        const result = new Matrix(a._rows, a._cols);
        for (let i = 0; i < a._rows; i++)
        {
            for (let j = 0; j < a._cols; j++)
            {
                result._data[i][j] = a._data[i][j] + b._data[i][j];
            }
        }
        return result;
    }

    static subtract(a: Matrix, b: Matrix): Matrix
    {
        if (a._rows !== b._rows || a._cols !== b._cols) throw new Error("Matrix dimensions must match for subtraction");
        const result = new Matrix(a._rows, a._cols);
        for (let i = 0; i < a._rows; i++)
        {
            for (let j = 0; j < a._cols; j++)
            {
                result._data[i][j] = a._data[i][j] - b._data[i][j];
            }
        }
        return result;
    }

    static scale(m: Matrix, scalar: number): Matrix
    {
        const result = new Matrix(m._rows, m._cols);
        for (let i = 0; i < m._rows; i++)
        {
            for (let j = 0; j < m._cols; j++)
            {
                result._data[i][j] = m._data[i][j] * scalar;
            }
        }
        return result;
    }

    static dot(a: Matrix, b: Matrix): Matrix
    {
        if (a._cols !== b._rows) throw new Error("Incompatible matrix dimensions for multiplication");
        const result = new Matrix(a._rows, b._cols);
        for (let i = 0; i < result._rows; i++)
        {
            for (let j = 0; j < result._cols; j++)
            {
                let sum = 0;
                for (let k = 0; k < a._cols; k++)
                {
                    sum += a._data[i][k] * b._data[k][j];
                }
                result._data[i][j] = sum;
            }
        }
        return result;
    }
}
