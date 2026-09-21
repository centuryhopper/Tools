

/*
Returning an array that adds up to 1 is quite simple, actually. Let’s say we have one with these values:
[3, 1.2, 4]
We simply do 1 / (sum of all 3) * (current index)
In this case, our sum is 8.2.
So, we do 1 / 8.2 (which is about 0.1219) and multiply it by each index.
Our new array looks like this:
[0.3659, 0.1463, 0.4878]
Add these three together, and we get none other than 1. Nice thing about this is that it works for all array sizes.
Just thought I’d point it out. :D

source:
youtube comments:
https://www.youtube.com/watch?v=KtPpoMThKUs&list=PLRqwX-V7Uu6aCibgK1PTWWu9by6XFdCfh&index=20

*/


// const main = () =>
// {
//   const xor_trainer = new XORTrainer();
//   xor_trainer.run()
// }

// main()
import { XORTrainer } from "./xor_trainer.js";
import * as THREE from 'three';

// we always need a camera, renderer, and scene

const container = document.getElementById('nn-visualizer')!;
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, container.clientWidth / container.clientHeight, 0.1, 1000);
camera.position.z = 8;
camera.position.x = 5;


const renderer = new THREE.WebGLRenderer({ antialias: true });
// renderer.setSize(container.clientWidth, container.clientHeight);
renderer.setSize(window.innerWidth, window.innerHeight);
window.addEventListener('resize', () =>
{
  camera.aspect = window.innerWidth / window.innerHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(window.innerWidth, window.innerHeight);
});

container.appendChild(renderer.domElement);

// Layers definition
const layers = [2, 4, 1]; // input, hidden, output
const layerSpacing = 5;
const neuronSpacing = 2;

const neurons: THREE.Mesh[][] = [];

const inputColor = 0x3399ff;
const hiddenColor = 0xff9933;
const outputColor = 0x33ff66;

const colors = [inputColor, hiddenColor, outputColor];

layers.forEach((count, layerIndex) =>
{
  const layerNeurons: THREE.Mesh[] = [];

  for (let i = 0; i < count; i++)
  {
    const geometry = new THREE.SphereGeometry(0.3, 32, 32);
    const material = new THREE.MeshBasicMaterial({ color: colors[layerIndex] });
    const neuron = new THREE.Mesh(geometry, material);

    neuron.position.x = layerIndex * layerSpacing;
    neuron.position.y = (-(count - 1) * neuronSpacing) / 2 + i * neuronSpacing;

    scene.add(neuron);
    layerNeurons.push(neuron);
  }

  neurons.push(layerNeurons);
});

// Draw lines (connections)
for (let i = 0; i < neurons.length - 1; i++)
{
  const currentLayer = neurons[i];
  const nextLayer = neurons[i + 1];

  currentLayer.forEach(source =>
  {
    nextLayer.forEach(target =>
    {
      const geometry = new THREE.BufferGeometry().setFromPoints([
        source.position,
        target.position
      ]);
      const material = new THREE.LineBasicMaterial({ color: 0x888888 });
      const line = new THREE.Line(geometry, material);
      scene.add(line);
    });
  });
}

// 👇 XOR Trainer
const trainer = new XORTrainer();

// 👇 Neuron reference: neurons[2][0] is output neuron (1 output)
const outputNeuron = neurons[2][0]; // third layer, single output node

// console.log((outputNeuron.material as THREE.MeshBasicMaterial).color);
// (outputNeuron.material as THREE.MeshBasicMaterial).color = new THREE.Color(255,0,0)

// 👇 Utility to map value to color
function getOutputColor(value: number): THREE.Color
{
  const color = new THREE.Color();
  color.setRGB(
    1 - value,         // R: Magenta (1) → Black (0)
    value,             // G: 0 → 1
    1 - value          // B: Magenta (1) → Black (0)
  );
  return color;
}

let frameCount = 0;

const trainXor = () =>
{
  // console.log('train!');
  trainer.trainEpochs(1); // 1 epoch per visual update

  // 👇 Predict input [1, 0] for demo, or average
  const predictions = trainer.predictAll();
  console.log(predictions);
  // const avgOutput = predictions.reduce((a, b) => a + b, 0) / predictions.length;

  // console.log(avgOutput);

  // 
  /*
  [0, 0],
  [0, 1],
  [1, 0],
  [1, 1],

  predictions[0] is [0,0]'s output
  predictions[1] is [0,1]'s output
  etc...
  */
  const color = getOutputColor(predictions[2]);
  (outputNeuron.material as THREE.MeshBasicMaterial).color = color;
  // console.log(color);
}

function animate()
{
  requestAnimationFrame(animate);

  // console.log('hi there');

  // 👇 Train and update every 10 frames
  if (frameCount < 10_000 && frameCount % 10 === 0)
  {
    trainXor()
  }

  renderer.render(scene, camera);
  frameCount++;
}

animate();
