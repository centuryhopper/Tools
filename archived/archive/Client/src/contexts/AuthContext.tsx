// AuthContext.tsx
import React, { createContext, useContext, useEffect, useState } from "react";

interface AuthContextType {
  token: string | undefined;
  claims: string[] | undefined;
  login: (token: string, rememberMe: boolean) => void;
  logout: () => void;
  isAuthenticated: boolean;
}

const AuthContext = createContext<AuthContextType | undefined>(undefined);

function parseJwt(token: string) {
  try {
    const base64Url = token.split(".")[1];
    // console.log(base64Url);
    const base64 = base64Url.replace(/-/g, "+").replace(/_/g, "/");
    // console.log(base64);
    const jsonPayload = decodeURIComponent(
      atob(base64)
        .split("")
        .map((c) => `%${("00" + c.charCodeAt(0).toString(16)).slice(-2)}`)
        .join("")
    );
    // console.log(jsonPayload);
    return JSON.parse(jsonPayload);
  } catch {
    return undefined;
  }
}

export const AuthProvider: React.FC<{ children: React.ReactNode }> = ({
  children,
}) => {
  const [token, setToken] = useState<string>('');
  const [claims, setClaims] = useState<string[] | undefined>(undefined);

  useEffect(() => {
    const storedToken = localStorage.getItem("token") ?? sessionStorage.getItem("token");
    if (storedToken) {
      const decoded = parseJwt(storedToken);
      const isExpired = decoded?.exp && Date.now() >= decoded.exp * 1000;
      if (!isExpired) {
        setToken(storedToken);
        setClaims(decoded);
      } else {
        localStorage.removeItem("token");
        sessionStorage.removeItem("token");
      }
    }
  }, []);

  const login = (newToken: string, rememberMe: boolean) => {
    const decoded = parseJwt(newToken);
    rememberMe ? localStorage.setItem("token", newToken) : sessionStorage.setItem("token", newToken)
    setToken(newToken);
    setClaims(decoded);
  };

  const logout = () => {
    localStorage.removeItem("token");
    sessionStorage.removeItem("token");
    setToken('');
    setClaims(undefined);
  };

  return (
    <AuthContext.Provider
      value={{ token, claims, login, logout, isAuthenticated: !!token }}
    >
      {children}
    </AuthContext.Provider>
  );
};

export const useAuth = () => {
  const context = useContext(AuthContext);
  if (!context) throw new Error("useAuth must be used within an AuthProvider");
  return context;
};
