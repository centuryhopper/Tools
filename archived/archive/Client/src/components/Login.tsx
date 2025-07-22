import axios from "axios";
import { useAuth } from "../contexts/AuthContext";
import type { LoginModel, LoginResponse } from "../types/LoginTypes";
import LoginComponent from "./LoginComponent";

export default function Login() {
  const { login, isAuthenticated } = useAuth();

  const loginCallback: (model: LoginModel) => Promise<LoginResponse> = async (
    model
  ) => {
    try {
      const response = await axios.post("/api/Account/login", {
        email: model.email,
        password: model.password,
        rememberMe: model.rememberMe,
      });

      login(response.data.token, model.rememberMe);

      return {
        flag: response.data.flag,
        message: response.data.message,
      };
    } catch (e: unknown) {
      let msg = "An unknown error occurred";

      if (axios.isAxiosError(e) && e.response) {
        // console.log(e.response.status);

        switch (e.response.status) {
          case 400:
            msg = "Invalid email/password";
            break;
          default:
            msg = e.response.data?.message || "Unexpected error occurred";
            break;
        }
      } else {
        // console.log("Unknown error:", e);
        msg = String(e);
      }
      return {
        flag: false,
        message: msg,
      };
    }
  };

  return isAuthenticated ? (
    <>
      <div className="p-5 m-5">
        You are already logged in.
      </div>
    </>
  ) : (
    <>
      <div className="p-5 m-5">
        <LoginComponent
          noticeText="Please login to view your information. To request that your account be deleted, please email me."
          loginCallback={loginCallback}
        />
      </div>
    </>
  );
}
