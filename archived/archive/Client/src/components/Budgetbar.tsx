import React from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';


type BudgetBarProps = {
  category: string;
  spent: number;
  budget: number;
};

const BudgetBar: React.FC<BudgetBarProps> = ({ category, spent, budget }) => {
  const progress = Math.min((spent / budget) * 100, 100);

  return (
    <div className="m-1 p-3 rounded shadow-sm bg-black" style={{width: '30rem'}}>
      <div className="d-flex justify-content-between align-items-center mb-1">
        <span className="fw-semibold text-white">{category}</span>
        <span className="fw-bold text-white">${budget.toFixed(2)}</span>
      </div>
      <div className="progress" style={{ height: '8px' }}>
        <div
          className="progress-bar bg-primary"
          role="progressbar"
          style={{ width: `${progress}%` }}
          aria-valuenow={spent}
          aria-valuemin={0}
          aria-valuemax={budget}
        ></div>
      </div>
      <div className={`text-end mt-1 small text-${spent <= budget ? 'white' : 'danger'}`}>${spent.toFixed(2)}</div>
    </div>
  );
};

export default BudgetBar;
