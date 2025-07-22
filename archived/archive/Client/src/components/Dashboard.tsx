// import React from 'react'

import { useAuth } from "../contexts/AuthContext";

export default function Dashboard() {
  console.log("hello from dashboard");

  const { isAuthenticated, token, claims } = useAuth();

  return (
    <>
      <div className="text-center mt-5">
        <h1 className="fw-bold fs-3 mt-5 mb-3">Dashboard</h1>
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
              <h5 className="card-title">Total Balance</h5>
              <p className="card-text">
                The total balance for this bank account (if there is one)
              </p>
            </div>

            {/* Card Footer */}
            <div className="card-footer text-muted">
              <div className="row" style={{ color: "white" }}>
                <div className="col-md-5">
                  <div>Income</div>
                  <div>Enter income amount here</div>
                </div>

                <div className="col-md-2">
                  <div
                    className="d-flex justify-content-center"
                    style={{ height: "5rem" }}
                  >
                    <div className="vr"></div>
                  </div>
                </div>

                <div className="col-md-5">
                  <div>Expenses</div>
                  <div>Enter expense amount here</div>
                </div>
              </div>
            </div>
          </div>
        </div>

        <div className="d-flex justify-content-center mt-3">
          <div
            className="card bg-dark bg-gradient" /*style={{ width: "25rem"}}*/
          >
            {/* Card Body */}
            <div className="card-body" style={{ color: "white" }}>
              <h5 className="card-title">Recent Transactions</h5>
              <p className="card-text">
                Show the 5 most recent transactions here
              </p>
            </div>
          </div>
        </div>
      </div>
    </>
  );
}
