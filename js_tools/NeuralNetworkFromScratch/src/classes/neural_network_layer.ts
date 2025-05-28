import { Matrix } from "../utils/matrix.js";

export class NeuralNetworkLayer
{
  private readonly lstOfWeights: Matrix;
  private readonly biases: Matrix;

  constructor(numInputs: number, numNeurons: number)
  {
    this.lstOfWeights = new Matrix(numInputs, numNeurons).randomize();
    this.biases = new Matrix(1, numNeurons);

    // console.log(MatrixMath.getShape(this.lstOfWeights), this.lstOfWeights);
    // console.log(MatrixMath.getShape(this.biases), this.biases);
    // console.log(numNeurons);
  }

  public forward(inputs: Matrix): Matrix | undefined
  {
    console.log(inputs.getShape(), inputs);
    console.log(this.lstOfWeights.getShape(), this.lstOfWeights);

    const dotProduct = Matrix.multiply(inputs, this.lstOfWeights);
    
    if (!dotProduct)
    {
      return undefined
    }

    // Add biases to dotProduct (row-wise broadcast)
    dotProduct.sumBias(this.biases.data[0]);

    return dotProduct;
  }

}
