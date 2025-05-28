import { ActivationReLU } from '../classes/activation_ReLU.js';

function assertEqual(actual: any, expected: any, message: string) {
  const pass = JSON.stringify(actual) === JSON.stringify(expected);
  const color = pass ? 'green' : 'red';
  const result = document.createElement('div');
  result.style.color = color;
  result.textContent = pass ? `✅ ${message}` : `❌ ${message}\nExpected: ${JSON.stringify(expected)}\nActual: ${JSON.stringify(actual)}`;
  document.body.appendChild(result);
}

function testReLU() {
  const relu = new ActivationReLU();
  const input = [
    [-1, 0, 2],
    [3, -2, -5]
  ];
  const expected = [
    [0, 0, 2],
    [3, 0, 0]
  ];
  const output = relu.forward(input);
  assertEqual(output, expected, 'ActivationReLU.forward applies ReLU correctly');
}

testReLU();