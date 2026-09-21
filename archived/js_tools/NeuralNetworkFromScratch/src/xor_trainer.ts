import { NeuralNetwork } from "./nn.js";
import { Matrix } from "./utils/matrix.js";

export class XORTrainer
{
    private nn: NeuralNetwork;

    constructor()
    {
        // Create neural network: 2 inputs, 4 hidden nodes, 1 output
        this.nn = new NeuralNetwork(2, 4, 1)
    }

    private trainingInputs: number[][] = [
        [0, 0],
        [0, 1],
        [1, 0],
        [1, 1],
    ];

    private trainingOutputs: number[][] = [
        [0],
        [1],
        [1],
        [0],
    ];

    trainEpochs(epochs: number = 10_000): void
    {
        // Train for epochs iterations (10,000 epochs by default)
        for (let epoch = 0; epoch < epochs; epoch++)
        {
            for (let i = 0; i < this.trainingInputs.length; i++)
            {
                const input = this.trainingInputs[i];
                const output = this.trainingOutputs[i];

                // Train the network with input and expected output
                this.nn.train(input, output);
            }
        }
    }

    predict(input: [number, number]): number
    {
        const inputMatrix = Matrix.fromArray(input);
        const outputMatrix = this.nn.predict(inputMatrix);
        return Matrix.toArray(outputMatrix)[0]; // one output node
    }

    predictAll(): number[]
    {
        return this.trainingInputs.map(input =>
        {
            const inputMatrix = Matrix.fromArray(input);
            const outputMatrix = this.nn.predict(inputMatrix);
            return Matrix.toArray(outputMatrix)[0]; // one output node
        });
    }

    // private testXOR()
    // {
    //     for (let i = 0; i < this.trainingInputs.length; i++)
    //     {
    //         const input = this.trainingInputs[i];
    //         const inputMatrix = Matrix.fromArray(input);
    //         const outputMatrix = this.nn.predict(inputMatrix);
    //         const result = Matrix.toArray(outputMatrix);

    //         console.log(`Input: ${input[0]}, ${input[1]} => Output: ${result[0]}`);
    //     }
    // }
}
