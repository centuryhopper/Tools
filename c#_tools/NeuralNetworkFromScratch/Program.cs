
float[] inputs = [1, 2, 3, 2.5f];
float[,] lstOfWeights = {
    {0.2f, 0.8f, -0.5f, 1.0f},
    {0.5f, -0.91f, 0.26f, -0.5f},
    {-0.26f, -0.27f, 0.17f, 0.87f},
};
float[] biases = [2, 3, 0.5f];

NeuralNetworkLayer p = new(inputs, lstOfWeights, biases);

p.ComputeLayerOutput().ForEach(value => System.Console.WriteLine(value));
