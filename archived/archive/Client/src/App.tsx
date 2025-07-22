import type { JSX } from "react";
import { BrowserRouter, Route, Routes } from "react-router-dom";
import "./App.css";
import Budgets from "./components/Budgets";
import Dashboard from "./components/Dashboard";
import Footer from "./components/Footer";
import Home from "./components/Home";
import Login from "./components/Login";
import Navbar from "./components/Navbar";
import Settings from "./components/Settings";
import Transactions from "./components/Transactions";
import { AuthProvider } from "./contexts/AuthContext";
import Logout from "./components/Logout";

export const App: () => JSX.Element = () => {
  return (
    <AuthProvider>
      <div className="app-wrapper bg-dark text-white">
        <BrowserRouter>
          <Navbar />
          <div className="app-content container-fluid p-5">
            <Routes>
              <Route path="/budgets" element={<Budgets />} />
              <Route path="/dashboard" element={<Dashboard />} />
              <Route path="/transactions" element={<Transactions />} />
              <Route path="/settings" element={<Settings />} />
              <Route path="/login" element={<Login />} />
              <Route path="/logout" element={<Logout />} />
              <Route path="/" element={<Home />} />
            </Routes>
          </div>
          <Footer />
        </BrowserRouter>
      </div>
    </AuthProvider>
  );
};

export default App;
