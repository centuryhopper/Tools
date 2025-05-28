
export class ActivationReLU
{
    forward(inputs: number[][]): number[][]
    {
        return inputs.map(row =>
            row.map(value => Math.max(0, value))
        )
    }
}