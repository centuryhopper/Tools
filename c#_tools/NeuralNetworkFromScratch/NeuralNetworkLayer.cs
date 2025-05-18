

/// <summary>
/// Represents one layer in a neural network
/// </summary>
public class NeuralNetworkLayer
{
    private readonly float[] inputs;
    private readonly float[,] lstOfWeights;
    private readonly float[,] biases;

    public NeuralNetworkLayer(int numInputs, int numNeurons)
    {
        this.lstOfWeights = MatrixMath.GenerateRandomMatrix((numInputs, numNeurons));
        this.biases = new float[1, numNeurons];

        if (lstOfWeights.GetLength(0) != biases.Length)
        {
            throw new Exception("the number of sets of weights and the number of biases should match");
        }
    }

    public float[,] Forward(float[,] inputs)
    {
        var dotProduct = MatrixMath.MultiplyMatrices(inputs, lstOfWeights);

        for (int i = 0; i < dotProduct.GetLength(0); i++)
        {
            for (int j = 0; j < dotProduct.GetLength(1); j++)
            {
                dotProduct[i, j] += biases[0,j];
            }
        }

        return dotProduct;
    }

    // TODO: modify to perform matrix multiplication (i.e. multiple sets of inputs times multiple sets of weights)
    // public List<float> ComputeLayerOutput()
    // {
    //     List<float> layerOutputs = new(biases.Length);
    //     for (int i = 0; i < biases.Length; i++)
    //     {
    //         float output = 0;

    //         // grab the ith array of weights
    //         var weights = Enumerable.Range(0, lstOfWeights.GetLength(1))
    //         .Select(j => lstOfWeights[i, j]).ToArray();

    //         // multiply weight and input values and add them
    //         foreach (var (weight, input) in weights.Zip(inputs, (weight, input) => (weight, input)))
    //         {
    //             output += weight * input;
    //         }

    //         // finally add the bias
    //         output += biases[i];
    //         layerOutputs.Add(output);
    //     }

    //     return layerOutputs;

    // }
}