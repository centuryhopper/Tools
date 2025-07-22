import { Menu, X } from "lucide-react";
import React, { useState } from "react";
import { Link } from "react-router-dom";
import { useAuth } from "../contexts/AuthContext";
import "./Navbar.css";

const Navbar: React.FC = () => {
  const [isOpen, setIsOpen] = useState(false);

  const { isAuthenticated } = useAuth();

  // if (!isAuthenticated) return <div>You must log in</div>;

  const navLinks = isAuthenticated
    ? [
        { name: "Home", href: "/" },
        { name: "Dashboard", href: "/dashboard" },
        { name: "Transactions", href: "/transactions" },
        { name: "Budgets", href: "/budgets" },
        { name: "Settings", href: "/settings" },
        { name: "Logout", href: "/logout" },
      ]
    : [
        {
          name: "Login",
          href: "/login",
        },
      ];

  return (
    <nav className="navbar navbar-expand-md bg-dark border-bottom border-secondary fixed-top shadow">
      <div className="container-fluid px-4 py-2 d-flex justify-content-between align-items-center">
        {/* Hamburger - Mobile only */}
        <button
          onClick={() => setIsOpen(true)}
          className="d-md-none btn btn-link text-white p-0"
          aria-label="Open menu"
        >
          <Menu size={28} />
        </button>

        {/* Logo */}
        <div className="position-absolute top-50 start-50 translate-middle text-white fw-bold text-uppercase text-center d-md-none">
          <Link to="/" className="text-white text-decoration-none">
            Personal Spending
          </Link>
        </div>
        <div className="d-none d-md-block text-white fw-bold text-uppercase">
          <Link to="/" className="text-white text-decoration-none">
            Personal Spending
          </Link>
        </div>

        {/* Desktop nav */}
        <div className="d-none d-md-flex gap-4 fw-medium">
          {navLinks.map((link) => (
            <Link
              key={link.name}
              to={link.href}
              className="btn btn-dark text-white px-3 py-2"
              style={{ whiteSpace: "nowrap" }}
            >
              {link.name}
            </Link>
          ))}
        </div>
      </div>

      {/* Mobile Menu Overlay */}
      {isOpen && (
        <>
          <div
            className="position-fixed top-0 start-0 w-100 h-100 bg-black bg-opacity-75"
            style={{ backdropFilter: "blur(4px)" }}
            onClick={() => setIsOpen(false)}
          />
          <aside
            className="position-fixed top-0 start-0 bg-dark text-white p-4 d-flex flex-column gap-4 h-100 shadow animate-slide-in"
            style={{ width: "250px", zIndex: 1050, overflowY: "auto" }}
          >
            <button
              onClick={() => setIsOpen(false)}
              className="btn btn-link text-white align-self-end p-0"
              aria-label="Close menu"
            >
              <X size={28} />
            </button>
            {navLinks.map((link) => (
              <Link
                key={link.name}
                to={link.href}
                onClick={() => setIsOpen(false)}
                className="text-white text-decoration-none fs-5"
              >
                {link.name}
              </Link>
            ))}
          </aside>

          <style>{`
            @keyframes slide-in {
              from { transform: translateX(-100%); }
              to { transform: translateX(0); }
            }
            .animate-slide-in {
              animation: slide-in 0.3s ease forwards;
            }
          `}</style>
        </>
      )}
    </nav>
  );
};

export default Navbar;
