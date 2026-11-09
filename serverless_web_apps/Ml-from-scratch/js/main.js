import { trainTestSplit } from "./data_functions/split.js";
import { KNN } from "./model/knn.js";

const parseBreastCancerCSV = (csvText) => {
  const rows = csvText.split("\n").slice(1); // drop header
  const X = []; // feature values
  const y = []; // labels (answers i.e. 0 for benign, 1 for malignant)

  for (const [_, row] of rows.entries()) {
    if (!row) continue;
    const cols = row.split(",");
    X.push(cols.slice(2, 32).map(Number));
    y.push(cols[1] === "M" ? 1 : 0);
  }

  return {
    X,
    y,
  };
};

const url = new URL("../data/breast-cancer.csv", import.meta.url);
const res = await fetch(url);
if (!res.ok) throw new Error(`${url}: ${res.status} ${res.statusText}`);
const text = await res.text();
const { X, y } = parseBreastCancerCSV(text);

const { xTrain, xTest, yTrain, yTest } = trainTestSplit(X, y, {
  testSize: 0.3,
  randomState: 1003,
});

// normalize xTrain/xTest here, then:
const model = new KNN(5);
model.fit(xTrain, yTrain);

const predictions = model.predict(xTest).flatten();
const yTest_FLATTENED = yTest.data.flat();

const correct = predictions.filter((p, i) => p === yTest_FLATTENED[i]).length;
console.log(`accuracy: ${((correct / yTest.data.length) * 100).toFixed(1)}%`);
