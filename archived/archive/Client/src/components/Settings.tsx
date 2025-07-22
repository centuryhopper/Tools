// import React from 'react'
import './Settings.css';

export default function Settings() {

  const selections = [
    {
      name: 'App Appearance',
    },
    {
      name: 'Security'
    },
    {
      name: 'Notifications'
    },
  ];

  return (
    <>
      <div className="text-center mt-5" >
        <h1 className="fw-bold fs-3 mt-5 mb-3">Settings</h1>
        <div className="d-flex justify-content-center">

          <div className="card bg-dark bg-gradient" /*style={{ width: "25rem"}}*/>

            {/* Card Body */}
            <div className="card-body" style={{color: 'white'}}>
              {/* <h5 className="card-title">Total Balance</h5> */}
              <p className="card-text">
                <ul className="list-group" style={{width: '15rem'}}>
                  {
                    selections.map(sel => (
                      <li key={sel.name} className="list-group-item custom-setting-item d-flex justify-content-between align-items-center" onClick={() => console.log('clicked ' + sel.name)}>
                        {sel.name}
                        <span className="arrow">&rarr;</span>
                      </li>
                    ))
                  }
                </ul>
              </p>
            </div>
          </div>

          
        </div>

      </div>
    </>
  )
}
