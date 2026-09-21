import { ActivationFunctions } from "./utils/activation_functions.js";
import { Matrix } from "./utils/matrix.js";

export class NeuralNetwork
{
    private numInputs: number;
    private numHidden: number;
    private numOutputs: number;

    /**
     * the connectors from the inputs to the hidden layer nodes
     */
    private weightsIH: Matrix;

    /**
     * the connectors from the hidden layer nodes to the output node(s)
     */
    private weightsHO: Matrix;

    /**
     * the bias to be added after the matrix dot product of the inputs with the weights of the hidden layer
     */
    private biasH: Matrix;
    /**
     * the bias to be added after the matrix dot product of the hidden layer's output with the weights of the hidden layer
     */
    private biasO: Matrix;

    constructor(numInputs: number, numHidden: number, numOutputs: number)
    {
        this.numInputs = numInputs;
        this.numHidden = numHidden;
        this.numOutputs = numOutputs;

        this.weightsIH = new Matrix(numHidden, numInputs).randomize();
        this.weightsHO = new Matrix(numOutputs, numHidden).randomize();
        this.biasH = new Matrix(numHidden, 1).randomize();
        this.biasO = new Matrix(numOutputs, 1).randomize();
    }

    // Forward pass
    predict(input: Matrix): Matrix
    {
        // Hidden layer
        // [numHidden x numInputs] . []
        let hidden = Matrix.dot(this.weightsIH, input);
        hidden = Matrix.add(hidden, this.biasH);
        hidden.map(ActivationFunctions.tanh);

        // Output layer
        let output = Matrix.dot(this.weightsHO, hidden);
        output = Matrix.add(output, this.biasO);
        output.map(ActivationFunctions.sigmoid);

        return output;
    }

    // Train with one step of backpropagation
    train(inputArray: number[], targetArray: number[], learningRate = 0.3): void
    {
        // Convert arrays to matrices (column vectors)
        const inputs = Matrix.fromArray(inputArray);
        const targets = Matrix.fromArray(targetArray);

        // console.log(inputs.shape());
        // console.log(targets.shape());

        // Feed Forward
        // steps:
        // weights dot the inputs plus the bias
        // apply activation function to each value in the matrix
        let hiddenLayerOutput = Matrix.dot(this.weightsIH, inputs);
        hiddenLayerOutput = Matrix.add(hiddenLayerOutput, this.biasH);
        hiddenLayerOutput.map(ActivationFunctions.tanh);

        let finalOutput = Matrix.dot(this.weightsHO, hiddenLayerOutput);
        finalOutput = Matrix.add(finalOutput, this.biasO);
        finalOutput.map(ActivationFunctions.sigmoid);

        // GRADIENT DESCENT BEGINS HERE
        // Output error = target - output
        let outputErrors = Matrix.subtract(targets, finalOutput);

        // Calculate output gradient
        let gradients: Matrix = finalOutput.clone();
        gradients.map(ActivationFunctions.dSigmoid);
        // console.log(gradients.shape());
        // console.log(outputErrors.shape());
        
        gradients = Matrix.multiply(gradients, outputErrors);
        gradients.scale(learningRate);

        // Calculate weight deltas for weightsHO
        const hiddenLayerOutputTransposed = Matrix.transpose(hiddenLayerOutput);
        const weightHODeltas = Matrix.dot(gradients, hiddenLayerOutputTransposed);

        // Update weightsHO and biasO
        this.weightsHO = Matrix.add(this.weightsHO, weightHODeltas);
        this.biasO = Matrix.add(this.biasO, gradients);

        // Calculate hidden layer errors
        const weightsHOT = Matrix.transpose(this.weightsHO);
        const hiddenLayerErrors = Matrix.dot(weightsHOT, outputErrors);

        // Calculate hidden layer gradient
        let hiddenLayerGradient: Matrix = hiddenLayerOutput.clone();
        hiddenLayerGradient.map(ActivationFunctions.dTanh);
        hiddenLayerGradient = Matrix.multiply(hiddenLayerGradient, hiddenLayerErrors);
        hiddenLayerGradient.scale(learningRate);

        // Calculate weight deltas for weightsIH
        const inputsTransposed = Matrix.transpose(inputs);
        const weightIHDeltas = Matrix.dot(hiddenLayerGradient, inputsTransposed);

        // Update weightsIH and biasH
        this.weightsIH = Matrix.add(this.weightsIH, weightIHDeltas);
        this.biasH = Matrix.add(this.biasH, hiddenLayerGradient);
    }
}
