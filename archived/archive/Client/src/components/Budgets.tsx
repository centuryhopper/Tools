// import React from 'react'

import BudgetBar from "./Budgetbar";

export default function Budgets() {
  return (
    <>
      <div className="text-center mt-5">
        <h1 className="fw-bold fs-3 mt-5 mb-3">Budgets</h1>
        <div className="d-flex justify-content-center">
          <div
            className="card bg-dark bg-gradient" /*style={{ width: "25rem"}}*/
          >
            {/* Card Header */}
            {/* <div className="card-header">
              Total Balance
            </div> */}

            {/* Card Body */}
            <div className="card-body" style={{ color: "white" }}>
              {/* <h5 className="card-title">Total Balance</h5> */}
              <p className="card-text">
                <BudgetBar category="Groceries" spent={255} budget={400} />
                <BudgetBar category="Rent" spent={1200} budget={1200} />
                <BudgetBar category="Entertainment" spent={400} budget={300} />
              </p>
            </div>
          </div>
        </div>
      </div>
    </>
  );
}
