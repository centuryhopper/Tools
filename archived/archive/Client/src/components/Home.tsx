import axios from "axios";
import { useEffect, useState } from "react";
import { usePlaidLink } from "react-plaid-link";
import { useAuth } from "../contexts/AuthContext";

// axios.defaults.baseURL = "http://localhost:5003"

/*
    transaction model:
    {
        date: ...,
        amount: ...,
        merchant: ...,
        category
    }           
*/

/*
  ✅ Plaid API Integration Flow (Link Token → Access Token)
🧩 Step-by-step:
  Client asks server for link token → Server requests Plaid → Server sends Link token to Client →
  Client opens Plaid UI → Plaid returns Public Token → Client sends it to server →
  Server exchanges for Access Token → Server stores it securely (tied to user)

  DO NOT send access token back to client

*/

export default function Home() {
  const [linkToken, setLinkToken] = useState("");
  const [_, setpublicToken] = useState("");

  const { isAuthenticated, token, claims } = useAuth();

  // console.log('claims ' + JSON.stringify(claims));
  console.log("hello");

  // const { open, ready } = usePlaidLink({
  //   token: linkToken,
  //   onSuccess: (public_token, _) => {
  //     // setpublicToken(public_token);
  //     // send public_token to server
  //     (async () => {
  //       const obj = await axios.post(
  //         "api/Plaid/exchange-public-token",
  //         {
  //           publicToken: public_token,
  //         },
  //         {
  //           headers: {
  //             Authorization: `Bearer ${token}`,
  //           },
  //         }
  //       );
  //       console.log(obj);
  //     })();
  //   },
  // });

  // componentDidMount()
  // useEffect(() => {
    // (async () => {
    //   const response = await axios.get("api/Plaid/get-link-token");
    //   setLinkToken(response.data.link_token);
    // })();
  // }, []);

  return isAuthenticated ? (
    <>
      <h1 className="fw-bold fs-3 mt-5 mb-3">Welcome to My Finance App</h1>

      <button
        className="btn btn-primary mb-4"
        onClick={() => open()}
        disabled={!false}
      >
        Connect a bank account
      </button>

      {/* First row of cards */}
      <div className="row g-4">
        <div className="col-md-6">
          <div className="bg-secondary text-white p-4 rounded shadow">
            <div className="text-center">
              <h2 className="fs-4 fw-semibold mb-2">My accounts</h2>
              <p>Display list of bank accounts you are connected to</p>
            </div>
          </div>
        </div>
        <div className="col-md-6">
          <div className="bg-secondary text-white p-4 rounded shadow">
            <div className="text-center">
              <h2 className="fs-4 fw-semibold mb-2">Recent transactions</h2>
              <p>
                List the five most recent transactions as a preview to the user
                {/*
                      transaction model:
                      {
                          date: ...,
                          amount: ...,
                          merchant: ...,
                          category
                      }
                */}
              </p>
            </div>
          </div>
        </div>
      </div>

      {/* Second row of cards */}
      <div className="row g-4 mt-4">
        <div className="col-md-3"></div>
        <div className="col-md-6">
          <div className="bg-secondary text-white p-4 rounded shadow">
            <div className="text-center">
              <h2 className="fs-4 fw-semibold mb-2">Inflow & Income</h2>
              <p>Display list of bank accounts you are connected to</p>
            </div>
          </div>
        </div>
        <div className="col-md-3"></div>
      </div>
    </>
  ) : (
    <h1 className="fw-bold fs-3 mt-5 mb-3">Please Sign in to continue</h1>
  );
}
