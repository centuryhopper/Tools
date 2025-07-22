// LoginPage.tsx
import React, { useState } from "react";
import { useNavigate } from "react-router-dom";
import useCountDown from "../hooks/useCountDown";
import type { LoginModel, LoginPageProps } from "../types/LoginTypes";
import "./LoginComponent.css";

const LoginPage: React.FC<LoginPageProps> = ({
  cooldownSeconds = 5,
  maxAttempts = 5,
  noticeText = "",
  redirectLink = "/",
  loginCallback,
}) => {
  const [model, setModel] = useState<LoginModel>({
    email: "",
    password: "",
    rememberMe: false,
  });
  const [isPasswordVisible, setIsPasswordVisible] = useState(false);
  const [loading, setLoading] = useState(false);
  const [errMsg, setErrMsg] = useState("");
  const [attemptsNotice, setAttemptsNotice] = useState("");
  const [attempts, setAttempts] = useState(0);
  const [isCoolingDown, setIsCoolingDown] = useState(false);
  const navigate = useNavigate();

  const { secondsLeft, startTimer } = useCountDown({
    onComplete: () => {
      // console.log("time is up");
      setIsCoolingDown(false);
      setAttempts(0);
      setErrMsg("");
      setAttemptsNotice("");
    },
    onTick: () => {
      setErrMsg(
        `Too many attempts. Please wait ${secondsLeft} seconds before trying again.`
      );
    },
  });

  const togglePasswordVisibility = () => setIsPasswordVisible((prev) => !prev);

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { id, value, checked, type } = e.target;
    // console.log(id, value, checked, type);
    setModel((prev) => ({
      ...prev,
      [id]: type === "checkbox" ? checked : value,
    }));
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (isCoolingDown) return;

    setLoading(true);
    // setErrMsg("");
    // setAttemptsNotice("");
    setAttempts((prev) => prev + 1);

    const response = await loginCallback(model);
    setLoading(false);

    if (!response.flag) {
      if (attempts + 1 >= maxAttempts) {
        setIsCoolingDown(true);
        setAttemptsNotice("");

        // start timer here
        startTimer(cooldownSeconds);
      } else {
        setErrMsg(response.message);
        setAttemptsNotice(`${attempts + 1} of ${maxAttempts} attempts`);
      }

      return;
    }

    navigate(redirectLink);
  };

  return (
    <div className="container">
      <form
        id="login_form"
        onSubmit={handleSubmit}
        className="card text-white bg-black"
      >
        <h4 className="card-header">Login</h4>
        <div className="card-body">
          <div className="form-group">
            <label htmlFor="email">Email</label>
            <input
              id="email"
              type="email"
              className="form-control"
              value={model.email}
              onChange={handleChange}
              required
            />
          </div>
          <div className="form-group mt-3">
            <label htmlFor="password">Password</label>
            <div className="input-group">
              <input
                id="password"
                type={isPasswordVisible ? "text" : "password"}
                className="form-control"
                value={model.password}
                onChange={handleChange}
                required
              />
              <div className="input-group-append">
                <button
                  type="button"
                  className="btn btn-outline-secondary"
                  onClick={togglePasswordVisibility}
                >
                  <i
                    className={`fa ${
                      isPasswordVisible ? "fa-eye" : "fa-eye-slash"
                    }`}
                    aria-hidden="true"
                  ></i>
                </button>
              </div>
            </div>
          </div>

          <div className="form-check my-3">
            <input
              id="rememberMe"
              className="form-check-input"
              type="checkbox"
              checked={model.rememberMe}
              onChange={handleChange}
            />
            <label className="form-check-label" htmlFor="rememberMe">
              Remember Me
            </label>
          </div>

          <button
            className="btn btn-primary"
            type="submit"
            disabled={loading || isCoolingDown}
          >
            {loading && (
              <span className="spinner-border spinner-border-sm mr-1"></span>
            )}{" "}
            Login
          </button>

          {errMsg && <p className="text-danger mt-3">{errMsg}</p>}
          {attemptsNotice && <p className="text-danger">{attemptsNotice}</p>}
        </div>
      </form>

      {noticeText && (
        <div className="text-center font-weight-bold mt-3">{noticeText}</div>
      )}
    </div>
  );
};

export default LoginPage;
