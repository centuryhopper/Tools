import Matrix from "./matrix.js";

// [a,b,c]
// [d,e,f]
// sqrt( (a-d)^2 + (b-e)^2 + (c-f)^2 )

/** @type m1: Matrix, m2: Matrix */
const euclidianDistance = (m1, m2) => {
  const diffMatrix = Matrix.subtract(m1, m2);
  diffMatrix.apply((val) => val * val);
  return Math.sqrt(diffMatrix.sumAll());
};

export class KNN {
  constructor(k = 3) {
    if ((k & 1) !== 1)
      throw new Error(
        "Make sure k is an odd number so that we always have a tie breaker classification",
      );
    this.k = k;
    this.X_train = [];
    this.y_train = [];
  }

  /**
   * @param {Matrix} X - Rows of data to train on
   * @param {Array} y - array of labels associated with the row
   */
  fit(X, y) {
    this.X_train = X;
    this.y_train = y;
  }

  // gather the prediction of every row of data
  /**
   * @param {Matrix} X - 2D matrix
   * @returns {Matrix} - 1D matrix of answers
   */
  predict(X) {
    const y_prediction = [];
    for (const row of X.data) {
      y_prediction.push(this.#predict(row));
    }

    return Matrix.fromArray(y_prediction);
  }

  /**
   * @param {Array} x -
   * @returns {Number} - most common label
   */
  #predict(x) {
    // get distances between x and all examples in the training set
    const distances = [];
    for (const [idx, x_train] of this.X_train.data.entries()) {
      distances.push({
        distance: euclidianDistance(
          Matrix.fromArray(x),
          Matrix.fromArray(x_train),
        ),
        idx,
      });
    }

    // sort distances from least to greatest and get the first k indices
    distances.sort((a, b) => a.distance - b.distance);
    const kNearestDistanceIndices = distances
      .slice(0, this.k)
      .map((o) => o.idx);
    const kNeighborLabels = kNearestDistanceIndices
      .map((i) => this.y_train.data[i])
      .flat();

    const { winner } = kNeighborLabels.reduce(
      (acc, lbl) => {
        acc.counts[lbl] = (acc.counts[lbl] || 0) + 1;
        if (acc.counts[lbl] > acc.best) {
          acc.best = acc.counts[lbl];
          acc.winner = lbl;
        }
        return acc;
      },
      { counts: {}, best: 0, winner: undefined },
    );

    return winner;
  }
}

//const rng = mulberry32(1234);
//rng(); // same value every time you run with seed 1234
//rng(); // next value in the sequence

// IIFE (immediately invoked function expression)
//(async function () {
//  /*
//xTrain - holds the features of the training patients: 30 measurements per row. This is what the model learns from.
//  455 rows × 30 features — array of arrays
//  [[13.54, 14.36, 87.46, ...], [11.42, 20.38, 77.58, ...], ...]
//
//xTest - holds the features of held-out patients. The model makes predictions from these.
//  114 rows × 30 features — array of arrays
//  [[19.81, 22.15, 130.0, ...], ...]
//
//yTrain - holds the correct labels for those patients (1 = malignant, 0 = benign). The model sees these answers while learning.
//  455 labels — flat array, lines up with xTrain
//  [0, 0, 1, 0, 1, ...]
//
//yTest - holds the true labels for the held-out patients. The model never sees these; you use them only to grade its predictions.
//  114 labels — flat array, lines up with xTest
//  [1, 0, 0, ...]
//*/
//
//  const { xTrain, xTest, yTrain, yTest } = trainTestSplit(X, y, {
//    testSize: 0.2,
//    randomState: 1234,
//  });
//})();
