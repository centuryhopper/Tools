// import React from 'react'

export default function Transactions() {
  return (
    <>
      <div className="text-center mt-5">
        <h1 className="fw-bold fs-3 mt-5 mb-3">Transactions</h1>
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
                Show all transactions here. Users should be able to sort by
                filtering for the last week last month, etc. And make list
                scrollable
              </p>
            </div>
          </div>
        </div>
      </div>
    </>
  );
}
