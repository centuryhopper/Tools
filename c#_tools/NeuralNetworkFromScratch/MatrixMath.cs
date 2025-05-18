

public static class MatrixMath
{
    public static float[,] Transpose(this float[,] matrix)
    {
        for (int i = 0; i < matrix.GetLength(0); i++)
        {
            for (int j = 0; j < matrix.GetLength(1); j++)
            {
                matrix[i, j] = matrix[j, i];
            }
        }
        return matrix;
    }

    public static float[,] GenerateRandomMatrix((int, int) shape)
    {
        float[,] matrix = new float[shape.Item1, shape.Item2];
        Random rand = new();
        for (int i = 0; i < matrix.GetLength(0); i++)
        {
            // fill row with random values that are normalized between -1 and 1
            for (int j = 0; j < matrix.GetLength(1); j++)
            {
                float randomValue = (float)(rand.NextDouble() * (1 - (-1)) + (-1));
                matrix[i, j] = randomValue;
            }
        }

        return matrix;
    }


    public static float[,] MultiplyMatrices(float[,] A, float[,] B)
    {
        /*

                2 x 3 matrix
        A = [
                [1.23, 4.56, 7.89],
                [0.12, 3.45, 6.78]
        ]
                3 x 2 matrix
        B = [
                [9.87, 6.54],
                [3.21, 0.98],
                [7.65, 4.32]
        ]
        
        result matrix is 2 x 2
        [
            [(1.23*9.87)+(4.56*3.21)+(7.89*7.65) = 87.1362, (1.23*6.54)+(4.56*0.98)+(7.89*4.32) = 46.5978],
            [(0.12*9.87)+(3.45*3.21)+(6.78*7.65) = 64.1259, (0.12*6.54)+(3.45*0.98)+(6.78*4.32) = 33.4554]
        ]

        =

        [
            [87.1362,46.5978],
            [64.1259,33.4554]
        ]

        */
        int rowsA = A.GetLength(0);
        int colsA = A.GetLength(1);
        int rowsB = B.GetLength(0);
        int colsB = B.GetLength(1);

        if (colsA != rowsB)
            throw new Exception("Incompatible matrix dimensions for multiplication.");

        float[,] result = new float[rowsA, colsB];

        for (int i = 0; i < rowsA; i++)
        {
            for (int j = 0; j < colsB; j++)
            {
                result[i, j] = 0;
                for (int k = 0; k < colsA; k++)
                {
                    result[i, j] += A[i, k] * B[k, j];
                }
            }
        }

        return result;
    }
}