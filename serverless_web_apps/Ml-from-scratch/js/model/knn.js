// [a,b,c]
// [d,e,f]
// sqrt( (a-d)^2 + (b-e)^2 + (c-f)^2 )
const euclidianDistance = (m1, m2) => {
  const diffMatrix = Matrix.subtract(m1, m2);
  diffMatrix.apply((val) => val * val);
  return Math.sqrt(diffMatrix.sumAll());
};

// X is a matrix and y is an array of labels
// for breast cancer dataset:
// X is an array of arrays: 569 rows × 30 features
// [[17.99, 10.38, 122.8, ...], [20.57, 17.77, 132.9, ...], ...]
// y is a flat array: 569 labels
// [1, 1, 0, ...]
function trainTestSplit(X, y, { testSize = 0.2, randomState } = {}) {
  // 1. rng = randomState is set ? seeded PRNG (e.g. mulberry32(randomState)) : Math.random
  const rng = randomState !== undefined ? mulberry32(randomState) : Math.random;
  // 2. indices = [0, 1, ..., X.length - 1]
  const indices = Array.from({ length: X.length }, (_, i) => i); // [0, 1, ..., n-1]
  // 3. Fisher–Yates shuffle indices using rng
  for (let i = indices.length - 1; i > 0; i--) {
    // from 0 up to i (inclusive)
    const randomIndexChoice = Math.floor(rng() * (i + 1));
    [indices[randomIndexChoice], indices[i]] = [
      indices[i],
      indices[randomIndexChoice],
    ];
  }
  // 4. nTest = Math.ceil(X.length * testSize)   // sklearn rounds the test size up
  // [1,2,3,4,5,6]
  const nTest = Math.ceil(X.length * testSize);
  const testIndices = indices.slice(0, nTest);
  const trainIndices = indices.slice(nTest);
  // 5. testIdx = first nTest indices, trainIdx = the rest
  // 6. map each index list back to rows of X and y
  const xTrain = trainIndices.map((i) => X[i]);
  const xTest = testIndices.map((i) => X[i]);

  const yTrain = trainIndices.map((i) => y[i]);
  const yTest = testIndices.map((i) => y[i]);

  return { xTrain, xTest, yTrain, yTest };
}

const { xTrain, xTest, yTrain, yTest } = trainTestSplit(X, y, {
  testSize: 0.2,
  randomState: 1234,
});

class KNN {
  constructor(k = 3) {
    if (k & (1 !== 1))
      throw new Error(
        "Make sure k is an odd number so that we always have a tie breaker classification",
      );
    this.k = k;
  }

  fit() {}

  predict() {}

  #predict() {}
}

function mulberry32(seed) {
  let state = seed >>> 0; // force to unsigned 32-bit int
  return function () {
    state = (state + 0x6d2b79f5) >>> 0;
    let t = state;
    t = Math.imul(t ^ (t >>> 15), t | 1);
    t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296; // divide by 2^32 → [0, 1)
  };
}

//const rng = mulberry32(1234);
//rng(); // same value every time you run with seed 1234
//rng(); // next value in the sequence
